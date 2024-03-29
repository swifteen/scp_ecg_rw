#include "SCPSection1.h"
#include "CRCTool.h"
#include "BytesTool.h"

using namespace Communication_IO_Tools;
using namespace ECGConversion::ECGDemographics;

namespace ECGConversion
{
namespace SCP
{
/// <summary>
/// Class for a header field.
/// </summary>
class SCPHeaderField
{
public:
    /// <summary>
    /// Constructor to make an SCP header field.
    /// </summary>
    SCPHeaderField()
    {
        Tag = 0;
        Length = 0;
        Value = null;
    }
    /// <summary>
    /// Constructor to make an SCP header field.
    /// </summary>
    /// <param name="tag">tag to use</param>
    /// <param name="length">length to use</param>
    /// <param name="value">array to use</param>
    SCPHeaderField(uchar tag, ushort length, uchar* value)
    {
        Tag = tag;
        Length = (value == null ? (ushort)0 : length);
        Value = null;

        if (Length > 0)
        {
            Value = new uchar[Length];

            if (Value != null)
            {
                memcpy(Value, value, length);
            }
        }
    }

    SCPHeaderField(const SCPHeaderField& rhs)
    {
        Tag = 0;
        Length = 0;
        Value = null;//need init as null
        deepCopy(rhs);
    }

    SCPHeaderField& operator=(const SCPHeaderField& rhs)
    {
        // Prevent self-assignment
        if (&rhs != this)
        {
            if (this->Value != null)
            {
                delete[] this->Value;
                this->Value = null;
            }

            deepCopy(rhs);
        }

        return *this;
    }

    ~SCPHeaderField()
    {
        if (this->Value != null)
        {
            delete[] this->Value;
            this->Value = null;
        }
    }
    void deepCopy(const SCPHeaderField& rhs)
    {
        this->Tag = rhs.Tag;
        this->Length = rhs.Length;

        if ((rhs.Length > 0) && (rhs.Value != null))
        {
            this->Value = new uchar[rhs.Length];

            if (this->Value != null)
            {
                memcpy(this->Value, rhs.Value, rhs.Length);
            }
        }
    }
public:
    uchar Tag;
    ushort Length;
    uchar* Value;
};

// Fields that must be made empty for anonymous. (must be sorted from low to high)
uchar SCPSection1::_AnonymousFields[7] = {0, 1, 2, 3, 5, 30, 31};
// Defined in SCP.
uchar SCPSection1::_MustBePresent[4] = {2, 14, 25, 26}; // defined in paragraph 5.4.3.1 of SCP
uchar SCPSection1::_MultipleInstanceFields[5] = {10, 13, 30, 32, 35}; // Must be sorted
ushort SCPSection1::_MaximumFieldLength = 64;
uchar SCPSection1::_MaximumLengthExceptions[5] = {13, 14, 15, 30, 35}; // Must be sorted
ushort SCPSection1::_ExceptionsMaximumLength = 256; // should be 80, but some scp file doen't use this maximum. apparantly 128 wasn't enough as well
uchar SCPSection1::_ManufactorField = 0xc8;
ushort SCPSection1::_SectionID = 1;
uchar SCPSection1::_DemographicTerminator = 0xff;

// ResizeSpeed for the array to store the Fields.
int SCPSection1::_ResizeSpeed = 8;

/// <summary>
/// Class contains section 1 (Header Information).
/// </summary>
SCPSection1::SCPSection1():
    _NrFields(0)
{
    SCPSection::Empty();
    // Part of the stored Data Structure.
    _Fields.clear();
}
int SCPSection1::_Read(uchar* buffer, int bufferLength, int offset)
{
    Init();
    int end = offset - Size + Length;

    while (offset < end)
    {
        SCPHeaderField field;
        field.Tag = (uchar) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(field.Tag), true);
        offset += sizeof(field.Tag);

        if (field.Tag == _DemographicTerminator)
        {
            break;
        }
        else if ((offset + 2) > end)
        {
            _Empty();
            return 0x1;
        }

        field.Length = (ushort) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(field.Length), true);
        offset += sizeof(field.Length);

        if ((offset + field.Length) > end)
        {
            _Empty();
            return 0x2;
        }

        field.Value = new uchar[field.Length];
        offset += BytesTool::copy(field.Value, field.Length, 0, buffer, bufferLength, offset, field.Length);
        Insert(field);
    }

    return 0x0;
}
int SCPSection1::_Write(uchar* buffer, int bufferLength, int offset)
{
    for (int loper = 0; loper < _NrFields; loper++)
    {
        SCP_PD("loper[%d],Tag[%d],FieldsLength[%d]\n", loper, _Fields[loper].Tag, _Fields[loper].Length);
        BytesTool::writeBytes(_Fields[loper].Tag, buffer, bufferLength, offset, sizeof(_Fields[loper].Tag), true);
        offset += sizeof(_Fields[loper].Tag);
        BytesTool::writeBytes(_Fields[loper].Length, buffer, bufferLength, offset, sizeof(_Fields[loper].Length), true);
        offset += sizeof(_Fields[loper].Length);
        offset += BytesTool::copy(buffer, bufferLength, offset, _Fields[loper].Value, _Fields[loper].Length, 0, _Fields[loper].Length);
    }

    return 0x0;
}

void SCPSection1::_Empty()
{
    _NrFields = 0;
    _Fields.clear();
}

ushort SCPSection1::getSectionID()
{
    return _SectionID;
}

int SCPSection1::_getLength()
{
    if (Works())
    {
        int sum = 0;

        for (int loper = 0; loper < _NrFields; loper++)
        {
            sum += (sizeof(_Fields[loper].Tag) + sizeof(_Fields[loper].Length) + _Fields[loper].Length);
        }

        return ((sum % 2) == 0 ? sum : sum + 1);
    }

    return 0;
}

bool SCPSection1::Works()
{
    if (CheckInstances())
    {
        for (int loper = 0; loper < _NrFields; loper++)
        {
            if (((_Fields[loper].Value == null) && (_Fields[loper].Length != 0))
                || ((_Fields[loper].Length > _MaximumFieldLength)
                    && (_Fields[loper].Tag < _ManufactorField)
                    && (!isException(_MaximumLengthExceptions, sizeof(_MaximumLengthExceptions), _Fields[loper].Tag)
                        || (_Fields[loper].Length > _ExceptionsMaximumLength))))
            {
                SCP_PW("not Works:%d,Tag:%d,Length:%d\n", loper, _Fields[loper].Tag, _Fields[loper].Length);
                return false;
            }
        }

        int mustBePresentLength = sizeof(_MustBePresent) / sizeof(_MustBePresent[0]);

        for (int loper = 0; loper < mustBePresentLength; loper++)
        {
            if (_SearchField(_MustBePresent[loper]) < 0)
            {
                SCP_PW("not Works:%d\n", loper);
                return false;
            }
        }

        return (_Fields[_NrFields - 1].Tag == 0xff) && (_Fields[_NrFields - 1].Length == 0);
    }

    SCP_PW("not Works\n");
    return false;
}

/// <summary>
/// Function to initialize a section 1. Only needed when not reading from buffer.
/// </summary>
void SCPSection1::Init()
{
    _Empty();
    _Fields.resize(_ResizeSpeed);
    SCP_PD("Init\n");
    _Fields[_NrFields++] = SCPHeaderField(_DemographicTerminator, 0, null);
}

/// <summary>
/// Function to insert a field into section.
/// </summary>
/// <param name="field">field to insert</param>
/// <returns>0 on success</returns>
int SCPSection1::Insert(const SCPHeaderField& field)
{
    if ((field.Tag != _DemographicTerminator)
        && (_NrFields <= _Fields.size())
        && (_Fields[_NrFields - 1].Tag == _DemographicTerminator))
    {
        if (field.Length > 0)
        {
            int p1 = _SearchField(field.Tag);

            // If field exist must override or can be an multiple instances.
            if (p1 >= 0)
            {
                // If multiple instaces field, add field as last of this kind of field.
                if (isException(_MultipleInstanceFields, sizeof(_MultipleInstanceFields), field.Tag))
                {
                    // Resize if space is needed.
                    if (_NrFields == _Fields.size())
                    {
                        Resize();
                    }

                    // Find last of this kind.
                    for (; (p1 < _NrFields) && (_Fields[p1].Tag == field.Tag); p1++);

                    // Make space in array for field.
                    for (int loper = _NrFields; loper > p1; loper--)
                    {
                        _Fields[loper] = _Fields[loper - 1];
                    }

                    _Fields[p1] = field;
                    _NrFields++;
                }
                else
                {
                    // Overwrite existing field.
                    _Fields[p1] = field;
                }
            }
            else
            {
                // Resize if space is needed
                if (_NrFields == _Fields.size())
                {
                    Resize();
                }

                int p2 = _InsertSearch(field.Tag);

                // Make space to insert.
                for (int loper = _NrFields; loper > p2; loper--)
                {
                    _Fields[loper] = _Fields[loper - 1];
                }

                _Fields[p2] = field;
                _NrFields++;
            }

            return 0x0;
        }

        SCP_PE("field.Length:%d\n", field.Length);
        return 0x2;
    }

    SCP_PE("field.Tag:%d,_NrFields:%d,_Fields.size:%d\n", field.Tag, _NrFields, (int)_Fields.size());
    return 0x1;
}

/// <summary>
/// Function to remove a certain field from section.
/// </summary>
/// <param name="tag">tag of field.</param>
/// <returns>0 on success</returns>
int SCPSection1::Remove(uchar tag)
{
    if ((tag != _DemographicTerminator)
        && (_NrFields <= _Fields.size()))
    {
        int p = _SearchField(tag);

        if (p >= 0)
        {
            _NrFields--;

            for (; p < _NrFields; p++)
            {
                _Fields[p] = _Fields[p + 1];
            }

            return 0x0;
        }

        SCP_PE("tag:%d\n", tag);
        return 0x2;
    }

    SCP_PE("tag:%d\n", tag);
    return 0x1;
}

/// <summary>
/// Function to resize the space for header fields.
/// </summary>
void SCPSection1::Resize()
{
    SCP_PD("_NrFields[%d],_ResizeSpeed[%d]\n", _NrFields, _ResizeSpeed);
    _Fields.resize(_NrFields + _ResizeSpeed);//additional default-inserted elements are appended
    //resize会执行拷贝构造
}

#if 0
/// <summary>
/// Function to get a field from this section
/// </summary>
/// <param name="tag">tag to search for</param>
/// <returns></returns>
SCPHeaderField& SCPSection1::GetField(uchar tag)
{
    int pos = _SearchField(tag);

    if ((_Fields != null)
        && (pos >= 0)
        && (pos < _Fields.size()))
    {
        return _Fields[pos];
    }

    return null;
}
#endif

/// <summary>
/// Function to search for a field with a certain tag.
/// </summary>
/// <param name="tag">tag to search for</param>
/// <returns>position of this field</returns>
int SCPSection1::_SearchField(uchar tag)
{
    int l = 0;
    int h = _NrFields - 1;
    int m = (h >> 1);

    while (l <= h && _Fields[m].Tag != tag)
    {
        if (tag > _Fields[m].Tag)
        {
            l = m + 1;
        }
        else
        {
            h = m - 1;
        }

        m = ((l + h) >> 1);
    }

    if ((m >= 0) && (m < _NrFields) && (_Fields[m].Tag == tag))
    {
        //      SCP_PD("_SearchField ok tag:%d \n",tag);
        return m;
    }

    return -1;
}

/// <summary>
/// Function to find position to insert a field with a certain tag.
/// </summary>
/// <param name="tag">tag to search on.</param>
/// <returns>position to insert</returns>
int SCPSection1::_InsertSearch(uchar tag)
{
    int l = 0, h = _NrFields;

    while (l < h)
    {
        int m = (l + h) / 2;

        if (_Fields[m].Tag < tag)
        {
            l = m + 1;
        }
        else
        {
            h = m;
        }
    }

    return l;
}

/// <summary>
/// Function to check wheter the used fields are indeed sorted.
/// </summary>
bool SCPSection1::CheckInstances()
{
    if ((_NrFields > 0)  && (_NrFields <= _Fields.size()))
    {
        uchar prev = _Fields[0].Tag;

        for (int loper = 1; loper < _NrFields; loper++)
        {
            if ((prev == _Fields[loper].Tag)
                &&  !isException(_MultipleInstanceFields, sizeof(_MultipleInstanceFields), prev))
            {
                SCP_PE("_Fields:%d\n", loper);
                return false;
            }

            prev = _Fields[loper].Tag;
        }

        return true;
    }

    SCP_PE("NrFields:%d,Fields size:%d\n", _NrFields, (int)_Fields.size());
    return false;
}

/// <summary>
/// Function to check for exception case.
/// </summary>
/// <param name="condition">condition</param>
/// <param name="tag">value of tag</param>
/// <returns>is exception then true</returns>
bool SCPSection1::isException(uchar* condition, int conditionLength, uchar tag)
{
    if (condition == null)
    {
        SCP_PE("condition is null\n");
        return false;
    }

    int l = 0;
    int h = conditionLength - 1;
    int m = (h >> 1);

    while (l <= h && condition[m] != tag)
    {
        if (tag > condition[m])
        {
            l = m + 1;
        }
        else
        {
            h = m - 1;
        }

        m = ((l + h) >> 1);
    }

    return (m >= 0) && (m < conditionLength) && (condition[m] == tag);
}
/// <summary>
/// Function to anonymous this section.
/// </summary>
/// <param name="type">value to empty with</param>
void SCPSection1::Anonymous(uchar type)
{
    for (int loper = 0; loper < _NrFields; loper++)
    {
        if (isException(_AnonymousFields, sizeof(_AnonymousFields), _Fields[loper].Tag)
            && (_Fields[loper].Value != null))
        {
            if (_Fields[loper].Tag == 5)
            {
                SCPDate date2;
                date2.Read(_Fields[loper].Value, _Fields[loper].Length, 0);
                date2.Day = 1;
                date2.Month = 1;
                date2.Write(_Fields[loper].Value, _Fields[loper].Length, 0);
            }
            else
            {
                BytesTool::emptyBuffer(_Fields[loper].Value, _Fields[loper].Length, 0, _Fields[loper].Length - 1, type); //TODO check
            }
        }
    }
}
/// <summary>
/// Get encoding for text from language support code.
/// </summary>
/// <returns>used encoding</returns>
std::string SCPSection1::getLanguageSupportCode()
{
    std::string enc;
    getLanguageSupportCode(enc);
    return enc;
}
/// <summary>
/// Get encoding for text from language support code.
/// </summary>
/// <param name="enc">used encoding</param>
/// <returns>0 if successfull</returns>
int SCPSection1::getLanguageSupportCode(std::string& enc)
{
    enc = "ASCII";
    int p = _SearchField(14);

    if ((p >= 0)
        && (_Fields[p].Value != null)
        && (_Fields[p].Length > 16))
    {
        uchar lsc = _Fields[p].Value[16];

        if ((lsc & 0x1) == 0x0)
        {
            return 0;
        }
        else if ((lsc & 0x3) == 0x1)
        {
            enc = "ISO-8859-1";
            return 0;
        }
        else
        {
            string encName = "";

            switch (lsc)//TODO add UTF-8
            {
                case 0x03:
                    encName = "ISO-8859-2";
                    break;

                case 0x0b:
                    encName = "ISO-8859-4";
                    break;

                case 0x13:
                    encName = "ISO-8859-5";
                    break;

                case 0x1b:
                    encName = "ISO-8859-6";
                    break;

                case 0x23:
                    encName = "ISO-8859-7";
                    break;

                case 0x2b:
                    encName = "ISO-8859-8";
                    break;

                case 0x33:
                    encName = "ISO-8859-11";
                    break;

                case 0x3b:
                    encName = "ISO-8859-15";
                    break;

                case 0x07:
                    encName = "utf-16";
                    break; //case 0x07: encName = "ISO-60646"; break;

                case 0x0f: //encName = "JIS X0201-1976";
                case 0x17: //encName = "JIS X0208-1997";
                case 0x1f: //encName = "JIS X0212-1990";
                    encName = "EUC-JP";
                    break;

                case 0x27:
                    encName = "GB18030";
                    break;

                case 0x2f:
                    encName = "ks_c_5601-1987";
                    break;

                default:
                    break;
            }

            if (encName.length() > 0)
            {
                enc = encName;
                return 0;
            }
        }
    }

    return 1;
}
/// <summary>
/// Set language support code based on encoding.
/// </summary>
/// <param name="enc">encoding to set lsc with.</param>
/// <returns>0 if successfull</returns>
int SCPSection1::setLanguageSupportCode(const std::string& enc)
{
    int ret = 0;
    uchar lsc = 0;
#if 0

    switch (enc.CodePage)
    {
        case 20127:
            break;//US-ASCII

        case 28591:
            lsc = 0x01;
            break;//iso-8859-1

        case 28592:
            lsc = 0x03;
            break;//iso-8859-2

        case 28594:
            lsc = 0x0b;
            break;//iso-8859-4

        case 28595:
            lsc = 0x13;
            break;//iso-8859-5

        case 28596:
            lsc = 0x1b;
            break;//iso-8859-6

        case 28597:
            lsc = 0x23;
            break;//iso-8859-7

        case 28598:
            lsc = 0x2b;
            break;//iso-8859-8

        case 28603:
            lsc = 0x33;
            break;//iso-8859-13

        case 28605:
            lsc = 0x3b;
            break;//iso-8859-15

        case  1200:
            lsc = 0x07;
            break;//utf-16

        case 20932:
            lsc = 0x1f;
            break;//EUC-JP  Japanese (JIS 0208-1990 and 0212-1990)

        case 20936:
            lsc = 0x27;
            break;//x-cp20936   Chinese Simplified (GB2312-80)

        case   949:
            lsc = 0x2f;
            break;//ks_c_5601-1987  Korean

        default:
            ret = 1;
            break;
    }

#endif

    if (enc.compare("ASCII") == 0)
    {
        lsc = 0;
    }
    else if (enc.compare("GB18030") == 0)
    {
        lsc = 0x27;
    }
    else if (enc.compare("ISO−8859−1") == 0)
    {
        lsc = 0x01;
    }
    else if (enc.compare("ISO−8859−2") == 0)
    {
        lsc = 0x03;
    }
    else if (enc.compare("ISO−8859−4") == 0)
    {
        lsc = 0x0b;
    }
    else if (enc.compare("ISO−8859−5") == 0)
    {
        lsc = 0x13;
    }
    else if (enc.compare("ISO−8859−6") == 0)
    {
        lsc = 0x1b;
    }
    else if (enc.compare("ISO−8859−7") == 0)
    {
        lsc = 0x23;
    }
    else if (enc.compare("ISO−8859−8") == 0)
    {
        lsc = 0x2b;
    }
    else if (enc.compare("ISO−8859−13") == 0)
    {
        lsc = 0x33;
    }
    else if (enc.compare("ISO−8859−15") == 0)
    {
        lsc = 0x3b;
    }
    else
    {
        ret = 1;
        SCP_PE("not support LanguageSupportCode[%s]\n", enc.c_str());
    }

    int p = _SearchField(14);

    if ((p >= 0)
        && (p < _Fields.size())
        && (_Fields[p].Value != null)
        && (_Fields[p].Length > 16))
    {
        _Fields[p].Value[16] = lsc;
    }
    else if (ret != 1)
    {
        ret = 2;
    }

    p = _SearchField(15);

    if ((p >= 0)
        && (p < _Fields.size())
        && (_Fields[p].Value != null)
        && (_Fields[p].Length > 16))
    {
        _Fields[p].Value[16] = lsc;
    }

    return ret;
}
/// <summary>
/// Function to get Protocol Compatability Level.
/// </summary>
/// <param name="pc">Protocol Compatability Level</param>
/// <returns>0 on succes</returns>
int SCPSection1::getProtocolCompatibilityLevel(ProtocolCompatibility& pc)
{
    //pc = 0;
    int p = _SearchField(14);

    if ((p >= 0)
        && (_Fields[p].Value != null)
        && (_Fields[p].Length > 15))
    {
        pc = (ProtocolCompatibility) _Fields[p].Value[15];
        return 0;
    }

    return 1;
}
/// <summary>
/// Function to set Protocol Compatability Level.
/// </summary>
/// <param name="pc">Protocol Compatability Level</param>
/// <returns>0 on succes</returns>
int SCPSection1::setProtocolCompatibilityLevel(ProtocolCompatibility pc)
{
    int p = _SearchField(14);

    if ((p >= 0)
        && (_Fields[p].Value != null)
        && (_Fields[p].Length > 15))
    {
        _Fields[p].Value[15] = (uchar) pc;
        SCP_PD("ProtocolCompatibilityLevel:%d\n", pc);
        return 0;
    }

    SCP_PE("ProtocolCompatibilityLevel:%d\n", pc);
    return 1;
}
/// <summary>
/// Function to get a text from a certain tag.
/// </summary>
/// <param name="tag">id of tag</param>
/// <param name="text">a string</param>
/// <returns>0 on success</returns>
string SCPSection1::getText(uchar tag)//TODO
{
    int p = _SearchField(tag);

    if ((p >= 0)
        && (_Fields[p].Value != null)
        && (_Fields[p].Length > 0))
    {
        SCP_PD("tag:%d,text Length:%d\n", tag, _Fields[p].Length);
        return BytesTool::readString(_Encoding, _Fields[p].Value, _Fields[p].Length, 0, _Fields[p].Length);;
    }

    SCP_PE("tag:%d,text is empty\n", tag);
    return "";
}
/// <summary>
/// Function to set a text from a cetain tag.
/// </summary>
/// <param name="tag">id of tag</param>
/// <param name="text">a string</param>
/// <returns>0 on success</returns>
int SCPSection1::setText(uchar tag, const string& text)
{
    if (text.length() > 0)
    {
        SCPHeaderField field;
        field.Tag = tag;
        field.Length = (ushort)(text.length() >= _MaximumFieldLength ? _MaximumFieldLength :  text.length() + 1);
        field.Value = new uchar[field.Length];
        BytesTool::writeString(_Encoding, text, field.Value, field.Length, 0, field.Length);
        SCP_PD("tag:%d,text:%s\n", tag, text.c_str());
        return Insert(field) << 1;
    }

    SCP_PE("tag:%d,text:%s\n", tag, text.c_str());
    return 1;
}

// Getting Demographics information
string SCPSection1::getLastName()
{
    return getText(0);
}

void SCPSection1::setLastName(const string& value)
{
    setText(0, value);
}

string SCPSection1::getFirstName()
{
    return getText(1);
}

void SCPSection1::setFirstName(const string& value)
{
    setText(1, value);
}

string SCPSection1::getPatientID()
{
    return getText(2);
}

void SCPSection1::setPatientID(const string& value)
{
    SCP_PD("PatientID:%s\n", value.c_str());
    setText(2, value);
}

string SCPSection1::getSecondLastName()
{
    return getText(3);
}

void SCPSection1::setSecondLastName(const string& value)
{
    setText(3, value);
}

string SCPSection1::getPrefixName()
{
    return string("");
}

void SCPSection1::setPrefixName(const string& value)
{
}

string SCPSection1::getSuffixName()
{
    return string("");
}

void SCPSection1::setSuffixName(const string& value)
{
}

int SCPSection1::getPatientAge(ushort& val, AgeDefinition& def)
{
    val = 0;
    def = kAgeUnspecified;
    int p = _SearchField(4);

    if ((p >= 0)
        && (_Fields[p].Length == 3)
        && (_Fields[p].Value != null))
    {
        val = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(val), true);

        switch (BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, sizeof(val), 1, true))
        {
            case 1:
                def = kAgeYears;
                break;

            case 2:
                def = kAgeMonths;
                break;

            case 3:
                def = kAgeWeeks;
                break;

            case 4:
                def = kAgeDays;
                break;

            case 5:
                def = kAgeHours;
                break;

            default:
                def = kAgeUnspecified;
                break;
        }

        return (val == 0) && (def == kAgeUnspecified) ? 2 : 0;
    }

    return 1;
}

int SCPSection1::setPatientAge(ushort val, AgeDefinition def)
{
    SCPHeaderField field;
    field.Tag = 4;
    field.Length = (ushort)(sizeof(val) + sizeof(uchar));
    field.Value = new uchar[field.Length];
    BytesTool::writeBytes(val, field.Value, field.Length, 0, sizeof(val), true);
    BytesTool::writeBytes((uchar)def, field.Value, field.Length, sizeof(val), sizeof(uchar), true);
    SCP_PD("val:%d,AgeDef:%d\n", val, def);
    return Insert(field) << 1;
}

Date SCPSection1::getPatientBirthDate()
{
    int p = _SearchField(5);

    if ((p >= 0)
        && (_Fields[p].Length == 4)
        && (_Fields[p].Value != null))
    {
        SCPDate scpdate;
        scpdate.Read(_Fields[p].Value, _Fields[p].Length, 0);
        return Date(scpdate.Year, scpdate.Month, scpdate.Day);
    }

    SCP_PE("invalid PatientBirthDate\n");
    return Date();//TODO返回默认构造是否合理
}

void SCPSection1::setPatientBirthDate(Date& PatientBirthDate)
{
    if (PatientBirthDate.isExistingDate())
    {
        SCPHeaderField field;
        field.Tag = 5;
        field.Length = (ushort) SCPDate::Size;
        field.Value = new uchar[field.Length];
        SCPDate scpdate;
        scpdate.Year = PatientBirthDate.Year;
        scpdate.Month = PatientBirthDate.Month;
        scpdate.Day = PatientBirthDate.Day;
        scpdate.Write(field.Value, field.Length, 0);
        SCP_PD("Year:%d,Month:%d,Day:%d\n", PatientBirthDate.Year, PatientBirthDate.Month, PatientBirthDate.Day);
        Insert(field);
        return;
    }

    SCP_PE("Year:%d,Month:%d,Day:%d\n", PatientBirthDate.Year, PatientBirthDate.Month, PatientBirthDate.Day);
}

int SCPSection1::getPatientHeight(ushort& val, HeightDefinition& def)
{
    val = 0;
    def = kHeightUnspecified;
    int p = _SearchField(6);

    if ((p >= 0)
        && (_Fields[p].Length == 3)
        && (_Fields[p].Value != null))
    {
        val = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(val), true);

        switch (BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, sizeof(val), 1, true))
        {
            case 1:
                def = kHeightCentimeters;
                break;

            case 2:
                def = kHeightInches;
                break;

            case 3:
                def = kHeightMillimeters;
                break;

            default:
                def = kHeightUnspecified;
                break;
        }

        return (val == 0) && (def == kHeightUnspecified) ? 2 : 0;
    }

    return 1;
}

int SCPSection1::setPatientHeight(ushort val, HeightDefinition def)
{
    SCPHeaderField field;
    field.Tag = 6;
    field.Length = (ushort)(sizeof(val) + sizeof(uchar));
    field.Value = new uchar[field.Length];
    BytesTool::writeBytes(val, field.Value, field.Length, 0, sizeof(val), true);
    BytesTool::writeBytes((uchar)def, field.Value, field.Length, sizeof(val), 1, true);
    return Insert(field) << 1;
}

int SCPSection1::getPatientWeight(ushort& val, WeightDefinition& def)
{
    val = 0;
    def = kWeightUnspecified;
    int p = _SearchField(7);

    if ((p >= 0)
        && (_Fields[p].Length == 3)
        && (_Fields[p].Value != null))
    {
        val = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(val), true);

        switch (BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, sizeof(val), 1, true))
        {
            case 1:
                def = kWeightKilogram;
                break;

            case 2:
                def = kWeightGram;
                break;

            case 3:
                def = kWeightPound;
                break;

            case 4:
                def = kWeightOunce;
                break;

            default:
                def = kWeightUnspecified;
                break;
        }

        return (val == 0) && (def == kWeightUnspecified) ? 2 : 0;
    }

    return 1;
}

int SCPSection1::setPatientWeight(ushort val, WeightDefinition def)
{
    SCPHeaderField field;
    field.Tag = 7;
    field.Length = (ushort)(sizeof(val) + sizeof(uchar));
    field.Value = new uchar[field.Length];
    BytesTool::writeBytes(val, field.Value, field.Length, 0, sizeof(val), true);
    BytesTool::writeBytes((uchar)def, field.Value, field.Length, sizeof(val), sizeof(uchar), true);
    return Insert(field);
}

Sex SCPSection1::getGender()
{
    int p = _SearchField(8);

    if ((p >= 0)
        && (_Fields[p].Length == 1)
        && (_Fields[p].Value != null))
    {
        switch (BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, 1, true))
        {
            case 1:
                return kSexMale;

            case 2:
                return kSexFemale;

            default:
                return kSexUnspecified;
        }
    }

    return kSexNull;
}

void SCPSection1::setGender(Sex Gender)
{
    if (Gender != ECGDemographics::kSexNull)
    {
        SCPHeaderField field;
        field.Tag = 8;
        field.Length = (ushort) sizeof(uchar);
        field.Value = new uchar[field.Length];
        BytesTool::writeBytes((uchar)Gender, field.Value, field.Length, 0, sizeof(uchar), true);
        Insert(field);
    }
}

Race SCPSection1::getPatientRace()
{
    int p = _SearchField(9);

    if ((p >= 0)
        && (_Fields[p].Length == 1)
        && (_Fields[p].Value != null))
    {
        switch (BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, 1, true))
        {
            case 1:
                return kRaceCaucasian;

            case 2:
                return kRaceBlack;

            case 3:
                return kRaceOriental;

            default:
                return kRaceUnspecified;
        }
    }

    return kRaceNull;
}

void SCPSection1::setPatientRace(Race PatientRace)
{
    if (PatientRace != ECGDemographics::kRaceNull)
    {
        SCPHeaderField field;
        field.Tag = 9;
        field.Length = (ushort) sizeof(uchar);
        field.Value = new uchar[field.Length];
        BytesTool::writeBytes((uchar)PatientRace, field.Value, field.Length, 0, sizeof(uchar), true);
        Insert(field);
    }
}

AcquiringDeviceID SCPSection1::getAcqMachineID()
{
    int p = _SearchField(14);

    if ((p >= 0)
        && (_Fields[p].Length >= 36)
        && (_Fields[p].Value != null))
    {
        AcquiringDeviceID id = new AcquiringDeviceID();
        int offset = 0;
        id.InstitutionNr = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.InstitutionNr), true);
        offset += sizeof(id.InstitutionNr);
        id.DepartmentNr = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DepartmentNr), true);
        offset += sizeof(id.DepartmentNr);
        id.DeviceID = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DeviceID), true);
        offset += sizeof(id.DeviceID);
        id.DeviceType = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DeviceType), true);
        offset += sizeof(id.DeviceType);
        id.ManufactorID = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.ManufactorID), true);
        offset += sizeof(id.ManufactorID);
        offset += BytesTool::copy(id.ModelDescription, sizeof(id.ModelDescription), 0, _Fields[p].Value, _Fields[p].Length, offset, sizeof(id.ModelDescription));
        // Skip some not needed info.
        offset += 3;
        id.DeviceCapabilities = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DeviceCapabilities), true);
        offset += sizeof(id.DeviceCapabilities);
        id.ACFrequencyEnvironment = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.ACFrequencyEnvironment), true);
        offset += sizeof(id.ACFrequencyEnvironment);
        return id;
    }

    return AcquiringDeviceID();
}

void SCPSection1::setAcqMachineID(const AcquiringDeviceID& id)
{
    SCPHeaderField field;
    string deviceManufactor = (id.ManufactorID == 0 ?
                               "defaultManufactorID" :
                               (std::to_string((DeviceManufactor)id.ManufactorID)));
    string SoftwareName = "scp-ecg writer";
    string unknown = "unknown";
    field.Tag = 14;
    field.Length = (ushort)(41 + (SoftwareName.length() > 24 ?
                                  24 : SoftwareName.length()) + deviceManufactor.length() + (3 * unknown.length()));
    field.Value = new uchar[field.Length];
    int offset = 0;
    BytesTool::writeBytes(id.InstitutionNr, field.Value, field.Length, offset, sizeof(id.InstitutionNr), true);
    offset += sizeof(id.InstitutionNr);
    BytesTool::writeBytes(id.DepartmentNr, field.Value, field.Length, offset, sizeof(id.DepartmentNr), true);
    offset += sizeof(id.DepartmentNr);
    BytesTool::writeBytes(id.DeviceID, field.Value, field.Length, offset, sizeof(id.DeviceID), true);
    offset += sizeof(id.DeviceID);
    BytesTool::writeBytes(id.DeviceType, field.Value, field.Length, offset, sizeof(id.DeviceType), true);
    offset += sizeof(id.DeviceType);
    BytesTool::writeBytes((id.ManufactorID == 0 ? (uchar) 0xff : id.ManufactorID),
                          field.Value,
                          field.Length,
                          offset,
                          sizeof(id.ManufactorID),
                          true);
    offset += sizeof(id.ManufactorID);
    offset += BytesTool::copy(field.Value,
                              field.Length,
                              offset,
                              id.ModelDescription,
                              sizeof(id.ModelDescription),
                              0,
                              sizeof(id.ModelDescription));
    field.Value[offset++] = ProtocolVersionNr;
    field.Value[offset++] = 0x00;
    field.Value[offset++] = 0x00;
    field.Value[offset++] = (id.DeviceCapabilities == 0 ? (uchar) 0x8 : id.DeviceCapabilities);
    field.Value[offset++] = id.ACFrequencyEnvironment;
    // Skip Reserved for Future field
    offset += 16;
    field.Value[offset++] = (uchar)(unknown.length() + 1);
    BytesTool::writeString(_Encoding, unknown, field.Value, field.Length, offset, unknown.length() + 1);
    offset += unknown.length() + 1;
    BytesTool::writeString(_Encoding, unknown, field.Value, field.Length, offset, unknown.length() + 1);
    offset += unknown.length() + 1;
    BytesTool::writeString(_Encoding, unknown, field.Value, field.Length, offset, unknown.length() + 1);
    offset += unknown.length() + 1;
    BytesTool::writeString(_Encoding,
                           SoftwareName,
                           field.Value,
                           field.Length,
                           offset,
                           (SoftwareName.length() > 24 ? 24 : SoftwareName.length()) + 1);
    offset += (SoftwareName.length() > 24 ? 24 : SoftwareName.length()) + 1;
    BytesTool::writeString(_Encoding, deviceManufactor, field.Value, field.Length, offset, deviceManufactor.length() + 1);
    offset += deviceManufactor.length() + 1;
    int ret = Insert(field);

    if (ret == 0)
    {
        ret = setLanguageSupportCode(_Encoding);
    }
}

AcquiringDeviceID SCPSection1::getAnalyzingMachineID()
{
    int p = _SearchField(15);

    if ((p >= 0)
        && (_Fields[p].Length >= 36)
        && (_Fields[p].Value != null))
    {
        AcquiringDeviceID id = new AcquiringDeviceID();
        int offset = 0;
        id.InstitutionNr = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.InstitutionNr), true);
        offset += sizeof(id.InstitutionNr);
        id.DepartmentNr = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DepartmentNr), true);
        offset += sizeof(id.DepartmentNr);
        id.DeviceID = (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DeviceID), true);
        offset += sizeof(id.DeviceID);
        id.DeviceType = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DeviceType), true);
        offset += sizeof(id.DeviceType);
        id.ManufactorID = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.ManufactorID), true);
        offset += sizeof(id.ManufactorID);
        offset += BytesTool::copy(id.ModelDescription, sizeof(id.ModelDescription), 0, _Fields[p].Value, _Fields[p].Length, offset, sizeof(id.ModelDescription));
        // Skip some not needed info.
        offset += 3;
        id.DeviceCapabilities = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.DeviceCapabilities), true);
        offset += sizeof(id.DeviceCapabilities);
        id.ACFrequencyEnvironment = (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, offset, sizeof(id.ACFrequencyEnvironment), true);
        offset += sizeof(id.ACFrequencyEnvironment);
        return id;
    }

    return AcquiringDeviceID();
}

void SCPSection1::setAnalyzingMachineID(const AcquiringDeviceID& id)
{
    SCPHeaderField field;
    string SoftwareName = "scp-ecg writer";
    string deviceManufactor = (id.ManufactorID == 0 ?
                               SoftwareName : (std::to_string((DeviceManufactor)id.ManufactorID)));
    string unknown = "unknown";
    field.Tag = 15;
    field.Length = (ushort)(41 + (SoftwareName.length() > 24 ?
                                  24 : SoftwareName.length()) + deviceManufactor.length() + (3 * unknown.length()));
    field.Value = new uchar[field.Length];
    int offset = 0;
    BytesTool::writeBytes(id.InstitutionNr, field.Value, field.Length, offset, sizeof(id.InstitutionNr), true);
    offset += sizeof(id.InstitutionNr);
    BytesTool::writeBytes(id.DepartmentNr, field.Value, field.Length, offset, sizeof(id.DepartmentNr), true);
    offset += sizeof(id.DepartmentNr);
    BytesTool::writeBytes(id.DeviceID, field.Value, field.Length, offset, sizeof(id.DeviceID), true);
    offset += sizeof(id.DeviceID);
    BytesTool::writeBytes(id.DeviceType, field.Value, field.Length, offset, sizeof(id.DeviceType), true);
    offset += sizeof(id.DeviceType);
    BytesTool::writeBytes(id.ManufactorID == 0 ? (uchar)0xff : id.ManufactorID,
                          field.Value,
                          field.Length,
                          offset,
                          sizeof(id.ManufactorID),
                          true);
    offset += sizeof(id.ManufactorID);
    offset += BytesTool::copy(field.Value,
                              field.Length,
                              offset,
                              id.ModelDescription,
                              sizeof(id.ModelDescription),
                              0,
                              sizeof(id.ModelDescription));
    field.Value[offset++] = ProtocolVersionNr;
    field.Value[offset++] = 0x00;
    field.Value[offset++] = 0x00;
    field.Value[offset++] = (id.DeviceCapabilities == 0 ? (uchar) 0x8 : id.DeviceCapabilities);
    field.Value[offset++] = id.ACFrequencyEnvironment;
    // Skip Reserved for Future field
    offset += 16;
    field.Value[offset++] = (uchar)(unknown.length() + 1);
    BytesTool::writeString(_Encoding, unknown, field.Value, field.Length, offset, unknown.length() + 1);
    offset += unknown.length() + 1;
    BytesTool::writeString(_Encoding, unknown, field.Value, field.Length, offset, unknown.length() + 1);
    offset += unknown.length() + 1;
    BytesTool::writeString(_Encoding, unknown, field.Value, field.Length, offset, unknown.length() + 1);
    offset += unknown.length() + 1;
    BytesTool::writeString(_Encoding, SoftwareName, field.Value, field.Length, offset, (SoftwareName.length() > 24 ? 24 : SoftwareName.length()) + 1);
    offset += (SoftwareName.length() > 24 ? 24 : SoftwareName.length()) + 1;
    BytesTool::writeString(_Encoding, deviceManufactor, field.Value, field.Length, offset, deviceManufactor.length() + 1);
    offset += deviceManufactor.length() + 1;
    int ret = Insert(field);

    if (ret == 0)
    {
        ret = setLanguageSupportCode(_Encoding);
    }
}

DateTime SCPSection1::getTimeAcquisition()
{
    DateTime time;
    memset(&time, 0, sizeof(DateTime));
    int p = _SearchField(25);

    if ((p >= 0)
        && (_Fields[p].Length == 4)
        && (_Fields[p].Value != null))
    {
        SCPDate scpdate;
        scpdate.Read(_Fields[p].Value, _Fields[p].Length, 0);
        time.Year = scpdate.Year;
        time.Month = scpdate.Month;
        time.Day = scpdate.Day;
        p = _SearchField(26);

        if ((p >= 0)
            && (_Fields[p].Length == 3)
            && (_Fields[p].Value != null))
        {
            SCPTime scptime;
            scptime.Read(_Fields[p].Value, _Fields[p].Length, 0);
            time.Hour = scptime.Hour;
            time.Minute = scptime.Min;
            time.Second = scptime.Sec;
        }
    }

    return time;
}

void SCPSection1::setTimeAcquisition(const DateTime& time)
{
    if (time.Year > 1000)
    {
        SCPHeaderField field;
        field.Tag = 25;
        field.Length = (ushort) SCPDate::Size;
        field.Value = new uchar[field.Length];
        SCPDate scpdate;
        scpdate.Year = (ushort) time.Year;
        scpdate.Month = (uchar) time.Month;
        scpdate.Day = (uchar) time.Day;
        scpdate.Write(field.Value, field.Length, 0);
        int ret = Insert(field);

        if (ret != 0)
        {
            return;
        }

        SCPHeaderField time_field;
        time_field.Tag = 26;
        time_field.Length = (ushort) SCPTime::Size;
        time_field.Value = new uchar[time_field.Length];
        SCPTime scptime;
        scptime.Hour = (uchar) time.Hour;
        scptime.Min = (uchar) time.Minute;
        scptime.Sec = (uchar) time.Second;
        scptime.Write(time_field.Value, time_field.Length, 0);
        Insert(time_field);
    }
}

ushort SCPSection1::getBaselineFilter()
{
    int p = _SearchField(27);

    if ((p >= 0)
        && (_Fields[p].Length == 2)
        && (_Fields[p].Value != null))
    {
        return (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(ushort), true);
    }

    return 0;
}

void SCPSection1::setBaselineFilter(ushort BaselineFilter)
{
    SCPHeaderField field;
    field.Tag = 27;
    field.Length = (ushort) sizeof(BaselineFilter);
    field.Value = new uchar[field.Length];
    BytesTool::writeBytes(BaselineFilter, field.Value, field.Length, 0, sizeof(BaselineFilter), true);
    Insert(field);
    SCP_PD("BaselineFilter:%u\n", BaselineFilter);
}

ushort SCPSection1::getLowpassFilter()
{
    int p = _SearchField(28);

    if ((p >= 0)
        && (_Fields[p].Length == 2)
        && (_Fields[p].Value != null))
    {
        return (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(ushort), true);
    }

    return 0;
}

void SCPSection1::setLowpassFilter(ushort LowpassFilter)
{
    SCPHeaderField field;
    field.Tag = 28;
    field.Length = (ushort) sizeof(LowpassFilter);
    field.Value = new uchar[field.Length];
    BytesTool::writeBytes(LowpassFilter, field.Value, field.Length, 0, sizeof(LowpassFilter), true);
    Insert(field);
    SCP_PD("LowpassFilter:%u\n", LowpassFilter);
}

uchar SCPSection1::getFilterBitmap()
{
    int p = _SearchField(29);

    if ((p >= 0)
        && (_Fields[p].Length == 1)
        && (_Fields[p].Value != null))
    {
        return (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(uchar), true);
    }

    return 0;
}

void SCPSection1::setFilterBitmap(uchar FilterBitmap)
{
    if (FilterBitmap != 0)
    {
        SCPHeaderField field;
        field.Tag = 29;
        field.Length = (ushort) sizeof(FilterBitmap);
        field.Value = new uchar[field.Length];
        BytesTool::writeBytes(FilterBitmap, field.Value, field.Length, 0, sizeof(FilterBitmap), true);
        Insert(field);
        SCP_PD("LowpassFilter:%hhu\n", FilterBitmap);
        return;
    }
}

std::vector<string> SCPSection1::getFreeTextFields()
{
    std::vector<string> textVector;
    int p = _SearchField(30);

    if (p >= 0)
    {
        for (; (p > 0) && (_Fields[p - 1].Tag == 30); p--) {}

        int len = 0;

        for (; ((p + len) < _NrFields) && (_Fields[p + len].Tag == 30); len++) {}

        if (len > 0)
        {
            for (int loper = 0; loper < len; loper++)
            {
                if (_Fields[p + loper].Value != null)
                {
                    textVector.push_back(BytesTool::readString(_Encoding,
                                         _Fields[p + loper].Value,
                                         _Fields[p + loper].Length,
                                         0,
                                         _Fields[p + loper].Length));
                }
            }
        }
    }

    return textVector;
}

void SCPSection1::setFreeTextFields(const std::vector<string>& FreeTextFields)
{
    int size = FreeTextFields.size();

    for (int loper = 0; loper < size; loper++)
    {
        if (FreeTextFields[loper].length() > 0)
        {
            SCPHeaderField field;
            field.Tag = 30;
            field.Length = (ushort)(FreeTextFields[loper].length() >= _ExceptionsMaximumLength ?
                                    _ExceptionsMaximumLength : FreeTextFields[loper].length() + 1);
            field.Value = new uchar[field.Length];
            BytesTool::writeString(_Encoding, FreeTextFields[loper], field.Value, field.Length, 0, field.Length);
            Insert(field);
            SCP_PD("loper:%d,FreeTextFields:%s\n", loper, FreeTextFields[loper].c_str());
        }
    }
}

string SCPSection1::getSequenceNr()
{
    return getText(31);
}

void SCPSection1::setSequenceNr(const string& SequenceNr)
{
    setText(31, SequenceNr);
}

string SCPSection1::getAcqInstitution()
{
    return getText(16);
}

void SCPSection1::setAcqInstitution(const string&  AcqInstitution)
{
    setText(16, AcqInstitution);
}

string SCPSection1::getAnalyzingInstitution()
{
    return getText(17);
}

void SCPSection1::setAnalyzingInstitution(const string&  AnalyzingInstitution)
{
    setText(17, AnalyzingInstitution);
}

string SCPSection1::getAcqDepartment()
{
    return getText(18);
}

void SCPSection1::setAcqDepartment(const string&  AcqDepartment)
{
    setText(18, AcqDepartment);
}

string SCPSection1::getAnalyzingDepartment()
{
    return getText(19);
}

void SCPSection1::setAnalyzingDepartment(const string&  AnalyzingDepartment)
{
    setText(19, AnalyzingDepartment);
}

string SCPSection1::getReferringPhysician()
{
    return getText(20);
}

void SCPSection1::setReferringPhysician(const string&  ReferringPhysician)
{
    setText(20, ReferringPhysician);
}

string SCPSection1::getOverreadingPhysician()
{
    return getText(21);
}

void SCPSection1::setOverreadingPhysician(const string&  OverreadingPhysician)
{
    setText(21, OverreadingPhysician);
}

string SCPSection1::getTechnicianDescription()
{
    return getText(22);
}

void SCPSection1::setTechnicianDescription(const string&  TechnicianDescription)
{
    setText(22, TechnicianDescription);
}

ushort SCPSection1::getSystolicBloodPressure()
{
    int p = _SearchField(11);

    if (p >= 0)
    {
        return (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(ushort), true);
    }

    return 0;
}

void SCPSection1::setSystolicBloodPressure(ushort SystolicBloodPressure)
{
    if (SystolicBloodPressure != 0)
    {
        SCPHeaderField field;
        field.Tag = 11;
        field.Length = (uchar) sizeof(SystolicBloodPressure);
        field.Value = new uchar[field.Length];
        BytesTool::writeBytes(SystolicBloodPressure, field.Value, field.Length, 0, field.Length, true);
        Insert(field);
    }
}

ushort SCPSection1::getDiastolicBloodPressure()
{
    int p = _SearchField(12);

    if (p >= 0)
    {
        return (ushort) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(ushort), true);
    }

    return 0;
}

void SCPSection1::setDiastolicBloodPressure(ushort DiastolicBloodPressure)
{
    if (DiastolicBloodPressure != 0)
    {
        SCPHeaderField field;
        field.Tag = 12;
        field.Length = (ushort) sizeof(DiastolicBloodPressure);
        field.Value = new uchar[field.Length];
        BytesTool::writeBytes(DiastolicBloodPressure, field.Value, field.Length, 0, field.Length, true);
        Insert(field);
    }
}

std::vector<Drug> SCPSection1::getDrugs()
{
    std::vector<Drug> drugVector;
    int p = _SearchField(10);

    if (p >= 0)
    {
        for (; (p > 0) && (_Fields[p - 1].Tag == 10); p--) {}

        int len = 0;

        for (; ((p + len) < _NrFields) && (_Fields[p + len].Tag == 10); len++) {}

        if (len > 0)
        {
            for (int loper = 0; loper < len; loper++)
            {
                if ((_Fields[p + loper].Length > 3)
                    && (_Fields[p + loper].Value != null))
                {
                    Drug drug;
                    drug.DrugClass = _Fields[p + loper].Value[1];
                    drug.ClassCode = _Fields[p + loper].Value[2];
                    drug.TextDesciption = BytesTool::readString(_Encoding,
                                          _Fields[p + loper].Value,
                                          _Fields[p + loper].Length,
                                          3,
                                          _Fields[p + loper].Length - 3);
                    drugVector.push_back(drug);
                }
            }
        }
    }

    return drugVector;
}

void SCPSection1::setDrugs(std::vector<Drug>& Drugs)
{
    int size = Drugs.size();

    for (int loper = 0; loper < size; loper++)
    {
        SCPHeaderField field;
        field.Tag = 10;
        field.Length = (ushort)(4 +
                                (Drugs[loper].TextDesciption.length() > 0 ? Drugs[loper].TextDesciption.length() + 1 : 0));
        field.Value = new uchar[field.Length];
        field.Value[0] = 0;
        field.Value[1] = Drugs[loper].DrugClass;
        field.Value[2] = Drugs[loper].ClassCode;
        BytesTool::writeString(_Encoding,
                               Drugs[loper].TextDesciption,
                               field.Value,
                               field.Length,
                               3,
                               field.Length - 3);
        Insert(field);
    }
}

std::vector<string> SCPSection1::getReferralIndication()
{
    std::vector<string> textVector;
    int p = _SearchField(13);

    if (p >= 0)
    {
        for (; (p > 0) && (_Fields[p - 1].Tag == 13); p--) {}

        int len = 0;

        for (; ((p + len) < _NrFields) && (_Fields[p + len].Tag == 13); len++) {}

        if (len > 0)
        {
            for (int loper = 0; loper < len; loper++)
            {
                if ((_Fields[p + loper].Length > 0) && (_Fields[p + loper].Value != null))
                {
                    textVector.push_back(BytesTool::readString(_Encoding,
                                         _Fields[p + loper].Value,
                                         _Fields[p + loper].Length,
                                         0,
                                         _Fields[p + loper].Length));
                }
            }
        }
    }

    return textVector;
}

void SCPSection1::setReferralIndication(const std::vector<string>& ReferralIndication)
{
    int size = ReferralIndication.size();

    for (int loper = 0; loper < size; loper++)
    {
        if (ReferralIndication[loper].length() > 0)
        {
            SCPHeaderField field;
            field.Tag = 13;
            field.Length = (ushort)(ReferralIndication[loper].length() >= _ExceptionsMaximumLength ?
                                    _ExceptionsMaximumLength : ReferralIndication[loper].length() + 1);
            field.Value = new uchar[field.Length];
            BytesTool::writeString(_Encoding, ReferralIndication[loper], field.Value, field.Length, 0, field.Length);
            Insert(field);
        }
    }
}

string SCPSection1::getRoomDescription()
{
    return getText(23);
}

void SCPSection1::setRoomDescription(const string& RoomDescription)
{
    setText(23, RoomDescription);
}

uchar SCPSection1::getStatCode()
{
    int p = _SearchField(24);

    if ((p >= 0)
        && (_Fields[p].Value != null)
        && (_Fields[p].Length == sizeof(uchar)))
    {
        return (uchar) BytesTool::readBytes(_Fields[p].Value, _Fields[p].Length, 0, sizeof(uchar), true);
    }

    return 0xff;
}

void SCPSection1::setStatCode(uchar StatCode)
{
    if (StatCode != 0xff)
    {
        SCPHeaderField field;
        field.Tag = 24;
        field.Length = (ushort) sizeof(StatCode);
        field.Value = new uchar[field.Length];
        BytesTool::writeBytes(StatCode, field.Value, field.Length, 0, sizeof(StatCode), true);
        Insert(field);
    }
}

}
}
