#include "FWCore/Framework/interface/ModuleFactory.h"

#include "RecoBTau/JetTagComputer/interface/JetTagComputerESProducer.h"
#include "RecoBTau/JetTagComputer/interface/CombinedTMVAJetTagComputer.h"

typedef JetTagComputerESProducer<CombinedTMVAJetTagComputer> CombinedTMVAJetTagESProducer;
DEFINE_FWK_EVENTSETUP_MODULE(CombinedTMVAJetTagESProducer);
