
#include <TRestDetectorGas.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto gasDefaultRml = filesPath / "TRestDetectorGasExample.rml";

TEST(TRestDetectorGas, TestFiles) {
    cout << "FrameworkCore test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(gasDefaultRml));
}

TEST(TRestDetectorGas, Default) {
    TRestDetectorGas gas;

    gas.PrintGasInfo();

    // Default parameters
    EXPECT_TRUE(gas.GetPressure() == 1);
    EXPECT_TRUE(gas.GetTemperature() == 300);
    EXPECT_TRUE(gas.GetElectricField() == 0);
}

TEST(TRestDetectorGas, FromRml) {
    TRestDetectorGas gas(gasDefaultRml.c_str(),  // config file
                         "Xenon 10-10E3Vcm",     // name
                         false,                  // generation
                         false                   // test
    );

    EXPECT_TRUE(gas.GetW() == 21.9);
    EXPECT_TRUE(gas.GetTemperature() == 293.15);

    EXPECT_TRUE(gas.GetNofGases() == 1);
    EXPECT_TRUE(gas.GetGasComponentName(0) == "xe");
    EXPECT_TRUE(gas.GetGasComponentFraction(0) == 1.0);

    EXPECT_TRUE(!gas.GetError());
}

/*
TEST(TRestDetectorGas, FromServer) {
    GTEST_SKIP_("Not working on test but works on `restRoot` prompt");

    TRestDetectorGas gas("server",            // use gas server
                         "Xenon 10-10E3Vcm",  // name
                         false,               // generation
                         true                 // test
    );
    gas.PrintGasInfo();

    EXPECT_TRUE(!gas.GetError());

    EXPECT_TRUE(gas.GetW() == 21.9);
    EXPECT_TRUE(gas.GetTemperature() == 293.15);

    EXPECT_TRUE(gas.GetNofGases() == 1);
    EXPECT_TRUE(gas.GetGasComponentName(0) == "xe");
    EXPECT_TRUE(gas.GetGasComponentFraction(0) == 1.0);
}
*/
