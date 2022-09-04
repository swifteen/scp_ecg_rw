#include "BytesTool.h"
#include <cstring>
#include <iconv.h>

namespace Communication_IO_Tools
{
/// <summary>
/// Function to read a integer from a buffer at an offset.
/// </summary>
/// <param name="buffer">source buffer</param>
/// <param name="offset"></param>
/// <param name="bytes">length of integer</param>
/// <param name="littleEndian">little endian or big endian integer</param>
/// <returns>read integer</returns>
long BytesTool::readBytes(uchar* buffer, int bufferLength, int offset, int bytes, bool littleEndian)
{
    long returnValue = 0;

    if (bytes > 8)
    {
        bytes = 8;
    }

    if (offset + bytes <= bufferLength)
    {
        for (int read = 0; read < bytes; read++)
        {
            returnValue |= (long) buffer[offset + (littleEndian ? read : (bytes - read - 1))] << (read << 3);
        }
    }

    return returnValue;
}

/// <summary>
/// Function to write an integer to a buffer at an offset.
/// </summary>
/// <param name="values">integer to write</param>
/// <param name="buffer">buffer to write to</param>
/// <param name="offset">position to start writing</param>
/// <param name="bytes">nr bytes to write</param>
/// <param name="littleEndian">little endian or big endian integer</param>
/// <returns></returns>
bool BytesTool::writeBytes(long values, uchar* buffer, int bufferLength, int offset, int bytes, bool littleEndian)
{
    if ((buffer != null)
        && (offset + bytes) <= bufferLength
        && (bytes <= 8)
        && (bytes > 0))
    {
        for (int read = 0; read < bytes; read++)
        {
            buffer[offset + (littleEndian ? read : (bytes - read - 1))] = (uchar)((values >> (read << 3)) & 0xff);
        }

        //      SCP_PD("values:%ld,bufferLength:%d,offset:%d,bytes:%d\n",values,bufferLength,offset,bytes);
        return true;
    }

    SCP_PE("values:%ld,bufferLength:%d,offset:%d,bytes:%d\n", values, bufferLength, offset, bytes);
    return false;
}
/// <summary>
/// Function to read a string from a byte array at a given offset
/// </summary>
/// <param name="buffer">to read the string from</param>
/// <param name="offset">position to start reading</param>
/// <param name="length">max length of string</param>
/// <returns>a string</returns>
string BytesTool::readString(uchar* buffer, int bufferLength, int offset, int length)
{
    return readString("ASCII", buffer, bufferLength, offset, length);
}
/// <summary>
/// Function to read a string from a byte array at a given offset
/// </summary>
/// <param name="buffer">to read the string from</param>
/// <param name="offset">position to start reading</param>
/// <param name="length">max length of string</param>
/// <param name="value">value to use as terminator of string</param>
/// <returns>a string</returns>
string BytesTool::readString(uchar* buffer, int bufferLength, int offset, int length, uchar terminator)
{
    int strLen = stringLength(buffer, bufferLength, offset, length, terminator);

    if (strLen != 0)
    {
        char* a = new char[strLen + 1];//TODO验证是否需要加1
        memcpy(a, buffer + offset, strLen);
        a[strLen] = '\0';
        string str(a);
        delete [] a;
        return str;
    }

    return string("");
}
/// <summary>
/// Function to read a string from a byte array at a given offset
///将源字符集为srcEncoding为字符串转换为UTF-8编码的字符串
/// </summary>
/// <param name="enc">enconding type</param>
/// <param name="buffer">to read the string from</param>
/// <param name="offset">position to start reading</param>
/// <param name="length">max length of string</param>
/// <returns>a string</returns>
string BytesTool::readString(const std::string& srcEncoding, uchar* buffer, int bufferLength, int offset, int length)
{
    string ret = "";
#if 0
    int strLen = stringLength(srcEncoding, buffer, bufferLength, offset, length);

    if (strLen != 0)
    {
        int maxLen = enc.GetMaxByteCount(strLen);
        length = (length > maxLen) ? maxLen : length;
        ret = enc.GetString(buffer, offset, length);

        if ((ret != null)
            && (ret.Length > strLen))
        {
            ret = ret.Substring(0, strLen);
        }
    }

#endif
    return ret;
}
/// <summary>
/// Function to calculate length of string in buffer starting at an offset.
/// </summary>
/// <param name="buffer">source buffer</param>
/// <param name="offset">position to start counting from</param>
/// <param name="length">max length of string</param>
/// <returns>length of string</returns>
int BytesTool::stringLength(uchar* buffer, int bufferLength, int offset, int length)
{
    return stringLength("ASCII", buffer, bufferLength, offset, length);
}
/// <summary>
/// Function to calculate length of string in buffer starting at an offset.
/// </summary>
/// <param name="buffer">source buffer</param>
/// <param name="offset">position to start counting from</param>
/// <param name="length">max length of string</param>
/// <param name="value">value to use as terminator of string</param>
/// <returns>length of string</returns>
int BytesTool::stringLength(uchar* buffer, int bufferLength, int offset, int length, uchar terminator)
{
    int x = 0;

    if (length < 0)
    {
        while ((buffer != null)
               && ((offset + x) < bufferLength)
               && (buffer[offset + x] != terminator))
        {
            x++;
        }
    }
    else
    {
        while ((buffer != null)
               && ((offset + x) < bufferLength)
               && (x < length)
               && (buffer[offset + x] != terminator))
        {
            x++;
        }
    }

    return x;
}
/// <summary>
/// Function to calculate length of string in buffer starting at an offset.
/// </summary>
/// <param name="enc">enconding type</param>
/// <param name="buffer">source buffer</param>
/// <param name="offset">position to start counting from</param>
/// <param name="length">max length of string</param>
/// <returns>length of string</returns>
int BytesTool::stringLength(const std::string& srcEncoding, uchar* buffer, int bufferLength, int offset, int length)
{
    int len = 0;
#if 0 //TODO

    foreach (char temp in enc.GetChars(buffer, offset, (bufferLength < (offset + length)) ? bufferLength - offset : length))
    {
        if (temp == '\0')
        {
            break;
        }

        len++;
    }

#endif
    return len;
}

/// <summary>
/// Function to write a string too a byte array at a given offset
/// </summary>
/// <param name="src">to read from</param>
/// <param name="buffer">to write the string too</param>
/// <param name="offset">position to start reading</param>
/// <param name="length">max length of string</param>
void BytesTool::writeString(const std::string& src, uchar* buffer, int bufferLength, int offset, int length)
{
    writeString("ASCII", src, buffer, bufferLength, offset, length);
}
/// <summary>
/// Function to write a string too a byte array at a given offset
/// </summary>
/// <param name="enc">enconding type</param>
/// <param name="src">to read from</param>
/// <param name="buffer">to write the string too</param>
/// <param name="offset">position to start reading</param>
/// <param name="length">max length of string</param>
void BytesTool::writeString(const std::string& dstEncoding,
                            const std::string& src,
                            uchar* buffer,
                            int bufferLength,
                            int offset,
                            int length)
{
    if ((src.length() > 0) && (buffer != null))
    {
        char* src_cstr = new char [src.length() + 1];
        char* tmp_src_cstr = src_cstr;

        if (src_cstr == null)
        {
            return;
        }

        memset(src_cstr, 0, src.length() + 1);
        snprintf(src_cstr, src.length() + 1, "%s", src.c_str());
        size_t src_cstr_len = strlen(src_cstr) + 1;
        //The maximum number of characters produced by decoding the specified number of bytes.
        size_t nrChars = (bufferLength < (offset + length)) ? bufferLength - offset : length;
        nrChars = (src.length() < nrChars) ? src.length() : nrChars;

        if (nrChars > 0)
        {
            /* Assign enough space to put the UTF-8. */
            size_t outbytesleft = 2 * nrChars;
            char* outbuf = new char[outbytesleft];

            if (outbuf == null)
            {
                delete[] src_cstr;
                return;
            }

            memset(outbuf, 0, outbytesleft);
            int ret = convert_charset("UTF-8", dstEncoding.c_str(), src_cstr, src_cstr_len, outbuf, &outbytesleft);

            if (0 == ret)
            {
                memcpy(buffer + offset, outbuf, outbytesleft);
            }
            else
            {
                SCP_PD("iconv failed: in string '%s', length %d, "
                       "out string '%s', length %d",
                       src_cstr, (int)src_cstr_len, outbuf, (int)outbytesleft);
                memcpy(buffer + offset, src_cstr, src_cstr_len);
            }

            delete[] outbuf;
        }

        delete[] src_cstr;
    }
}
/// <summary>
/// Function to copy content of one buffer to another.
/// </summary>
/// <param name="dst">destination buffer</param>
/// <param name="offdst">offset in destination buffer</param>
/// <param name="src">source buffer</param>
/// <param name="offsrc">offset in source buffer</param>
/// <param name="length">number bytes to copy</param>
int BytesTool::copy(uchar* dst, int dstLength, int offdst, const uchar* src, int srcLength, int offsrc, int length)
{
    int loper = 0;

    for (; (loper < length) && ((offdst + loper) < dstLength) && ((offsrc + loper) < srcLength); loper++)
    {
        dst[offdst + loper] = src[offsrc + loper];
    }

    return loper;
}
/// <summary>
/// Function to empty a buffer to a defined number.
/// </summary>
/// <param name="buffer">destination buffer</param>
/// <param name="offset">offset in buffer</param>
/// <param name="nrbytes">number byte to empty</param>
/// <param name="type">number to empty to</param>
void BytesTool::emptyBuffer(uchar* buffer, int bufferLength, int offset, int nrbytes, uchar type)
{
    for (int x = 0; (x < nrbytes) && ((x + offset) < bufferLength); x++)
    {
        buffer[offset + x] = type;
    }
}

int BytesTool::convert_charset(const char* from_charset,
                               const char* to_charset,
                               char* src_buf,
                               size_t src_len,
                               char* dst_buf,
                               size_t* p_dst_len)
{
    /*
    If you have opened the conversion descriptor without //TRANSLIT or //IGNORE,
    then iconv() will return an error when the input character cannot be represented in the target character set.
    */
    iconv_t icd = iconv_open(to_charset, from_charset);

    if ((iconv_t) - 1 == icd)
    {
        /* Initialization failure. */
        if (errno == EINVAL)
        {
            SCP_PE("Conversion from '%s' to '%s' is not supported.\n", from_charset, to_charset);
        }
        else
        {
            SCP_PE("Initialization failure: %s\n", strerror(errno));
        }

        return -1;
    }

    char** pin = &src_buf;
    char** pout = &dst_buf;
    size_t iconv_value = iconv(icd, pin, &src_len, pout, p_dst_len);

    if (iconv_value == (size_t) - 1)
    {
        switch (errno)
        {
            /* See "man 3 iconv" for an explanation. */
            case EILSEQ:
                SCP_PE("Invalid multibyte sequence.\n");
                break;

            case EINVAL:
                SCP_PE("Incomplete multibyte sequence.\n");
                break;

            case E2BIG:
                SCP_PE("No more room.\n");
                break;

            default:
                SCP_PE("Error: %s.\n", strerror(errno));
                break;
        }

        iconv_close(icd);
        return -1;
    }

    iconv_close(icd);
    return 0;
}

}
