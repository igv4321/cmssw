#ifndef RecoLocalCalo_HcalRecAlgos_HFSimpleTimeCheck_h_
#define RecoLocalCalo_HcalRecAlgos_HFSimpleTimeCheck_h_

#include <utility>
#include "RecoLocalCalo/HcalRecAlgos/interface/AbsHFPhase1Algo.h"

class HFSimpleTimeCheck : public AbsHFPhase1Algo
{
public:
    // Possible states of each anode
    enum {
        NOT_PRESENT = 0,
        FAILS_TIMING_CUT,
        PASSES_TIMING_CUT,
        N_POSSIBLE_STATES
    };

    // "tlimits" are the rise time limits for the anode pair.
    // The first element of the pair is the min rise time and the
    // second element is the max rise time. tlimits[0] is for the
    // first anode and tlimits[1] is for the second one.
    //
    // "energyWeights" is the lookup table for the energy weights
    // based on the trinary decision about timing. The first array
    // index is the state of the first anode and the second index
    // is the state of the second. Each anode can be in three different
    // states: not present in the data, fails the timing cut, passes
    // the timing cut. Naturally, if the anode is not present in the
    // data, its weight is ignored. The configurations in which none
    // of the anodes pass the timing cut are ignored as well.
    //
    HFSimpleTimeCheck(const std::pair<float,float> tlimits[2],
                      const float energyWeights[N_POSSIBLE_STATES][N_POSSIBLE_STATES][2]);

    inline virtual ~HFSimpleTimeCheck() {}

    inline virtual bool isConfigurable() const override {return false;}

    virtual HFRecHit reconstruct(const HFPreRecHit& prehit,
                                 const HcalCalibrations& calibs) override;
private:
    std::pair<float,float> tlimits_[2];
    float energyWeights_[N_POSSIBLE_STATES][N_POSSIBLE_STATES][2];
};

#endif // RecoLocalCalo_HcalRecAlgos_HFSimpleTimeCheck_h_
