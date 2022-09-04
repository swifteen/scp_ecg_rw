#ifndef _SCPSECTION1_H_
#define _SCPSECTION1_H_
#include "SCPSection.h"
#include "IDemographic.h"
#include"Drug.h"
#include "SCPDate.h"
#include "SCPTime.h"

using namespace std;
using namespace ECGConversion::ECGDemographics;

namespace ECGConversion
{
namespace SCP
{

class SCPHeaderField;

/// <summary>
/// Class contains section 1 (Header Information).
/// </summary>
class SCPSection1 : public SCPSection, public IDemographic
{
public:
    enum ProtocolCompatibility
    {
        kProtocolCompatibilityCatI = 0x90,
        kProtocolCompatibilityCatII = 0xa0,
        kProtocolCompatibilityCatIII = 0xb0,
        kProtocolCompatibilityCatIV = 0xc0
    };
public:
    SCPSection1();
    ushort getSectionID();
    bool Works();
    /// <summary>
    /// Function to initialize a section 1. Only needed when not reading from buffer.
    /// </summary>
    void Init();
    /// <summary>
    /// Function to insert a field into section.
    /// </summary>
    /// <param name="field">field to insert</param>
    /// <returns>0 on success</returns>
    int Insert(const SCPHeaderField& field);
    /// <summary>
    /// Function to remove a certain field from section.
    /// </summary>
    /// <param name="tag">tag of field.</param>
    /// <returns>0 on success</returns>
    int Remove(uchar tag);
    /// <summary>
    /// Function to resize the space for header fields.
    /// </summary>
    void Resize();
#if 0
    /// <summary>
    /// Function to get a field from this section
    /// </summary>
    /// <param name="tag">tag to search for</param>
    /// <returns></returns>
    SCPHeaderField GetField(uchar tag);
#endif
    /// <summary>
    /// Function to anonymous this section.
    /// </summary>
    /// <param name="type">value to empty with</param>
    void Anonymous(uchar type);
    /// <summary>
    /// Get encoding for text from language support code.
    /// </summary>
    /// <returns>used encoding</returns>
    std::string getLanguageSupportCode();
    /// <summary>
    /// Get encoding for text from language support code.
    /// </summary>
    /// <param name="enc">used encoding</param>
    /// <returns>0 if successfull</returns>
    int getLanguageSupportCode(std::string& enc);
    /// <summary>
    /// Set language support code based on encoding.
    /// </summary>
    /// <param name="enc">encoding to set lsc with.</param>
    /// <returns>0 if successfull</returns>
    int setLanguageSupportCode(const std::string& enc);
    /// <summary>
    /// Function to get Protocol Compatability Level.
    /// </summary>
    /// <param name="pc">Protocol Compatability Level</param>
    /// <returns>0 on succes</returns>
    int getProtocolCompatibilityLevel(ProtocolCompatibility& pc);
    /// <summary>
    /// Function to set Protocol Compatability Level.
    /// </summary>
    /// <param name="pc">Protocol Compatability Level</param>
    /// <returns>0 on succes</returns>
    int setProtocolCompatibilityLevel(ProtocolCompatibility pc);
    /// <summary>
    /// Function to get a text from a certain tag.
    /// </summary>
    /// <param name="tag">id of tag</param>
    /// <param name="text">a string</param>
    /// <returns>0 on success</returns>
    string getText(uchar tag);
    /// <summary>
    /// Function to set a text from a cetain tag.
    /// </summary>
    /// <param name="tag">id of tag</param>
    /// <param name="text">a string</param>
    /// <returns>0 on success</returns>
    int setText(uchar tag, const string& text);

    // Getting Demographics information
    string getLastName();
    void setLastName(const string& value);

    string getFirstName();
    void setFirstName(const string& value);

    string getPatientID();
    void setPatientID(const string& value);

    string getSecondLastName();
    void setSecondLastName(const string& value);

    string getPrefixName();
    void setPrefixName(const string& value);

    string getSuffixName();
    void setSuffixName(const string& value);

    int getPatientAge(ushort& val, AgeDefinition& def);
    int setPatientAge(ushort val, AgeDefinition def);

    Date getPatientBirthDate();
    void setPatientBirthDate(Date& PatientBirthDate);

    int getPatientHeight(ushort& val, HeightDefinition& def);
    int setPatientHeight(ushort val, HeightDefinition def);

    int getPatientWeight(ushort& val, WeightDefinition& def);
    int setPatientWeight(ushort val, WeightDefinition def);

    Sex getGender();
    void setGender(Sex Gender);

    Race getPatientRace();
    void setPatientRace(Race PatientRace);

    AcquiringDeviceID getAcqMachineID();
    void setAcqMachineID(const AcquiringDeviceID& AcqMachineID);

    AcquiringDeviceID getAnalyzingMachineID();
    void setAnalyzingMachineID(const AcquiringDeviceID& AnalyzingMachineID);

    DateTime getTimeAcquisition();
    void setTimeAcquisition(const DateTime& time);

    ushort getBaselineFilter();
    void setBaselineFilter(ushort BaselineFilter);

    ushort getLowpassFilter();
    void setLowpassFilter(ushort LowpassFilter);

    uchar getFilterBitmap();
    void setFilterBitmap(uchar FilterBitmap);

    std::vector<string> getFreeTextFields();
    void setFreeTextFields(const std::vector<string>&      FreeTextFields);

    string getSequenceNr();
    void setSequenceNr(const string& SequenceNr);

    string getAcqInstitution();
    void setAcqInstitution(const string& AcqInstitution);

    string getAnalyzingInstitution();
    void setAnalyzingInstitution(const string& AnalyzingInstitution);

    string getAcqDepartment();
    void setAcqDepartment(const string& AcqDepartment);

    string getAnalyzingDepartment();
    void setAnalyzingDepartment(const string& AnalyzingDepartment);

    string getReferringPhysician();
    void setReferringPhysician(const string& ReferringPhysician);

    string getOverreadingPhysician();
    void setOverreadingPhysician(const string& OverreadingPhysician);

    string getTechnicianDescription();
    void setTechnicianDescription(const string& TechnicianDescription);

    ushort getSystolicBloodPressure();
    void setSystolicBloodPressure(ushort SystolicBloodPressure);

    ushort getDiastolicBloodPressure();
    void setDiastolicBloodPressure(ushort DiastolicBloodPressure);

    std::vector<Drug> getDrugs();
    void setDrugs(std::vector<Drug>& Drugs);

    std::vector<string>  getReferralIndication();
    void setReferralIndication(const std::vector<string>& ReferralIndication);

    string getRoomDescription();
    void setRoomDescription(const string& RoomDescription);

    uchar getStatCode();
    void setStatCode(uchar StatCode);
protected:
    int _Read(uchar* buffer, int bufferLength, int offset);
    int _Write(uchar* buffer, int bufferLength, int offset);
    void _Empty();
    int _getLength();
private:
    /// <summary>
    /// Function to search for a field with a certain tag.
    /// </summary>
    /// <param name="tag">tag to search for</param>
    /// <returns>position of this field</returns>
    int _SearchField(uchar tag);
    /// <summary>
    /// Function to find position to insert a field with a certain tag.
    /// </summary>
    /// <param name="tag">tag to search on.</param>
    /// <returns>position to insert</returns>
    int _InsertSearch(uchar tag);
    /// <summary>
    /// Function to check wheter the used fields are indeed sorted.
    /// </summary>
    bool CheckInstances();
    /// <summary>
    /// Function to check for exception case.
    /// </summary>
    /// <param name="condition">condition</param>
    /// <param name="tag">value of tag</param>
    /// <returns>is exception then true</returns>
    static bool isException(uchar* condition, int conditionLength, uchar tag);
private:
    // Fields that must be made empty for anonymous. (must be sorted from low to high)
    static uchar _AnonymousFields[7];
    // Defined in SCP.
    static uchar _MustBePresent[4]; // defined in paragraph 5.4.3.1 of SCP
    static uchar _MultipleInstanceFields[5]; // Must be sorted
    static ushort _MaximumFieldLength;
    static uchar _MaximumLengthExceptions[5]; // Must be sorted
    static ushort _ExceptionsMaximumLength; // should be 80, but some scp file doen't use this maximum. apparantly 128 wasn't enough as well
    static uchar _ManufactorField;
    static ushort _SectionID;
    static uchar _DemographicTerminator;

    // ResizeSpeed for the array to store the Fields.
    static int _ResizeSpeed;

    // Part of the stored Data Structure.
    int _NrFields;
    std::vector<SCPHeaderField> _Fields;
};
}
}
#endif  /*#ifndef _SCPSECTION1_H_*/
