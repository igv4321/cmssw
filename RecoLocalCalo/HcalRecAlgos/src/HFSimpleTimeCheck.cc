#include <cstring>
#include "RecoLocalCalo/HcalRecAlgos/interface/HFSimpleTimeCheck.h"

HFSimpleTimeCheck::HFSimpleTimeCheck(const std::pair<float,float> tlimits[2],
                                     const float energyWeights[N_POSSIBLE_STATES][N_POSSIBLE_STATES][2])
{
    tlimits_[0] = tlimits[0];
    tlimits_[1] = tlimits[1];
    float* to = &energyWeights_[0][0][0];
    const float* from = &energyWeights[0][0][0];
    memcpy(to, from, sizeof(energyWeights_));
}

HFRecHit HFSimpleTimeCheck::reconstruct(const HFPreRecHit& prehit,
                                        const HcalCalibrations& /* calibs */)
{
    HFRecHit rh;

    // Determine the state of each anode
    unsigned states[2] = {NOT_PRESENT, NOT_PRESENT};
    for (unsigned ianode=0; ianode<2; ++ianode)
    {
        const HFQIE10Info* anodeInfo = prehit.getHFQIE10Info(ianode);
        if (anodeInfo)
        {
            const float trise = anodeInfo->timeRising();
            if (tlimits_[ianode].first <= trise && trise <= tlimits_[ianode].second)
                states[ianode] = PASSES_TIMING_CUT;
            else
                states[ianode] = FAILS_TIMING_CUT;
        }
    }

    // Reconstruct energy and time
    if (states[0] == PASSES_TIMING_CUT || states[1] == PASSES_TIMING_CUT)
    {
        const float* weights = &energyWeights_[states[0]][states[1]][0];
        float energy = 0.f, t = 0.f, tfall = 0.f, timeWeightSum = 0.f;

        for (unsigned ianode=0; ianode<2; ++ianode)
        {
            const HFQIE10Info* anodeInfo = prehit.getHFQIE10Info(ianode);
            if (anodeInfo)
            {
                const float weightedEnergy = weights[ianode]*anodeInfo->energy();
                energy += weightedEnergy;
                if (weightedEnergy > 0.f)
                {
                    timeWeightSum += weightedEnergy;
                    t += anodeInfo->timeRising()*weightedEnergy;
                    tfall += anodeInfo->timeFalling()*weightedEnergy;
                }
            }
        }
        if (timeWeightSum > 0.f)
        {
            t /= timeWeightSum;
            tfall /= timeWeightSum;
        }

        rh = HFRecHit(prehit.id(), energy, t, tfall);
    }

    return rh;
}
