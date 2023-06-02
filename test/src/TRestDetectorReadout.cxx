
#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TRestDetectorExperimentalReadout.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";

std::tuple<unsigned long, double*, double*> GetGraphData(std::vector<TVector2> vertices) {
    const auto n = vertices.size() + 1;

    double x[n];
    double y[n];

    for (int i = 0; i < vertices.size(); i++) {
        x[i] = vertices[i].X();
        y[i] = vertices[i].Y();
    }

    x[n - 1] = vertices[0].X();
    y[n - 1] = vertices[0].Y();

    return {n, x, y};
}

TEST(TRestDetectorReadout, Initialize) {
    auto readout = TRestDetectorExperimentalReadout();

    auto module = TRestDetectorExperimentalReadoutModule();
    readout.AddModule(module);

    double pixelSize = 1.0;
    int moduleSizeInPixels = 10;

    for (int i = 0; i < moduleSizeInPixels; i++) {
        for (int j = 0; j < moduleSizeInPixels; j++) {
            TVector2 center = {i * pixelSize, j * pixelSize};
            auto pixel = TRestDetectorExperimentalReadoutPixel(center, pixelSize);
            module.AddPixel(pixel);
        }
    }

    auto canvas = new TCanvas("canvas", "Shape Canvas", 800, 600);

    // first draw the convex hull for the module
    const auto hullVertices = module.GetConvexHull();

    TGraph* graph;
    {
        const auto& vertices = hullVertices;
        const auto n = vertices.size() + 1;
        double x[n];
        double y[n];

        for (int i = 0; i < n - 1; i++) {
            x[i] = vertices[i].X();
            y[i] = vertices[i].Y();
        }
        x[n - 1] = vertices[0].X();
        y[n - 1] = vertices[0].Y();
        graph = new TGraph(n, x, y);
    }
    graph->Draw();

    // print pixel info
    for (int i = 0; i < module.GetNumberOfPixels(); i++) {
        const auto pixel = module.GetPixels()[i];
        cout << "Pixel X: " << pixel.GetCenter().X() << " Y: " << pixel.GetCenter().Y() << endl;
        const auto vertices = pixel.GetVertices();

        const auto n = vertices.size() + 1;
        double x[n];
        double y[n];

        for (int i = 0; i < n - 1; i++) {
            x[i] = vertices[i].X();
            y[i] = vertices[i].Y();
        }
        x[n - 1] = vertices[0].X();
        y[n - 1] = vertices[0].Y();

        for (int i = 0; i < n; i++) {
            cout << "i: " << i << " X: " << x[i] << " Y: " << y[i] << endl;
        }

        auto graph = new TGraph(vertices.size() + 1, x, y);

        graph->SetLineColor(kRed);
        graph->SetLineWidth(2);

        graph->Draw("SAME");
    }

    canvas->Update();
}
