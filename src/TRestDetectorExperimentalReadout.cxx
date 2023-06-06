#include "TRestDetectorExperimentalReadout.h"

using namespace std;

ClassImp(TRestDetectorExperimentalReadout);

vector<const TRestDetectorExperimentalReadoutModule*> TRestDetectorExperimentalReadout::GetModulesForPoint(
    const TVector3& point) const {
    vector<const TRestDetectorExperimentalReadoutModule*> modules;
    for (const auto& module : fModules) {
        if (module.IsInside(point)) {
            modules.push_back(&module);
        }
    }
    return modules;
}

vector<const TRestDetectorExperimentalReadoutModule*> TRestDetectorExperimentalReadout::GetModules() const {
    vector<const TRestDetectorExperimentalReadoutModule*> modules;
    modules.reserve(fModules.size());
    for (const auto& module : fModules) {
        modules.push_back(&module);
    }
    return modules;
}

vector<const TRestDetectorExperimentalReadoutPixel*> TRestDetectorExperimentalReadout::GetPixelsForPoint(
    const TVector3& point) const {
    vector<const TRestDetectorExperimentalReadoutPixel*> pixels;
    for (const auto& module : GetModulesForPoint(point)) {
        module->GetPixelsForPoint(point);
    }
}

vector<unsigned short> TRestDetectorExperimentalReadout::GetChannelsForPoint(const TVector3& point) const {
    vector<unsigned short> channels;

    for (const auto& pixel : GetPixelsForPoint(point)) {
        channels.push_back(pixel->GetChannel());
    }

    return channels;
}
