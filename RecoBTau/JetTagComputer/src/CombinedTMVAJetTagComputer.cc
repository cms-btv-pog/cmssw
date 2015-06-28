#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"
#include "RecoBTau/JetTagComputer/interface/CombinedTMVAJetTagComputer.h"

using namespace reco;

CombinedTMVAJetTagComputer::CombinedTMVAJetTagComputer(const edm::ParameterSet &params) :

  inputComputerNames( params.getParameter<std::vector<std::string> >("jetTagComputers") )

{
  uses(0, "ipTagInfos");
  uses(1, "svAVRTagInfos");
  uses(2, "svIVFTagInfos");
  uses(3, "smTagInfos");
  uses(4, "seTagInfos");

  mvaID.reset(new TMVAEvaluator());

  // variable order needs to be the same as in the training
  //std::vector<std::string> variables({"pfJP", "pfJBP", "pfCSVv2AVR", "pfCSVv2IVF", "SMT", "SET"}); // FIXME
  //std::vector<std::string> spectators(); // FIXME

  //edm::FileInPath weightFile = parameters.getParameter<edm::FileInPath>("weightFile"); // FIXME
  //mvaID->initialize("Color:Silent:Error", "BDTG", weightFile.fullPath(), variables, spectators); // FIXME
}

CombinedTMVAJetTagComputer::~CombinedTMVAJetTagComputer()
{
}

void CombinedTMVAJetTagComputer::initialize(const JetTagComputerRecord & record) {

  for (auto & name : inputComputerNames) {
    edm::ESHandle<JetTagComputer> computerHandle;
    record.get(name, computerHandle);
    const JetTagComputer* comp = computerHandle.product();
    computers.push_back(comp);
  }
}

float CombinedTMVAJetTagComputer::discriminator(const JetTagComputer::TagInfoHelper &info) const
{
  // default discriminator value
  float value = -10.;

  // TMVAEvaluator is not thread safe
  std::lock_guard<std::mutex> lock(m_mutex);

  // TagInfos for JP taggers
  std::vector<const BaseTagInfo*> jpTagInfos({ &info.getBase(0) });

  // TagInfos for the CSVv2AVR tagger
  std::vector<const BaseTagInfo*> avrTagInfos({ &info.getBase(0), &info.getBase(1) });

  // TagInfos for the CSVv2IVF tagger
  std::vector<const BaseTagInfo*> ivfTagInfos({ &info.getBase(0), &info.getBase(2) });

  // TagInfos for the SoftMuon tagger
  std::vector<const BaseTagInfo*> smTagInfos({ &info.getBase(3) });

  // TagInfos for the SoftElectron tagger
  std::vector<const BaseTagInfo*> seTagInfos({ &info.getBase(4) });

  std::map<std::string,float> inputs;
  //["pfJP"]       = (*(computers[0]))( TagInfoHelper(jpTagInfos) ); // FIXME
  //inputs["pfJBP"]      = (*(computers[1]))( TagInfoHelper(jpTagInfos) ); // FIXME
  //inputs["pfCSVv2AVR"] = (*(computers[2]))( TagInfoHelper(avrTagInfos) ); // FIXME
  //inputs["pfCSVv2IVF"] = (*(computers[2]))( TagInfoHelper(ivfTagInfos) ); // FIXME
  //inputs["SMT"]      = (*(computers[3]))( TagInfoHelper(smTagInfos) ); // FIXME
  //inputs["SET"]      = (*(computers[4]))( TagInfoHelper(seTagInfos) ); // FIXME

  // evaluate the MVA
  //value = mvaID->evaluate(inputs); // FIXME

  // return the final discriminator value
  return value;
}
