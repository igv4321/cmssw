#include "RecoLocalCalo/HcalRecAlgos/interface/parseHBHEPhase1AlgoDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// Phase 1 HBHE reco algorithm headers
#include "RecoLocalCalo/HcalRecAlgos/interface/SimpleHBHEPhase1Algo.h"

std::unique_ptr<AbsHBHEPhase1Algo>
parseHBHEPhase1AlgoDescription(const edm::ParameterSet& ps)
{
    std::unique_ptr<AbsHBHEPhase1Algo> algo;

    const std::string& className = ps.getParameter<std::string>("Class");

    if (className == "SimpleHBHEPhase1Algo")
    {
        algo = std::unique_ptr<AbsHBHEPhase1Algo>(
            new SimpleHBHEPhase1Algo(ps.getParameter<int>("firstSampleShift"),
                                     ps.getParameter<int>("samplesToAdd"),
                                     ps.getParameter<double>("phaseNS")
            ));
    }

    return algo;
}
