
#include <TRestDetectorGas.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

#define FILES_PATH fs::path(__FILE__).parent_path().parent_path() / "files"

TEST(TRestDetectorGas, TestFiles) {
    cout << "FrameworkCore test files path: " << FILES_PATH << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(FILES_PATH));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(FILES_PATH));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(FILES_PATH / "TRestDetectorGasExample.rml"));
}

TEST(TRestDetectorGas, Default) {
    TRestDetectorGas gas;

    gas.PrintGasInfo();

    // Default parameters
    EXPECT_TRUE(gas.GetPressure() == 1);
    EXPECT_TRUE(gas.GetTemperature() == 300);
    EXPECT_TRUE(gas.GetElectricField() == 0);
}

TEST(TRestDetectorGas, FromRML) {
    GTEST_SKIP_("Currently gas gives error. TODO: fix this");

    const auto gasConfigRml = FILES_PATH / "TRestDetectorGasExample.rml";

    TRestDetectorGas gas(gasConfigRml.c_str(),  // config file
                         "Xenon 10-10E3Vcm",    // name
                         false,                 // generation
                         false                  // test
    );

    EXPECT_TRUE(gas.GetW() == 21.9);
    EXPECT_TRUE(gas.GetTemperature() == 293.15);

    EXPECT_TRUE(gas.GetNofGases() == 1);
    EXPECT_TRUE(gas.GetGasComponentName(0) == "xe");
    EXPECT_TRUE(gas.GetGasComponentFraction(0) == 1.0);

    gas.PrintGasInfo();

    EXPECT_TRUE(!gas.GetError());
}
