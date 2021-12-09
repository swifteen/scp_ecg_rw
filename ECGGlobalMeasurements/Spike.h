#ifndef _ZQ_SPIKE_H_
#define _ZQ_SPIKE_H_

namespace ECGConversion.ECGGlobalMeasurements
{
/// <summary>
/// Class containing one spike (SCP defined).
/// </summary>
class Spike
{
public:
    Spike();
public:
    ushort Time;
    short Amplitude;
};
}
