# The following comments couldn't be translated into the new config version:

#! /bin/env cmsRun

import FWCore.ParameterSet.Config as cms

process = cms.Process("rereco2")

#keep the logging output to a nice level
process.load("FWCore.MessageLogger.MessageLogger_cfi")

#process.options = cms.untracked.PSet(multiProcesses=cms.untracked.PSet(
#        maxChildProcesses=cms.untracked.int32(15),
#        maxSequentialEventsPerChild=cms.untracked.uint32(10)))



# load the full reconstraction configuration, to make sure we're getting all needed dependencies
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

#parallel processing

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.source = cms.Source("PoolSource",
#     skipEvents = cms.untracked.uint32(281),	
    fileNames = cms.untracked.vstring(),
    secondaryFileNames = cms.untracked.vstring()
)

process.GlobalTag.globaltag = 'PLS170_V7AN1::All'

#process.reco = cms.Sequence(process.siPixelRecHits+process.siStripMatchedRecHits+process.ckftracks_wodEdX+process.offlinePrimaryVertices+process.ak5JetTracksAssociatorAtVertex*process.btagging)

process.siPixelClusters = cms.EDProducer("JetCoreClusterSplitter",
    pixelClusters         = cms.InputTag("siPixelClusters","","RECO"),
    vertices              = cms.InputTag('offlinePrimaryVertices',"","RECO"),
    pixelCPE = cms.string( "PixelCPEGeneric" ),
    verbose     = cms.bool(True),

    )

process.IdealsiPixelClusters = cms.EDProducer(
    "TrackClusterSplitter",
    stripClusters         = cms.InputTag("siStripClusters","","RECO"),
    pixelClusters         = cms.InputTag("siPixelClusters","","RECO"),
    useTrajectories       = cms.bool(False),
    trajTrackAssociations = cms.InputTag('generalTracks'),
    tracks                = cms.InputTag('pixelTracks'),
    propagator            = cms.string('AnalyticalPropagator'),
    vertices              = cms.InputTag('pixelVertices'),
    simSplitPixel         = cms.bool(True), # ideal pixel splitting turned OFF
    simSplitStrip         = cms.bool(False), # ideal strip splitting turned OFF
    tmpSplitPixel         = cms.bool(False), # template pixel spliting
    tmpSplitStrip         = cms.bool(False), # template strip splitting
    useStraightTracks     = cms.bool(True),
    test     = cms.bool(True)
    )

#this is needed if we run off RECO, comment it if you run RECO in this job
process.caloTowerForTrk = process.calotowermaker.clone(hbheInput=cms.InputTag('hbhereco'))

process.p = cms.Path(process.IdealsiPixelClusters*process.siPixelClusters*process.MeasurementTrackerEvent+process.siPixelRecHits+process.siStripMatchedRecHits+process.ckftracks_wodEdX+process.offlinePrimaryVertices+process.ak5JetTracksAssociatorAtVertexPF*process.btagging)
# process.reconstruction_fromRECO)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('trk.root'),
)
process.endpath= cms.EndPath(process.out)

process.PoolSource.secondaryFileNames =[
"root://xrootd-redic.pi.infn.it:1194//store/relval/CMSSW_7_0_6/RelValQCD_Pt_600_800_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PLS170_V7AN1-v1/00000/B2C90403-1FFA-E311-9058-00261894398A.root",
"root://xrootd-redic.pi.infn.it:1194//store/relval/CMSSW_7_0_6/RelValQCD_Pt_600_800_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PLS170_V7AN1-v1/00000/EE8BE35D-1EFA-E311-AD6F-0025905A6064.root",
"root://xrootd-redic.pi.infn.it:1194//store/relval/CMSSW_7_0_6/RelValQCD_Pt_600_800_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PLS170_V7AN1-v1/00000/980E34FE-1EFA-E311-B316-002618943836.root",
"root://xrootd-redic.pi.infn.it:1194//store/relval/CMSSW_7_0_6/RelValQCD_Pt_600_800_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PLS170_V7AN1-v1/00000/A8938841-1FFA-E311-9F64-002618943908.root",
"root://xrootd-redic.pi.infn.it:1194//store/relval/CMSSW_7_0_6/RelValQCD_Pt_600_800_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PLS170_V7AN1-v1/00000/3C78E1C1-1DFA-E311-BEB5-0025905A6068.root",
"root://xrootd-redic.pi.infn.it:1194//store/relval/CMSSW_7_0_6/RelValQCD_Pt_600_800_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PLS170_V7AN1-v1/00000/861F988B-1FFA-E311-B7CC-002590596490.root"
]

process.PoolSource.fileNames = [
"root://xrootd-redic.pi.infn.it:1194//store/relval/CMSSW_7_0_6/RelValQCD_Pt_600_800_13/GEN-SIM-RECO/PLS170_V7AN1-v1/00000/BAE49338-32FA-E311-9609-00261894394B.root"
]

