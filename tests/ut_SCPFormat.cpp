#include "gtest/gtest.h"
#include "SCPFormat.h"
using namespace ECGConversion::SCP;
using namespace ECGConversion::ECGSignals;
using namespace ECGConversion::ECGDemographics;
using namespace ECGConversion::ECGDiagnostic;
using namespace ECGConversion::ECGGlobalMeasurements;
using namespace ECGConversion::ECGLeadMeasurements;
using namespace ECGConversion::SCP;

// The fixture for testing class Project1. From google test primer.
class ut_SCPFormat : public ::testing::Test
{
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    ut_SCPFormat()
    {
        // You can do set-up work for each test here.
    }

    virtual ~ut_SCPFormat()
    {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // Per-test-suite set-up.
    // Called before the first test in this test suite.
    // Can be omitted if not needed.
    static void SetUpTestSuite()
    {
        // Avoid reallocating static objects if called in subclasses of FooTest.
        if (shared_resource_ == nullptr)
        {
            shared_resource_ = new SCPFormat;
        }
    }

    // Per-test-suite tear-down.
    // Called after the last test in this test suite.
    // Can be omitted if not needed.
    static void TearDownTestSuite()
    {
        delete shared_resource_;
        shared_resource_ = nullptr;
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    virtual void SetUp()
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Objects declared here can be used by all tests in the test case for Project1.
    static SCPFormat* shared_resource_;// Some expensive resource shared by all tests.
};
SCPFormat* ut_SCPFormat::shared_resource_ = nullptr;

static int DemographicWrite(IDemographic* dst)
{
    // Check for correct input.
    if (dst == null)
    {
        return 1;
    }

    // Do copy of all Demographics.
    int err = 0;
    dst->setLastName("src.LastName");
    dst->setFirstName("src.FirstName");
    dst->setPatientID("src.PatientID");
    dst->setSecondLastName("src.SecondLastName");
    dst->setPrefixName("src.PrefixName");
    dst->setSuffixName("src.SuffixName");
    ushort val = 20;
    AgeDefinition age = kAgeYears;

    if (dst->setPatientAge(val, age) != 0)
    {
        err |= (0x1 << 4);
    }

    Date date = Date(1988, 1, 18);
    dst->setPatientBirthDate(date);
    HeightDefinition height = kHeightCentimeters;
    val = 170;

    if (dst->setPatientHeight(val, height) != 0)
    {
        err |= (0x1 << 6);
    }

    WeightDefinition weight = kWeightKilogram;
    val = 50;

    if (dst->setPatientWeight(val, weight) != 0)
    {
        err |= (0x1 << 7);
    }

    dst->setGender(kSexMale);
    dst->setPatientRace(kRaceCaucasian);
    AcquiringDeviceID AcqMachineID;
    AcqMachineID.InstitutionNr = 1;
    AcqMachineID.DepartmentNr = 1;
    AcqMachineID.DeviceID = 1244;
    AcqMachineID.DeviceType = 1;
    AcqMachineID.ManufactorID = 1;
    AcqMachineID.DeviceCapabilities = 1; // Is defined in SCP Section1 tag 14 uchar 18.
    AcqMachineID.ACFrequencyEnvironment = 1; // Is defined in SCP Section1 tag 14 uchar 19.
    uchar ModelDescription[6];
    dst->setAcqMachineID(AcqMachineID);
    dst->setAnalyzingMachineID(AcqMachineID);
    DateTime dateTime;
    dateTime.Year = 2021;
    dateTime.Month = 12;
    dateTime.Day = 12;
    dateTime.Hour = 20;
    dateTime.Minute = 0;
    dateTime.Second = 0;
    dst->setTimeAcquisition(dateTime);
    dst->setBaselineFilter(50);
    dst->setLowpassFilter(10);
    dst->setFilterBitmap(10);
    std::vector<string> FreeTextFields;
    FreeTextFields.clear();
    string freeText = "test freeText";
    FreeTextFields.push_back(freeText);
    dst->setFreeTextFields(FreeTextFields);
    dst->setSequenceNr("src.SequenceNr");
    dst->setAcqInstitution("src.AcqInstitution");
    dst->setAnalyzingInstitution("src.AnalyzingInstitution");
    dst->setAcqDepartment("src.AcqDepartment");
    dst->setAnalyzingDepartment("src.AnalyzingDepartment");
    dst->setReferringPhysician("src.ReferringPhysician");
    dst->setOverreadingPhysician("src.OverreadingPhysician");
    dst->setTechnicianDescription("src.TechnicianDescription");
    dst->setSystolicBloodPressure(50);
    dst->setDiastolicBloodPressure(60);
    std::vector<Drug> Drugs;
    Drugs.clear();
    Drugs.push_back(Drug(1, 2, "Drug TextDesciption"));
    dst->setDrugs(Drugs);
    std::vector<string> ReferralIndication;
    ReferralIndication.clear();
    string referralText = "test referralText";
    ReferralIndication.push_back(referralText);
    dst->setReferralIndication(ReferralIndication);
    dst->setRoomDescription("src.RoomDescription");
    dst->setStatCode(212);
    return err;
}

// Test case must be called the class above
// Also note: use TEST_F instead of TEST to access the test fixture (from google test primer)
TEST_F(ut_SCPFormat, DemographicRW)
{
    IDemographic* dst = shared_resource_->getDemographic();
    ASSERT_NE(dst, nullptr);
    dst->Init();
    int ret = DemographicWrite(dst);
    ASSERT_EQ(ret, 0) << ret;
    EXPECT_STREQ(dst->getLastName().c_str(), "src.LastName");
    EXPECT_STREQ(dst->getFirstName().c_str(), "src.FirstName");
    EXPECT_STREQ(dst->getPatientID().c_str(), "src.PatientID");
    EXPECT_STREQ(dst->getSecondLastName().c_str(), "src.SecondLastName");
    EXPECT_STREQ(dst->getPrefixName().c_str(), "src.PrefixName");
    EXPECT_STREQ(dst->getSuffixName().c_str(), "src.SuffixName");
}

