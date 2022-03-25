
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
}

TEST(TRestDetectorGas, Basic) {
    TRestDetectorGas gas;

    gas.PrintGasInfo();
}

TEST(TRestDetectorGas, FromRML) {
    const auto gasConfigRml = FILES_PATH / "TRestDetectorGasExample.rml";

    TRestDetectorGas gas(gasConfigRml.c_str(),  // config file
                         "Xenon 10-10E3Vcm",    // name
                         false,                 // generation
                         false                  // test
    );

    cout << "Printing gas info:" << endl;

    gas.PrintGasInfo();
}