#ifndef CommonTools_PFCandProducer_PFPileUpAlgo_
#define CommonTools_PFCandProducer_PFPileUpAlgo_

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

class PFPileUpAlgo {
 public:
  PFPileUpAlgo():checkClosestZVertex_(true), verbose_(false),
                 useJets_(false), minJetPt_(0.),
                 maxJetDeltaR_(999.), maxDistanceToJetAxis_(999.),
                 useMuons_(false), minMuonPt_(0.),
                 maxMuonDeltaR_(999.), maxDistanceToMuon_(999.) {;}
    
  PFPileUpAlgo( bool checkClosestZVertex, bool verbose=false,
                bool useJets=false, double minJetPt=0.,
                double maxJetDeltaR=999., double maxDistanceToJetAxis=999.,
                bool useMuons=false, double minMuonPt=0.,
                double maxMuonDeltaR=999., double maxDistanceToMuon=999. ):
    checkClosestZVertex_(checkClosestZVertex), verbose_(verbose),
    useJets_(useJets), minJetPt_(minJetPt), maxJetDeltaR_(maxJetDeltaR),
    maxDistanceToJetAxis_(maxDistanceToJetAxis),
    useMuons_(useMuons), minMuonPt_(minMuonPt), maxMuonDeltaR_(maxMuonDeltaR),
    maxDistanceToMuon_(maxDistanceToMuon) {;}

  ~PFPileUpAlgo(){;}

  // the last parameter is needed if you want to use the sourceCandidatePtr
  void process(const reco::PFCandidateCollection & pfCandidates, 
	       const reco::VertexCollection & vertices, 
              const edm::View<reco::Candidate> & jets,
              const TransientTrackBuilder & builder,
	       const edm::Handle<reco::PFCandidateCollection> * handle=0)  ;

  inline void setVerbose(bool verbose) { verbose_ = verbose; }

  inline void setCheckClosestZVertex(bool val) { checkClosestZVertex_ = val;}

  inline void setUseJets(bool val) { useJets_ = val;}

  inline void setMinJetPt(double val) { minJetPt_ = val;}

  inline void setMaxJetDeltaR(double val) { maxJetDeltaR_ = val;}

  inline void setMaxDistanceToJetAxis(double val) { maxDistanceToJetAxis_ = val;}

  inline void setUseMuons(bool val) { useMuons_ = val;}

  inline void setMinMuonPt(double val) { minMuonPt_ = val;}

  inline void setMaxMuonDeltaR(double val) { maxMuonDeltaR_ = val;}

  inline void setMaxDistanceToMuon(double val) { maxDistanceToMuon_ = val;}

  const reco::PFCandidateCollection & getPFCandidatesFromPU() const {return pfCandidatesFromPU_;}
  
  const reco::PFCandidateCollection & getPFCandidatesFromVtx() const {return pfCandidatesFromVtx_;}

  int chargedHadronVertex(const reco::VertexCollection& vertices, 
			const reco::PFCandidate& pfcand,
                     const edm::View<reco::Candidate> & jets,
                     const TransientTrackBuilder & builder) const;

  int chargedHadronMuon(const int ivertex, 
                     const reco::PFCandidate& pfcand,
                     const reco::PFCandidateCollection & pfmuons,
                     const TransientTrackBuilder & builder) const;


 private  :

  /// use the closest z vertex if a track is not in a vertex
  bool   checkClosestZVertex_;
  
  
  /// verbose ?
  bool   verbose_;

  /// use jets ?
  bool  useJets_;

  /// minimum jet Pt
  double  minJetPt_;

  /// maximum dR to the jet axis
  double  maxJetDeltaR_;

  /// maximum distance to the jet axis
  double  maxDistanceToJetAxis_;

  /// use muons ?
  bool  useMuons_;

  /// minimum muon Pt
  double  minMuonPt_;

  /// maximum dR to the muon
  double  maxMuonDeltaR_;

  /// maximum distance to the muon track
  double  maxDistanceToMuon_;
  
  reco::PFCandidateCollection pfCandidatesFromVtx_;
  reco::PFCandidateCollection pfCandidatesFromPU_;
  
};

#endif
