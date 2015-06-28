import FWCore.ParameterSet.Config as cms

candidateCombinedTMVAComputer = cms.ESProducer("CombinedTMVAJetTagESProducer",
	jetTagComputers = cms.vstring(
		'candidateJetProbabilityComputer',
		'candidateJetBProbabilityComputer',
		'candidateCombinedSecondaryVertexV2Computer',
		'softPFMuonComputer',
		'softPFElectronComputer'
	),
	#weightFile = cms.FileInPath('RecoBTau/JetTagComputer/data/XYZ.weights.xml.gz') # FIXME
)
