#include "TRestDetectorExperimentalReadout.h"

using namespace std;

ClassImp(TRestDetectorExperimentalReadout);

std::vector<const TRestDetectorExperimentalReadoutModule*>
TRestDetectorExperimentalReadout::GetModulesInPoint(const TVector3& point) const {
    std::vector<const TRestDetectorExperimentalReadoutModule*> modules;
    for (const auto& module : fModules) {
        if (module.IsInside(point)) {
            modules.push_back(&module);
        }
    }
    return modules;
}

std::vector<const TRestDetectorExperimentalReadoutModule*> TRestDetectorExperimentalReadout::GetModules()
    const {
    std::vector<const TRestDetectorExperimentalReadoutModule*> modules;
    modules.reserve(fModules.size());
    for (const auto& module : fModules) {
        modules.push_back(&module);
    }
    return modules;
}
