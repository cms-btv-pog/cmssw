#include "RecoBTag/SecondaryVertex/interface/CombinedSVSoftLeptonComputer.h"

using namespace reco;
using namespace std;


CombinedSVSoftLeptonComputer::CombinedSVSoftLeptonComputer(const edm::ParameterSet &params) :
	CombinedSVComputer(params),
	SoftLeptonFlip_(params.getParameter<bool>("SoftLeptonFlip"))
{
	edm::ParameterSet leps = params.getParameter<edm::ParameterSet>("leptonsSelection");
	lepSip2dSigMin_ = leps.getParameter<double>("sip2dSigMax");
	lepSip2dSigMax_ = leps.getParameter<double>("sip2dSigMin");
	lepSip3dSigMin_ = leps.getParameter<double>("sip3dSigMax");
	lepSip3dSigMax_ = leps.getParameter<double>("sip3dSigMin");
}
