#include "gtest/gtest.h"
#include "SCPSection0.h"

using namespace ECGConversion::SCP;

// The fixture for testing class Project1. From google test primer.
class ut_SCPSection0 : public ::testing::Test
{
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    ut_SCPSection0()
    {
        // You can do set-up work for each test here.
    }

    virtual ~ut_SCPSection0()
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
            shared_resource_ = new SCPSection0;
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
    static SCPSection0* shared_resource_;// Some expensive resource shared by all tests.
};
SCPSection0* ut_SCPSection0::shared_resource_ = nullptr;

// Test case must be called the class above
// Also note: use TEST_F instead of TEST to access the test fixture (from google test primer)
TEST_F(ut_SCPSection0, getSectionID)
{
    ushort sectionID = shared_resource_->getSectionID(); // we have access to p, declared in the fixture
    EXPECT_EQ(0, sectionID) << "sectionID: " << sectionID << std::endl;
}

