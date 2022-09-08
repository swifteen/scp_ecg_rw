#ifndef _SCPSECTION0_H_
#define _SCPSECTION0_H_
#include "SCPSection.h"
#include "BytesTool.h"

using namespace Communication_IO_Tools;

namespace ECGConversion
{
namespace SCP
{
/// <summary>
/// Class containing a SCP pointer.
/// </summary>
class SCPPointer
{
public:
    SCPPointer()
    {
    }
    /// <summary>
    /// Function to read SCP pointer.
    /// </summary>
    /// <param name="buffer">byte array to read from</param>
    /// <param name="offset">position to start reading</param>
    /// <returns>0 on success</returns>
    int Read(uchar* buffer, int bufferLength, int offset)
    {
        if ((offset + Size) > bufferLength)
        {
            return 0x1;
        }

        Nr = (ushort) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(Nr), true);
        offset += sizeof(Nr);
        Length = (int) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(Length), true);
        offset += sizeof(Length);
        Index = (int) BytesTool::readBytes(buffer, bufferLength, offset, sizeof(Index), true);
        offset += sizeof(Index);
        return 0x0;
    }
    /// <summary>
    /// Function to write SCP pointer.
    /// </summary>
    /// <param name="buffer">byte array to write section into</param>
    /// <param name="offset">position to start writing</param>
    /// <returns>0 on success</returns>
    int Write(uchar* buffer, int bufferLength, int offset)
    {
        if ((offset + Size) > bufferLength)
        {
            SCP_PE("out of range,offset: %d,Size: %d,bufferLength: %d\n", offset, Size, bufferLength);
            return 0x1;
        }

        BytesTool::writeBytes(Nr, buffer, bufferLength, offset, sizeof(Nr), true);
        offset += sizeof(Nr);
        BytesTool::writeBytes(Length, buffer, bufferLength, offset, sizeof(Length), true);
        offset += sizeof(Length);
        BytesTool::writeBytes(Index, buffer, bufferLength, offset, sizeof(Index), true);
        offset += sizeof(Index);
        SCP_PD("write over,offset: %d,Size: %d,bufferLength: %d\n", offset, Size, bufferLength);
        return 0x0;
    }
public:
    static int Size;//类的const变量在构造函数初始化列表中完成初始化
    ushort Nr;
    int Length;
    int Index;
};


/// <summary>
/// Class contains section 0 (Pointer section).
/// </summary>
class SCPSection0 : public SCPSection
{
private:
    enum
    {
        kNrMandatory = 12,
    };
public:
    SCPSection0();
    virtual ~SCPSection0();
    ushort getSectionID();
    bool Works();
    /// <summary>
    /// Function to get number of pointers contained in pointer section.
    /// </summary>
    /// <returns>number of pointers</returns>
    int getNrPointers();
    /// <summary>
    /// Function to set number of pointers contained in pointer section.
    /// </summary>
    /// <param name="nr">number of pointers</param>
    void setNrPointers(int nr);
    /// <summary>
    /// Function to get section id of a pointer.
    /// </summary>
    /// <param name="nr">pointer number to get section id from</param>
    /// <returns>id of section</returns>
    ushort getSectionID(int nr);
    /// <summary>
    /// Function to set section id of a pointer.
    /// </summary>
    /// <param name="nr">number of pointer to set</param>
    /// <param name="ID">id of section</param>
    void setSectionID(int nr, ushort ID);
    /// <summary>
    /// Function to get index of a pointer.
    /// </summary>
    /// <param name="nr">number of pointer to get from</param>
    /// <returns>0 on success</returns>
    int getIndex(int nr);
    /// <summary>
    /// Function to set index of a pointer.
    /// </summary>
    /// <param name="nr">number of pointer to set</param>
    /// <param name="index">index to set pointer to</param>
    void setIndex(int nr, int index);
    /// <summary>
    /// Function to get length of section from pointer.
    /// </summary>
    /// <param name="nr">number of pointer to get from</param>
    /// <returns>length of section</returns>
    int getLength(int nr);
    /// <summary>
    /// Function to get length of section from pointer.
    /// </summary>
    /// <param name="nr">number of pointer to set</param>
    /// <param name="length">length of section</param>
    void setLength(int nr, int length);
    /// <summary>
    /// Function to get all values of a pointer.
    /// </summary>
    /// <param name="nr">number of pointer to read from</param>
    /// <param name="id">id of section</param>
    /// <param name="length">length of section</param>
    /// <param name="index">index of section</param>
    void getPointer(int nr, ushort& id, int& length, int& index);
    /// <summary>
    /// Function to set all values of a pointer
    /// </summary>
    /// <param name="nr">number of pointer to set</param>
    /// <param name="id">id of section</param>
    /// <param name="length">length of section</param>
    /// <param name="index">index of section</param>
    void setPointer(int nr, ushort id, int length, int index);
protected:
    int _Read(uchar* buffer, int bufferLength, int offset);
    int _Write(uchar* buffer, int bufferLength, int offset);
    void _Empty();
    int _getLength();
private:
    // Defined in SCP.
    static uchar _Reserved[6];
    static ushort _SectionID;
    static int _NrMandatory;

    //class SCPPointer;
    // Part of the stored Data Structure.
    std::vector<SCPPointer> _MandatoryPointers;
    std::vector<SCPPointer> _OptionalPointers;
};
}
}

#endif  /*#ifndef _SCPSECTION0_H_*/
