#ifndef RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_
#define RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_

#include "RecoLocalCalo/HcalRecAlgos/interface/AbsHBHEPhase1Algo.h"

class SimpleHBHEPhase1Algo : public AbsHBHEPhase1Algo
{
public:
    inline virtual ~SimpleHBHEPhase1Algo() {}

    inline virtual bool isConfigurable() const override {return false;}

    virtual HBHERecHit reconstruct(const HBHEChannelInfo& info,
                                   const HcalCalibrations& calibs) override;
private:
};

#endif // RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_
