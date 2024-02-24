
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

TEST(TRestDetectorReadout, SetAxisX) {
    TRestDetectorReadoutPlane plane;
    plane.SetNormal({0, 0.5, 1});
    plane.SetPosition({10.0, 5.0, 50.0});
    plane.SetRotation(TMath::DegToRad() * 32.0);
    plane.SetHeight(10.0);
    plane.PrintMetadata();
    // default values

    // axis will be projected into the plane
    const TVector3 newXAxis = {1.0, -2.0, 5.0};

    plane.SetAxisX(newXAxis);

    const TVector3 axisX = plane.GetAxisX();
    const TVector3 newXAxisProjected =
        (newXAxis - plane.GetNormal() * newXAxis.Dot(plane.GetNormal())).Unit();
    cout << "axisX: " << axisX.X() << ", " << axisX.Y() << ", " << axisX.Z() << endl;
    cout << "axisXProjected: " << newXAxisProjected.X() << ", " << newXAxisProjected.Y() << ", "
         << newXAxisProjected.Z() << endl;
    EXPECT_TRUE(AreEqual(plane.GetAxisX(), newXAxisProjected));
}

TEST(TRestDetectorReadout, Module) {
    TRestDetectorReadoutPlane plane;
    plane.SetNormal({0, 0, 1});
    plane.SetPosition({10.0, 5.0, 50.0});
    plane.SetRotation(TMath::DegToRad() * 90.0);
    plane.SetHeight(10.0);

    plane.PrintMetadata();

    const TVector3 worldPoint = {0, 0, 100.0};
    const TVector2 local = plane.GetPositionInPlane(worldPoint);
    cout << "local: " << local.X() << ", " << local.Y() << endl;

    const auto distance = plane.GetDistanceToPlane(worldPoint);
    cout << "distance: " << distance << endl;
}
