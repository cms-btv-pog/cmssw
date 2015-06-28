import FWCore.ParameterSet.Config as cms

pfCombinedTMVABJetTags = cms.EDProducer("JetTagProducer",
	jetTagComputer = cms.string('candidateCombinedTMVAComputer'),
	tagInfos = cms.VInputTag(
		cms.InputTag("pfImpactParameterTagInfos"),
		cms.InputTag("pfSecondaryVertexTagInfos"),
		cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfos"),
		cms.InputTag("softPFMuonsTagInfos"),
		cms.InputTag("softPFElectronsTagInfos")
	)
)
