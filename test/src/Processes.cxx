
#include <TRestDetectorAvalancheProcess.h>
#include <TRestDetectorElectronDiffusionProcess.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto restDetectorElectronDiffusionProcess = filesPath / "TRestDetectorElectronDiffusionProcess.rml";

TEST(DetectorLib, TestFiles) {
    cout << "Test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));
    EXPECT_TRUE(fs::exists(restDetectorElectronDiffusionProcess));
}

TEST(TRestDetectorElectronDiffusionProcess, Default) {
    TRestDetectorElectronDiffusionProcess process;
    process.PrintMetadata();

    EXPECT_TRUE(process.GetProcessName() == (std::string) "electronDiffusion");

    // TODO: add setters and getters to process and check values
}

TEST(TRestDetectorElectronDiffusionProcess, FromRml) {
    TRestDetectorElectronDiffusionProcess process(restDetectorElectronDiffusionProcess.c_str());

    process.PrintMetadata();
}
