#ifndef RecoBTau_JetTagComputer_CombinedTMVAJetTagComputer_h
#define RecoBTau_JetTagComputer_CombinedTMVAJetTagComputer_h

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CommonTools/Utils/interface/TMVAEvaluator.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"

class CombinedTMVAJetTagComputer : public JetTagComputer {
    public:
        CombinedTMVAJetTagComputer(const edm::ParameterSet &parameters);
        virtual ~CombinedTMVAJetTagComputer();

        virtual void initialize(const JetTagComputerRecord & record) override;

        float discriminator(const TagInfoHelper &info) const override;

    private:
        std::vector<const JetTagComputer*> computers;

        const std::vector<std::string> inputComputerNames;
        mutable std::mutex m_mutex;
        [[cms::thread_guard("m_mutex")]] std::unique_ptr<TMVAEvaluator> mvaID;
};

#endif // RecoBTau_JetTagComputer_CombinedTMVAJetTagComputer_h
