#include "ECGTool.h"
#include <math.h>
namespace ECGConversion
{
// const limit for the holter resample algorithm
const int ECGTool::MAX_HOLTER_SPS = 25;
const int ECGTool::MIN_HOLTER_SPS = 10;

/// <summary>
///  Interval to use for Resampling with polynomial in msec.
/// </summary>
int ECGTool::ResampleInterval = 20;
double** ECGTool::Fast = null;
int ECGTool::FastLength = 0;

/// <summary>
/// Function to resample one lead of a signal.
/// </summary>
/// <param name="src">lead of signal to resample</param>
/// <param name="srcFreq">sample rate of signal</param>
/// <param name="dstFreq">destination sample rate</param>
/// <param name="dst">resampled signals</param>
/// <returns>0 on success</returns>
int ECGTool::ResampleLead(short* src, int srcLength, int srcFreq, int dstFreq, short** dst)
{
    *dst = null;

    if (src != null)
    {
        return ResampleLead(src, srcLength, 0, srcLength, srcFreq, dstFreq, dst);
    }

    return 1;
}

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
int ECGTool::ResampleLead(short* src, int srcLength, int startsample, int nrsamples, int srcFreq, int dstFreq, short** dst)
{
    int n = ((ResampleInterval * srcFreq) / 1000); // n= number of samples for a 20 ms (resample)interval
    *dst = null;
    // Make n a even number and larger then or equal to 2
    n >>= 1;

    if (n <= 0)
    {
        n = 1;
    }

    n <<= 1;
    MakeFastTable(n, srcFreq, dstFreq);

    if ((src != null)
        && (n > 1)
        && (srcFreq > 0)
        && (dstFreq > 0)
        && (startsample >= 0)
        && (nrsamples > 0)
        && ((startsample + nrsamples) <= srcLength))
    {
        int err = 0;
        //unsafe
        {
            if (srcFreq == dstFreq)
            {
                *dst = new short[nrsamples];
                short* pSrc = src;
                short* pDst = *dst;
                //fixed (short* pSrc = src, pDst = dst)
                {
                    short* pd = pDst;
                    short* pdend = pd + nrsamples;
                    short* ps = pSrc + startsample;

                    while (pd < pdend)
                    {
                        *(pd++) = *(ps++);
                    }
                }
                return 0;
            }

            *dst = new short[(nrsamples * dstFreq) / srcFreq + 1];
            int tussenFreq = KGV(srcFreq, dstFreq);
            int srcAdd = tussenFreq / srcFreq;
            int dstAdd = tussenFreq / dstFreq;
            int start = (startsample * srcAdd);
            int end = (startsample + nrsamples) * srcAdd;
            // Allocate two arrays for calculations
            double* c = new double[n + 1];
            double* d = new double[n + 1];
            short* pSrc = src;
            short* pDst = *dst;
            //fixed (short* pSrc = src, pDst = dst)
            {
                for (int tussenLoper = start; tussenLoper < end; tussenLoper += dstAdd)
                {
                    // If sample matches precisly a sample of source do no calculations.
                    if ((tussenLoper % srcAdd) == 0)
                    {
                        pDst[(tussenLoper - start) / dstAdd] = pSrc[tussenLoper / srcAdd];
                    }
                    else
                    {
                        // Determine first sample for polynoom.
                        int first = tussenLoper / srcAdd - (n >> 1);
                        // determine used N (for n at begin and end of data).
                        int usedN = n;

                        // if first is smaller then 0 make N smaller.
                        if (first < -1)
                        {
                            usedN -= ((-1 - first) << 1);
                            first = -1;
                        }

                        // if last is greater or equal then nrsamples make N smaller.
                        if (first + usedN >= nrsamples)
                        {
                            usedN -= (((first + usedN) - nrsamples) << 1);
                            first = nrsamples - usedN - 1;
                        }

                        if (((dstFreq / srcFreq) == 2)
                            && ((dstFreq % srcFreq) == 0))
                        {
                            int p = ((usedN >> 1) - 1);
                            double result = 0;

                            for (int loper = 0; loper < usedN; loper++)
                            {
                                result += (pSrc[first + 1 + loper] * Fast[p][loper]);
                            }

                            pDst[(tussenLoper - start) / dstAdd] = (short) result;
                        }
                        else
                        {
                            double den = 0;
                            int ns = 1;
                            int dif = abs(tussenLoper - ((first + 1) * srcAdd));

                            // Fill arrays with source samples.
                            for (int loper = 1; loper <= usedN; loper++)
                            {
                                int dift;

                                if ((dift = abs(tussenLoper - ((first + loper) * srcAdd))) < dif)
                                {
                                    ns = loper;
                                    dif = dift;
                                }

                                c[loper] = pSrc[first + loper];
                                d[loper] = pSrc[first + loper];
                            }

                            // The initial approximation
                            double y = pSrc[first + ns--];

                            for (int loper1 = 1; loper1 < usedN; loper1++)
                            {
                                for (int loper2 = 1; loper2 <= (usedN - loper1); loper2++)
                                {
                                    int ho = ((first + loper2) * srcAdd) - tussenLoper;
                                    int hp = ((first + loper2 + loper1) * srcAdd) - tussenLoper;
                                    double w = c[loper2 + 1] - d[loper2];

                                    if ((den = ho - hp) == 0)
                                    {
                                        // Error when no difference (dividing by zero is impossible)
                                        err |= 0x2;
                                    }

                                    den = w / den;
                                    d[loper2] = hp * den;
                                    c[loper2] = ho * den;
                                }

                                // Change approxiamation.
                                y += ((ns << 1) < (usedN - loper1) ? c[ns + 1] : d[ns--]);
                            }

                            // set value destination with approxiamation.
                            pDst[(tussenLoper - start) / dstAdd] = (short) y;
                        }
                    }
                }
            }
        }
        return err;
    }

    return 1;
}

/// <summary>
/// Make the fasttable.
/// </summary>
/// <param name="n">nr of samples resambles 20ms</param>
/// <param name="srcFreq">sample rate of signal</param>
/// <param name="dstFreq">destination sample rate</param>
void ECGTool::MakeFastTable(int n, int srcFreq, int dstFreq)
{
    // Make a table when fast calculation is possible
    if (((dstFreq / srcFreq) == 2)
        && ((dstFreq % srcFreq) == 0))
    {
        // Only make a new table if needed.
        if ((Fast == null)
            || (FastLength < (n >> 1)))
        {
            double** temp = Fast;
            Fast = new double*[n >> 1];
            FastLength = n >> 1;

            for (int x = 0; x < FastLength; x++)
            {
                // If fast table previously available, don't calculate again.
                if ((temp != null)
                    && (FastLength > x))
                {
                    Fast[x] = temp[x];
                }
                else
                {
                    Fast[x] = new double[(x + 1) << 1];
                    int newSingleFastLength = (x + 1) << 1;

                    for (int y = 0; y < newSingleFastLength; y++)
                    {
                        Fast[x][y] = 1;

                        for (int z = 0, c = 0; z < newSingleFastLength - 1; z++, c += 2)
                        {
                            if ((y << 1) == c)
                            {
                                c += 2;
                            }

                            Fast[x][y] *= (double)(((x << 1) + 1) - c) / (double)((y << 1) - c);
                        }
                    }
                }
            }
        }
    }
}

#if 0
/// <summary>
/// Function to set an other multiplier (if this function is improperly used data will be lost).
/// </summary>
/// <param name="src">signal to change multiplier</param>
/// <param name="srcmulti">orignal multiplier</param>
/// <param name="dstmulti">preferred multiplier</param>
/// <returns>0 on success</returns>
int ECGTool::ChangeMultiplier(short[][] src, double srcmulti, double dstmulti)
{
    if ((src != null)
        && (srcmulti == dstmulti))
    {
        return 0;
    }
    else if ((src != null)
             && (srcmulti > 0)
             && (dstmulti > 0))
    {
        for (int loper = 0; loper < src.Length; loper++)
        {
            if (ChangeMultiplier(src[loper], srcmulti, dstmulti) != 0)
            {
                return (0x2 << loper);
            }
        }

        return 0;
    }

    return 1;
}
#endif
/// <summary>
/// Function to set an other multiplier (if this function is improperly used data will be lost).
/// </summary>
/// <param name="src">Lead to change multiplier</param>
/// <param name="srcmulti">orignal multiplier</param>
/// <param name="dstmulti">preferred multiplier</param>
/// <returns>0 on success</returns>
int ECGTool::ChangeMultiplier(short* src, int srcLength, double srcmulti, double dstmulti)
{
    if ((src != null)
        && (srcmulti == dstmulti))
    {
        return 0;
    }
    else if ((src != null)
             && (srcmulti > 0)
             && (dstmulti > 0))
    {
        short* ps = src;
        short* psend = ps + srcLength;

        while (ps < psend)
        {
            *ps = (short)((*ps * srcmulti) / dstmulti);
            ps++;
        }

        return 0;
    }

    return 1;
}

/// <summary>
/// Function to determine the "grootste gemene deler"
/// </summary>
/// <param name="x1">value 1</param>
/// <param name="x2">value 2</param>
/// <returns>"grootste gemene deler"</returns>
int ECGTool::GGD(int x1, int x2)
{
    if ((x1 == 0)
        || (x2 == 0))
    {
        return 0;
    }

    if (x1 >= x2)
    {
        if ((x1 % x2) == 0)
        {
            return x2;
        }

        return GGD(x2, x1 % x2);
    }

    return GGD(x2, x1);
}
/// <summary>
/// Function to determine the "kleinst gemene veelvoud"
/// </summary>
/// <param name="x1">value 1</param>
/// <param name="x2">value 2</param>
/// <returns>"kleinst gemene veelvoud"</returns>
int ECGTool::KGV(int x1, int x2)
{
    int ggd = GGD(x1, x2);
    return (ggd == 0 ? 0 : (x1 * x2) / ggd);
}

}
