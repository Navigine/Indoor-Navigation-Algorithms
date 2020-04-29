#pragma once

#include <functional>
#include <vector>
#include <map>
#include <string>

#include "navigation_structures.h"

namespace navigine {
namespace navigation_core {

class RadioMeasurementBuffer
{
public:
  RadioMeasurementBuffer() = default;
  RadioMeasurementBuffer(long long sigWindowShiftMs, long long radioKeepPeriodMs);

  std::vector<RadioMeasurement> extractMeasurements();
  void addMeasurements(long long messageTs, const std::vector<RadioMeasurement>& signalEntries);

private:
  std::vector<RadioMeasurement> mMeasurements = {};
  long long mLastExtractionTs = -1;
  long long mCurrentTs = -1;
  long long mRadioKeepPeriodMs = 0;
  long long mSignalWindowShiftMs = 0;
};

class MeasurementsPreprocessor
{
  public:
    MeasurementsPreprocessor();

    void                           update(const RadioMeasurement &msr);
    long long                      getCurrentTs() const;
    std::vector<RadioMeasurement>  extractRadioMeasurements();

  private:
    bool isSignalTypeSupported(RadioMeasurement::Type signalType) const;
    bool isRssiValid(double rssi) const;

  private:
    RadioMeasurementBuffer    mRadiosBuffer;
    double                    mCutOffRssi      = -100;
    bool                      mUseWifi         = true;
    bool                      mUseBle          = true;
    long long                 mCurrentTs       = -1;
    bool                      mUseClosestAps   = true;
    int                       mNumClosestAps   = 5;
    std::string               mMeasurementType = "";
};

} } // namespace navigine::navigation_core
