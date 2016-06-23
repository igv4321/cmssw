#ifndef DataFormats_HcalRecHit_HBHEChannelInfo_h_
#define DataFormats_HcalRecHit_HBHEChannelInfo_h_

#include "DataFormats/HcalDetId/interface/HcalDetId.h"
// #include "DataFormats/HcalRecHit/interface/HFQIE10Info.h"

// Unpacked charge and TDC information in a format which works
// for both QIE8 and QIE11
class HBHEChannelInfo
{
public:
    typedef HcalDetId key_type;

    static const unsigned MAXSAMPLES = 10;

    // Special value for the rise time used in case the QIE11 pulse
    // is always below the discriminator
    // static constexpr float UNKNOWN_T_UNDERSHOOT = HFQIE10Info::UNKNOWN_T_UNDERSHOOT;
    static constexpr float UNKNOWN_T_UNDERSHOOT = -100.f;

    // Special value for the rise time used in case the QIE11 pulse
    // is always above the discriminator
    // static constexpr float UNKNOWN_T_OVERSHOOT = HFQIE10Info::UNKNOWN_T_OVERSHOOT;
    static constexpr float UNKNOWN_T_OVERSHOOT = -110.f;

    // Special value for the rise time that can be used in case the TDC
    // info is not available at all (as for QIE8)
    static constexpr float UNKNOWN_T_NOTDC = -120.f;


    inline HBHEChannelInfo()
        : charge_{0.}, gain_{0.}, riseTime_{0.f}, adc_{0},
          hasTimeInfo_(false) {clear();}

    inline explicit HBHEChannelInfo(const bool hasTimeFromTDC)
        : charge_{0.}, gain_{0.}, riseTime_{0.f}, adc_{0},
          hasTimeInfo_(hasTimeFromTDC) {clear();}

    inline void clear()
    {
        id_ = HcalDetId(0U);
        nSamples_ = 0;
        soi_ = 0;
        capid_ = 0;
        dropped_ = true;
        hasLinkError_ = false;
        hasCapidError_ = false;
    }

    inline void setChannelInfo(const HcalDetId& detId, const unsigned nSamp,
                               const unsigned iSoi, const int iCapid,
                               const bool linkError, const bool capidError,
                               const bool dropThisChannel)
    {
        id_ = detId;
        nSamples_ = nSamp < MAXSAMPLES ? nSamp : MAXSAMPLES;
        soi_ = iSoi;
        capid_ = iCapid;
        dropped_ = dropThisChannel;
        hasLinkError_ = linkError;
        hasCapidError_ = capidError;
    }

    inline void tagAsDropped()
        {dropped_ = true;}

    // For speed, the "setSample" function does not perform bounds checking
    inline void setSample(const unsigned ts, const uint8_t rawADC,
                          const double q, const double g,
                          const float t = UNKNOWN_T_NOTDC)
        {charge_[ts] = q; gain_[ts] = g; riseTime_[ts] = t; adc_[ts] = rawADC;}

    // Inspectors
    inline HcalDetId id() const {return id_;}

    inline unsigned nSamples() const {return nSamples_;}
    inline unsigned soi() const {return soi_;}
    inline int capid() const {return capid_;}
    inline bool hasTimeInfo() const {return hasTimeInfo_;}
    inline bool isDropped() const {return dropped_;}
    inline bool hasLinkError() const {return hasLinkError_;}
    inline bool hasCapidError() const {return hasCapidError_;}

    // Access to time slice arrays
    inline const double* charge() const {return charge_;}
    inline const double* gain() const {return gain_;}
    inline const uint8_t* adc() const {return adc_;}
    inline const float* riseTime() const
        {if (hasTimeInfo_) return riseTime_; else return nullptr;}

    // Indexed access to time slice quiantities. No bounds checking.
    inline double tsCharge(const unsigned ts) const {return charge_[ts];}
    inline double tsEnergy(const unsigned ts) const
        {return charge_[ts]*gain_[ts];}
    inline double tsGain(const unsigned ts) const {return gain_[ts];}
    inline uint8_t tsAdc(const unsigned ts) const {return adc_[ts];}
    inline float tsRiseTime(const unsigned ts) const
        {return hasTimeInfo_ ? riseTime_[ts] : UNKNOWN_T_NOTDC;}

    // The TS with the "end" index is not included in the window
    inline double chargeInWindow(const unsigned begin, const unsigned end) const
    {
        double sum = 0.0;
        const unsigned imax = end < nSamples_ ? end : nSamples_;
        for (unsigned i=begin; i<imax; ++i)
            sum += charge_[i];
        return sum;
    }

    inline double energyInWindow(const unsigned begin, const unsigned end) const
    {
        double sum = 0.0;
        const unsigned imax = end < nSamples_ ? end : nSamples_;
        for (unsigned i=begin; i<imax; ++i)
            sum += charge_[i]*gain_[i];
        return sum;
    }

private:
    HcalDetId id_;

    // Charge in fC for all time slices
    double charge_[MAXSAMPLES];

    // fC to GeV conversion factor (can depend on CAPID)
    double gain_[MAXSAMPLES];

    // Signal rise time from TDC in ns (if provided)
    float riseTime_[MAXSAMPLES];

    // Raw QIE ADC values
    uint8_t adc_[MAXSAMPLES];

    // Number of time slices actually filled
    uint32_t nSamples_;

    // "Sample of interest" in the array of time slices
    uint32_t soi_;

    // QIE8 or QIE11 CAPID for the sample of interest
    int32_t capid_;

    // Flag indicating presence of the time info from TDC (QIE11)
    bool hasTimeInfo_;

    // Flag indicating that this channel should be dropped
    // (typically, tagged bad from DB or zero-suppressed)
    bool dropped_;

    // Flags indicating presence of hardware errors
    bool hasLinkError_;
    bool hasCapidError_;
};

#endif // DataFormats_HcalRecHit_HBHEChannelInfo_h_
