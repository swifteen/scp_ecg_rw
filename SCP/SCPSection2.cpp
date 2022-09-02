#include "SCPSection2.h"
#include "CRCTool.h"
#include "BytesTool.h"
#include <stdlib.h>

using namespace Communication_IO_Tools;

namespace ECGConversion
{
namespace SCP
{
/// <summary>
/// Class that contains a SCP Huffman struct
/// </summary>
class SCPHuffmanStruct
{
public:
    SCPHuffmanStruct()
    {}
    SCPHuffmanStruct(uchar prefix, uchar entire, uchar tablemode, short value, uint code)
    {
        this->prefix = prefix;
        this->entire = entire;
        this->tablemode = tablemode;
        this->value = value;
        this->code = code;
    }
    /// <summary>
    /// Function to read an SCP huffman struct.
    /// </summary>
    /// <param name="buffer">byte array to read from</param>
    /// <param name="offset">position to start reading</param>
    /// <returns>0 on success</returns>
    int Read(uchar* buffer, int bufferLength, int offset)
    {
        if ((offset + Size) > buffer.Length)
        {
            return 0x1;
        }

        prefix = (uchar) BytesTool.readBytes(buffer, offset, sizeof(prefix), true);
        offset += sizeof(prefix);
        entire = (uchar) BytesTool.readBytes(buffer, offset, sizeof(entire), true);
        offset += sizeof(entire);
        tablemode = (uchar) BytesTool.readBytes(buffer, offset, sizeof(tablemode), true);
        offset += sizeof(tablemode);
        value = (short) BytesTool.readBytes(buffer, offset, sizeof(value), true);
        offset += sizeof(value);
        uint tempCode = (uchar) BytesTool.readBytes(buffer, offset, sizeof(code), true);
        offset += sizeof(code);
        // Have to reverse the code, because SCP stores it that way.
        code = 0;

        for (int loper = prefix; loper > 0; loper--)
        {
            code <<= 1;
            code |= (tempCode & 0x1);
            tempCode >>= 1;
        }

        return 0x0;
    }

    /// <summary>
    /// Function to write SCP huffman struct.
    /// </summary>
    /// <param name="buffer">byte array to write into</param>
    /// <param name="offset">position to start writing</param>
    /// <returns>0 on success</returns>
    int Write(uchar* buffer, int bufferLength, int offset)
    {
        if ((offset + Size) > bufferLength)
        {
            return 0x1;
        }

        BytesTool::writeBytes(prefix, buffer, bufferLength, offset, sizeof(prefix), true);
        offset += sizeof(prefix);
        BytesTool::writeBytes(entire, buffer, bufferLength, offset, sizeof(entire), true);
        offset += sizeof(entire);
        BytesTool::writeBytes(tablemode, buffer, bufferLength, offset, sizeof(tablemode), true);
        offset += sizeof(tablemode);
        BytesTool::writeBytes(this->value, buffer, bufferLength, offset, sizeof(this->value), true);
        offset += sizeof(value);
        // Have to reverse the code, becaus SCP stores it that way.
        uint tempCode1 = code;
        uint tempCode2 = 0;

        for (int loper = prefix; loper > 0; loper--)
        {
            tempCode2 <<= 1;
            tempCode2 |= (tempCode1 & 0x1);
            tempCode1 >>= 1;
        }

        BytesTool::writeBytes((int)tempCode2, buffer, bufferLength, offset, sizeof(code), true);
        offset += sizeof(code);
        return 0x0;
    }
public:
    static int Size;

    uchar prefix;
    uchar entire;
    uchar tablemode;
    short value;
    uint code;
};

int SCPHuffmanStruct::Size = 9;

// Defined in SCP.
ushort SCPSection2::_SectionID = 2;
ushort SCPSection2::_DefaultTable = 19999;
/// <summary>
/// Class contains section 2 (HuffmanTables).
/// </summary>
/// <remarks>
/// SCP uses a diffrent way to store codes, then UNIPRO. because I preffer the way UNIPRO
/// stores its codes. I store the SCP codes the same way as UNIPRO, but when I read/write
/// them from/to a buffer I reverse the code. This solution keeps the working of the SCP
/// and UNIPRO decode/encode very simalar, which is always a positive thing.
/// </remarks>
SCPSection2::SCPSection2()
{
    SCPSection::Empty();
    // currently selected table.
    _Selected = 0;
    // Part of the stored Data Structure.
    _NrTables = 0;
#if 0 //use DefaultTable
    _Tables = null;
#endif
}

int SCPSection2::_Read(uchar* buffer, int bufferLength, int offset)
{
    int end = offset - Size + Length;

    if ((offset + sizeof(_NrTables)) > end)
    {
        return 0x1;
    }

    _NrTables = (ushort) BytesTool.readBytes(buffer, offset, sizeof(_NrTables), true);
    offset += sizeof(_NrTables);

    if (_NrTables < _DefaultTable)
    {
#if 0 //TODO
        _Tables = new SCPHuffmanStruct[_NrTables][];

        for (int table = 0; table < _NrTables; table++)
        {
            if ((offset + sizeof(_NrTables)) > end)
            {
                _Empty();
                return 0x2;
            }

            _Tables[table] = new SCPHuffmanStruct[BytesTool.readBytes(buffer, offset, sizeof(_NrTables), true)];
            offset += sizeof(_NrTables);

            if ((offset + (_Tables[table].Length * SCPHuffmanStruct.Size)) > end)
            {
                _Empty();
                return 0x4;
            }

            for (int loper = 0; loper < _Tables[table].Length; loper++)
            {
                _Tables[table][loper] = new SCPHuffmanStruct();
                int err = _Tables[table][loper].Read(buffer, offset);

                if (err != 0)
                {
                    return err << 3 + table;
                }

                offset += SCPHuffmanStruct.Size;
            }
        }

#endif
    }

    return 0x0;
}

int SCPSection2::_Write(uchar* buffer, int bufferLength, int offset)
{
    BytesTool::writeBytes(_NrTables, buffer, bufferLength, offset, sizeof(_NrTables), true);
    offset += sizeof(_NrTables);
#if 0 //use DefaultTable

    if (_NrTables < _DefaultTable)
    {
        for (int table = 0; table < _NrTables; table++)
        {
            BytesTool::writeBytes(_Tables[table].Length, buffer, bufferLength, offset, sizeof(_NrTables), true);
            offset += sizeof(_NrTables);

            for (int loper = 0; loper < _Tables[table].Length; loper++)
            {
                int err = _Tables[table][loper].Write(buffer, bufferLength, offset);

                if (err != 0)
                {
                    return err << table;
                }

                offset += SCPHuffmanStruct::Size;
            }
        }
    }

#endif
    return 0x0;
}
void SCPSection2::_Empty()
{
#if 0 //use DefaultTable
    _Tables = null;
#endif
    _NrTables = 0;
}
int SCPSection2::_getLength()
{
    if (Works())
    {
        int sum = sizeof(_NrTables);
#if 0 //use DefaultTable

        if (_NrTables != _DefaultTable)
        {
            for (int table = 0; table < _NrTables; table++)
            {
                sum += sizeof(_NrTables) + (_Tables[table].Length * SCPHuffmanStruct::Size);
            }
        }

#endif
        return ((sum % 2) == 0 ? sum : sum + 1);
    }

    return 0;
}
ushort SCPSection2::getSectionID()
{
    return _SectionID;
}
bool SCPSection2::Works()
{
    return true;
#if 0 //use DefaultTable

    if ((_Tables != null)
        && (_NrTables == _Tables.Length))
    {
        for (int table = 0; table < _Tables.Length; table++)
        {
            if (_Tables[table] == null)
            {
                return false;
            }

            for (int loper = 0; loper < _Tables[table].Length; loper++)
            {
                if (_Tables[table][loper] == null)
                {
                    return false;
                }
            }
        }

        return true;
    }
    else if ((_Tables == null)
             && ((_NrTables == _DefaultTable)
                 || (_NrTables == 0)))
    {
        return true;
    }

    return false;
#endif
}
/// <summary>
/// Function to decode encoded data.
/// </summary>
/// <param name="buffer">buffer to read in</param>
/// <param name="offset">position to start reading</param>
/// <param name="nrbytes">nrbytes of encoded bytes in buffer</param>
/// <param name="length">length of signal in samples</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>short array containing decoded data</returns>
short* SCPSection2::Decode(uchar* buffer, int bufferLength, int offset, int nrbytes, int length, uchar difference)
{
    if (Works() || (_NrTables == 0))
    {
        if (_NrTables == _DefaultTable)
        {
            return InhouseDecode(buffer, bufferLength, offset, nrbytes, length, difference);
        }

#if 0 //TODO
        else if (_NrTables == 0)
        {
            return NoDecode(buffer, offset, nrbytes, length, difference);
        }
        else
        {
            return HuffmanTableDecode(buffer, offset, nrbytes, length, difference);
        }

#endif
    }

    return null;
}
/// <summary>
/// Function to do huffman decode of encoded data. (using SCP default huffmantable)
/// </summary>
/// <param name="buffer">buffer to read in</param>
/// <param name="offset">position to start reading</param>
/// <param name="nrbytes">nrbytes of encoded bytes in buffer</param>
/// <param name="length">length of signal in samples</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>short array containing decoded data</returns>
static short* SCPSection2::InhouseDecode(uchar* buffer, int bufferLength, int offset, int nrbytes, int length, uchar difference)
{
    // This safes us some calculations.
    nrbytes += offset;

    // Check if input data makes sense.
    if ((buffer != null)
        && (nrbytes <= bufferLength))
    {
        // Setting up the variables for decode.
        short* leadData = new short[length];
        int currentTime = 0;
        int currentBit = (offset << 3);
        int max = 9;

        while (((currentBit >> 3) < nrbytes)
               && ((currentTime) < length))
        {
            int count = currentBit;
            int cmax = currentBit + max;

            // Read in bits till 0 bit or defined maximum.
            for (; (currentBit < cmax) && ((currentBit >> 3) < nrbytes) && (((buffer[currentBit >> 3] >> (0x7 - (currentBit & 0x7))) & 0x1) != 0); currentBit++);

            // determine number of bits
            count = currentBit - count;

            // Increase current bit one more time
            if (count != max)
            {
                currentBit++;
            }

            // If it doesn't fit stop
            if ((currentBit >> 3) >= nrbytes)
            {
                break;
            }

            if (count >= max)
            {
                // Read in last bit
                bool tmp = (((buffer[currentBit >> 3] >> (0x7 - (currentBit & 0x7))) & 0x1) == 0);
                currentBit++;
                // store starting point of additional bits.
                int start = currentBit;
                // If last bit 0 read in 8 additional bits else 16 additional bits.
                int stop = currentBit + (tmp ? 8 : 16);

                // If it doesn't fit return with error
                if ((stop >> 3) >= nrbytes)
                {
                    break;
                }

                // Reading in number of extra  bits.
                for (count = 0; currentBit < stop; currentBit++)
                {
                    count <<= 1;
                    count |= ((buffer[currentBit >> 3] >> (0x7 - (currentBit & 0x7))) & 0x1);

                    if ((start == currentBit)
                        && (count != 0))
                    {
                        count = -1;
                    }
                }
            }
            else if (count != 0)
            {
                // If it doesn't fit stop
                if ((currentBit >> 3) >= nrbytes)
                {
                    break;
                }

                // if last bit is one value is negative.
                if (((buffer[currentBit >> 3] >> (0x7 - (currentBit & 0x7))) & 0x1) != 0)
                {
                    count = -count;
                }

                currentBit++;
            }

            // Decode Differences.
            switch (difference)
            {
                case 0:
                    leadData[currentTime] = ((short) count);
                    break;

                case 1:
                    leadData[currentTime] = ((currentTime == 0) ? (short) count : (short)(count + leadData[currentTime - 1]));
                    break;

                case 2:
                    leadData[currentTime] = ((currentTime < 2) ? (short) count : (short)(count + (leadData[currentTime - 1] << 1) - leadData[currentTime - 2]));
                    break;

                default:
                    // Undefined difference used exit empty.
                    return null;
            }

            // Increment time by one.
            currentTime++;
        }

        return leadData;
    }

    return null;
}
#if 0//TODO
/// <summary>
/// Function to do huffman decode of encoded data.
/// </summary>
/// <param name="buffer">buffer to read in</param>
/// <param name="offset">position to start reading</param>
/// <param name="nrbytes">nrbytes of encoded bytes in buffer</param>
/// <param name="length">length of signal in samples</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>short array containing decoded data</returns>
short[] SCPSection2::HuffmanTableDecode(byte[] buffer, int offset, int nrbytes, int length, byte difference)
{
    // This safes us some calculations.
    nrbytes += offset;

    // Check if input data makes sense.
    if ((buffer != null)
        && (nrbytes <= buffer.Length))
    {
        // Setting up the variables for decode.
        short[] leadData = new short[length];
        int currentTime = 0;
        int currentBit = (offset << 3);

        while (((currentBit >> 3) < nrbytes)
               && ((currentTime) < length))
        {
            // Search for a hit.
            SCPHuffmanStruct h = InterpettingData(buffer, currentBit);

            // Exit if there was no hit.
            if (h == null)
            {
                return null;
            }

            // Check if hit fits.
            if (((currentBit + h.entire) >> 3) >= nrbytes)
            {
                break;
            }

            // If table mode is 0 do switch.
            if (h.tablemode == 0)
            {
                _Selected = h.value - 1;
                continue;
            }

            short code = 0;

            // read extra data behind hit if available.
            for (int count = 0, start = (currentBit + h.prefix); count < (h.entire - h.prefix); count++)
            {
                code <<= 1;
                code += (short)((buffer[(start + count) >> 3] >> (0x7 - ((start + count) & 0x7))) & 0x1);

                if ((count == 0)
                    && (code != 0))
                {
                    code = -1;
                }
            }

            // add up a the value of the hit.
            code += h.value;

            // Decode Differences.
            switch (difference)
            {
                case 0:
                    leadData[currentTime] = code;
                    break;

                case 1:
                    leadData[currentTime] = ((currentTime == 0) ? code : (short)(code + leadData[currentTime - 1]));
                    break;

                case 2:
                    leadData[currentTime] = ((currentTime < 2) ? code : (short)(code + (leadData[currentTime - 1] << 1) - leadData[currentTime - 2]));
                    break;

                default:
                    // Undefined difference used exit empty.
                    return null;
            }

            // Increment current bit
            currentBit += h.entire;
            // Increment time by one.
            currentTime++;
        }

        return leadData;
    }

    return null;
}
/// <summary>
/// Find next hit in byte array starting at an offset in bits.
/// </summary>
/// <param name="buffer">byte array containing encoded data</param>
/// <param name="offset">position (in bits) to start searching for a hit</param>
/// <returns>Info of hit.</returns>
SCPHuffmanStruct SCPSection2::InterpettingData(byte[] buffer, int offset)
{
    if ((_Tables[_Selected] != null)
        && (buffer != null))
    {
        uint bitBuffer = 0;
        int readMax = _Tables[_Selected][_Tables[_Selected].Length - 1].prefix;

        for (int read = 0; read < readMax && ((read + offset) >> 3) < buffer.Length; read++)
        {
            bitBuffer <<= 1;
            bitBuffer |= (uint)((buffer[(offset + read) >> 3] >> (0x7 - ((offset + read) & 0x7))) & 0x1);

            for (int table = 0; table < _Tables[_Selected].Length; table++)
            {
                if ((bitBuffer == _Tables[_Selected][table].code)
                    && ((read + 1) == _Tables[_Selected][table].prefix))
                {
                    return _Tables[_Selected][table];
                }
            }
        }
    }

    return null;
}
/// <summary>
/// Function to do interpetting of unencoded data.
/// </summary>
/// <param name="buffer">buffer to read in</param>
/// <param name="offset">position to start reading</param>
/// <param name="nrbytes">nrbytes of encoded bytes in buffer</param>
/// <param name="length">length of signal in samples</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>short array containing decoded data</returns>
static short[] SCPSection2::NoDecode(uchar* buffer, int bufferLength, int offset, int nrbytes, int length, uchar difference)
{
    // Check if input data makes sense.
    if ((buffer != null)
        && ((offset + nrbytes) <= bufferLength)
        && ((length * sizeof(short)) <= nrbytes))
    {
        short[] leadData = new short[length];

        for (int currentTime = 0; currentTime < length; currentTime++)
        {
            short code = (short) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(short), true);
            offset += sizeof(short);

            // Decode Differences.
            switch (difference)
            {
                case 0:
                    leadData[currentTime] = code;
                    break;

                case 1:
                    leadData[currentTime] = ((currentTime == 0) ? code : (short)(code + leadData[currentTime - 1]));
                    break;

                case 2:
                    leadData[currentTime] = ((currentTime < 2) ? code : (short)(code + (leadData[currentTime - 1] << 1) - leadData[currentTime - 2]));
                    break;

                default:
                    // Undefined difference used exit empty.
                    return null;
            }
        }

        return leadData;
    }

    return null;
}
#endif
/// <summary>
/// Function to encode data.
/// </summary>
/// <param name="data">signal to read from</param>
/// <param name="time">number of samples to use</param>
/// <param name="usedTable">table to use for encoding</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>byte array containing encoded data</returns>
uchar* SCPSection2::Encode(short* buffer, int dataLength, int time, short usedTable, uchar difference, int* encodeLength)
{
    if (Works() || _NrTables == 0)
    {
        if (_NrTables == _DefaultTable)
        {
            return InhouseEncode(buffer, dataLength, time, difference, encodeLength);
        }

#if 0 //use DefaultTable
        else if (_NrTables == 0)
        {
            return NoEncode(data, time, difference);
        }
        else
        {
            return HuffmanTableEncode(data, time, usedTable, difference);
        }

#endif
    }

    return null;
}
/// <summary>
/// Function to encode signal using the default huffman table (using optimized code).
/// </summary>
/// <param name="data">signal to read from</param>
/// <param name="time">number of samples to use</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>byte array containing encoded data</returns>
uchar* SCPSection2::InhouseEncode(short* data, int dataLength, int time, uchar difference, int* encodeLength)
{
    uchar* ret = null;

    // Check if input makes sense
    if ((data != null)
        && (time <= dataLength))
    {
        // Initialize some handy variables
        int currentBit = 0;
        // Make buffer for worst case.
        uchar* buffer = new uchar[((time * 26) >> 3) + 1];

        // For each sample do encode.
        for (int currentTime = 0; currentTime < time; currentTime++)
        {
            short code = 0;

            // Encode Differences.
            switch (difference)
            {
                case 0:
                    code = data[currentTime];
                    break;

                case 1:
                    code = (short)((currentTime < 1) ? data[currentTime] : data[currentTime] - data[currentTime - 1]);
                    break;

                case 2:
                    code = (short)((currentTime < 2) ? data[currentTime] : data[currentTime] - (data[currentTime - 1] << 1) + data[currentTime - 2]);
                    break;

                default:
                    // Undefined difference used exit empty.
                    return null;
            }

            // Do inhouse encode
            //System.Diagnostics.Debug.WriteLine("Begin inhouse!"+code.ToString());
            if (code == -32768)
            {
                code = 32767;
            }

            if (abs(code) <= 8)
            {
                //  System.Diagnostics.Debug.WriteLine("Begin !");
                // if code 0 then add one 0 bit.
                if (code == 0)
                {
                    buffer[currentBit >> 3] <<= 1;
                    currentBit++;
                }
                else
                {
                    // add absolute number of 1 bits.
                    int codeAbs = abs(code);

                    for (int loper = 0; loper < codeAbs; loper++)
                    {
                        buffer[currentBit >> 3] <<= 1;
                        buffer[currentBit >> 3] |= 1;
                        currentBit++;
                    }

                    // add one 0 bit.
                    buffer[currentBit >> 3] <<= 1;
                    currentBit++;
                    // add one more bit for positive of negative
                    buffer[currentBit >> 3] <<= 1;

                    if (code < 0)
                    {
                        buffer[currentBit >> 3] |= 1;
                    }

                    currentBit++;
                }
            }
            else
            {
                // Code doesn't fit in normal table do special.
                // First add nine 1 bits.
                for (int loper = 0; loper < 9; loper++)
                {
                    buffer[currentBit >> 3] <<= 1;
                    buffer[currentBit >> 3] |= 1;
                    currentBit++;
                }

                // Add one more bit depending on size of code
                buffer[currentBit >> 3] <<= 1;
                int extraLength = 8;

                if (!((code <= 127) && (code >= -128)))
                {
                    buffer[currentBit >> 3] |= 1;
                    extraLength = 16;
                }

                currentBit++;

                // Add bits for extra code.
                for (extraLength--; extraLength >= 0; extraLength--)
                {
                    buffer[currentBit >> 3] <<= 1;
                    buffer[currentBit >> 3] |= (uchar)((code >> extraLength) & 0x1);
                    currentBit++;
                    int currentBitShift = currentBit >> 3;
                }
            }
        }

        // Shift end to right position.
        if ((currentBit & 0x7) != 0x0)
        {
            buffer[(currentBit >> 3)] <<= (0x8 - (currentBit & 0x7));
            currentBit += (0x8 - (currentBit & 0x7));
        }
        else
        {
            // seems to solve a small encoding bug.
            currentBit += 8;
        }

        // Allocate a fitting buffer
        int retLength = currentBit >> 3;
        ret = new uchar[retLength];
        // Copy worst case buffer in fitting buffer.
#if 0

        for (int loper = 0; loper < retLength; loper++)
        {
            ret[loper] = buffer[loper];
        }

#else
        memcpy(ret, buffer, retLength * sizeof(uchar));
        *encodeLength = retLength;
        delete[] buffer;
        buffer = null;
#endif
    }

    return ret;
}
#if 0 //use DefaultTable
/// <summary>
/// Function to encode signal using the huffman table.
/// </summary>
/// <param name="data">signal to read from</param>
/// <param name="time">number of samples to use</param>
/// <param name="quanta">sample distance in signal</param>
/// <param name="usedTable">table to use for encoding</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>byte array containing encoded data</returns>
byte[] SCPSection2::HuffmanTableEncode(short[] data, int time, short usedTable, uchar difference)
{
    byte[] ret = null;

    // Check if input makes sense
    if ((data != null)
        && (time <= data.Length))
    {
        // Initialize some handy variables
        int currentBit = 0;
        // Make buffer for worst case.
        byte[] buffer = null;

        if ((usedTable >= 0)
            && (usedTable < _Tables.Length)
            && (usedTable != _Selected))
        {
            uint code = 0;
            int len = 0;
            // get TableSwap position in HuffmanTable.
            int p = getTableSwap(usedTable);

            // Check if table swap is possible in this table.
            if (p >= 0)
            {
                // Store needed data from swap HuffmanStruct.
                code = _Tables[_Selected][p].code;
                len = _Tables[_Selected][p].entire;
                // set currently selected table.
                _Selected = usedTable;
            }

            // allocate buffer for worstcase.
            buffer = new byte[((len + (time * getWorstCase())) >> 3) + 1];

            // add table swap.
            for (len--; len >= 0; len--)
            {
                buffer[currentBit >> 3] <<= 1;
                buffer[currentBit >> 3] |= (uchar)((code >> len) & 0x1);
                currentBit++;
            }
        }
        else
        {
            // No tables swap, so only space needed for worst case.
            buffer = new byte[((time * getWorstCase()) >> 3) + 1];
        }

        // For each sample do encode.
        for (int currentTime = 0; currentTime < time; currentTime++)
        {
            short code = 0;

            // Encode Differences.
            switch (difference)
            {
                case 0:
                    code = data[currentTime];
                    break;

                case 1:
                    code = (short)((currentTime < 1) ? data[currentTime] : data[currentTime] - data[currentTime - 1]);
                    break;

                case 2:
                    code = (short)((currentTime < 2) ? data[currentTime] : data[currentTime] - (data[currentTime - 1] << 1) + data[currentTime - 2]);
                    break;

                default:
                    // Undefined difference used exit empty.
                    return null;
            }

            // Call Interpetting data to get an hit.
            SCPHuffmanStruct h = InterpettingData(code);

            if (h == null)
            {
                // not hit table or data must be wrong.
                return null;
            }

            // Push in the code.
            for (int loper = (h.prefix - 1); loper >= 0; loper--)
            {
                buffer[currentBit >> 3] <<= 1;
                buffer[currentBit >> 3] |= (uchar)((h.code >> loper) & 0x1);
                currentBit++;
            }

            // Push in the extra code, for special case.
            uint now = (uint)(code - h.value);

            for (int loper = (h.entire - h.prefix - 1); loper >= 0; loper--)
            {
                buffer[currentBit >> 3] <<= 1;
                buffer[currentBit >> 3] |= (uchar)((code >> loper) & 0x1);
                currentBit++;
            }
        }

        // Shift end to right position.
        if ((currentBit & 0x7) != 0x0)
        {
            buffer[(currentBit >> 3)] <<= (0x8 - (currentBit & 0x7));
            currentBit += (0x8 - (currentBit & 0x7));
        }
        else
        {
            // seems to solve a small encoding bug.
            currentBit += 8;
        }

        // Allocate a fitting buffer
        ret = new byte[(currentBit >> 3)];

        // Copy worst case buffer in fitting buffer.
        for (int loper = 0; loper < ret.Length; loper++)
        {
            ret[loper] = buffer[loper];
        }
    }

    return ret;
}
/// <summary>
/// Function to find corresponding HuffmanStruct with value.
/// </summary>
/// <param name="value">value to search</param>
/// <returns>corresponding HuffmanStruct</returns>
SCPHuffmanStruct SCPSection2::InterpettingData(short value)
{
    // Check if selected Table exists
    if ((_Tables != null)
        && (_Tables[_Selected] != null))
    {
        // Search in structs of table.
        for (int loper = 0; loper < _Tables[_Selected].Length; loper++)
        {
            SCPHuffmanStruct h = _Tables[_Selected][loper];
            // -1, because it can be positive and negative
            int extra = (h.entire - h.prefix - 1);

            // Check if value is equal to struct.
            if ((h.value == value)
                && (h.tablemode != 0))
            {
                return h;
            }
            // Check if value fits in special case.
            else if ((extra > 0)
                     && ((value - h.value) < (0x1 << extra))
                     && ((value - h.value) >= -(0x1 << extra))
                     && (h.tablemode != 0))
            {
                return h;
            }
        }
    }

    return null;
}
/// <summary>
/// Function to store signal using no compression.
/// </summary>
/// <param name="data">signal to read from</param>
/// <param name="time">number of samples to use</param>
/// <param name="quanta">sample distance in signal</param>
/// <param name="difference">difference to use durring decoding</param>
/// <returns>byte array containing encoded data</returns>
static byte[] SCPSection2::NoEncode(short[] data, int time, uchar difference)
{
    // Check if input data makes sense.
    if ((data != null)
        && (time <= data.Length))
    {
        // Initializing some handy variables
        int offset = 0;
        int sizeOfSample = sizeof(typeof(short));
        // Make buffer to contain samples
        byte[] ret = new byte[time * sizeOfSample];

        // For each sample do encode.
        for (int currentTime = 0; currentTime < time; currentTime++)
        {
            short code = 0;

            // Encode Differences.
            switch (difference)
            {
                case 0:
                    code = data[currentTime];
                    break;

                case 1:
                    code = (short)((currentTime < 1) ? data[currentTime] : data[currentTime] - data[currentTime - 1]);
                    break;

                case 2:
                    code = (short)((currentTime < 2) ? data[currentTime] : data[currentTime] - (data[currentTime - 1] << 1) + data[currentTime - 2]);
                    break;

                default:
                    // Undefined difference used exit empty.
                    return null;
            }

            // Write data in buffer.
            BytesTool::writeBytes(code, ret, offset, sizeOfSample, true);
            offset += sizeOfSample;
        }

        return ret;
    }

    return null;
}
#endif
/// <summary>
/// Resets the current selected HuffmanTable
/// </summary>
void SCPSection2::ResetSelect()
{
    if (Works())
    {
        _Selected = 0;
    }
}
#if 0 //use DefaultTable
/// <summary>
/// Function to get position of table swap.
/// </summary>
/// <param name="table">prefered table</param>
/// <returns>position in current table</returns>
int SCPSection2::getTableSwap(int table)
{
    if (Works()
        && (table < _Tables.Length)
        && (_Selected < _Tables.Length))
    {
        for (int loper = 0; loper < _Tables[_Selected].Length; loper++)
        {
            if (_Tables[_Selected][loper].tablemode == 0
                &&  _Tables[_Selected][loper].value == (table + 1))
            {
                return loper;
            }
        }
    }

    return -1;
}
#endif
/// <summary>
/// Function to get binary length of worst case of selected table.
/// </summary>
/// <returns>length of worst case</returns>
int SCPSection2::getWorstCase()
{
    int worst = -1;

    if (Works())
    {
        if (_NrTables == _DefaultTable)
        {
            worst = 26;
        }
        else if (_NrTables == 0)
        {
            worst = 16;
        }
        else
        {
#if 0 //use DefaultTable

            for (int loper = 0; loper < _Tables[_Selected].Length; loper++)
            {
                if (_Tables[_Selected][loper].entire > worst)
                {
                    worst = _Tables[_Selected][loper].entire;
                }
            }

#endif
        }
    }

    return worst;
}
/// <summary>
/// Function to set standard huffman table.
/// </summary>
void SCPSection2::UseStandard()
{
    _NrTables = _DefaultTable;
}
/// <summary>
/// Function to set no huffman encoding.
/// </summary>
void SCPSection2::UseNoHuffman()
{
    _NrTables = 0;
}
}
}
