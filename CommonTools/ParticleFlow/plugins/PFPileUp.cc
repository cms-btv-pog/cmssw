#include "CommonTools/ParticleFlow/plugins/PFPileUp.h"

#include "DataFormats/ParticleFlowCandidate/interface/PileUpPFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PileUpPFCandidateFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "FWCore/Framework/interface/ESHandle.h"

// #include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"


using namespace std;
using namespace edm;
using namespace reco;

PFPileUp::PFPileUp(const edm::ParameterSet& iConfig) {
  
  inputTagPFCandidates_ 
    = iConfig.getParameter<InputTag>("PFCandidates");

  inputTagVertices_ 
    = iConfig.getParameter<InputTag>("Vertices");

  enable_ = iConfig.getParameter<bool>("Enable");

  verbose_ = 
    iConfig.getUntrackedParameter<bool>("verbose",false);


  if ( iConfig.exists("checkClosestZVertex") ) {
    checkClosestZVertex_ = iConfig.getParameter<bool>("checkClosestZVertex");
  } else {
    checkClosestZVertex_ = false;
  }

  if ( iConfig.exists("Jets") )
  {
    useJets_ = true;

    inputTagJets_
      = iConfig.getParameter<InputTag>("Jets");

    minJetPt_
      = iConfig.getParameter<double>("minJetPt");

    maxJetDeltaR_
      = iConfig.getParameter<double>("maxJetDeltaR");

    maxDistanceToJetAxis_
      = iConfig.getParameter<double>("maxDistanceToJetAxis");
  }
  else
  {
    useJets_ = false;
    inputTagJets_ = InputTag();
    minJetPt_ = 0.;
    maxJetDeltaR_ = 999.;
    maxDistanceToJetAxis_ = 999.;
  }
  // Configure the algo
  pileUpAlgo_.setVerbose(verbose_);
  pileUpAlgo_.setCheckClosestZVertex(checkClosestZVertex_);
  pileUpAlgo_.setUseJets(useJets_);
  pileUpAlgo_.setMinJetPt(minJetPt_);
  pileUpAlgo_.setMaxJetDeltaR(maxJetDeltaR_);
  pileUpAlgo_.setMaxDistanceToJetAxis(maxDistanceToJetAxis_);

  produces<reco::PFCandidateCollection>();
  
}



PFPileUp::~PFPileUp() { }



void PFPileUp::beginJob() { }


void PFPileUp::produce(Event& iEvent, 
			  const EventSetup& iSetup) {
  
//   LogDebug("PFPileUp")<<"START event: "<<iEvent.id().event()
// 			 <<" in run "<<iEvent.id().run()<<endl;
  
   
  // get PFCandidates

  auto_ptr< reco::PFCandidateCollection > 
    pOutput( new reco::PFCandidateCollection ); 
  
  if(enable_) {

    Handle<PFCandidateCollection> pfCandidates;
    iEvent.getByLabel( inputTagPFCandidates_, pfCandidates);

  
    // get vertices 

    Handle<VertexCollection> vertices;
    iEvent.getByLabel( inputTagVertices_, vertices);
    
    // get jets
    Handle<edm::View<reco::Candidate> > jets;
    if( useJets_ )
      iEvent.getByLabel( inputTagJets_, jets);

    // get TransientTrackBuilder
    ESHandle<TransientTrackBuilder> builder;
    if( useJets_ )
      iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);


    pileUpAlgo_.process(*pfCandidates,*vertices,*jets,*builder,&pfCandidates);
    
    pOutput->insert(pOutput->end(),pileUpAlgo_.getPFCandidatesFromPU().begin(),pileUpAlgo_.getPFCandidatesFromPU().end());
  }  
  // outsize of the loop to fill the collection anyway even when disabled
  iEvent.put( pOutput );
}

