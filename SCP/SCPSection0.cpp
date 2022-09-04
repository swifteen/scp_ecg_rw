#include "SCPSection0.h"
#include "CRCTool.h"
#include "BytesTool.h"

using namespace Communication_IO_Tools;

namespace ECGConversion
{
namespace SCP
{
/// <summary>
/// Class containing a SCP pointer.
/// </summary>
class SCPSection0::SCPPointer
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

int SCPSection0::SCPPointer::Size = 10;

// Defined in SCP.
uchar SCPSection0::_Reserved[6] = {'S', 'C', 'P', 'E', 'C', 'G'};
ushort SCPSection0::_SectionID = 0;
int SCPSection0::_NrMandatory = 12;

/// <summary>
/// Class contains section 0 (Pointer section).
/// </summary>

SCPSection0::SCPSection0()
{
    SCPSection::Empty();
    // Part of the stored Data Structure.
    _MandatoryPointers.resize(kNrMandatory);
    _OptionalPointers.clear();
}

int SCPSection0::_Read(uchar* buffer, int bufferLength, int offset)
{
    /* Very stange, but most SCP files I got didn't seem to be live up to this law.
     * the check will now only be doen when the Protocol Version Nr is greator equal
     * to 14. This value might need changes in future.
     *
     *  Reference 5.3.1 of the "Standard Communications protocol for computer-assisted
     *   electrocardiography".
     */
    if ((ProtocolVersionNr >= 13)
        && ((Reserved[0] != _Reserved[0])
            || (Reserved[1] != _Reserved[1])
            || (Reserved[2] != _Reserved[2])
            || (Reserved[3] != _Reserved[3])
            || (Reserved[4] != _Reserved[4])
            || (Reserved[5] != _Reserved[5])))
    {
        return 0x1;
    }

    int nrPointers = (Length - Size) / SCPPointer::Size;

    if (nrPointers < 12)
    {
        return 0x2;
    }

    for (int loper = 0; loper < _NrMandatory; loper++)
    {
        int err = _MandatoryPointers[loper].Read(buffer, bufferLength, offset);

        if (err != 0)
        {
            return err << 2;
        }

        offset += SCPPointer::Size;
    }

    nrPointers -= _NrMandatory;

    if (nrPointers > 0)
    {
        _OptionalPointers.resize(nrPointers);

        for (int loper = 0; loper < nrPointers; loper++)
        {
            int err = _OptionalPointers[loper].Read(buffer, bufferLength, offset);

            if (err != 0)
            {
                return err << 3;
            }

            offset += SCPPointer::Size;
        }
    }

    return 0x0;
}

int SCPSection0::_Write(uchar* buffer, int bufferLength, int offset)
{
    for (int loper = 0; loper < _NrMandatory; loper++)
    {
        int err = _MandatoryPointers[loper].Write(buffer, bufferLength, offset);

        if (err != 0)
        {
            return err;
        }

        offset += SCPPointer::Size;
    }

    if (_OptionalPointers.size() > 0)
    {
        for (int loper = 0; loper < _OptionalPointers.size(); loper++)
        {
            int err = _OptionalPointers[loper].Write(buffer, bufferLength, offset);

            if (err != 0)
            {
                return err << 1;
            }

            offset += SCPPointer::Size;
        }
    }

    return 0x0;
}

void SCPSection0::_Empty()
{
    BytesTool::copy(Reserved, sizeof(Reserved), 0, _Reserved, sizeof(_Reserved), 0, 6);
#if 0

    if ((this._MandatoryPointers.size() > 0)
        && (_MandatoryPointers.size() == _NrMandatory))
    {
        for (int loper = 0; loper < _NrMandatory; loper++)
        {
            _MandatoryPointers[loper] = null;
        }
    }
    else
    {
        _MandatoryPointers = new SCPPointer[_NrMandatory];
    }

#endif
    _MandatoryPointers.resize(_NrMandatory);
    _OptionalPointers.clear();
}
ushort SCPSection0::getSectionID()
{
    return _SectionID;
}

int SCPSection0::_getLength()
{
    int sum = SCPPointer::Size * getNrPointers();
    return ((sum % 2) == 0 ? sum : sum + 1);
}

bool SCPSection0::Works()
{
    if ((_MandatoryPointers.size() > 0)
        && (_MandatoryPointers.size() == _NrMandatory))
    {
# if 0

        for (int loper = 0; loper < _NrMandatory; loper++)
        {
            if (_MandatoryPointers[loper] == null)
            {
                return false;
            }
        }

        if (_OptionalPointers.size() > 0)
        {
            for (int loper = 0; loper < _OptionalPointers.size(); loper++)
            {
                if (_OptionalPointers[loper] == null)
                {
                    return false;
                }
            }
        }

#endif
        return true;
    }

    return false;
}

/// <summary>
/// Function to get number of pointers contained in pointer section.
/// </summary>
/// <returns>number of pointers</returns>
int SCPSection0::getNrPointers()
{
    return _NrMandatory + (_OptionalPointers.size() > 0 ? _OptionalPointers.size() : 0);
}

/// <summary>
/// Function to set number of pointers contained in pointer section.
/// </summary>
/// <param name="nr">number of pointers</param>
void SCPSection0::setNrPointers(int nr)
{
    int current = _NrMandatory + (_OptionalPointers.size() > 0 ? _OptionalPointers.size() : 0);

    if ((nr != current)
        && (nr >= _NrMandatory))
    {
        _MandatoryPointers.resize(_NrMandatory);

        if (nr > _NrMandatory)
        {
            _OptionalPointers.resize(nr - _NrMandatory);
        }
    }
}
/// <summary>
/// Function to get section id of a pointer.
/// </summary>
/// <param name="nr">pointer number to get section id from</param>
/// <returns>id of section</returns>
ushort SCPSection0::getSectionID(int nr)
{
    if ((nr >= 0)
        && (nr < getNrPointers()))
    {
        if (nr < _NrMandatory)
        {
            return _MandatoryPointers[nr].Nr;
        }

        return _OptionalPointers[nr - _NrMandatory].Nr;
    }

    return 0;
}
/// <summary>
/// Function to set section id of a pointer.
/// </summary>
/// <param name="nr">number of pointer to set</param>
/// <param name="ID">id of section</param>
void SCPSection0::setSectionID(int nr, ushort ID)
{
    nr -= _NrMandatory;

    if ((_OptionalPointers.size() > 0)
        && (nr >= 0)
        && (nr < _OptionalPointers.size()))
    {
        _OptionalPointers[nr].Nr = ID;
    }
}
/// <summary>
/// Function to get index of a pointer.
/// </summary>
/// <param name="nr">number of pointer to get from</param>
/// <returns>0 on success</returns>
int SCPSection0::getIndex(int nr)
{
    if ((nr >= 0)
        && (nr < getNrPointers()))
    {
        if (nr < _NrMandatory)
        {
            return _MandatoryPointers[nr].Index;
        }

        return _OptionalPointers[nr - _NrMandatory].Index;
    }

    return 0;
}


/// <summary>
/// Function to set index of a pointer.
/// </summary>
/// <param name="nr">number of pointer to set</param>
/// <param name="index">index to set pointer to</param>
void SCPSection0::setIndex(int nr, int index)
{
    if ((nr >= 0)
        && (nr < getNrPointers()))
    {
        if (nr < _NrMandatory)
        {
            _MandatoryPointers[nr].Index = index;
        }
        else
        {
            _OptionalPointers[nr - _NrMandatory].Index = index;
        }
    }
}


/// <summary>
/// Function to get length of section from pointer.
/// </summary>
/// <param name="nr">number of pointer to get from</param>
/// <returns>length of section</returns>
int SCPSection0::getLength(int nr)
{
    if ((nr >= 0)
        && (nr < getNrPointers()))
    {
        if (nr < _NrMandatory)
        {
            return _MandatoryPointers[nr].Length;
        }

        return _OptionalPointers[nr - _NrMandatory].Length;
    }

    return 0;
}


/// <summary>
/// Function to get length of section from pointer.
/// </summary>
/// <param name="nr">number of pointer to set</param>
/// <param name="length">length of section</param>
void SCPSection0::setLength(int nr, int length)
{
    if ((nr >= 0)
        && (nr < getNrPointers()))
    {
        if (nr < _NrMandatory)
        {
            _MandatoryPointers[nr].Length = length;
        }
        else
        {
            _OptionalPointers[nr - _NrMandatory].Length = length;
        }
    }
}


/// <summary>
/// Function to get all values of a pointer.
/// </summary>
/// <param name="nr">number of pointer to read from</param>
/// <param name="id">id of section</param>
/// <param name="length">length of section</param>
/// <param name="index">index of section</param>
void SCPSection0::getPointer(int nr, ushort& id, int& length, int& index)
{
    id = 0;
    length = 0;
    index = 0;

    if ((nr >= 0)
        && (_MandatoryPointers.size() > 0)
        && (_MandatoryPointers.size() == _NrMandatory))
    {
        if (nr < _NrMandatory)
        {
            id = _MandatoryPointers[nr].Nr;
            length = _MandatoryPointers[nr].Length;
            index = _MandatoryPointers[nr].Index;
        }
        else if ((_OptionalPointers.size() > 0)
                 && ((nr - _NrMandatory) < _OptionalPointers.size()))
        {
            nr -= _NrMandatory;
            id = _OptionalPointers[nr].Nr;
            length = _OptionalPointers[nr].Length;
            index = _OptionalPointers[nr].Index;
        }
    }
}


/// <summary>
/// Function to set all values of a pointer
/// </summary>
/// <param name="nr">number of pointer to set</param>
/// <param name="id">id of section</param>
/// <param name="length">length of section</param>
/// <param name="index">index of section</param>
void SCPSection0::setPointer(int nr, ushort id, int length, int index)
{
    if (nr >= 0)
    {
        if (nr < _NrMandatory)
        {
            _MandatoryPointers[nr].Nr = id;
            _MandatoryPointers[nr].Length = length;
            _MandatoryPointers[nr].Index = index;
        }
        else if ((_OptionalPointers.size() > 0)
                 && ((nr - _NrMandatory) < _OptionalPointers.size()))
        {
            nr -= _NrMandatory;
            _OptionalPointers[nr].Nr = id;
            _OptionalPointers[nr].Length = length;
            _OptionalPointers[nr].Index = index;
        }
    }
}

}
}
