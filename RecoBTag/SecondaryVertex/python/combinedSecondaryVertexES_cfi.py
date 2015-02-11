import FWCore.ParameterSet.Config as cms

from RecoBTag.SecondaryVertex.combinedSecondaryVertexCommon_cfi import *

combinedSecondaryVertex = cms.ESProducer("CombinedSecondaryVertexESProducer",
	combinedSecondaryVertexCommon,
	useCategories = cms.bool(True),
	calibrationRecords = cms.vstring(
		'CombinedSVRecoVertex', 
		'CombinedSVPseudoVertex', 
		'CombinedSVNoVertex'),
	categoryVariableName = cms.string('vertexCategory')
)

combinedSecondaryVertexV1 = cms.ESProducer("CombinedSecondaryVertexESProducer",
	combinedSecondaryVertexCommon,
	useCategories = cms.bool(True),
	calibrationRecords = cms.vstring(
		'CombinedSVRetrainRecoVertex', 
		'CombinedSVRetrainPseudoVertex', 
		'CombinedSVRetrainNoVertex'),
	categoryVariableName = cms.string('vertexCategory')
)

combinedSecondaryVertexV2 = cms.ESProducer("CombinedSecondaryVertexESProducerV2",
	combinedSecondaryVertexCommon,
	useCategories = cms.bool(True),
	calibrationRecords = cms.vstring(
		'CombinedSVIVFV2RecoVertex', 
		'CombinedSVIVFV2PseudoVertex', 
		'CombinedSVIVFV2NoVertex'),
	categoryVariableName = cms.string('vertexCategory')
)
combinedSecondaryVertexV2.trackSelection.qualityClass = cms.string('any')
combinedSecondaryVertexV2.trackPseudoSelection.qualityClass = cms.string('any')
combinedSecondaryVertexV2.trackMultiplicityMin = cms.uint32(2)
