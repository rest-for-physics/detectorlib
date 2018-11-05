#ifndef RestCore_TRestThread
#define RestCore_TRestThread

#include <iostream>

#include <mutex>
#include <thread>


#include "TObject.h"
#include "TFile.h"
#include "TTree.h"

#include <string>
#include "TString.h"
#include <TKey.h>
#include "TFileMerger.h"

#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestEventProcess.h"

#include "TRestAnalysisTree.h"
#include "TRestProcessRunner.h"

//class TRestRun;
//enum REST_Process_Output;
//class TRestThread;

/// Threaded worker of a process chain
class TRestThread : public TRestMetadata
{
private:
	Bool_t isFinished;
	Int_t fThreadId;

	TRestProcessRunner* fHostRunner;//!

	TRestEvent* fInputEvent;//!
	TRestEvent* fOutputEvent;//!

	TTree* fEventTree;//!
	TRestAnalysisTree* fAnalysisTree;//!

	vector<TRestEventProcess*> fProcessChain;//!

#ifndef __CINT__
	TFile* fOutputFile;//!
	vector<string> fTreeBranchDef;//!
	std::thread t;//!
#endif

public:

	void Initialize();
	void InitFromConfigFile() {}

	bool TestRun();
	void PrepareToProcess(bool testrun=true);

	void StartProcess();


	void ProcessEvent();
	//void FillEvent();
	void WriteFile();

	void StartThread();


	void AddProcess(TRestEventProcess *process) { fProcessChain.push_back(process); }

	Int_t ValidateInput(TRestEvent* input);
	Int_t ValidateChain();


	//getter and setter
	void SetThreadId(Int_t id);
	void SetBranchConfig(vector<string> i) { fTreeBranchDef = i; }
	void SetOutputTree(TRestAnalysisTree* t) { fAnalysisTree = t; }
	void SetTRestRunner(TRestProcessRunner* r) { fHostRunner = r; }
	TFile* GetOutputFile() { return fOutputFile; };
	TRestEvent* GetOutputEvent() { return fOutputEvent; }
	Int_t GetProcessnum() { return fProcessChain.size(); }
	TRestEventProcess* GetProcess(int i) { return fProcessChain[i]; }
	TRestAnalysisTree* GetAnalysisTree() { return fAnalysisTree; }
	TTree* GetEventTree() { return fEventTree; }
	Bool_t Finished() { return isFinished; }

	TRestThread() { Initialize(); }
	~TRestThread() {};

	/// Calling CINT
	ClassDef(TRestThread, 1);
};



#endif