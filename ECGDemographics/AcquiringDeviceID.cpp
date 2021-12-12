#include "AcquiringDeviceID.h"
#include "DemographicEnumrations.h"
#include "BytesTool.h"
using namespace Communication_IO_Tools;

namespace ECGConversion
{
namespace ECGDemographics
{

/// <summary>
/// Device information that can be imported and exported in both SCP and UNIPRO.

AcquiringDeviceID::AcquiringDeviceID()
{
	InstitutionNr = 0;
	DepartmentNr = 0;
	DeviceID = 0;
	DeviceType = 0;
	ManufactorID = (uchar) kDeviceManufactorUnknown;
	DeviceCapabilities = 0; // Is defined in SCP Section1 tag 14 byte 18.
	ACFrequencyEnvironment = 0; // Is defined in SCP Section1 tag 14 byte 19.
	memset(ModelDescription,0,sizeof(ModelDescription));
}

AcquiringDeviceID::AcquiringDeviceID(bool bNoDeviceId)
{
    if (bNoDeviceId)
    {
        InstitutionNr = 0;
        DepartmentNr = 11;
        DeviceID = 51;
        DeviceType = (uchar) kDeviceTypeSystem;
        ManufactorID = (uchar) kDeviceManufactorUnknown;
        DeviceCapabilities = 0x8;
        ACFrequencyEnvironment = 1;
        BytesTool::writeString("MCONV", ModelDescription, sizeof(ModelDescription), 0, sizeof(ModelDescription));
    }
	memset(ModelDescription,0,sizeof(ModelDescription));
}

}

}
