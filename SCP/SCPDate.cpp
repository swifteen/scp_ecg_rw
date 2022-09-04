#include "SCPDate.h"
#include "BytesTool.h"

using namespace Communication_IO_Tools;
namespace ECGConversion
{
namespace SCP
{
int SCPDate::Size = 4;

/// <summary>
/// class containing date in SCP format.
/// </summary>


/// <summary>
/// Constructor of a SCP date.
/// </summary>
SCPDate::SCPDate()
{
    Year = 0;
    Month = 0;
    Day = 0;
}
/// <summary>
/// Constructor of a SCP date.
/// </summary>
/// <param name="year">number of year</param>
/// <param name="month">number of month</param>
/// <param name="day">number of day</param>
SCPDate::SCPDate(int year, int month, int day)
{
    Year = (ushort) year;
    Month = (uchar) month;
    Day = (uchar) day;
}
/// <summary>
/// Function to read an SCP date from byte array.
/// </summary>
/// <param name="buffer">byte array to read from</param>
/// <param name="offset">position to start reading</param>
/// <returns>0 on success</returns>
int SCPDate::Read(uchar* buffer, int bufferLength, int offset)
{
    if ((offset + Size) > bufferLength)
    {
        return 0x1;
    }

    Year = (ushort) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(Year), true);
    offset += sizeof(Year);
    Month = (uchar) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(Month), true);
    offset += sizeof(Month);
    Day = (uchar) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(Day), true);
    offset += sizeof(Day);
    return 0x0;
}
/// <summary>
/// Function to write an SCP date to a byte array.
/// </summary>
/// <param name="buffer">byte array to write in</param>
/// <param name="offset">position to start writing</param>
/// <returns></returns>
int SCPDate::Write(uchar* buffer, int bufferLength, int offset)
{
    if ((offset + Size) > bufferLength)
    {
        return 0x1;
    }

    BytesTool::writeBytes(Year, buffer, bufferLength, offset, sizeof(Year), true);
    offset += sizeof(Year);
    BytesTool::writeBytes(Month, buffer, bufferLength, offset, sizeof(Month), true);
    offset += sizeof(Month);
    BytesTool::writeBytes(Day, buffer, bufferLength, offset, sizeof(Day), true);
    offset += sizeof(Day);
    return 0x0;
}

}
}
