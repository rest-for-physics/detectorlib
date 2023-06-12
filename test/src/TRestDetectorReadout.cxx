
#include <TRestDetectorReadoutPlane.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";

constexpr double tolerance = 1E-6;

bool AreEqual(const TVector3& a, const TVector3& b) { return (a - b).Mag2() < tolerance; }

TEST(TRestDetectorReadout, TestFiles) {
    cout << "FrameworkCore test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));
}

TEST(TRestDetectorReadout, Axes) {
    {
        TRestDetectorReadoutPlane plane;
        plane.PrintMetadata();
        // default values

        EXPECT_TRUE(AreEqual(plane.GetNormal(), {0, 0, 1}));
        EXPECT_TRUE(AreEqual(plane.GetAxisX(), {1, 0, 0}));
        EXPECT_TRUE(AreEqual(plane.GetAxisY(), {0, 1, 0}));
    }

    {
        TRestDetectorReadoutPlane plane;
        plane.SetNormal({0, 0, -1});
        plane.PrintMetadata();

        EXPECT_TRUE(AreEqual(plane.GetNormal(), {0, 0, -1}));
        EXPECT_TRUE(AreEqual(plane.GetAxisX(), {0, -1, 0}));
        EXPECT_TRUE(AreEqual(plane.GetAxisY(), {-1, 0, 0}));
    }

    {
        TRestDetectorReadoutPlane plane;
        plane.SetNormal({0, 0.01, 1});
        plane.PrintMetadata();

        EXPECT_TRUE(AreEqual(plane.GetNormal(), {0, 0.0099995, 0.99995}));
        EXPECT_TRUE(AreEqual(plane.GetAxisX(), {1, 0, 0}));
        EXPECT_TRUE(AreEqual(plane.GetAxisY(), {0, 0.99995, -0.0099995}));
    }
}
