#ifndef RecoBTag_SecondaryVertex_CombinedSVSoftLeptonComputer_h
#define RecoBTag_SecondaryVertex_CombinedSVSoftLeptonComputer_h

#include "DataFormats/BTauReco/interface/SoftLeptonTagInfo.h"

#include "RecoBTag/SecondaryVertex/interface/CombinedSVComputer.h"

//(z)
#include "DataFormats/BTauReco/interface/CandSoftLeptonTagInfo.h"
#include "DataFormats/BTauReco/src/classes.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include <random>

class CombinedSVSoftLeptonComputer : public CombinedSVComputer {
    public:
	explicit CombinedSVSoftLeptonComputer(const edm::ParameterSet &params);
	
	inline double flipSoftLeptonValue(double value, bool doflip) const {return doflip ? -value : value;}
	
	template <class IPTI,class SVTI>
	reco::TaggingVariableList
	operator () (const IPTI &ipInfo, const SVTI &svInfo,
		     const reco::CandSoftLeptonTagInfo &muonInfo,
		     const reco::CandSoftLeptonTagInfo &elecInfo ) const;
	
	private:
	bool					SoftLeptonFlip_;
	double lepSip2dSigMin_, lepSip2dSigMax_;
	double lepSip3dSigMin_, lepSip3dSigMax_;
};


template <class IPTI,class SVTI>
reco::TaggingVariableList CombinedSVSoftLeptonComputer::operator () (const IPTI &ipInfo, const SVTI &svInfo,
								     const reco::CandSoftLeptonTagInfo &muonInfo,
								     const reco::CandSoftLeptonTagInfo &elecInfo) const
{
	using namespace reco;
	
	// call the inherited operator()
	TaggingVariableList vars = CombinedSVComputer::operator()(ipInfo,svInfo);

	//Jets with vtxCategory 99 cause problems
	unsigned int vtxType = ( vars.checkTag(reco::btau::vertexCategory) ? (unsigned int)(vars.get(reco::btau::vertexCategory)) : 99 );
	if (vtxType == 99)
     		return vars;

	//get seed. If no SL are found the seed is always 1, but we do not case since we do not fill anything
	int seed = 1;
	if(muonInfo.leptons() > 0) 
		seed = 1 + round(10000.*muonInfo.properties(0).deltaR);
	else if(elecInfo.leptons() > 0)
		seed = 1 + round(10000.*elecInfo.properties(0).deltaR);
	
	//Draw randomly (thread safe) to choose wether to flip or not the sign
	bool doflip = false;
	if(SoftLeptonFlip_) {
		std::mt19937_64 random;
		random.seed(seed);
		std::uniform_real_distribution<float> dist(0.f,1.f);
		float rndm = dist(random);
		if(rndm<0.5) doflip=true;
	}
	
	// the following is specific to soft leptons
	int leptonCategory = 0; // 0 = no lepton, 1 = muon, 2 = electron
	
	for (unsigned int i = 0; i < muonInfo.leptons(); ++i) // loop over all muons, not optimal -> find the best or use ranking from best to worst
	{
		const SoftLeptonProperties & propertiesMuon = muonInfo.properties(i);
		if(propertiesMuon.sip2dsig < lepSip2dSigMin_ || propertiesMuon.sip2dsig > lepSip2dSigMax_) continue;
		if(propertiesMuon.sip3dsig < lepSip3dSigMin_ || propertiesMuon.sip3dsig > lepSip3dSigMax_) continue;

		leptonCategory = 1; // muon category
		vars.insert(btau::leptonPtRel,propertiesMuon.ptRel , true);
		vars.insert(btau::leptonSip3d,flipSoftLeptonValue(propertiesMuon.sip3d, doflip) , true);
		vars.insert(btau::leptonDeltaR,propertiesMuon.deltaR , true);
		vars.insert(btau::leptonRatioRel,propertiesMuon.ratioRel , true);
		vars.insert(btau::leptonEtaRel,propertiesMuon.etaRel , true);
		vars.insert(btau::leptonRatio,propertiesMuon.ratio , true);
	}
	
	if(leptonCategory != 1) // no soft muon found, try soft electron
	{ 
		for (unsigned int i = 0; i < elecInfo.leptons(); ++i) // loop over all electrons, not optimal -> find the best or use ranking from best to worst
		{
			const SoftLeptonProperties & propertiesElec = elecInfo.properties(i);
			if(propertiesElec.sip2dsig < lepSip2dSigMin_ || propertiesElec.sip2dsig > lepSip2dSigMax_) continue;
			if(propertiesElec.sip3dsig < lepSip3dSigMin_ || propertiesElec.sip3dsig > lepSip3dSigMax_) continue;

			leptonCategory = 2; // electron category
			vars.insert(btau::leptonPtRel,propertiesElec.ptRel , true);
			vars.insert(btau::leptonSip3d,flipSoftLeptonValue(propertiesElec.sip3d, doflip) , true);
			vars.insert(btau::leptonDeltaR,propertiesElec.deltaR , true);
			vars.insert(btau::leptonRatioRel,propertiesElec.ratioRel , true);
			vars.insert(btau::leptonEtaRel,propertiesElec.etaRel , true);
			vars.insert(btau::leptonRatio,propertiesElec.ratio , true);
		}
	}
	

	// set the default value for vertexLeptonCategory to 2 (= NoVertexNoSoftLepton)
	int vertexLepCat = 2; 

	
	if(leptonCategory == 0) // no soft lepton
	{
		if (vtxType == (unsigned int)(btag::Vertices::RecoVertex))
			vertexLepCat = 0;
		else if (vtxType == (unsigned int)(btag::Vertices::PseudoVertex))
			vertexLepCat = 1;
		else
			vertexLepCat = 2;
	} 
	else if(leptonCategory == 1) // soft muon
	{
		if (vtxType == (unsigned int)(btag::Vertices::RecoVertex))
			vertexLepCat = 3;
		else if(vtxType == (unsigned int)(btag::Vertices::PseudoVertex))
			vertexLepCat = 4;
		else 
			vertexLepCat = 5;
	} 
	else if(leptonCategory == 2) // soft electron
	{
		if (vtxType == (unsigned int)(btag::Vertices::RecoVertex))
			vertexLepCat = 6;
		else if (vtxType == (unsigned int)(btag::Vertices::PseudoVertex))
			vertexLepCat = 7;
		else 
			vertexLepCat = 8;
	}
	vars.insert(btau::vertexLeptonCategory, vertexLepCat , true);	
	
	vars.finalize();
	return vars;
}

#endif // RecoBTag_SecondaryVertex_CombinedSVSoftLeptonComputer_h
