#include <fstream>
#include <iostream>
#include <string>

#include "TRestDetectorReadout.h"
#include "TRestTask.h"

void PrintReadout(TString fName) {
    TString fileName = fName;

    string fname = fileName.Data();

    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    }

    fstream file;
    file.open("print.txt", ios::out);
    string line;

    streambuf* stream_buffer_cout = cout.rdbuf();
    streambuf* stream_buffer_file = file.rdbuf();

    cout.rdbuf(stream_buffer_file);

    TRestRun run(fname);
    TRestDetectorReadout* r = (TRestDetectorReadout*)run.GetMetadata("Prototype_2020_06");
    r->PrintMetadata(3);

    cout.rdbuf(stream_buffer_cout);  // back to the old buffer
    file.close();

    return 0;
}
