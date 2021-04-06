#include "TRestDetector.h"

void TRestDetector::PrintMetadata() {
    TRestMetadata::PrintMetadata();
    any cl = any(this, this->ClassName());
    for (int i = 0; i < cl.GetNumberOfDataMembers(); i++) {
        if (cl.GetDataMember(i).name == "fgIsA") continue;
        if (cl.GetDataMember(i).ToString() != "-1")
            metadata << "Field: " << cl.GetDataMember(i).name << ", value: " << cl.GetDataMember(i).ToString()
                     << endl;
    }
    metadata << "---------------------------------------" << endl;
}

ClassImp(TRestDetector);
