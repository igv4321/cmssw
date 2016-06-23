#ifndef RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_
#define RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_

#include "RecoLocalCalo/HcalRecAlgos/interface/AbsHBHEPhase1Algo.h"

class SimpleHBHEPhase1Algo : public AbsHBHEPhase1Algo
{
public:
    // Constructor arguments:
    //
    //   firstSampleShift -- first TS w.r.t. SOI to use for "Method 0"
    //                       reconstruction.
    //
    //   samplesToAdd     -- number of samples to add for "Method 0"
    //                       reconstruction. If, let say, SOI = 4,
    //                       firstSampleShift = -1, and samplesToAdd = 3
    //                       then the code will add time slices 3, 4, and 5.
    //
    SimpleHBHEPhase1Algo(int firstSampleShift,
                         int samplesToAdd);

    inline virtual ~SimpleHBHEPhase1Algo() {}

    inline virtual bool isConfigurable() const override {return false;}

    virtual HBHERecHit reconstruct(const HBHEChannelInfo& info,
                                   const HcalCalibrations& calibs) override;

    // "Method 0" rechit energy
    inline float m0Energy(const HBHEChannelInfo& info) const
    {
        int ibeg = static_cast<int>(info.soi()) + firstSampleShift_;
        if (ibeg < 0)
            ibeg = 0;
        return info.energyInWindow(ibeg, ibeg + samplesToAdd_);
    }

    // "Method 0" rechit timing (original low-pileup QIE8 algorithm)
    float m0Time(const HBHEChannelInfo& info,
                 const HcalCalibrations& calibs) const;

private:
    int firstSampleShift_;
    int samplesToAdd_;
};

#endif // RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_
