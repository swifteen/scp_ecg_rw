#include "SCPSection.h"
#include "SCPFormat.h"
#include "CRCTool.h"
#include "BytesTool.h"

using namespace Communication_IO_Tools;

namespace ECGConversion
{
namespace SCP
{
int SCPSection::Size = 16;
/// <summary>
/// Constructor for a SCP Section.
/// </summary>
SCPSection::SCPSection()
{
    _Encoding = "GB18030";
    SectionID = 0;
    memset(Reserved, 0, sizeof(Reserved));
    SectionID = getSectionID();
    //    Empty();//Usually you get this error when call your virtual function from constructor or destructor.
}
SCPSection::~SCPSection()
{
}

/// <summary>
/// Set encoding used for section.
/// </summary>
/// <param name="enc">encoding to use in section.</param>
void SCPSection::SetEncoding(const string& encoding)
{
    _Encoding = encoding;
}
/// <summary>
/// Function to read an SCP Section.
/// </summary>
/// <param name="buffer">buffer to read from</param>
/// <param name="offset">position on buffer to start reading</param>
/// <returns>error:
/// 0x00) succes
/// 0x01) no buffer provided or buffer to small for header
/// 0x02) Section ID doesn't seem to be right
/// 0x04) buffer not big enough for entire section
/// 0x08) CRC Check Failed
/// rest) Section specific error </returns>
int SCPSection::Read(uchar* buffer, int bufferLength, int offset, int length)
{
    Empty();

    if ((buffer != null)
        && (offset + Size) <= bufferLength)
    {
        int crcoffset = offset;
        CRC = (ushort) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(CRC), true);
        offset += sizeof(CRC);
        SectionID = (ushort) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(SectionID), true);

        if (SectionID != getSectionID())
        {
            return 0x2;
        }

        offset += sizeof(SectionID);
        Length = (int) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(Length), true);

        if (((length != Length)
             && (SectionID != 0))
            || (crcoffset + Length) > bufferLength)
        {
            return 0x4;
        }

        offset += sizeof(Length);
        CRCTool crc;
        crc.Init(CRCTool::CRCCode::CRC_CCITT);

        if (CRC != crc.CalcCRCITT(buffer, bufferLength, crcoffset + sizeof(CRC), Length - sizeof(CRC)))
        {
            return 0x8;
        }

        SectionVersionNr = (uchar) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(SectionVersionNr), true);
        offset += sizeof(SectionVersionNr);
        ProtocolVersionNr = (uchar) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(ProtocolVersionNr), true);
        offset += sizeof(ProtocolVersionNr);
        offset += BytesTool::copy(Reserved, sizeof(Reserved), 0, buffer, bufferLength, offset, kReservedLength);
        return _Read(buffer, bufferLength, offset) << 4;
    }

    return 0x1;
}

#if 0
/// <summary>
/// Function to write an SCP Section.
/// </summary>
/// <param name="buffer">buffer allocated to write in</param>
/// <returns>error:
/// 0x00) succes
/// 0x01) section incorrect
/// 0x02) no buffer provided or buffer to small for header
/// rest) Section specific error </returns>
int SCPSection::Write(out byte[] buffer)
{
    buffer = null;

    if (Works())
    {
        buffer = new byte[_getLength() + Size];

        if (buffer.Length <= Size)
        {
            buffer = null;
            return 0;
        }

        int err = Write(buffer, 0);

        if (err != 0)
        {
            buffer = null;
        }

        return err;
    }

    return 0x1;
}
#endif
/// <summary>
/// Function to write an SCP Section.
/// </summary>
/// <param name="buffer">buffer to write to</param>
/// <param name="offset">position on buffer to start writing</param>
/// <returns>error:
/// 0x00) succes
/// 0x01) section incorrect
/// 0x02) no buffer provided or buffer to small for header
/// rest) Section specific error </returns>
int SCPSection::Write(uchar* buffer, int bufferLength, int offset)
{
    Length = _getLength() + Size;

    if (Length == Size)
    {
        return 0;
    }

    if (Works())
    {
        if ((buffer != null)
            && ((offset + Length) <= bufferLength))
        {
            int crcoffset = offset;
            offset += sizeof(CRC);
            SectionID = getSectionID();
            BytesTool::writeBytes(SectionID, buffer, bufferLength, offset, sizeof(SectionID), true);
            offset += sizeof(SectionID);
            BytesTool::writeBytes(Length, buffer, bufferLength, offset, sizeof(Length), true);
            offset += sizeof(Length);
            BytesTool::writeBytes(SectionVersionNr, buffer, bufferLength, offset, sizeof(SectionVersionNr), true);
            offset += sizeof(SectionVersionNr);
            BytesTool::writeBytes(ProtocolVersionNr, buffer, bufferLength, offset, sizeof(ProtocolVersionNr), true);
            offset += sizeof(ProtocolVersionNr);
            offset += BytesTool::copy(buffer, bufferLength, offset, Reserved, kReservedLength, 0, kReservedLength);
            int err = _Write(buffer, bufferLength, offset);

            if (err == 0)
            {
                CRCTool crc;
                crc.Init(CRCTool::CRC_CCITT);
                CRC = crc.CalcCRCITT(buffer, bufferLength, crcoffset + sizeof(CRC), Length - sizeof(CRC));
                BytesTool::writeBytes(CRC, buffer, bufferLength, crcoffset, sizeof(CRC), true);
            }
            else
            {
                SCP_PW("_Write failed,offset: %d,offset: %d,bufferLength: %d\n", offset, offset, bufferLength);
            }

            return err << 2;
        }

        SCP_PW("out of range,offset: %d,Length: %d,bufferLength: %d\n", offset, Length, bufferLength);
        return 0x2;
    }

    SCP_PW("not work\n");
    return 0x1;
}
/// <summary>
/// Function to empty a SCP section.
/// </summary>
void SCPSection::Empty()
{
    CRC = 0;
    SectionID = getSectionID();
    Length = 0;
    SectionVersionNr = SCPFormat::DefaultSectionVersion;
    ProtocolVersionNr = SCPFormat::DefaultProtocolVersion;
    Reserved[0] = 0;
    Reserved[1] = 0;
    Reserved[2] = 0;
    Reserved[3] = 0;
    Reserved[4] = 0;
    Reserved[5] = 0;
    _Empty();
}
/// <summary>
/// Function to get the length of a SCP section.
/// </summary>
/// <returns>length of section</returns>
int SCPSection::getLength()
{
    int len = _getLength();
    return (len == 0 ? 0 : len + Size);
}
}
}
