#ifndef _SCPFORMAT_H_
#define _SCPFORMAT_H_
#include "IECGFormat.h"    
#include "ISignal.h"    
#include "IDemographic.h"    
#include "IDiagnostic.h"    
#include "IGlobalMeasurement.h"    
#include "ILeadMeasurement.h"    
#include "SCPSection.h"    
#include "ScpGlobal.h"

using namespace std;
using namespace ECGConversion::ECGSignals;
using namespace ECGConversion::ECGDemographics;
using namespace ECGConversion::ECGDiagnostic;	
using namespace ECGConversion::ECGGlobalMeasurements;
using namespace ECGConversion::ECGLeadMeasurements;

namespace ECGConversion
{
namespace SCP
{
/// <summary>
/// Class containing the entire SCP format.
/// </summary>
class SCPFormat : public IECGFormat, public ISignal
{
public:
    /// <summary>
    /// Enumration to set encoding used during encoding of signal.
    /// </summary>
    /// <remarks>
    /// OptimizedHuffman is same as DefaultHuffman, because it isn't implemented
    /// </remarks>
    enum EncodingType
    {
	    EncodingTypeNone = 0, 
	    EncodingTypeDefaultHuffman, 
	    EncodingTypeOptimizedHuffman
    };
public:
    SCPFormat();
	~SCPFormat();
    //region IECGFormat Members
#if 0//todo
	int Write(string file);	
	int Write(Stream output);
#endif
    int Write(uchar* buffer, int bufferLength, int offset);
    int getFileSize();

    IDemographic* getDemographic();
    IDiagnostic* getDiagnostic();
    IGlobalMeasurement* getGlobalMeasurements();
    ISignal* getSignals();
    ILeadMeasurement* getLeadMeasurements();

    bool Works();
    void Empty();
    int setSignals(Signals& signals);

    /// <summary>
    /// Function to set pointers.
    /// </summary>
    void setPointers();

    //region IDisposable Members
    void Dispose();

#if 0//todo
    /// <summary>
    /// Function to convert to SCP.
    /// </summary>
    /// <param name="src">an ECG file to convert</param>
    /// <param name="dst">SCP file returned</param>
    /// <returns>0 on success</returns>
    static int ToSCP(IECGFormat src, ECGConfig cfg, out IECGFormat dst);
#endif
private:
    /// <summary>
    /// Function to empty entire format.
    /// </summary>
    void EmptyFormat();
public:
    // Static settings of format.
    static uchar DefaultSectionVersion;
    static uchar DefaultProtocolVersion;
private:
    // settings for support with other readers.
    bool _QRSSubtractionSupport;
    bool _BimodalCompressionUsed;
    int _BimodalCompressionRate;
    EncodingType _EncodingType;

    uchar _DifferenceDataSection5Used;
    uchar _DifferenceDataSection6Used;
    bool _UseLeadMeasurements;
    static int _MinFileLength;
    static int _MinNrSections;
    static int _MinNrWorkingSections;
    // data structure of format.
    ushort _CRC;
    int _Length;
    std::vector<SCPSection*> _Default;
};
}
}
#endif  /*#ifndef _SCPFORMAT_H_*/
