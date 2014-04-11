import FWCore.ParameterSet.Config as cms


pfPileUp = cms.EDProducer(
    "PFPileUp",
    PFCandidates = cms.InputTag("particleFlowTmp"),
    Vertices = cms.InputTag("offlinePrimaryVertices"),
    # pile-up identification now enabled by default. To be studied for jets
    Enable = cms.bool(True),
    verbose = cms.untracked.bool(False),
    checkClosestZVertex = cms.bool(True),
    useJets = cms.bool(False),
    Jets = cms.InputTag("ak5CaloJets"),
    minJetPt = cms.double(20.),
    maxJetDeltaR = cms.double(0.5),
    maxDistanceToJetAxis = cms.double(0.2),
    useMuons = cms.bool(False),
    minMuonPt = cms.double(5.),
    maxMuonDeltaR = cms.double(0.3),
    maxDistanceToMuon = cms.double(0.2)
    )
