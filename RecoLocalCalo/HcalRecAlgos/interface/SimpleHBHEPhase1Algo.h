#ifndef RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_
#define RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_

// Base class header
#include "RecoLocalCalo/HcalRecAlgos/interface/AbsHBHEPhase1Algo.h"

// Other headers
#include "CalibCalorimetry/HcalAlgos/interface/HcalPulseContainmentManager.h"


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
    //   phaseNS          -- default "phase" parameter for the pulse
    //                       containment correction
    //
    SimpleHBHEPhase1Algo(int firstSampleShift,
                         int samplesToAdd,
                         float phaseNS);

    inline virtual ~SimpleHBHEPhase1Algo() {}

    virtual void beginRun(const edm::EventSetup&) override;
    virtual void endRun() override;

    inline virtual bool isConfigurable() const override {return false;}

    virtual HBHERecHit reconstruct(const HBHEChannelInfo& info,
                                   const HcalRecoParam* params,
                                   const HcalCalibrations& calibs) override;

    // "Method 0" rechit energy. Calls a non-const member of
    // HcalPulseContainmentManager, so no const qualifier here.
    float m0Energy(const HBHEChannelInfo& info,
                   double reconstructedCharge,
                   bool applyContainmentCorrection,
                   double phaseNS);

    // "Method 0" rechit timing (original low-pileup QIE8 algorithm)
    float m0Time(const HBHEChannelInfo& info,
                 double reconstructedCharge,
                 const HcalCalibrations& calibs) const;

    inline float defaultPhaseNS() const {return phaseNS_;}

private:
    HcalPulseContainmentManager pulseCorr_;

    int firstSampleShift_;
    int samplesToAdd_;
    float phaseNS_;
};

#endif // RecoLocalCalo_HcalRecAlgos_SimpleHBHEPhase1Algo_h_
