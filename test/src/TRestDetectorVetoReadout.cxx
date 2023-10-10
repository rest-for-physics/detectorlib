
#include <TRestDetectorVetoReadout.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto readoutDefaultRml = filesPath / "TRestDetectorVetoReadout.rml";

TEST(TRestDetectorVetoReadout, TestFiles) {
    cout << "FrameworkCore test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(readoutDefaultRml));
}

TEST(TRestDetectorVetoReadout, Default) {
    TRestDetectorVetoReadout readout;

    readout.PrintMetadata();
}

TEST(TRestDetectorVetoReadout, FromRml) {
    TRestDetectorVetoReadout readout(readoutDefaultRml.c_str(), "vetoReadout");
    readout.PrintMetadata();
}
