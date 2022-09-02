#ifndef _IDEMOGRAPHIC_CS_
#define _IDEMOGRAPHIC_CS_
#include "DemographicEnumrations.h"
#include "Date.h"
#include "Drug.h"
#include "AcquiringDeviceID.h"

namespace ECGConversion
{
namespace ECGDemographics
{
/// <summary>
/// Interface for manupalation of demograpics information.
/// </summary>
class IDemographic
{
public:
    /// <summary>
    /// Function to initialize demographics information.
    /// </summary>
    virtual void Init() = 0;
    // Getting Demographics information
    /// <summary>
    /// Function to get the last name of the patient.
    /// </summary>
    virtual string getLastName() = 0;
    virtual void setLastName(const string& value) = 0;
    /// <summary>
    /// Function to get the first name of the patient.
    /// </summary>
    virtual string getFirstName() = 0;
    virtual void setFirstName(const string& value) = 0;
    /// <summary>
    /// Function to get the id of the patient.
    /// </summary>
    virtual string getPatientID() = 0;
    virtual void setPatientID(const string& value) = 0;
    /// <summary>
    /// Function to get the second last name of the patient.
    /// </summary>
    virtual string getSecondLastName() = 0;
    virtual void setSecondLastName(const string& value) = 0;
    /// <summary>
    /// Function to get the prefix of the patient name.
    /// </summary>
    virtual string getPrefixName() = 0;
    virtual void setPrefixName(const string& value) = 0;
    /// <summary>
    /// Function to get the suffix of the patient name.
    /// </summary>
    virtual string getSuffixName() = 0;
    virtual void setSuffixName(const string& value) = 0;
    /// <summary>
    /// Function to get the age of the patient.
    /// </summary>
    /// <param name="val">age value as defined</param>
    /// <param name="def">definition of the age value</param>
    /// <returns>0 on success</returns>
    virtual int getPatientAge(ushort& val, AgeDefinition& def) = 0;
    /// <summary>
    /// Function to set the last name of the patient.
    /// </summary>
    /// <param name="name">last name of patient</param>
    /// <returns>0 on success</returns>
    virtual int setPatientAge(ushort val, AgeDefinition def) = 0;
    /// <summary>
    /// Function to get the date of birth of patient.
    /// </summary>
    virtual Date getPatientBirthDate() = 0;
    virtual void setPatientBirthDate(Date& PatientBirthDate) = 0;
    /// <summary>
    /// Function to get the height of the patient.
    /// </summary>
    /// <param name="val">height value as defined</param>
    /// <param name="def">definition of the height value</param>
    /// <returns>0 on success</returns>
    virtual int getPatientHeight(ushort& val, HeightDefinition& def) = 0;
    /// <summary>
    /// Function to set the height of the patient.
    /// </summary>
    /// <param name="val">height value as defined</param>
    /// <param name="def">definition of the height value</param>
    /// <returns>0 on success</returns>
    virtual int setPatientHeight(ushort val, HeightDefinition def) = 0;
    /// <summary>
    /// Function to get the weight of the patient.
    /// </summary>
    /// <param name="val">weight value as defined</param>
    /// <param name="def">definition of the weight value</param>
    /// <returns>0 on success</returns>
    virtual int getPatientWeight(ushort& val, WeightDefinition& def) = 0;
    /// <summary>
    /// Function to set the weight of the patient.
    /// </summary>
    /// <param name="val">weight value as defined</param>
    /// <param name="def">definition of the weight value</param>
    /// <returns>0 on success</returns>
    virtual int setPatientWeight(ushort val, WeightDefinition def) = 0;
    /// <summary>
    /// Function to get sex of patient
    /// </summary>
    virtual Sex getGender() = 0;
    virtual void setGender(Sex Gender) = 0;
    /// <summary>
    /// Function to get race of patient.
    /// </summary>
    virtual Race getPatientRace() = 0;
    virtual void setPatientRace(Race PatientRace) = 0;
    /// <summary>
    /// Function to get the acquiring machine id.
    /// </summary>
    virtual AcquiringDeviceID getAcqMachineID() = 0;
    virtual void setAcqMachineID(const AcquiringDeviceID& AcqMachineID) = 0;
    /// <summary>
    /// Function to get the analyzing machine id.
    /// </summary>
    virtual AcquiringDeviceID getAnalyzingMachineID() = 0;
    virtual void setAnalyzingMachineID(const AcquiringDeviceID& AnalyzingMachineID) = 0;
    /// <summary>
    /// Function to get the time of acquisition.
    /// </summary>
    virtual DateTime getTimeAcquisition() = 0;
    virtual void setTimeAcquisition(const DateTime& time) = 0;
    /// <summary>
    /// Function to get the high pass baseline filter.
    /// </summary>
    virtual ushort getBaselineFilter() = 0;
    virtual void setBaselineFilter(ushort BaselineFilter) = 0;
    /// <summary>
    /// Function to get the low pass filter.
    /// </summary>
    virtual ushort getLowpassFilter() = 0;
    virtual void setLowpassFilter(ushort LowpassFilter) = 0;
    /// <summary>
    /// Function to get the filter bitmap.
    /// </summary>
    virtual uchar getFilterBitmap() = 0;
    virtual void setFilterBitmap(uchar FilterBitmap) = 0;
    /// <summary>
    /// Function to get the free text fields.
    /// </summary>
    virtual std::vector<string> getFreeTextFields() = 0;
    virtual void setFreeTextFields(const std::vector<string>&      FreeTextFields) = 0;
    /// <summary>
    /// Function to get the sequence number.
    /// </summary>
    virtual string getSequenceNr() = 0;
    virtual void setSequenceNr(const string& SequenceNr) = 0;
    /// <summary>
    /// Function to get the acquiring institution.
    /// </summary>
    virtual string getAcqInstitution() = 0;
    virtual void setAcqInstitution(const string& AcqInstitution) = 0;
    /// <summary>
    /// Function to get the analyzing institution.
    /// </summary>
    virtual string getAnalyzingInstitution() = 0;
    virtual void setAnalyzingInstitution(const string& AnalyzingInstitution) = 0;
    /// <summary>
    /// Function to get the acquiring departement.
    /// </summary>
    virtual string getAcqDepartment() = 0;
    virtual void setAcqDepartment(const string& AcqDepartment) = 0;
    /// <summary>
    /// get and set the analyzing departement.
    /// </summary>
    virtual string getAnalyzingDepartment() = 0;
    virtual void setAnalyzingDepartment(const string& AnalyzingDepartment) = 0;
    /// <summary>
    /// Function to get the referring physician.
    /// </summary>
    virtual string getReferringPhysician() = 0;
    virtual void setReferringPhysician(const string& ReferringPhysician) = 0;
    /// <summary>
    /// Function to get the overreading physician.
    /// </summary>
    virtual string getOverreadingPhysician() = 0;
    virtual void setOverreadingPhysician(const string& OverreadingPhysician) = 0;
    /// <summary>
    /// Function to get the technician desciption.
    /// </summary>
    virtual string getTechnicianDescription() = 0;
    virtual void setTechnicianDescription(const string& TechnicianDescription) = 0;
    /// <summary>
    /// Function to get the systolic blood pressure of the patient.
    /// </summary>
    virtual ushort getSystolicBloodPressure() = 0;
    virtual void setSystolicBloodPressure(ushort SystolicBloodPressure) = 0;
    /// <summary>
    /// Function to get the diastolic blood pressure of the patient.
    /// </summary>
    virtual ushort getDiastolicBloodPressure() = 0;
    virtual void setDiastolicBloodPressure(ushort DiastolicBloodPressure) = 0;
    /// <summary>
    /// Function to get the drugs given to patient.
    /// </summary>
    virtual std::vector<Drug> getDrugs() = 0;
    virtual void setDrugs(std::vector<Drug>& Drugs) = 0;// If used at all it probably needs to be implemented better
    /// <summary>
    /// Function to get referral indication.
    /// </summary>
    virtual std::vector<string> getReferralIndication() = 0;
    virtual void setReferralIndication(const std::vector<string>& ReferralIndication) = 0;
    /// <summary>
    /// Function to get room description.
    /// </summary>
    virtual string getRoomDescription() = 0;
    virtual void setRoomDescription(const string& RoomDescription) = 0;
    /// <summary>
    /// Function to get stat code.
    /// </summary>
    virtual uchar getStatCode() = 0;
    virtual void setStatCode(uchar StatCode) = 0;
};
}
}

#endif  /*#ifndef _IDEMOGRAPHIC_CS_*/
