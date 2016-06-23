#include <algorithm>

#include "CalibCalorimetry/HcalAlgos/interface/HcalTimeSlew.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/SimpleHBHEPhase1Algo.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/timeshift_ns_hbheho.h"

#include "FWCore/Framework/interface/Run.h"


// Maximum fractional error for calculating Method 0
// pulse containment correction
constexpr float PulseContainmentFractionalError = 0.002f;


SimpleHBHEPhase1Algo::SimpleHBHEPhase1Algo(const int firstSampleShift,
                                           const int samplesToAdd,
                                           const float phaseNS)
    : pulseCorr_(PulseContainmentFractionalError),
      firstSampleShift_(firstSampleShift),
      samplesToAdd_(samplesToAdd),
      phaseNS_(phaseNS),
      runnum_(0)
{
}

void SimpleHBHEPhase1Algo::beginRun(const edm::Run& r,
                                    const edm::EventSetup& es)
{
    runnum_ = r.run();
    pulseCorr_.beginRun(es);
}

void SimpleHBHEPhase1Algo::endRun()
{
    pulseCorr_.endRun();
}

HBHERecHit SimpleHBHEPhase1Algo::reconstruct(const HBHEChannelInfo& info,
                                             const HcalRecoParam* params,
                                             const HcalCalibrations& calibs,
                                             const bool isData)
{
    HBHERecHit rh;

    // Calculate "method 0" quantities
    int ibeg = static_cast<int>(info.soi()) + firstSampleShift_;
    if (ibeg < 0)
        ibeg = 0;
    const double fc_ampl = info.chargeInWindow(ibeg, ibeg + samplesToAdd_);
    const bool applyContainment = params ? params->correctForPhaseContainment() : true;
    const float phasens = params ? params->correctionPhaseNS() : phaseNS_;
    const float m0E = m0Energy(info, fc_ampl, applyContainment, phasens, isData);
    const float m0T = m0Time(info, fc_ampl, calibs);
    rh = HBHERecHit(info.id(), m0E, m0T);

    return rh;
}

float SimpleHBHEPhase1Algo::m0Energy(const HBHEChannelInfo& info,
                                     const double fc_ampl,
                                     const bool applyContainmentCorrection,
                                     const double phaseNs,
                                     const bool isData)
{
    int ibeg = static_cast<int>(info.soi()) + firstSampleShift_;
    if (ibeg < 0)
        ibeg = 0;
    double e = info.energyInWindow(ibeg, ibeg + samplesToAdd_);

    // Pulse containment correction
    {
        double corrFactor = 1.0;
        if (applyContainmentCorrection)
            corrFactor = pulseCorr_.get(info.id(), samplesToAdd_, phaseNs)->getCorrection(fc_ampl);
        e *= corrFactor;
    }

    // Special HB- correction
    {
        double corrFactor = 1.0;
        if (isData && runnum_ > 0)
        {
            const HcalDetId& cell = info.id();
            if (cell.subdet() == HcalBarrel)
            {
                const int ieta = cell.ieta();
                const int iphi = cell.iphi();
                corrFactor = hbminus_special_ecorr(ieta, iphi, e, runnum_);
            }
        }
        e *= corrFactor;
    }

    return e;
}

float SimpleHBHEPhase1Algo::m0Time(const HBHEChannelInfo& info,
                                   const double fc_ampl,
                                   const HcalCalibrations& calibs) const
{
    float time = -9999.f; // historic value

    const unsigned nSamples = info.nSamples();
    if (nSamples > 2U)
    {
        const int soi = info.soi();
        int ibeg = soi + firstSampleShift_;
        if (ibeg < 0)
            ibeg = 0;
        const int iend = ibeg + samplesToAdd_;
        unsigned maxI = info.peakEnergyTS(ibeg, iend);
        if (maxI < HBHEChannelInfo::MAXSAMPLES)
        {
            if (!maxI)
                maxI = 1U;
            else if (maxI >= nSamples - 1U)
                maxI = nSamples - 2U;

            // The remaining code in this scope emulates
            // the historic algorithm
            float t0 = info.tsEnergy(maxI - 1U);
            float maxA = info.tsEnergy(maxI);
            float t2 = info.tsEnergy(maxI + 1U);

            // Handle negative excursions by moving "zero"
            float minA = t0;
            if (maxA < minA) minA = maxA;
            if (t2 < minA)   minA=t2;
            if (minA < 0.f) { maxA-=minA; t0-=minA; t2-=minA; }
            float wpksamp = (t0 + maxA + t2);
            if (wpksamp) wpksamp = (maxA + 2.f*t2) / wpksamp;
            time = (maxI - soi)*25.f + timeshift_ns_hbheho(wpksamp);

            // Legacy QIE8 timing correction
            time -= HcalTimeSlew::delay(std::max(1.0, fc_ampl),
                                        HcalTimeSlew::Medium);
            // Time calibration
            time -= calibs.timecorr();
        }
    }
    return time;
}
