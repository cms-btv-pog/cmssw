import FWCore.ParameterSet.Config as cms

from RecoBTag.SecondaryVertex.combinedSecondaryVertexCommon_cff import *

candidateCombinedSecondaryVertexSoftLeptonComputer = cms.ESProducer("CandidateCombinedSecondaryVertexSoftLeptonESProducer",
	combinedSecondaryVertexCommon,
	useCategories = cms.bool(True),
	calibrationRecords = cms.vstring(
		'CombinedSVRecoVertexNoSoftLepton', 
		'CombinedSVPseudoVertexNoSoftLepton', 
		'CombinedSVNoVertexNoSoftLepton',
		'CombinedSVRecoVertexSoftMuon', 
		'CombinedSVPseudoVertexSoftMuon', 
		'CombinedSVNoVertexSoftMuon',
		'CombinedSVRecoVertexSoftElectron', 
		'CombinedSVPseudoVertexSoftElectron', 
		'CombinedSVNoVertexSoftElectron'),
        recordLabel = cms.string(''),
	categoryVariableName = cms.string('vertexLeptonCategory'),
	SoftLeptonFlip = cms.bool(False),
  leptonsSelection = cms.PSet(
      sip2dSigMax = cms.double(99999.9),
      sip2dSigMin = cms.double(-99999.9),
      sip3dSigMax = cms.double(99999.9),
      sip3dSigMin = cms.double(-99999.9),
      )
)

