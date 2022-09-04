#include "SCPFormat.h"
#include <typeinfo>
#include "SCPSection0.h"
#include "SCPSection1.h"
#include "SCPSection2.h"
#include "SCPSection3.h"
#include "SCPSection4.h"
#include "SCPSection5.h"
#include "SCPSection6.h"
#include "SCPSection7.h"
#include "SCPSection8.h"
#include "SCPSectionUnkown.h"
#include "SCPSection10.h"
#include "SCPSection11.h"
#include "CRCTool.h"
#include "BytesTool.h"

using namespace Communication_IO_Tools;
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
// Static settings of format.
uchar SCPFormat::DefaultSectionVersion = 20;
uchar SCPFormat::DefaultProtocolVersion = 20;
int SCPFormat::_MinFileLength = 158;
int SCPFormat::_MinNrSections = 12;
int SCPFormat::_MinNrWorkingSections = 2;

SCPFormat::SCPFormat()
{
    _QRSSubtractionSupport = false;
    _BimodalCompressionUsed = false;
    _BimodalCompressionRate = 0;
    _EncodingType = EncodingTypeDefaultHuffman;
    _DifferenceDataSection5Used = 2;
    _DifferenceDataSection6Used = 2;
    _UseLeadMeasurements = false;
    // data structure of format.
    _CRC = 0;
    _Default = {    new SCPSection0(),
                 new SCPSection1(),
                 new SCPSection2(),
                 new SCPSection3(),
                 new SCPSection4(),
                 new SCPSection5(),
                 new SCPSection6(),
                 new SCPSection7(),
                 new SCPSection8(),
                 new SCPSectionUnknown(),
                 new SCPSection10(),
                 new SCPSection11()
    };
    _Manufactor.clear();
}
SCPFormat::~SCPFormat()
{
    int size = _Default.size();

    for (int i = 0; i < size; i++)
    {
        delete _Default[i];
    }

    size = _Manufactor.size();

    for (int i = 0; i < size; i++)
    {
        delete _Manufactor[i];
    }
}

int SCPFormat::Read(uchar* buffer, int bufferLength, int offset)
{
    // Read in pointers (section0)
    SCPSection0* pointers = dynamic_cast<SCPSection0*>(_Default[0]);
    int err = _Default[0]->Read(buffer, bufferLength, offset + sizeof(_CRC) + sizeof(_Length), 0);

    if ((err != 0)
        || (pointers == null))
    {
        return 0x1;
    }

    ushort nrleads = 0;

    // set extra space for extra sections.
    if (pointers->getNrPointers() > _MinNrSections)
    {
        int size = pointers->getNrPointers() - _MinNrSections;
        _Manufactor.resize(size);

        for (int i = 0; i < size; i++)
        {
            _Manufactor[i] = null;
        }
    }

    string usedEncoding = "";

    // read in all section but pointers (section0).
    for (int loper = 1; loper < pointers->getNrPointers(); loper++)
    {
        // Special case for SCP Section 5 and 6 (they need to know the nr of leads used).
        if ((loper < _MinNrSections)
            && (typeid(_Default[loper]) == typeid(SCPSection5*)))
        {
            ((SCPSection5*)_Default[loper])->setNrLeads(nrleads);
        }
        else if ((loper < _MinNrSections)
                 && (typeid(_Default[loper]) == typeid(SCPSection6*)))
        {
            ((SCPSection6*)_Default[loper])->setNrLeads(nrleads);
        }

        // Section works if length if greater then size of section header.
        if (pointers->getLength(loper) > 16)
        {
            if (loper < _MinNrSections)
            {
                int ret = _Default[loper]->Read(buffer, bufferLength, offset + pointers->getIndex(loper) - 1, pointers->getLength(loper));

                if (ret != 0)
                {
                    err |= (0x2 << loper);
                }

                if (usedEncoding.length() > 0)
                {
                    _Default[loper]->SetEncoding(usedEncoding);
                }
            }
            else
            {
                _Manufactor[loper - _MinNrSections] = new SCPSectionUnknown();
                int ret = _Manufactor[loper - _MinNrSections]->Read(buffer, bufferLength, offset + pointers->getIndex(loper) - 1, pointers->getLength(loper));

                if (ret != 0)
                {
                    err |= (0x2 << loper);
                }

                if (usedEncoding.length() > 0)
                {
                    _Manufactor[loper - _MinNrSections]->SetEncoding(usedEncoding);
                }
            }
        }

        if ((loper < _MinNrSections)
            && (typeid(_Default[loper]) == typeid(SCPSection1*)))
        {
            usedEncoding = ((SCPSection1*)_Default[loper])->getLanguageSupportCode();
            _Default[0]->SetEncoding(usedEncoding);
            _Default[1]->SetEncoding(usedEncoding);
        }
        else if ((loper < _MinNrSections)
                 && (typeid(_Default[loper]) == typeid(SCPSection3*)))
        {
            nrleads = ((SCPSection3*)_Default[loper])->getNrLeads();
        }
    }

    return err;
}

int SCPFormat::Write(const string& file)
{
    if (file.length() > 0)
    {
        FILE* outputFile = fopen(file.c_str(), "wb");

        if (outputFile != null)
        {
            // set pointers
            setPointers();
            int fileSize = getFileSize();
            uchar* buffer = new uchar[fileSize];
            // use write function for byte arrays.
            int err = Write(buffer, fileSize, 0);

            if (err == 0)
            {
                fwrite(buffer, 1, fileSize, outputFile);
            }

            fclose(outputFile);
            delete [] buffer;
            return 0;
        }

        return 2;
    }

    return 0x1;
}

int SCPFormat::Write(uchar* buffer, int bufferLength, int offset)
{
    // Check if format works.
    if (Works())
    {
        _Length = getFileSize();
        SCPSection0* pointers = dynamic_cast<SCPSection0*>(_Default[0]);

        if ((buffer != null)
            && ((offset + _Length) <= bufferLength)
            && (pointers != null))
        {
            // Write length of file.
            BytesTool::writeBytes(_Length, buffer, bufferLength, offset + sizeof(_CRC), sizeof(_Length), true);

            // Write all sections in format.
            for (int loper = 0; loper < pointers->getNrPointers(); loper++)
            {
                if (loper < _MinNrSections)
                {
                    _Default[loper]->Write(buffer, bufferLength, offset + pointers->getIndex(loper) - 1);
                }
                else if ((pointers->getLength(loper) > SCPSection::Size)
                         && (_Manufactor[loper - _MinNrSections] != null))
                {
                    _Manufactor[loper - _MinNrSections]->Write(buffer, bufferLength, offset + pointers->getIndex(loper) - 1);
                }
            }

            // Calculate CRC of byte array.
            CRCTool crctool;
            crctool.Init(CRCTool::CRCCode::CRC_CCITT);
            _CRC = crctool.CalcCRCITT(buffer, bufferLength, offset + sizeof(_CRC), _Length - sizeof(_CRC));
            BytesTool::writeBytes(_CRC, buffer, bufferLength, offset, sizeof(_CRC), true);
            return 0x0;
        }

        return 0x2;
    }

    return 0x1;
}

#if 0
bool SCPFormat::CheckFormat(string file, int offset)
{
    if (file != null)
    {
        Stream read = new FileStream(file, FileMode.Open);
        bool ret = CheckFormat(read, offset);
        read.Close();
        return ret;
    }

    return false;
}
#endif

bool SCPFormat::CheckFormat(uchar* buffer, int bufferLength, int offset)
{
    ushort crc = (ushort) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(_CRC), true);
    int length = (int) BytesTool::readBytes(buffer, bufferLength, offset + sizeof(_CRC), sizeof(_Length), true);

    if (((offset + length) < bufferLength)
        && (length < _MinFileLength))
    {
        return false;
    }

    return CheckSCP(buffer, bufferLength, offset, crc, length);
}
void SCPFormat::Anonymous(uchar type)
{
    if ((_Default[1] != null)
        && (_Default[1]->Works())
        && (typeid(_Default[1]) == typeid(SCPSection1*)))
    {
        ((SCPSection1*)_Default[1])->Anonymous(type);
    }
}

int SCPFormat::getFileSize()
{
    SCPSection0* pointers = dynamic_cast<SCPSection0*>(_Default[0]);

    if (Works()
        && (pointers != null))
    {
        int sum = sizeof(_CRC) + sizeof(_Length);

        for (int loper = 0; loper < pointers->getNrPointers(); loper++)
        {
            sum += pointers->getLength(loper);
        }

        return sum;
    }

    return 0;
}

IDemographic* SCPFormat::getDemographic()
{
    SCPSection1* section1 = dynamic_cast<SCPSection1*>(_Default[1]);

    if (section1 != null)
    {
        return section1;
    }

    return null;
}
IDiagnostic* SCPFormat::getDiagnostic()
{
    SCPSection8* section8 = dynamic_cast<SCPSection8*>(_Default[8]);

    if (section8 != null)
    {
        return section8;
    }

    return null;
}
IGlobalMeasurement* SCPFormat::getGlobalMeasurements()
{
    SCPSection7* section7 = dynamic_cast<SCPSection7*>(_Default[7]);

    if (section7 != null)
    {
        return section7;
    }

    return null;
}
ISignal* SCPFormat::getSignals()
{
    return this;
}
ILeadMeasurement* SCPFormat::getLeadMeasurements()
{
    SCPSection10* section10 = dynamic_cast<SCPSection10*>(_Default[10]);

    if (section10 != null)
    {
        return section10;
    }

    return null;
}

bool SCPFormat::Works()
{
    SCPSection0* pointers = dynamic_cast<SCPSection0*>(_Default[0]);

    if ((_Default.size() == _MinNrSections)
        && (pointers != null)
        && (_MinNrSections /* + (_Manufactor != null ? _Manufactor.Length : 0) */) == pointers->getNrPointers())
    {
        for (int loper = 0; loper < _MinNrWorkingSections; loper++)
        {
            if (_Default[loper] == null)
            {
                return false;
            }
        }

        return (_Default[0]->Works() &&  _Default[1]->Works());
    }

    return false;
}

void SCPFormat::Empty()
{
    EmptyFormat();
}

// Signal Manupalations
int SCPFormat::getSignals(Signals& signals)
{
    SCPSection3* section3 = dynamic_cast<SCPSection3*>(_Default[3]);

    if ((section3 == null) || (section3->getSignals(signals) != 0))
    {
        return 2;
    }

    short* medianData = null;
    SCPSection4* section4 = dynamic_cast<SCPSection4*>(_Default[4]);

    if ((section4 == null) || (section4->getSignals(signals) == 0))
    {
#if 0 //TODO for median data
        SCPSection5 median = (SCPSection5) _Default[5];

        if (median == null)
        {
            return 4;
        }

        medianData = median.DecodeData((SCPSection2) _Default[2], signals.MedianLength);
        signals.MedianAVM = median.getAVM();
        signals.MedianSamplesPerSecond = median.getSamplesPerSecond();

        for (int loper = 0; loper < signals.NrLeads; loper++)
        {
            signals[loper].Median = medianData[loper];
        }

#endif
    }
    else
    {
        // this will make sure that Decoding of rhythm data will work also for strange files
        signals.MedianAVM = 0;
        signals.MedianLength = 0;
        signals.MedianSamplesPerSecond = 0;
    }

    SCPSection6* rhythm = dynamic_cast<SCPSection6*>(_Default[6]);//rhythm
    SCPSection2* section2 = dynamic_cast<SCPSection2*>(_Default[2]);
    short** rhythmData = rhythm->DecodeData(section2,
                                            section3,
                                            section4,
                                            signals.MedianSamplesPerSecond);
    signals.RhythmAVM = rhythm->getAVM();

    if (rhythmData == null)
    {
        return 8;
    }

#if 0 //TODO for median data

    if ((medianData != null)
        && (((SCPSection3) _Default[3]).isMediansUsed()))
    {
        // check this because corpuls ECG are in violation of this rule, but don't use median subtraction
        if (((signals.MedianSamplesPerSecond % signals.RhythmSamplesPerSecond) != 0)
            || ((signals.MedianSamplesPerSecond / signals.RhythmSamplesPerSecond) < 1)
            || ((signals.MedianSamplesPerSecond / signals.RhythmSamplesPerSecond) > 4))
        {
            return 16;
        }

        if (signals.RhythmAVM <= signals.MedianAVM)
        {
            ECGTool.ChangeMultiplier(medianData, signals.MedianAVM, signals.RhythmAVM);
            signals.MedianAVM = signals.RhythmAVM;
        }
        else
        {
            ECGTool.ChangeMultiplier(rhythmData, signals.RhythmAVM, signals.MedianAVM);
            signals.RhythmAVM = signals.MedianAVM;
        }

        signals.RhythmSamplesPerSecond = signals.MedianSamplesPerSecond;
        ((SCPSection4) _Default[4]).AddMedians((SCPSection3) _Default[3], rhythmData, medianData);
    }
    else
    {
        signals.RhythmAVM = rhythm.getAVM();
        signals.RhythmSamplesPerSecond = rhythm.getSamplesPerSecond();

        // Begin: special correction for SCP-ECG by corpuls (part 2)
        if ((_Default[5] != null)
            &&  _Default[5].Works())
        {
            SCPSection5 medianSpecial = (SCPSection5) _Default[5];
            signals.MedianLength = 1000;
            signals.MedianAVM = medianSpecial.getAVM();
            signals.MedianSamplesPerSecond = medianSpecial.getSamplesPerSecond();
            medianData = medianSpecial.DecodeData((SCPSection2) _Default[2], signals.MedianLength);

            if (medianData != null)
            {
                for (int loper = 0; loper < signals.NrLeads; loper++)
                {
                    signals[loper].Median = medianData[loper];
                }
            }
            else
            {
                signals.MedianLength = 0;
                signals.MedianAVM = 0;
                signals.MedianSamplesPerSecond = 0;
            }
        }

        // End: special correction for SCP-ECG by corpuls (part 2)
    }

#endif

    for (int loper = 0; loper < signals.getNrLeads(); loper++)
    {
        signals[loper].Rhythm = rhythmData[loper];
    }

    //free the memory of the pointers to the rows
    free(rhythmData);
    return 0;
}

int SCPFormat::setSignals(Signals& signals)
{
    int NrLeads = signals.getNrLeads();

    if (NrLeads > 0)
    {
        SCPSection2* section2 = dynamic_cast<SCPSection2*>(_Default[2]);

        if (section2 != null)
        {
            // Decide wich encoding to use.
            switch (_EncodingType)
            {
                case EncodingTypeNone:
                    section2->UseNoHuffman();
                    break;

                case EncodingTypeOptimizedHuffman:
                    // not implemented!
                    section2->UseStandard();
                    break;

                case EncodingTypeDefaultHuffman:
                default:
                    section2->UseStandard();
                    break;
            }
        }

        SCPSection3* section3 = dynamic_cast<SCPSection3*>(_Default[3]);

        if ((section3 == null) || (section3->setSignals(signals) != 0))
        {
            return 2;
        }

        SCPSection5* section5 = dynamic_cast<SCPSection5*>(_Default[5]);

        if (section5 != null)
        {
            section5->setAVM(signals.MedianAVM);
            section5->setSamplesPerSecond(signals.MedianSamplesPerSecond);
        }

        SCPSection6* section6 = dynamic_cast<SCPSection6*>(_Default[6]);

        if (section6 != null)
        {
            section6->setAVM(signals.RhythmAVM);
            section6->setSamplesPerSecond(signals.RhythmSamplesPerSecond);
        }

        std::vector<short*> rhythmData;
        std::vector<int> rhythmDataLength;
        rhythmData.resize(NrLeads);
        rhythmDataLength.resize(NrLeads);

        //        short[][] medianData = new short[signals.NrLeads][];
        for (int loper = 0; loper < NrLeads; loper++)
        {
            if (signals[loper].Rhythm == null)
            {
                return 4;
            }

            rhythmData[loper] = signals[loper].Rhythm;
            rhythmDataLength[loper] = signals[loper].RhythmLength;
#if 0//current only support rhythm data

            if ((medianData == null)
                || (signals[loper].Median == null))
            {
                medianData = null;
            }
            else
            {
                medianData[loper] = signals[loper].Median;
            }

#endif
        }

#if 0 //current only support rhythm data

        if (medianData != null)
        {
            if (((ISignal)_Default[4]).setSignals(signals) != 0)
            {
                return 8;
            }

            if (signals.MedianSamplesPerSecond < signals.RhythmSamplesPerSecond)
            {
                median.setSamplesPerSecond(signals.RhythmSamplesPerSecond);
                ECGTool.ResampleSignal(medianData, signals.MedianSamplesPerSecond, signals.RhythmSamplesPerSecond, out medianData);
            }

            if (median.EncodeData(medianData, (SCPSection2) _Default[2], (ushort)((signals.MedianLength * signals.MedianSamplesPerSecond) / 1000), (_EncodingType == EncodingType.None ? (byte)0 : _DifferenceDataSection5Used)) != 0)
            {
                return 16;
            }

            if (signals.QRSZone != null)
            {
                if (signals.RhythmAVM <= signals.MedianAVM)
                {
                    ECGTool.ChangeMultiplier(medianData, signals.MedianAVM, signals.RhythmAVM);
                }
                else
                {
                    ECGTool.ChangeMultiplier(rhythmData, signals.RhythmAVM, signals.MedianAVM);
                    rhythm.setAVM(signals.MedianAVM);
                }
            }

            ECGTool.ResampleSignal(rhythmData, signals.RhythmSamplesPerSecond, median.getSamplesPerSecond(), out rhythmData);

            if (_QRSSubtractionSupport
                && (signals.QRSZone != null))
            {
                ((SCPSection3) _Default[3]).setMediansUsed(true);
                ((SCPSection4) _Default[4]).SubtractMedians((SCPSection3) _Default[3], rhythmData, medianData);
            }
        }

        if (_BimodalCompressionUsed
            && (_BimodalCompressionRate > 0)
            && (medianData != null)
            && (signals.QRSZone != null))
        {
            // Bimodal Compression must be set in set section 6.
            rhythm.setBimodal(true);
            rhythm.setSamplesPerSecond(signals.MedianSamplesPerSecond / _BimodalCompressionRate);
            // Determine QRS zones for bimodal compression
            GlobalMeasurements global;
            ((IGlobalMeasurement)_Default[7]).getGlobalMeasurements(out global);
            ((SCPSection4)_Default[4]).setProtected(global, median.getSamplesPerSecond(), _BimodalCompressionRate, ((SCPSection3)_Default[3]).getMinBegin(), ((SCPSection3)_Default[3]).getMaxEnd());
        }

#endif
        SCPSection4* section4 = dynamic_cast<SCPSection4*>(_Default[4]);

        if ((section4 == null)
            || (section6 == null)
            || (section6->EncodeData(rhythmData,
                                     rhythmDataLength,
                                     section2,
                                     section3,
                                     section4,
                                     signals.MedianSamplesPerSecond,
                                     (_EncodingType == EncodingTypeNone ? (uchar)0 : _DifferenceDataSection6Used)) != 0))
        {
            return 32;
        }

        return 0;
    }

    return 1;
}
/// <summary>
/// Function to check SCP.
/// </summary>
/// <param name="buffer">byte array to do check in</param>
/// <param name="offset">position to start checking</param>
/// <param name="crc">value crc should be</param>
/// <param name="length">length of section</param>
/// <returns>0 on success</returns>
bool SCPFormat::CheckSCP(uchar* buffer, int bufferLength, int offset, ushort crc, int length)
{
    CRCTool crctool;
    crctool.Init(CRCTool::CRCCode::CRC_CCITT);

    if (crctool.CalcCRCITT(buffer, bufferLength, offset + sizeof(_CRC), length - sizeof(_CRC)) == crc)
    {
        return true;
    }

    return false;
}
/// <summary>
/// Function to set pointers.
/// </summary>
void SCPFormat::setPointers()
{
    SCPSection0* pointers = dynamic_cast<SCPSection0*>(_Default[0]);

    if (pointers != null)
    {
        pointers->setNrPointers(_MinNrSections /* + (_Manufactor != null ? _Manufactor.Length : 0) */);
        int sum = sizeof(_CRC) + sizeof(_Length) + 1;

        for (int loper = 0; loper < _MinNrSections; loper++)
        {
            ushort id = (ushort) loper;
            int length = _Default[loper]->getLength();
            int index = (length > SCPSection::Size ? sum : 0);
            pointers->setPointer(loper, id, length, index);
            sum += length;
        }

#if 0

        if (_Manufactor != null)
        {
            for (int loper = 0; loper < _Manufactor.Length; loper++)
            {
                ushort id = _Manufactor[loper].getSectionID();
                int length = _Manufactor[loper].getLength();
                int index = (length > SCPSection.Size ? sum : 0);
                pointers.setPointer(_MinNrSections + loper, id, length, index);
                sum += length;
            }
        }

#endif
        _Length = sum - 1;
        SCPSection1* section1 = dynamic_cast<SCPSection1*>(_Default[1]);

        // Determine file Protocol Compatibility Level.
        if (pointers->Works()
            && (section1 != null)
            && (section1->Works()))
        {
            SCPSection2* section2 = dynamic_cast<SCPSection2*>(_Default[2]);
            SCPSection3* section3 = dynamic_cast<SCPSection3*>(_Default[3]);
            SCPSection7* section7 = dynamic_cast<SCPSection7*>(_Default[7]);
            SCPSection8* section8 = dynamic_cast<SCPSection8*>(_Default[8]);

            if ((section2 != null)
                && (section2->Works())
                && (section3 != null)
                && (section3->Works()))
            {
                SCPSection4* section4 = dynamic_cast<SCPSection4*>(_Default[4]);
                SCPSection5* section5 = dynamic_cast<SCPSection5*>(_Default[5]);
                SCPSection6* section6 = dynamic_cast<SCPSection6*>(_Default[6]);

                if ((section4 != null)
                    && (section4->Works())
                    && (section5 != null)
                    && (section5->Works())
                    && (section6 != null)
                    && (section6->Works()))
                {
                    section1->setProtocolCompatibilityLevel(SCPSection1::kProtocolCompatibilityCatIV);
                }
                else if ((section5 != null)
                         && (section5->Works()))
                {
                    section1->setProtocolCompatibilityLevel(SCPSection1::kProtocolCompatibilityCatIII);
                }
                else if ((section6 != null)
                         && (section6->Works()))
                {
                    section1->setProtocolCompatibilityLevel(SCPSection1::kProtocolCompatibilityCatII);
                }
                else if ((section7 != null)
                         && (section7->Works())
                         && (section8 != null)
                         && (section8->Works()))
                {
                    section1->setProtocolCompatibilityLevel(SCPSection1::kProtocolCompatibilityCatI);
                }
            }
            else if ((section7 != null)
                     && (section7->Works())
                     && (section8 != null)
                     && (section8->Works()))
            {
                section1->setProtocolCompatibilityLevel(SCPSection1::kProtocolCompatibilityCatI);
            }
        }
    }
}
/// <summary>
/// Function to empty entire format.
/// </summary>
void SCPFormat::EmptyFormat()
{
    for (int loper = 0; loper < _MinNrSections; loper++)
    {
        _Default[loper]->Empty();
    }

    //    _Manufactor = null;
}

//region IDisposable Members
void SCPFormat::Dispose()
{
    //          base.Dispose();
    _CRC = 0;
    _Length = 0;

    if (_Default.size() > 0)
    {
        for (int loper = 0; loper < _Default.size(); loper++)
        {
            if (_Default[loper] != null)
            {
                _Default[loper]->Empty();
                _Default[loper] = null;
            }
        }

        _Default.clear();
    }
}
}
}
