#ifndef _ECGTOOL_H_
#define _ECGTOOL_H_
#include "ScpGlobal.h"

namespace ECGConversion
{
/// <summary>
/// a tool for calculating some leads.
/// </summary>
class ECGTool
{
public:
    /// <summary>
    /// Function to resample one lead of a signal.
    /// </summary>
    /// <param name="src">lead of signal to resample</param>
    /// <param name="srcFreq">sample rate of signal</param>
    /// <param name="dstFreq">destination sample rate</param>
    /// <param name="dst">resampled signals</param>
    /// <returns>0 on success</returns>
    static int ResampleLead(short* src, int srcLength, int srcFreq, int dstFreq, short** dst);

    /// <summary>
    /// Function to resample one lead of a signal.
    /// </summary>
    /// <param name="src">lead of signal to resample</param>
    /// <param name="startsample">samplenr to start resampleing</param>
    /// <param name="nrsamples">nr of samples in source</param>
    /// <param name="srcFreq">sample rate of signal</param>
    /// <param name="dstFreq">destination sample rate</param>
    /// <param name="dst">resampled signals</param>
    /// <returns>0 on success</returns>
    static int ResampleLead(short* src, int srcLength, int startsample, int nrsamples, int srcFreq, int dstFreq, short** dst);
#if 0
    /// <summary>
    /// Function to set an other multiplier (if this function is improperly used data will be lost).
    /// </summary>
    /// <param name="src">signal to change multiplier</param>
    /// <param name="srcmulti">orignal multiplier</param>
    /// <param name="dstmulti">preferred multiplier</param>
    /// <returns>0 on success</returns>
    static int ChangeMultiplier(short[][] src, double srcmulti, double dstmulti);
#endif
    /// <summary>
    /// Function to set an other multiplier (if this function is improperly used data will be lost).
    /// </summary>
    /// <param name="src">Lead to change multiplier</param>
    /// <param name="srcmulti">orignal multiplier</param>
    /// <param name="dstmulti">preferred multiplier</param>
    /// <returns>0 on success</returns>
    static int ChangeMultiplier(short* src, int srcLength, double srcmulti, double dstmulti);
private:
    static void MakeFastTable(int n, int srcFreq, int dstFreq);
    /// <summary>
    /// Function to determine the "grootste gemene deler"
    /// </summary>
    /// <param name="x1">value 1</param>
    /// <param name="x2">value 2</param>
    /// <returns>"grootste gemene deler"</returns>
    static int GGD(int x1, int x2);
    /// <summary>
    /// Function to determine the "kleinst gemene veelvoud"
    /// </summary>
    /// <param name="x1">value 1</param>
    /// <param name="x2">value 2</param>
    /// <returns>"kleinst gemene veelvoud"</returns>
    static int KGV(int x1, int x2);
public:
    // const limit for the holter resample algorithm
    static const int MAX_HOLTER_SPS;
    static const int MIN_HOLTER_SPS;

    /// <summary>
    ///  Interval to use for Resampling with polynomial in msec.
    /// </summary>
    static int ResampleInterval;
    static double** Fast;
    static int FastLength;
};
}


#endif  /*#ifndef _ECGTOOL_H_*/
