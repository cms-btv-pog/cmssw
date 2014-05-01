#include <functional>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <boost/iterator/transform_iterator.hpp>

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "DataFormats/BTauReco/interface/TrackIPTagInfo.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"

#include "RecoVertex/VertexPrimitives/interface/VertexException.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"
#include "RecoVertex/ConfigurableVertexReco/interface/ConfigurableVertexReconstructor.h"
#include "RecoVertex/GhostTrackFitter/interface/GhostTrackVertexFinder.h"
#include "RecoVertex/GhostTrackFitter/interface/GhostTrackPrediction.h"
#include "RecoVertex/GhostTrackFitter/interface/GhostTrackState.h"
#include "RecoVertex/GhostTrackFitter/interface/GhostTrack.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "RecoBTag/SecondaryVertex/interface/TrackSelector.h"
#include "RecoBTag/SecondaryVertex/interface/TrackSorting.h"
#include "RecoBTag/SecondaryVertex/interface/SecondaryVertex.h"
#include "RecoBTag/SecondaryVertex/interface/VertexFilter.h"
#include "RecoBTag/SecondaryVertex/interface/VertexSorting.h"

//#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"

//
// constants, enums and typedefs
//
typedef boost::shared_ptr<fastjet::ClusterSequence>  ClusterSequencePtr;
typedef boost::shared_ptr<fastjet::JetDefinition>    JetDefPtr;


using namespace reco;

namespace {
	class VertexInfo : public fastjet::PseudoJet::UserInfoBase{
	  public:
	    VertexInfo(const int vertexIndex) :
	      m_vertexIndex(vertexIndex) { }

	    inline const int vertexIndex() const { return m_vertexIndex; }

	  protected:
	    int m_vertexIndex;
	};

	template<typename T>
	struct RefToBaseLess : public std::binary_function<edm::RefToBase<T>,
							   edm::RefToBase<T>,
							   bool> {
		inline bool operator()(const edm::RefToBase<T> &r1,
				       const edm::RefToBase<T> &r2) const
		{
			return r1.id() < r2.id() ||
			       (r1.id() == r2.id() && r1.key() < r2.key());
		}
	};
}

class SecondaryVertexProducer : public edm::EDProducer {
    public:
	explicit SecondaryVertexProducer(const edm::ParameterSet &params);
	~SecondaryVertexProducer();

	virtual void produce(edm::Event &event, const edm::EventSetup &es) override;

    private:
       void matchReclusteredJets(const edm::Handle<TrackIPTagInfoCollection>& jets,
                                 const std::vector<fastjet::PseudoJet>& matchedJets,
                                 std::vector<int>& matchedIndices);
       void matchReclusteredFatJets(const edm::Handle<edm::View<reco::Jet> >& jets,
                                    const std::vector<fastjet::PseudoJet>& matchedJets,
                                    std::vector<int>& matchedIndices);
       void matchGroomedJets(const edm::Handle<edm::View<reco::Jet> >& jets,
                             const edm::Handle<edm::View<reco::Jet> >& matchedJets,
                             std::vector<int>& matchedIndices);
       void matchSubjets(const std::vector<int>& groomedIndices,
                         const edm::Handle<edm::View<reco::Jet> >& groomedJets,
                         const edm::Handle<TrackIPTagInfoCollection>& subjets,
                         std::vector<std::vector<int> >& matchedIndices);

	enum ConstraintType {
		CONSTRAINT_NONE	= 0,
		CONSTRAINT_BEAMSPOT,
		CONSTRAINT_PV_BEAMSPOT_SIZE,
		CONSTRAINT_PV_BS_Z_ERRORS_SCALED,
		CONSTRAINT_PV_ERROR_SCALED,
		CONSTRAINT_PV_PRIMARIES_IN_FIT
	};

	static ConstraintType getConstraintType(const std::string &name);

        edm::EDGetTokenT<reco::BeamSpot> token_BeamSpot;
        edm::EDGetTokenT<reco::TrackIPTagInfoCollection> token_trackIPTagInfo;
	TrackIPTagInfo::SortCriteria	sortCriterium;
	TrackSelector			trackSelector;
	ConstraintType			constraint;
	double				constraintScaling;
	edm::ParameterSet		vtxRecoPSet;
	bool				useGhostTrack;
	bool				withPVError;
	double				minTrackWeight;
	VertexFilter			vertexFilter;
	VertexSorting			vertexSorting;
        bool                            useExternalSV;  
        double                          extSVDeltaRToJet;      
        edm::EDGetTokenT<reco::VertexCollection> token_extSVCollection;
	bool                        useSVClustering;
	bool                        useSVMomentum;
	std::string                 jetAlgorithm;
	double                      rParam;
	double                      jetPtMin;
	double                      ghostRescaling;
	bool                        useFatJets;
	edm::EDGetTokenT<edm::View<reco::Jet> > token_fatJets;
	edm::EDGetTokenT<edm::View<reco::Jet> > token_groomedFatJets;
	
	ClusterSequencePtr           fjClusterSeq;
	JetDefPtr                    fjJetDefinition;
};

SecondaryVertexProducer::ConstraintType
SecondaryVertexProducer::getConstraintType(const std::string &name)
{
	if (name == "None")
		return CONSTRAINT_NONE;
	else if (name == "BeamSpot")
		return CONSTRAINT_BEAMSPOT;
	else if (name == "BeamSpot+PVPosition")
		return CONSTRAINT_PV_BEAMSPOT_SIZE;
	else if (name == "BeamSpotZ+PVErrorScaledXY")
		return CONSTRAINT_PV_BS_Z_ERRORS_SCALED;
	else if (name == "PVErrorScaled")
		return CONSTRAINT_PV_ERROR_SCALED;
	else if (name == "BeamSpot+PVTracksInFit")
		return CONSTRAINT_PV_PRIMARIES_IN_FIT;
	else
		throw cms::Exception("InvalidArgument")
			<< "SecondaryVertexProducer: ``constraint'' parameter "
			   "value \"" << name << "\" not understood."
			<< std::endl;
}

static GhostTrackVertexFinder::FitType
getGhostTrackFitType(const std::string &name)
{
	if (name == "AlwaysWithGhostTrack")
		return GhostTrackVertexFinder::kAlwaysWithGhostTrack;
	else if (name == "SingleTracksWithGhostTrack")
		return GhostTrackVertexFinder::kSingleTracksWithGhostTrack;
	else if (name == "RefitGhostTrackWithVertices")
		return GhostTrackVertexFinder::kRefitGhostTrackWithVertices;
	else
		throw cms::Exception("InvalidArgument")
			<< "SecondaryVertexProducer: ``fitType'' "
			   "parameter value \"" << name << "\" for "
			   "GhostTrackVertexFinder settings not "
			   "understood." << std::endl;
}

SecondaryVertexProducer::SecondaryVertexProducer(
					const edm::ParameterSet &params) :
	sortCriterium(TrackSorting::getCriterium(params.getParameter<std::string>("trackSort"))),
	trackSelector(params.getParameter<edm::ParameterSet>("trackSelection")),
	constraint(getConstraintType(params.getParameter<std::string>("constraint"))),
	constraintScaling(1.0),
	vtxRecoPSet(params.getParameter<edm::ParameterSet>("vertexReco")),
	useGhostTrack(vtxRecoPSet.getParameter<std::string>("finder") == "gtvr"),
	withPVError(params.getParameter<bool>("usePVError")),
	minTrackWeight(params.getParameter<double>("minimumTrackWeight")),
	vertexFilter(params.getParameter<edm::ParameterSet>("vertexCuts")),
	vertexSorting(params.getParameter<edm::ParameterSet>("vertexSelection"))
{
	token_trackIPTagInfo =  consumes<reco::TrackIPTagInfoCollection>(params.getParameter<edm::InputTag>("trackIPTagInfos"));
	if (constraint == CONSTRAINT_PV_ERROR_SCALED ||
	    constraint == CONSTRAINT_PV_BS_Z_ERRORS_SCALED)
		constraintScaling = params.getParameter<double>("pvErrorScaling");

	if (constraint == CONSTRAINT_PV_BEAMSPOT_SIZE ||
	    constraint == CONSTRAINT_PV_BS_Z_ERRORS_SCALED ||
	    constraint == CONSTRAINT_BEAMSPOT ||
	    constraint == CONSTRAINT_PV_PRIMARIES_IN_FIT )
	    token_BeamSpot = consumes<reco::BeamSpot>(params.getParameter<edm::InputTag>("beamSpotTag"));
        useExternalSV = false;
        if(params.existsAs<bool>("useExternalSV")) useExternalSV = params.getParameter<bool> ("useExternalSV");
        if(useExternalSV) {
	   token_extSVCollection =  mayConsume<reco::VertexCollection>(params.getParameter<edm::InputTag>("extSVCollection"));
       	   extSVDeltaRToJet = params.getParameter<double>("extSVDeltaRToJet");
        }
       useSVClustering = ( params.exists("useSVClustering") ? params.getParameter<bool>("useSVClustering") : false );
	useSVMomentum = ( params.exists("useSVMomentum") ? params.getParameter<bool>("useSVMomentum") : false );
       useFatJets = ( useExternalSV && useSVClustering && params.exists("fatJets") && params.exists("groomedFatJets") );
	if( useSVClustering )
	{
	  jetAlgorithm = params.getParameter<std::string>("jetAlgorithm");
         rParam = params.getParameter<double>("rParam");
         jetPtMin = 0.; // hardcoded to 0. since we simply want to recluster all input jets which already had some PtMin applied
         ghostRescaling = ( params.exists("ghostRescaling") ? params.getParameter<double>("ghostRescaling") : 1e-18 );

         // set jet algorithm
	  if (jetAlgorithm=="Kt")
	    fjJetDefinition = JetDefPtr( new fastjet::JetDefinition(fastjet::kt_algorithm, rParam) );
	  else if (jetAlgorithm=="CambridgeAachen")
	    fjJetDefinition = JetDefPtr( new fastjet::JetDefinition(fastjet::cambridge_algorithm, rParam) );
	  else if (jetAlgorithm=="AntiKt")
	    fjJetDefinition = JetDefPtr( new fastjet::JetDefinition(fastjet::antikt_algorithm, rParam) );
	  else
	    throw cms::Exception("InvalidJetAlgorithm") << "Jet clustering algorithm is invalid: " << jetAlgorithm << ", use CambridgeAachen | Kt | AntiKt" << std::endl;
	}
	if( useFatJets )
	{
	  token_fatJets = mayConsume<edm::View<reco::Jet> >(params.getParameter<edm::InputTag>("fatJets"));
	  token_groomedFatJets = mayConsume<edm::View<reco::Jet> >(params.getParameter<edm::InputTag>("groomedFatJets"));
	}
	
	produces<SecondaryVertexTagInfoCollection>();
}

SecondaryVertexProducer::~SecondaryVertexProducer()
{
}

namespace {
	struct SVBuilder :
		public std::unary_function<const Vertex&, SecondaryVertex> {

		SVBuilder(const reco::Vertex &pv,
		          const GlobalVector &direction,
		          bool withPVError) :
			pv(pv), direction(direction),
			withPVError(withPVError) {}

		SecondaryVertex operator () (const reco::Vertex &sv) const
		{ return SecondaryVertex(pv, sv, direction, withPVError); }

		const Vertex		&pv;
		const GlobalVector	&direction;
		bool			withPVError;
	};

	struct SVFilter :
		public std::unary_function<const SecondaryVertex&, bool> {

		SVFilter(const VertexFilter &filter, const Vertex &pv,
		         const GlobalVector &direction) :
			filter(filter), pv(pv), direction(direction) {}

		inline bool operator () (const SecondaryVertex &sv) const
		{ return !filter(pv, sv, direction); }

		const VertexFilter	&filter;
		const Vertex		&pv;
		const GlobalVector	&direction;
	};
			
} // anonynmous namespace

void SecondaryVertexProducer::produce(edm::Event &event,
                                      const edm::EventSetup &es)
{
	typedef std::map<TrackBaseRef, TransientTrack,
	                 RefToBaseLess<Track> > TransientTrackMap;

	edm::ESHandle<TransientTrackBuilder> trackBuilder;
	es.get<TransientTrackRecord>().get("TransientTrackBuilder",
	                                   trackBuilder);

	edm::Handle<TrackIPTagInfoCollection> trackIPTagInfos;
	event.getByToken(token_trackIPTagInfo, trackIPTagInfos);

        // External Sec Vertex collection (e.g. for IVF usage)
        edm::Handle<reco::VertexCollection> extSecVertex;          
        if(useExternalSV) event.getByToken(token_extSVCollection,extSecVertex);
	 
	edm::Handle<edm::View<reco::Jet> > fatJetsHandle;
	edm::Handle<edm::View<reco::Jet> > groomedFatJetsHandle;
	if( useFatJets )
	{
	  event.getByToken(token_fatJets, fatJetsHandle);
	  event.getByToken(token_groomedFatJets, groomedFatJetsHandle);
	  
	  if( groomedFatJetsHandle->size() > fatJetsHandle->size() )
           edm::LogError("TooManyGroomedJets") << "There are more groomed (" << groomedFatJetsHandle->size() << ") than original fat jets (" << fatJetsHandle->size() << "). Please check that the two jet collections belong to each other.";
	}
                                                             

	edm::Handle<BeamSpot> beamSpot;
	unsigned int bsCovSrc[7] = { 0, };
	double sigmaZ = 0.0, beamWidth = 0.0;
	switch(constraint) {
	    case CONSTRAINT_PV_BEAMSPOT_SIZE:
		event.getByToken(token_BeamSpot,beamSpot);
		bsCovSrc[3] = bsCovSrc[4] = bsCovSrc[5] = bsCovSrc[6] = 1;
		sigmaZ = beamSpot->sigmaZ();
		beamWidth = beamSpot->BeamWidthX();
		break;

	    case CONSTRAINT_PV_BS_Z_ERRORS_SCALED:
		event.getByToken(token_BeamSpot,beamSpot);
		bsCovSrc[0] = bsCovSrc[1] = 2;
		bsCovSrc[3] = bsCovSrc[4] = bsCovSrc[5] = 1;
		sigmaZ = beamSpot->sigmaZ();
		break;

	    case CONSTRAINT_PV_ERROR_SCALED:
		bsCovSrc[0] = bsCovSrc[1] = bsCovSrc[2] = 2;
		break;

	    case CONSTRAINT_BEAMSPOT:
	    case CONSTRAINT_PV_PRIMARIES_IN_FIT:
		event.getByToken(token_BeamSpot,beamSpot);
		break;

	    default:
		/* nothing */;
	}

	std::vector<std::vector<int> > clusteredSVs(trackIPTagInfos->size(),std::vector<int>());
	if( useExternalSV && useSVClustering && trackIPTagInfos->size()>0 )
	{
	  // vector of constituents for reclustering jets and "ghost" SVs
	  std::vector<fastjet::PseudoJet> fjInputs;
	  // loop over all input jets and collect all their constituents
	  if( useFatJets )
	  {
	    for(edm::View<reco::Jet>::const_iterator it = fatJetsHandle->begin(); it != fatJetsHandle->end(); ++it)
	    {
	      std::vector<edm::Ptr<reco::Candidate> > constituents = it->getJetConstituents();
	      std::vector<edm::Ptr<reco::Candidate> >::const_iterator m;
	      for( m = constituents.begin(); m != constituents.end(); ++m )
	      {
		 reco::CandidatePtr constit = *m;
		 if(constit->pt() == 0)
		 {
		   edm::LogWarning("NullTransverseMomentum") << "dropping input candidate with pt=0";
		   continue;
		 }
		 fjInputs.push_back(fastjet::PseudoJet(constit->px(),constit->py(),constit->pz(),constit->energy()));
	      }
	    }
	  }
	  else
	  {
	    for(TrackIPTagInfoCollection::const_iterator it = trackIPTagInfos->begin(); it != trackIPTagInfos->end(); ++it)
	    {
	      std::vector<edm::Ptr<reco::Candidate> > constituents = it->jet()->getJetConstituents();
	      std::vector<edm::Ptr<reco::Candidate> >::const_iterator m;
	      for( m = constituents.begin(); m != constituents.end(); ++m )
	      {
		 reco::CandidatePtr constit = *m;
		 if(constit->pt() == 0)
		 {
		   edm::LogWarning("NullTransverseMomentum") << "dropping input candidate with pt=0";
		   continue;
		 }
		 fjInputs.push_back(fastjet::PseudoJet(constit->px(),constit->py(),constit->pz(),constit->energy()));
	      }
	    }
	  }
	  // insert "ghost" SVs in the vector of constituents
	  for(reco::VertexCollection::const_iterator it = extSecVertex->begin(); it != extSecVertex->end(); ++it)
	  {
	    const Vertex &pv = *(trackIPTagInfos->front().primaryVertex());
	    GlobalPoint pvPosition(pv.x(),pv.y(),pv.z());
	    GlobalPoint svPosition(it->x(),it->y(),it->z());
	    GlobalVector dir( svPosition - pvPosition );
	    dir = dir.unit();
	    fastjet::PseudoJet p(dir.x(),dir.y(),dir.z(),dir.mag()); // using SV flight direction so treating SV as massless
	    if( useSVMomentum )
	      p = fastjet::PseudoJet(it->p4().px(),it->p4().py(),it->p4().pz(),it->p4().energy());
	    p*=ghostRescaling; // rescale SV direction/momentum
	    p.set_user_info(new VertexInfo( it - extSecVertex->begin() ));
	    fjInputs.push_back(p);
	  }
	  
	  // define jet clustering sequence
	  fjClusterSeq = ClusterSequencePtr( new fastjet::ClusterSequence( fjInputs, *fjJetDefinition ) );
	  // recluster jet constituents and inserted "ghosts"
	  std::vector<fastjet::PseudoJet> inclusiveJets = fastjet::sorted_by_pt( fjClusterSeq->inclusive_jets(jetPtMin) );
	  
	  if( useFatJets )
	  {
	    if( inclusiveJets.size() < fatJetsHandle->size() )
             edm::LogError("TooFewReclusteredJets") << "There are fewer reclustered (" << inclusiveJets.size() << ") than original fat jets (" << fatJetsHandle->size() << "). Please check that the jet algorithm and jet size match those used for the original jet collection.";

	    // match reclustered and original fat jets
	    std::vector<int> reclusteredIndices;
	    matchReclusteredFatJets(fatJetsHandle,inclusiveJets,reclusteredIndices);

	    // match groomed and original fat jets
	    std::vector<int> groomedIndices;
	    matchGroomedJets(fatJetsHandle,groomedFatJetsHandle,groomedIndices);

	    // match subjets and original fat jets
	    std::vector<std::vector<int> > subjetIndices;
	    matchSubjets(groomedIndices,groomedFatJetsHandle,trackIPTagInfos,subjetIndices);
	
	    // collect clustered SVs
	    for(size_t i=0; i<fatJetsHandle->size(); ++i)
	    {
	      // since the "ghosts" are extremely soft, the configuration and ordering of the reclustered and original fat jets should in principle stay the same
	      if( ( fabs( inclusiveJets.at(reclusteredIndices.at(i)).pt() - fatJetsHandle->at(i).pt() ) / fatJetsHandle->at(i).pt() ) > 1e-3 ) // 0.1% difference in Pt should be sufficient to detect possible misconfigurations
	      {
		 if( fatJetsHandle->at(i).pt() < 10. )  // special handling for low-Pt jets (Pt<10 GeV)
		   edm::LogWarning("JetPtMismatchAtLowPt") << "The reclustered and original fat jet " << i << " have different Pt's (" << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << fatJetsHandle->at(i).pt() << " GeV, respectively).\n"
								 << "Please check that the jet algorithm and jet size match those used for the original fat jet collection and also make sure the original fat jets are uncorrected. In addition, make sure you are not using CaloJets which are presently not supported.\n"
								 << "Since the mismatch is at low Pt, it is ignored and only a warning is issued.\n"
								 << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision in which case make sure the original jet collection is produced and reclustering is performed in the same job.";
		 else
		   edm::LogError("JetPtMismatch") << "The reclustered and original fat jet " << i << " have different Pt's (" << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << fatJetsHandle->at(i).pt() << " GeV, respectively).\n"
						      << "Please check that the jet algorithm and jet size match those used for the original fat jet collection and also make sure the original fat jets are uncorrected. In addition, make sure you are not using CaloJets which are presently not supported.\n"
						      << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision in which case make sure the original jet collection is produced and reclustering is performed in the same job.";
	      }
	
	      // get jet constituents
	      std::vector<fastjet::PseudoJet> constituents = inclusiveJets.at(reclusteredIndices.at(i)).constituents();

	      std::vector<int> svIndices;
	      // loop over jet constituents and try to find "ghosts"
	      for(std::vector<fastjet::PseudoJet>::const_iterator it = constituents.begin(); it != constituents.end(); ++it)
	      {
		 if( !it->has_user_info() ) continue; // skip if not a "ghost"
		 
		 svIndices.push_back( it->user_info<VertexInfo>().vertexIndex() );
	      }

	      if( subjetIndices.at(i).size()==0 ) continue; // continue if the original jet does not have subjets assigned
	
	      // loop over clustered SVs and assign them to different subjets based on smallest dR
	      for(size_t sv=0; sv<svIndices.size(); ++sv)
	      {
               const Vertex &pv = *(trackIPTagInfos->front().primaryVertex());
		 const Vertex &extSV = (*extSecVertex)[ svIndices.at(sv) ];
		 GlobalPoint pvPosition(pv.x(),pv.y(),pv.z());
		 GlobalPoint svPosition(extSV.x(),extSV.y(),extSV.z());
		 GlobalVector dir( svPosition - pvPosition );
		 dir = dir.unit();
		 fastjet::PseudoJet p(dir.x(),dir.y(),dir.z(),dir.mag()); // using SV flight direction so treating SV as massless
		 if( useSVMomentum )
		   p = fastjet::PseudoJet(extSV.p4().px(),extSV.p4().py(),extSV.p4().pz(),extSV.p4().energy());
		
		 std::vector<double> dR2toSubjets;
		
		 for(size_t sj=0; sj<subjetIndices.at(i).size(); ++sj)
		   dR2toSubjets.push_back( reco::deltaR2( p.rapidity(), p.phi_std(), trackIPTagInfos->at(subjetIndices.at(i).at(sj)).jet()->rapidity(), trackIPTagInfos->at(subjetIndices.at(i).at(sj)).jet()->phi() ) );

		 // find the closest subjet
		 int closestSubjetIdx = std::distance( dR2toSubjets.begin(), std::min_element(dR2toSubjets.begin(), dR2toSubjets.end()) );

		 clusteredSVs.at(subjetIndices.at(i).at(closestSubjetIdx)).push_back( svIndices.at(sv) );
	      }
	    }
	  }
	  else
	  {
	    if( inclusiveJets.size() < trackIPTagInfos->size() )
             edm::LogError("TooFewReclusteredJets") << "There are fewer reclustered (" << inclusiveJets.size() << ") than original jets (" << trackIPTagInfos->size() << "). Please check that the jet algorithm and jet size match those used for the original jet collection.";
	
	    // match reclustered and original jets
           std::vector<int> reclusteredIndices;
           matchReclusteredJets(trackIPTagInfos,inclusiveJets,reclusteredIndices);
	
	    // collect clustered SVs
	    for(size_t i=0; i<trackIPTagInfos->size(); ++i)
	    {
	      // since the "ghosts" are extremely soft, the configuration and ordering of the reclustered and original jets should in principle stay the same
	      if( ( fabs( inclusiveJets.at(reclusteredIndices.at(i)).pt() - trackIPTagInfos->at(i).jet()->pt() ) / trackIPTagInfos->at(i).jet()->pt() ) > 1e-3 ) // 0.1% difference in Pt should be sufficient to detect possible misconfigurations
	      {
		 if( trackIPTagInfos->at(i).jet()->pt() < 10. )  // special handling for low-Pt jets (Pt<10 GeV)
		   edm::LogWarning("JetPtMismatchAtLowPt") << "The reclustered and original jet " << i << " have different Pt's (" << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << trackIPTagInfos->at(i).jet()->pt() << " GeV, respectively).\n"
								 << "Please check that the jet algorithm and jet size match those used for the original jet collection and also make sure the original jets are uncorrected. In addition, make sure you are not using CaloJets which are presently not supported.\n"
								 << "Since the mismatch is at low Pt, it is ignored and only a warning is issued.\n"
								 << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision in which case make sure the original jet collection is produced and reclustering is performed in the same job.";
		 else
		   edm::LogError("JetPtMismatch") << "The reclustered and original jet " << i << " have different Pt's (" << inclusiveJets.at(reclusteredIndices.at(i)).pt() << " vs " << trackIPTagInfos->at(i).jet()->pt() << " GeV, respectively).\n"
						      << "Please check that the jet algorithm and jet size match those used for the original jet collection and also make sure the original jets are uncorrected. In addition, make sure you are not using CaloJets which are presently not supported.\n"
						      << "\nIn extremely rare instances the mismatch could be caused by a difference in the machine precision in which case make sure the original jet collection is produced and reclustering is performed in the same job.";
	      }
	
	      // get jet constituents
	      std::vector<fastjet::PseudoJet> constituents = inclusiveJets.at(reclusteredIndices.at(i)).constituents();

	      // loop over jet constituents and try to find "ghosts"
	      for(std::vector<fastjet::PseudoJet>::const_iterator it = constituents.begin(); it != constituents.end(); ++it)
	      {
		 if( !it->has_user_info() ) continue; // skip if not a "ghost"
		 // push back clustered SV indices
		 clusteredSVs.at(i).push_back( it->user_info<VertexInfo>().vertexIndex() );
	      }
	    }
	  }
	}
	
	std::auto_ptr<ConfigurableVertexReconstructor> vertexReco;
	std::auto_ptr<GhostTrackVertexFinder> vertexRecoGT;
	if (useGhostTrack)
		vertexRecoGT.reset(new GhostTrackVertexFinder(
			vtxRecoPSet.getParameter<double>("maxFitChi2"),
			vtxRecoPSet.getParameter<double>("mergeThreshold"),
			vtxRecoPSet.getParameter<double>("primcut"),
			vtxRecoPSet.getParameter<double>("seccut"),
			getGhostTrackFitType(vtxRecoPSet.getParameter<std::string>("fitType"))));
	else
		vertexReco.reset(
			new ConfigurableVertexReconstructor(vtxRecoPSet));

	TransientTrackMap primariesMap;

	// result secondary vertices

	std::auto_ptr<SecondaryVertexTagInfoCollection>
			tagInfos(new SecondaryVertexTagInfoCollection);

	for(TrackIPTagInfoCollection::const_iterator iterJets =
		trackIPTagInfos->begin(); iterJets != trackIPTagInfos->end();
		++iterJets) {
		std::vector<SecondaryVertexTagInfo::IndexedTrackData> trackData;
//		      std::cout << "Jet " << iterJets-trackIPTagInfos->begin() << std::endl; 

		const Vertex &pv = *iterJets->primaryVertex();

		std::set<TransientTrack> primaries;
		if (constraint == CONSTRAINT_PV_PRIMARIES_IN_FIT) {
			for(Vertex::trackRef_iterator iter = pv.tracks_begin();
			    iter != pv.tracks_end(); ++iter) {
				TransientTrackMap::iterator pos =
					primariesMap.lower_bound(*iter);

				if (pos != primariesMap.end() &&
				    pos->first == *iter)
					primaries.insert(pos->second);
				else {
					TransientTrack track =
						trackBuilder->build(
							iter->castTo<TrackRef>());
					primariesMap.insert(pos,
						std::make_pair(*iter, track));
					primaries.insert(track);
				}
			}
		}

		edm::RefToBase<Jet> jetRef = iterJets->jet();

		GlobalVector jetDir(jetRef->momentum().x(),
		                    jetRef->momentum().y(),
		                    jetRef->momentum().z());

		std::vector<std::size_t> indices =
				iterJets->sortedIndexes(sortCriterium);

		TrackRefVector trackRefs = iterJets->sortedTracks(indices);

		const std::vector<TrackIPTagInfo::TrackIPData> &ipData =
					iterJets->impactParameterData();

		// build transient tracks used for vertex reconstruction

		std::vector<TransientTrack> fitTracks;
		std::vector<GhostTrackState> gtStates;
		std::auto_ptr<GhostTrackPrediction> gtPred;
		if (useGhostTrack)
			gtPred.reset(new GhostTrackPrediction(
						*iterJets->ghostTrack()));

		for(unsigned int i = 0; i < indices.size(); i++) {
			typedef SecondaryVertexTagInfo::IndexedTrackData IndexedTrackData;

			const TrackRef &trackRef = trackRefs[i];

			trackData.push_back(IndexedTrackData());
			trackData.back().first = indices[i];

			// select tracks for SV finder

			if (!trackSelector(*trackRef, ipData[indices[i]], *jetRef,
			                   RecoVertex::convertPos(
			                   		pv.position()))) {
				trackData.back().second.svStatus =
					SecondaryVertexTagInfo::TrackData::trackSelected;
				continue;
			}

			TransientTrackMap::const_iterator pos =
					primariesMap.find(
						TrackBaseRef(trackRef));
			TransientTrack fitTrack;
			if (pos != primariesMap.end()) {
				primaries.erase(pos->second);
				fitTrack = pos->second;
			} else
				fitTrack = trackBuilder->build(trackRef);
			fitTracks.push_back(fitTrack);

			trackData.back().second.svStatus =
				SecondaryVertexTagInfo::TrackData::trackUsedForVertexFit;

			if (useGhostTrack) {
				GhostTrackState gtState(fitTrack);
				GlobalPoint pos =
					ipData[indices[i]].closestToGhostTrack;
				gtState.linearize(*gtPred, true,
				                  gtPred->lambda(pos));
				gtState.setWeight(ipData[indices[i]].ghostTrackWeight);
				gtStates.push_back(gtState);
			}
		}

		std::auto_ptr<GhostTrack> ghostTrack;
		if (useGhostTrack)
			ghostTrack.reset(new GhostTrack(
				GhostTrackPrediction(
					RecoVertex::convertPos(pv.position()),
					RecoVertex::convertError(pv.error()),
					GlobalVector(
						iterJets->ghostTrack()->px(),
						iterJets->ghostTrack()->py(),
						iterJets->ghostTrack()->pz()),
					0.05),
				*gtPred, gtStates,
				iterJets->ghostTrack()->chi2(),
				iterJets->ghostTrack()->ndof()));

		// perform actual vertex finding


	 	std::vector<reco::Vertex>       extAssoCollection;    
		std::vector<TransientVertex> fittedSVs;
		std::vector<SecondaryVertex> SVs;
		if(!useExternalSV){ 
    		  switch(constraint)   {
		    case CONSTRAINT_NONE:
			if (useGhostTrack)
				fittedSVs = vertexRecoGT->vertices(
						pv, *ghostTrack);
			else
				fittedSVs = vertexReco->vertices(fitTracks);
			break;

		    case CONSTRAINT_BEAMSPOT:
			if (useGhostTrack)
				fittedSVs = vertexRecoGT->vertices(
						pv, *beamSpot, *ghostTrack);
			else
				fittedSVs = vertexReco->vertices(fitTracks,
				                                 *beamSpot);
			break;

		    case CONSTRAINT_PV_BEAMSPOT_SIZE:
		    case CONSTRAINT_PV_BS_Z_ERRORS_SCALED:
		    case CONSTRAINT_PV_ERROR_SCALED: {
			BeamSpot::CovarianceMatrix cov;
			for(unsigned int i = 0; i < 7; i++) {
				unsigned int covSrc = bsCovSrc[i];
				for(unsigned int j = 0; j < 7; j++) {
					double v=0.0;
					if (!covSrc || bsCovSrc[j] != covSrc)
						v = 0.0;
					else if (covSrc == 1)
						v = beamSpot->covariance(i, j);
					else if (j<3 && i<3)
						v = pv.covariance(i, j) *
						    constraintScaling;
					cov(i, j) = v;
				}
			}

			BeamSpot bs(pv.position(), sigmaZ,
			            beamSpot.isValid() ? beamSpot->dxdz() : 0.,
			            beamSpot.isValid() ? beamSpot->dydz() : 0.,
			            beamWidth, cov, BeamSpot::Unknown);

			if (useGhostTrack)
				fittedSVs = vertexRecoGT->vertices(
						pv, bs, *ghostTrack);
			else
				fittedSVs = vertexReco->vertices(fitTracks, bs);
		    }	break;

		    case CONSTRAINT_PV_PRIMARIES_IN_FIT: {
			std::vector<TransientTrack> primaries_(
					primaries.begin(), primaries.end());
			if (useGhostTrack)
				fittedSVs = vertexRecoGT->vertices(
						pv, *beamSpot, primaries_,
						*ghostTrack);
			else
				fittedSVs = vertexReco->vertices(
						primaries_, fitTracks,
						*beamSpot);
		    }	break;
		}

		// build combined SV information and filter

		SVBuilder svBuilder(pv, jetDir, withPVError);
		std::remove_copy_if(boost::make_transform_iterator(
		                    	fittedSVs.begin(), svBuilder),
		                    boost::make_transform_iterator(
		                    	fittedSVs.end(), svBuilder),
		                    std::back_inserter(SVs),
		                    SVFilter(vertexFilter, pv, jetDir));

		// clean up now unneeded collections
             }else{
		    if( !useSVClustering ) {
		
		      for(size_t iExtSv = 0; iExtSv < extSecVertex->size(); iExtSv++){
			 const reco::Vertex & extVertex = (*extSecVertex)[iExtSv];
  //    	              GlobalVector vtxDir = GlobalVector(extVertex.p4().X(),extVertex.p4().Y(),extVertex.p4().Z());
  //                     if(Geom::deltaR(extVertex.position() - pv.position(), vtxDir)>0.2) continue; //pointing angle
  //		      std::cout << " dR " << iExtSv << " " << Geom::deltaR( ( extVertex.position() - pv.position() ), jetDir ) << "eta: " << ( extVertex.position() - pv.position()).eta() << " vs " << jetDir.eta() << " phi: "  << ( extVertex.position() - pv.position()).phi() << " vs  " << jetDir.phi() <<  std::endl; 
			  if( reco::deltaR( ( extVertex.position() - pv.position() ), jetDir ) >  extSVDeltaRToJet || extVertex.p4().M() < 0.3)
			  continue;
  //		      std::cout << " SV added " << iExtSv << std::endl; 
			 extAssoCollection.push_back( extVertex );
		      }
		
		    }
                  else {
		
		      size_t jetIdx = ( iterJets - trackIPTagInfos->begin() );
		
		      for(size_t iExtSv = 0; iExtSv < clusteredSVs.at(jetIdx).size(); iExtSv++){
			 const reco::Vertex & extVertex = (*extSecVertex)[ clusteredSVs.at(jetIdx).at(iExtSv) ];
			
			 extAssoCollection.push_back( extVertex );
		      }
		    }
		
                   SVBuilder svBuilder(pv, jetDir, withPVError);
	           std::remove_copy_if(boost::make_transform_iterator( extAssoCollection.begin(), svBuilder),
                                boost::make_transform_iterator(extAssoCollection.end(), svBuilder),
                                std::back_inserter(SVs),
                                SVFilter(vertexFilter, pv, jetDir));


                 }
//		std::cout << "size: " << SVs.size() << std::endl; 
		gtPred.reset();
		ghostTrack.reset();
		gtStates.clear();
		fitTracks.clear();
		fittedSVs.clear();
		extAssoCollection.clear();

		// sort SVs by importance

		std::vector<unsigned int> vtxIndices = vertexSorting(SVs);

		std::vector<SecondaryVertexTagInfo::VertexData> svData;

		svData.resize(vtxIndices.size());
		for(unsigned int idx = 0; idx < vtxIndices.size(); idx++) {
			const SecondaryVertex &sv = SVs[vtxIndices[idx]];

			svData[idx].vertex = sv;
			svData[idx].dist2d = sv.dist2d();
			svData[idx].dist3d = sv.dist3d();
			svData[idx].direction =
				GlobalVector(sv.x() - pv.x(),
				             sv.y() - pv.y(),
				             sv.z() - pv.z());

			// mark tracks successfully used in vertex fit

			for(Vertex::trackRef_iterator iter = sv.tracks_begin();
			    iter != sv.tracks_end(); iter++) {
				if (sv.trackWeight(*iter) < minTrackWeight)
					continue;

				TrackRefVector::const_iterator pos =
					std::find(trackRefs.begin(), trackRefs.end(),
					          iter->castTo<TrackRef>());

				if (pos == trackRefs.end() ) {
				   if(!useExternalSV)
					throw cms::Exception("TrackNotFound")
						<< "Could not find track from secondary "
						   "vertex in original tracks."
						<< std::endl;
				} else {
				unsigned int index = pos - trackRefs.begin();
				trackData[index].second.svStatus =
					(SecondaryVertexTagInfo::TrackData::Status)
					((unsigned int)SecondaryVertexTagInfo::TrackData::trackAssociatedToVertex + idx);
 				}
			}
		}

		// fill result into tag infos

		tagInfos->push_back(
			SecondaryVertexTagInfo(
				trackData, svData, SVs.size(),
				TrackIPTagInfoRef(trackIPTagInfos,
					iterJets - trackIPTagInfos->begin())));
	}

	event.put(tagInfos);
}

// ------------ method that matches reclustered and original jets based on minimum dR ------------
void
SecondaryVertexProducer::matchReclusteredJets(const edm::Handle<TrackIPTagInfoCollection>& jets,
                                              const std::vector<fastjet::PseudoJet>& reclusteredJets,
                                              std::vector<int>& matchedIndices)
{
   std::vector<bool> matchedLocks(reclusteredJets.size(),false);

   for(size_t j=0; j<jets->size(); ++j)
   {
     double matchedDR2 = 1e9;
     int matchedIdx = -1;

     for(size_t rj=0; rj<reclusteredJets.size(); ++rj)
     {
       if( matchedLocks.at(rj) ) continue; // skip jets that have already been matched

       double tempDR2 = reco::deltaR2( jets->at(j).jet()->rapidity(), jets->at(j).jet()->phi(), reclusteredJets.at(rj).rapidity(), reclusteredJets.at(rj).phi_std() );
       if( tempDR2 < matchedDR2 )
       {
         matchedDR2 = tempDR2;
         matchedIdx = rj;
       }
     }

     if( matchedIdx>=0 )
     {
       if ( matchedDR2 > rParam*rParam )
       {
         edm::LogError("JetMatchingFailed") << "Matched reclustered jet " << matchedIdx << " and original jet " << j <<" are separated by dR=" << sqrt(matchedDR2) << " which is greater than the jet size R=" << rParam << ".\n"
                                            << "This is not expected so please check that the jet algorithm and jet size match those used for the original jet collection.";
       }
       else
         matchedLocks.at(matchedIdx) = true;
     }
     else
       edm::LogError("JetMatchingFailed") << "Matching reclustered to original jets failed. Please check that the jet algorithm and jet size match those used for the original jet collection.";

     matchedIndices.push_back(matchedIdx);
   }
}

// ------------ method that matches reclustered and original fat jets based on minimum dR ------------
void
SecondaryVertexProducer::matchReclusteredFatJets(const edm::Handle<edm::View<reco::Jet> >& jets,
                                                 const std::vector<fastjet::PseudoJet>& reclusteredJets,
                                                 std::vector<int>& matchedIndices)
{
   std::vector<bool> matchedLocks(reclusteredJets.size(),false);

   for(size_t j=0; j<jets->size(); ++j)
   {
     double matchedDR2 = 1e9;
     int matchedIdx = -1;

     for(size_t rj=0; rj<reclusteredJets.size(); ++rj)
     {
       if( matchedLocks.at(rj) ) continue; // skip jets that have already been matched

       double tempDR2 = reco::deltaR2( jets->at(j).rapidity(), jets->at(j).phi(), reclusteredJets.at(rj).rapidity(), reclusteredJets.at(rj).phi_std() );
       if( tempDR2 < matchedDR2 )
       {
         matchedDR2 = tempDR2;
         matchedIdx = rj;
       }
     }

     if( matchedIdx>=0 )
     {
       if ( matchedDR2 > rParam*rParam )
       {
         edm::LogError("JetMatchingFailed") << "Matched reclustered jet " << matchedIdx << " and original jet " << j <<" are separated by dR=" << sqrt(matchedDR2) << " which is greater than the jet size R=" << rParam << ".\n"
                                            << "This is not expected so please check that the jet algorithm and jet size match those used for the original jet collection.";
       }
       else
         matchedLocks.at(matchedIdx) = true;
     }
     else
       edm::LogError("JetMatchingFailed") << "Matching reclustered to original fat jets failed. Please check that the jet algorithm and jet size match those used for the original fat jet collection.";

     matchedIndices.push_back(matchedIdx);
   }
}

// ------------ method that matches groomed and original jets based on minimum dR ------------
void
SecondaryVertexProducer::matchGroomedJets(const edm::Handle<edm::View<reco::Jet> >& jets,
                                          const edm::Handle<edm::View<reco::Jet> >& groomedJets,
                                          std::vector<int>& matchedIndices)
{
   std::vector<bool> jetLocks(jets->size(),false);
   std::vector<int>  jetIndices;

   for(size_t gj=0; gj<groomedJets->size(); ++gj)
   {
     double matchedDR2 = 1e9;
     int matchedIdx = -1;

     if( groomedJets->at(gj).pt()>0. ) // skips pathological cases of groomed jets with Pt=0
     {
       for(size_t j=0; j<jets->size(); ++j)
       {
         if( jetLocks.at(j) ) continue; // skip jets that have already been matched

         double tempDR2 = reco::deltaR2( jets->at(j).rapidity(), jets->at(j).phi(), groomedJets->at(gj).rapidity(), groomedJets->at(gj).phi() );
         if( tempDR2 < matchedDR2 )
         {
           matchedDR2 = tempDR2;
           matchedIdx = j;
         }
       }
     }

     if( matchedIdx>=0 )
     {
       if ( matchedDR2 > rParam*rParam )
       {
         edm::LogWarning("MatchedJetsFarApart") << "Matched groomed jet " << gj << " and original jet " << matchedIdx <<" are separated by dR=" << sqrt(matchedDR2) << " which is greater than the jet size R=" << rParam << ".\n"
                                                << "This is not expected so the matching of these two jets has been discarded. Please check that the two jet collections belong to each other.";
         matchedIdx = -1;
       }
       else
         jetLocks.at(matchedIdx) = true;
     }
     jetIndices.push_back(matchedIdx);
   }

   for(size_t j=0; j<jets->size(); ++j)
   {
     std::vector<int>::iterator matchedIndex = std::find( jetIndices.begin(), jetIndices.end(), j );

     matchedIndices.push_back( matchedIndex != jetIndices.end() ? std::distance(jetIndices.begin(),matchedIndex) : -1 );
   }
}

// ------------ method that matches subjets and original fat jets ------------
void
SecondaryVertexProducer::matchSubjets(const std::vector<int>& groomedIndices,
                                      const edm::Handle<edm::View<reco::Jet> >& groomedJets,
                                      const edm::Handle<TrackIPTagInfoCollection>& subjets,
                                      std::vector<std::vector<int> >& matchedIndices)
{
   for(size_t g=0; g<groomedIndices.size(); ++g)
   {
     std::vector<int> subjetIndices;

     if( groomedIndices.at(g)>=0 )
     {
       for(size_t s=0; s<groomedJets->at(groomedIndices.at(g)).numberOfDaughters(); ++s)
       {
         const edm::Ptr<reco::Candidate> & subjet = groomedJets->at(groomedIndices.at(g)).daughterPtr(s);

         for(size_t sj=0; sj<subjets->size(); ++sj)
         {
	    const edm::RefToBase<reco::Jet> &subjetRef = subjets->at(sj).jet();
           if( subjet == edm::Ptr<reco::Candidate>( subjetRef.id(), subjetRef.get(), subjetRef.key() ) )
           {
             subjetIndices.push_back(sj);
             break;
           }
         }
       }

       if( subjetIndices.size() == 0 )
         edm::LogError("SubjetMatchingFailed") << "Matching subjets to original fat jets failed. Please check that the groomed fat jet and subjet collections belong to each other.";

       matchedIndices.push_back(subjetIndices);
     }
     else
       matchedIndices.push_back(subjetIndices);
   }
}

//define this as a plug-in
DEFINE_FWK_MODULE(SecondaryVertexProducer);
