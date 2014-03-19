#include "CommonTools/ParticleFlow/interface/PFPileUpAlgo.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

void PFPileUpAlgo::process(const reco::PFCandidateCollection & pfCandidates, 
			   const reco::VertexCollection & vertices, 
                        const edm::View<reco::Candidate> & jets,
                        const TransientTrackBuilder & builder,
			   const edm::Handle<reco::PFCandidateCollection> * handle)  {

  pfCandidatesFromVtx_.clear();
  pfCandidatesFromPU_.clear();

  for( unsigned i=0; i<pfCandidates.size(); i++ ) {
    
    const reco::PFCandidate& cand = pfCandidates[i];
    
    int ivertex;

    switch( cand.particleId() ) {
    case reco::PFCandidate::h:
      ivertex = chargedHadronVertex( vertices, cand, jets, builder );
      break;
    default:
      continue;
    } 
    
    // no associated vertex, or primary vertex
    // not pile-up
    if( ivertex == -1  || 
	ivertex == 0 ) {
      pfCandidatesFromVtx_.push_back(cand);
      pfCandidatesFromVtx_.back().setSourceCandidatePtr( reco::PFCandidatePtr(*handle,i) );
    } else {
      // associated to a vertex
      pfCandidatesFromPU_.push_back(cand);
      if(handle) {
	pfCandidatesFromPU_.back().setSourceCandidatePtr( reco::PFCandidatePtr(*handle,i) );
      }
    }
  }
}


int 
PFPileUpAlgo::chargedHadronVertex( const reco::VertexCollection& vertices,
                                   const reco::PFCandidate& pfcand,
                                   const edm::View<reco::Candidate>& jets,
                                   const TransientTrackBuilder& builder) const {

  
  reco::TrackBaseRef trackBaseRef( pfcand.trackRef() );
  
  size_t  iVertex = 0;
  unsigned index=0;
  unsigned nFoundVertex = 0;
  typedef reco::VertexCollection::const_iterator IV;
  typedef reco::Vertex::trackRef_iterator IT;
  float bestweight=0;
  for(IV iv=vertices.begin(); iv!=vertices.end(); ++iv, ++index) {

    const reco::Vertex& vtx = *iv;
    
    // loop on tracks in vertices
    for(IT iTrack=vtx.tracks_begin(); 
	iTrack!=vtx.tracks_end(); ++iTrack) {
	 
      const reco::TrackBaseRef& baseRef = *iTrack;

      // one of the tracks in the vertex is the same as 
      // the track considered in the function
      if(baseRef == trackBaseRef ) {
	float w = vtx.trackWeight(baseRef);
	//select the vertex for which the track has the highest weight
	if (w > bestweight){
	  bestweight=w;
	  iVertex=index;
	  nFoundVertex++;
	}	 	
      }
    }
  }

  // if using jets and no vertex found or vertex is not the primary interaction vertex
  if( useJets_ && ( nFoundVertex==0 || (nFoundVertex>0 && iVertex!=0) ) )
  {
    // first find the closest jet within maxJetDeltaR_
    int jetIdx = -1;
    double minDeltaR = 999.;
    for(edm::View<reco::Candidate>::const_iterator ij=jets.begin(); ij!=jets.end(); ++ij)
    {
      if( ij->pt() < minJetPt_ ) continue; // skip jets below the jet Pt threshold

      double deltaR = reco::deltaR( *ij, *trackBaseRef );
      if( deltaR < maxJetDeltaR_ && deltaR < minDeltaR )
      {
        minDeltaR = deltaR;
        jetIdx = std::distance(jets.begin(), ij);
      }
    }

    // if jet found
    if( jetIdx!=-1 )
    {
      reco::TransientTrack transientTrack = builder.build(*trackBaseRef);
      GlobalVector direction(jets.at(jetIdx).px(), jets.at(jetIdx).py(), jets.at(jetIdx).pz());
      // find the vertex with the smallest distanceToJetAxis that is still within maxDistaneToJetAxis_
      int vtxIdx = -1;
      double minDistanceToJetAxis = 999.;
      for(IV iv=vertices.begin(); iv!=vertices.end(); ++iv)
      {
        double distanceToJetAxis = IPTools::jetTrackDistance(transientTrack, direction, *iv).second.value();
        if( distanceToJetAxis < maxDistanceToJetAxis_ && distanceToJetAxis < minDistanceToJetAxis )
        {
          minDistanceToJetAxis = distanceToJetAxis;
          vtxIdx = std::distance(vertices.begin(), iv);
        }
      }
      // if the vertex with the smallest distanceToJetAxis is the primary interaction vertex, reassign iVertex
      if( vtxIdx==0 )
      {
        iVertex=vtxIdx;
        if( nFoundVertex==0 ) nFoundVertex++;
      }
    }
  }

  if (nFoundVertex>0){
    if (nFoundVertex!=1)
      edm::LogWarning("TrackOnTwoVertex")<<"a track is shared by at least two verteces. Used to be an assert";
    return iVertex;
  }
  // no vertex found with this track. 

  // optional: as a secondary solution, associate the closest vertex in z
  if ( checkClosestZVertex_ ) {

    double dzmin = 10000;
    double ztrack = pfcand.vertex().z();
    bool foundVertex = false;
    index = 0;
    for(IV iv=vertices.begin(); iv!=vertices.end(); ++iv, ++index) {

      double dz = fabs(ztrack - iv->z());
      if(dz<dzmin) {
	dzmin = dz; 
	iVertex = index;
	foundVertex = true;
      }
    }

    if( foundVertex ) 
      return iVertex;  

  }


  return -1 ;
}

