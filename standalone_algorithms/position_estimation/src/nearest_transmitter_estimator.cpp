#include <cmath>
#include <algorithm>
#include "nearest_transmitter_estimator.h"

namespace navigine {
namespace navigation_core {

static const double A = -82.0;
static const double B =  3.0;

NearestTransmitterPositionEstimator::NearestTransmitterPositionEstimator(const std::vector<Transmitter>& transmitters)
{
  m_smoother = PositionSmoother(0.9);
  for (const Transmitter& t: transmitters)
    m_transmitters[t.id] = t;
}

std::vector<RadioMeasurement> NearestTransmitterPositionEstimator::getRegisteredTransmittersMeasurements(
        const std::vector<RadioMeasurement>& radioMsr)
{
  std::vector<RadioMeasurement> registeredMeasurements;
  for (const RadioMeasurement& msr: radioMsr)
    if (m_transmitters.find(msr.id) != m_transmitters.end())
      registeredMeasurements.push_back(msr);
  return registeredMeasurements;
}

Position NearestTransmitterPositionEstimator::calculatePosition(const RadioMeasurements& inputMeasurements)
{
  Position position;
  position.isEmpty = true;
  std::vector<RadioMeasurement> radioMeasurements = getRegisteredTransmittersMeasurements(inputMeasurements);
  if (radioMeasurements.empty())
    return position;

  auto nearestTx = std::max_element(radioMeasurements.begin(), radioMeasurements.end(),
        [](RadioMeasurement msr1, RadioMeasurement msr2) {return msr1.rssi < msr2.rssi; });

  std::string nearestTxId = nearestTx->id;
  double nearestTxRssi = nearestTx->rssi;
  if (m_transmitters.find(nearestTxId) == m_transmitters.end())
    return position;
  else
  {
    const Transmitter& t= m_transmitters[nearestTxId];
    position.x = t.x;
    position.y = t.y;
    position.precision = std::sqrt(std::exp((A - nearestTxRssi) / B)) + 1.0;
    position.isEmpty = false;
    position.ts = radioMeasurements.back().ts;
    return m_smoother.smoothPosition(position);
  }
}

} } // namespace navigine::navigation_core
