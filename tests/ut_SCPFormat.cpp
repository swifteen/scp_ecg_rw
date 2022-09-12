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
    FreeTextFields.push_back("test freeText1");
    FreeTextFields.push_back("test freeText2");
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
    Drugs.push_back(Drug(1, 2, "Drug TextDesciption1"));
    Drugs.push_back(Drug(3, 4, "Drug TextDesciption2"));
    dst->setDrugs(Drugs);
    std::vector<string> ReferralIndication;
    ReferralIndication.clear();
    ReferralIndication.push_back("test referralText1");
    ReferralIndication.push_back("test referralText2");
    dst->setReferralIndication(ReferralIndication);
    dst->setRoomDescription("src.RoomDescription");
    dst->setStatCode(212);
    return err;
}

static int SignalWrite(ISignal* dst)
{
    // Check for correct input.
    if (dst == null)
    {
        return 2;
    }

    // Do copy of signals
    Signals signals;
    signals.setNrLeads(12);
    signals.SetAVM(2.5);// AVM in uV
    signals.RhythmSamplesPerSecond = 500;
    std::vector<Signal> leads;
    LeadType leadTypeArray[] =
    {
        LeadTypeI,
        LeadTypeII,
        LeadTypeV1,
        LeadTypeV2,
        LeadTypeV3,
        LeadTypeV4,
        LeadTypeV5,
        LeadTypeV6,
        LeadTypeIII,
        LeadType_aVR,
        LeadType_aVL,
        LeadType_aVF,
    };
    srand(time(null));

    for (int i = 0; i < 12; i++)
    {
        Signal leadSignal;
        leadSignal.Type = leadTypeArray[i];
        leadSignal.RhythmStart = 0;
        leadSignal.RhythmEnd = 5000;
        leadSignal.Rhythm = new short[5000];

        for (int k = 0; k < 5000; k++)
        {
            leadSignal.Rhythm[k] = rand() % 1000;
        }

        leadSignal.RhythmLength = 5000 * sizeof(short); //byte size
        leads.push_back(leadSignal);
    }

    signals.setLeads(leads);

    if (dst->setSignals(signals) != 0)
    {
        //TODO free leadSignal.Rhythm
        return 8;
    }

    //TODO free leadSignal.Rhythm
    return 0;
}


static int GlobalMeasurementWrite(IGlobalMeasurement* dst)
{
    // Check for correct input.
    if (dst == null)
    {
        return 2;
    }

    // Do copy of measurements
    GlobalMeasurements mes;
    mes.setVentRate(72);
    mes.setPdur(50);
    mes.setPRint(134);
    mes.setQRSdur(76);
    mes.setQTdur(404);
    mes.setQTc(425);
    //mes.setQTcType(GlobalMeasurement::QTcCalcTypeBazett);
    mes.setAxis(39, 38, 45);

    if (dst->setGlobalMeasurements(mes) != 0)
    {
        return 8;
    }

    return 0;
}


static int LeadMeasurementWrite(ILeadMeasurement* dst)
{
    // Check for correct input.
    if (dst == null)
    {
        return 2;
    }

    LeadMeasurement leadMeas1(LeadTypeI);
    leadMeas1.setMeasurement(MeasurementTypePdur, 51);
    leadMeas1.setMeasurement(MeasurementTypeQRSdur, 52);
    LeadMeasurement leadMeas2(LeadTypeII);
    leadMeas2.setMeasurement(MeasurementTypePdur, 53);
    leadMeas2.setMeasurement(MeasurementTypeQRSdur, 54);
    // Do copy of measurements
    LeadMeasurements mes(12);
    mes.Measurements[0] = leadMeas1;
    mes.Measurements[1] = leadMeas2;
    mes.Measurements[2] = leadMeas1;
    mes.Measurements[3] = leadMeas2;
    mes.Measurements[4] = leadMeas1;
    mes.Measurements[5] = leadMeas2;
    mes.Measurements[6] = leadMeas1;
    mes.Measurements[7] = leadMeas2;
    mes.Measurements[8] = leadMeas1;
    mes.Measurements[9] = leadMeas2;
    mes.Measurements[10] = leadMeas1;
    mes.Measurements[11] = leadMeas2;

    if (dst->setLeadMeasurements(mes) != 0)
    {
        return 8;
    }

    return 0;
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
    //EXPECT_STREQ(dst->getPrefixName().c_str(), "src.PrefixName");//The protocol is not currently supported
    //EXPECT_STREQ(dst->getSuffixName().c_str(), "src.SuffixName");//The protocol is not currently supported
    ushort val = 0;
    AgeDefinition age = kAgeUnspecified;
    ASSERT_EQ(dst->getPatientAge(val, age), 0);
    EXPECT_EQ(val, 20);
    EXPECT_EQ(age, kAgeYears);
    Date date = dst->getPatientBirthDate();
    EXPECT_EQ(date.Year, 1988);
    EXPECT_EQ(date.Month, 1);
    EXPECT_EQ(date.Day, 18);
    HeightDefinition height = kHeightUnspecified;
    val = 0;
    ASSERT_EQ(dst->getPatientHeight(val, height), 0);
    EXPECT_EQ(val, 170);
    EXPECT_EQ(height, kHeightCentimeters);
    WeightDefinition weight = kWeightUnspecified;
    val = 0;
    ASSERT_EQ(dst->getPatientWeight(val, weight), 0);
    EXPECT_EQ(val, 50);
    EXPECT_EQ(weight, kWeightKilogram);
    Sex sex = dst->getGender();
    EXPECT_EQ(sex, kSexMale);
    Race race = dst->getPatientRace();
    EXPECT_EQ(race, kRaceCaucasian);
    uchar ModelDescription[6];
    AcquiringDeviceID AcqMachineID = dst->getAcqMachineID();
    EXPECT_EQ(AcqMachineID.InstitutionNr, 1);
    EXPECT_EQ(AcqMachineID.DepartmentNr, 1);
    EXPECT_EQ(AcqMachineID.DeviceID, 1244);
    EXPECT_EQ(AcqMachineID.DeviceType, 1);
    EXPECT_EQ(AcqMachineID.ManufactorID, 1);
    EXPECT_EQ(AcqMachineID.DeviceCapabilities, 1);  // Is defined in SCP Section1 tag 14 uchar 18.
    EXPECT_EQ(AcqMachineID.ACFrequencyEnvironment, 1);  // Is defined in SCP Section1 tag 14 uchar 19.
    AcquiringDeviceID AnalyzingMachineID = dst->getAnalyzingMachineID();
    EXPECT_EQ(AnalyzingMachineID.InstitutionNr, 1);
    EXPECT_EQ(AnalyzingMachineID.DepartmentNr, 1);
    EXPECT_EQ(AnalyzingMachineID.DeviceID, 1244);
    EXPECT_EQ(AnalyzingMachineID.DeviceType, 1);
    EXPECT_EQ(AnalyzingMachineID.ManufactorID, 1);
    EXPECT_EQ(AnalyzingMachineID.DeviceCapabilities, 1);  // Is defined in SCP Section1 tag 14 uchar 18.
    EXPECT_EQ(AnalyzingMachineID.ACFrequencyEnvironment, 1);  // Is defined in SCP Section1 tag 14 uchar 19.
    DateTime dateTime = dst->getTimeAcquisition();
    EXPECT_EQ(dateTime.Year, 2021);
    EXPECT_EQ(dateTime.Month, 12);
    EXPECT_EQ(dateTime.Day, 12);
    EXPECT_EQ(dateTime.Hour, 20);
    EXPECT_EQ(dateTime.Minute, 0);
    EXPECT_EQ(dateTime.Second, 0);
    EXPECT_EQ(dst->getBaselineFilter(), 50);
    EXPECT_EQ(dst->getLowpassFilter(), 10);
    EXPECT_EQ(dst->getFilterBitmap(), 10);
    std::vector<string> FreeTextFields = dst->getFreeTextFields();
    ASSERT_EQ(FreeTextFields.size(), 2);
    EXPECT_STREQ(FreeTextFields.at(0).c_str(), "test freeText1");
    EXPECT_STREQ(FreeTextFields.at(1).c_str(), "test freeText2");
    EXPECT_STREQ(dst->getSequenceNr().c_str(), "src.SequenceNr");
    EXPECT_STREQ(dst->getAcqInstitution().c_str(), "src.AcqInstitution");
    EXPECT_STREQ(dst->getAnalyzingInstitution().c_str(), "src.AnalyzingInstitution");
    EXPECT_STREQ(dst->getAcqDepartment().c_str(), "src.AcqDepartment");
    EXPECT_STREQ(dst->getAnalyzingDepartment().c_str(), "src.AnalyzingDepartment");
    EXPECT_STREQ(dst->getReferringPhysician().c_str(), "src.ReferringPhysician");
    EXPECT_STREQ(dst->getOverreadingPhysician().c_str(), "src.OverreadingPhysician");
    EXPECT_STREQ(dst->getTechnicianDescription().c_str(), "src.TechnicianDescription");
    EXPECT_EQ(dst->getSystolicBloodPressure(), 50);
    EXPECT_EQ(dst->getDiastolicBloodPressure(), 60);
    std::vector<Drug> Drugs = dst->getDrugs();
    ASSERT_EQ(Drugs.size(), 2);
    EXPECT_EQ(Drugs.at(0).DrugClass, 1);
    EXPECT_EQ(Drugs.at(0).ClassCode, 2);
    EXPECT_STREQ(Drugs.at(0).TextDesciption.c_str(), "Drug TextDesciption1");
    EXPECT_EQ(Drugs.at(1).DrugClass, 3);
    EXPECT_EQ(Drugs.at(1).ClassCode, 4);
    EXPECT_STREQ(Drugs.at(1).TextDesciption.c_str(), "Drug TextDesciption2");
    std::vector<string> ReferralIndication = dst->getReferralIndication();
    ASSERT_EQ(ReferralIndication.size(), 2);
    EXPECT_STREQ(ReferralIndication.at(0).c_str(), "test referralText1");
    EXPECT_STREQ(ReferralIndication.at(1).c_str(), "test referralText2");
    EXPECT_STREQ(dst->getRoomDescription().c_str(), "src.RoomDescription");
    EXPECT_EQ(dst->getStatCode(), 212);
}

TEST_F(ut_SCPFormat, SignalRW)
{
    ISignal* sigdst = shared_resource_->getSignals();
    ASSERT_NE(sigdst, nullptr);
    int ret = SignalWrite(sigdst);
    ASSERT_EQ(ret, 0);
    Signals signals;
    ret = sigdst->getSignals(signals);
    ASSERT_EQ(ret, 0);
    EXPECT_EQ(signals.getNrLeads(), 12);
    ASSERT_DOUBLE_EQ(signals.RhythmAVM, 2.5);
    EXPECT_EQ(signals.RhythmSamplesPerSecond, 500);
    LeadType leadTypeArray[] =
    {
        LeadTypeI,
        LeadTypeII,
        LeadTypeV1,
        LeadTypeV2,
        LeadTypeV3,
        LeadTypeV4,
        LeadTypeV5,
        LeadTypeV6,
        LeadTypeIII,
        LeadType_aVR,
        LeadType_aVL,
        LeadType_aVF,
    };

    for (int i = 0; i < 12; i++)
    {
        EXPECT_EQ(signals[i].Type, leadTypeArray[i]);
        EXPECT_EQ(signals[i].RhythmStart, 0);
        EXPECT_EQ(signals[i].RhythmEnd, 5000);
        EXPECT_EQ(signals[i].RhythmLength, 5000);
#if 0
        leadSignal.Rhythm = new short[5000];

        for (int k = 0; k < 5000; k++)
        {
            leadSignal.Rhythm[k] = rand() % 1000;
        }

#endif
    }
}

TEST_F(ut_SCPFormat, GlobalMeasurementWrite)
{
    IGlobalMeasurement* mesdst = shared_resource_->getGlobalMeasurements();
    ASSERT_NE(mesdst, nullptr);
    int ret = GlobalMeasurementWrite(mesdst);
    ASSERT_EQ(ret, 0);
    GlobalMeasurements mes;
    ret = mesdst->getGlobalMeasurements(mes);
    ASSERT_EQ(ret, 0);
    EXPECT_EQ(mes.getVentRate(), 72);
    EXPECT_EQ(mes.getPdur(), 50);
    EXPECT_EQ(mes.getPRint(), 134);
    EXPECT_EQ(mes.getQRSdur(), 76);
    EXPECT_EQ(mes.getQTdur(), 404);
    EXPECT_EQ(mes.getQTc(), 425);
    //EXPECT_EQ(mes.getQTcType(), GlobalMeasurement::QTcCalcTypeBazett);
    EXPECT_EQ(mes.getQTdur(), 404);
    EXPECT_EQ(mes.measurment[0].Paxis, 39);
    EXPECT_EQ(mes.measurment[0].QRSaxis, 38);
    EXPECT_EQ(mes.measurment[0].Taxis, 45);
}

TEST_F(ut_SCPFormat, LeadMeasurementWrite)
{
    ILeadMeasurement* leaddst = shared_resource_->getLeadMeasurements();
    ASSERT_NE(leaddst, nullptr);
    int ret = LeadMeasurementWrite(leaddst);
    ASSERT_EQ(ret, 0);
    LeadMeasurements mes;
    ret = leaddst->getLeadMeasurements(mes);
    ASSERT_EQ(ret, 0);
    EXPECT_EQ(mes.Measurements[0].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[0].getMeasurement(MeasurementTypePdur), 51);
    EXPECT_EQ(mes.Measurements[0].getMeasurement(MeasurementTypeQRSdur), 52);
    EXPECT_EQ(mes.Measurements[1].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[1].getMeasurement(MeasurementTypePdur), 53);
    EXPECT_EQ(mes.Measurements[1].getMeasurement(MeasurementTypeQRSdur), 54);
    EXPECT_EQ(mes.Measurements[2].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[2].getMeasurement(MeasurementTypePdur), 51);
    EXPECT_EQ(mes.Measurements[2].getMeasurement(MeasurementTypeQRSdur), 52);
    EXPECT_EQ(mes.Measurements[3].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[3].getMeasurement(MeasurementTypePdur), 53);
    EXPECT_EQ(mes.Measurements[3].getMeasurement(MeasurementTypeQRSdur), 54);
    EXPECT_EQ(mes.Measurements[4].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[4].getMeasurement(MeasurementTypePdur), 51);
    EXPECT_EQ(mes.Measurements[4].getMeasurement(MeasurementTypeQRSdur), 52);
    EXPECT_EQ(mes.Measurements[5].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[5].getMeasurement(MeasurementTypePdur), 53);
    EXPECT_EQ(mes.Measurements[5].getMeasurement(MeasurementTypeQRSdur), 54);
    EXPECT_EQ(mes.Measurements[6].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[6].getMeasurement(MeasurementTypePdur), 51);
    EXPECT_EQ(mes.Measurements[6].getMeasurement(MeasurementTypeQRSdur), 52);
    EXPECT_EQ(mes.Measurements[7].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[7].getMeasurement(MeasurementTypePdur), 53);
    EXPECT_EQ(mes.Measurements[7].getMeasurement(MeasurementTypeQRSdur), 54);
    EXPECT_EQ(mes.Measurements[8].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[8].getMeasurement(MeasurementTypePdur), 51);
    EXPECT_EQ(mes.Measurements[8].getMeasurement(MeasurementTypeQRSdur), 52);
    EXPECT_EQ(mes.Measurements[9].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[9].getMeasurement(MeasurementTypePdur), 53);
    EXPECT_EQ(mes.Measurements[9].getMeasurement(MeasurementTypeQRSdur), 54);
    EXPECT_EQ(mes.Measurements[10].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[10].getMeasurement(MeasurementTypePdur), 51);
    EXPECT_EQ(mes.Measurements[10].getMeasurement(MeasurementTypeQRSdur), 52);
    EXPECT_EQ(mes.Measurements[11].getMeasurementCount(), 2);
    EXPECT_EQ(mes.Measurements[11].getMeasurement(MeasurementTypePdur), 53);
    EXPECT_EQ(mes.Measurements[11].getMeasurement(MeasurementTypeQRSdur), 54);
}
