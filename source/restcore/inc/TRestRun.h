
#ifndef RestCore_TRestRun
#define RestCore_TRestRun

#define REST_MAXIMUM_EVENTS 2E9

#include "TRestEvent.h"
#include "TRestMetadata.h"
#include "TRestAnalysisTree.h"
#include "TFileMerger.h"
#include "TFile.h"
#include "TKey.h"

//#include "TRestThread.h"

class TRestEventProcess;

/// Data provider and manager in REST
class TRestRun : public TRestMetadata {
public:
	/// REST run class
	ClassDef(TRestRun, 1);

	//overridden starters
	TRestRun();
	TRestRun(string rootfilename);
	~TRestRun();
	void Initialize();
	void InitFromConfigFile() { TRestMetadata::InitFromConfigFile(); }
	void BeginOfInit();
	Int_t ReadConfig(string keydeclare, TiXmlElement* e);
	void EndOfInit();


	//file operation
	void OpenInputFile(int i);
	void OpenInputFile(TString filename,string mode = "");
	void ReadFileInfo(string filename);

	void ResetEntry();

	Int_t GetNextEvent(TRestEvent* targetevt, TRestAnalysisTree* targettree);
	void GetEntry(int i) { 
		if (fAnalysisTree != NULL) { fAnalysisTree->GetEntry(i); } 
		if (fEventTree != NULL) { fEventTree->GetEntry(i); }
	}

	TString FormFormat(TString FilenameFormat);
	TFile* FormOutputFile(vector<string> filefullnames,string targetfilename="");
	void PassOutputFile() { fOutputFile = fInputFile; fOutputFileName = fOutputFile->GetName(); }
	TFile* FormOutputFile();
	
	void WriteWithDataBase(int level = 1, bool force = false);

	void CloseFile();

	void ImportMetadata(TString rootFile, TString name, Bool_t store);

	/// add metadata object to the metadata list
	void AddMetadata(TRestMetadata* meta) {
		fMetadataInfo.push_back(meta); 
		if (fVerboseLevel >= REST_Info)
		{
			fout << "=added metadata=" << endl;
			meta->PrintMetadata();
			fout << "=" << endl;
			fout << endl;
		}
	}
	void AddEventBranch(TRestEvent* eve);
	void SkipEventTree() {}

	


	//Getters
	TString GetVersion() { return  fVersion; }
	Int_t GetParentRunNumber() { return fParentRunNumber; }
	Int_t GetRunNumber() { return fRunNumber; }
	TString GetRunType() { return fRunType; }
	TString GetRunUser() { return fRunUser; }
	TString GetRunTag() { return fRunTag; }
	TString GetRunDescription() { return fRunDescription; }
	Double_t GetRunLength();
	Double_t GetStartTimestamp() { return fStartTime; }
	Double_t GetEndTimestamp() { return fEndTime; }
	TString GetExperimentName() { return fExperimentName; }
	string Get(string target);

	vector<TString> GetInputFileNames() { return fInputFileNames; }
	string GetInputFileName(int i) { return (string)fInputFileNames[i]; }
	string GetInputFileNamepattern() { return (string)fInputFileName; }
	TString GetOutputFileName() { return fOutputFileName; }
	TFile* GetInputFile() { return fInputFile; }
	TFile* GetOutputFile() { return fOutputFile; }
	int GetCurrentEntry() { return fCurrentEvent; }
	Long64_t GetBytesReaded() { return fBytesReaded; }
	Long64_t GetTotalBytes() { return fTotalBytes; }
	int GetEntries() { if (fAnalysisTree != NULL) { return fAnalysisTree->GetEntries(); } return REST_MAXIMUM_EVENTS; }

	TRestEvent* GetInputEvent() { return fInputEvent; }
	TRestEventProcess* GetFileProcess() { return fFileProcess; }
	string GetFileInfo(string infoname) { return FileInfo[infoname] == "" ? infoname : FileInfo[infoname]; }
	Int_t GetObservableID(TString name) { return fAnalysisTree->GetObservableID(name); }
	Bool_t ObservableExists(TString name) { return fAnalysisTree->ObservableExists(name); }
	TString GetInputEventName() { return fInputEvent->ClassName(); }
	TRestAnalysisTree* GetAnalysisTree() { return fAnalysisTree; }
	TTree* GetEventTree() { return fEventTree; }
	Int_t GetInputFileNumber() { return fFileProcess == NULL ? fInputFileNames.size() : 1; }

	TRestMetadata* GetMetadata(TString name);
	TRestMetadata* GetMetadataClass(string type);
	std::vector <std::string> GetMetadataStructureNames();
	std::vector <std::string> GetMetadataStructureTitles();
	int GetNumberOfMetadataStructures() { return fMetadataInfo.size(); }

	//Setters
	void SetInputFileName(string s) { fInputFileName = s; fInputFileNames = GetFilesMatchingPattern(fInputFileName); }
	void SetExtProcess(TRestEventProcess* p);
	void SetCurrentEntry(int i) { fCurrentEvent = i; }
	//void AddFileTask(TRestFileTask* t) { fFileTasks.push_back(t); }
	void SetInputEvent(TRestEvent* eve);
	void SetRunNumber(Int_t number) { fRunNumber = number; }
	void SetParentRunNumber(Int_t number) { fParentRunNumber = number; }
	void SetRunType(TString type)
	{
		std::string cleanType = RemoveWhiteSpaces((std::string) type);
		fRunType = (TString)cleanType;
	}
	void SetRunTag(TString tag)
	{
		std::string cleanTag = RemoveWhiteSpaces((std::string) tag);
		fRunTag = (TString)cleanTag;
	}
	void SetRunUser(TString user)
	{
		std::string cleanUser = RemoveWhiteSpaces((std::string) user);
		fRunUser = (TString)cleanUser;
	}
	void SetRunDescription(TString description) { fRunDescription = description; }
	void SetEndTimeStamp(Double_t tStamp) { fEndTime = tStamp; }
	void SetTotalBytes(Long64_t b) { fTotalBytes = b; }

	// Printers
	void PrintStartDate();
	void PrintEndDate();

	void PrintInfo();
	void PrintMetadata() { PrintInfo(); }
	void PrintAllMetadata() { 
		PrintInfo();
		for (unsigned int i = 0; i < fMetadataInfo.size(); i++)
			fMetadataInfo[i]->PrintMetadata();
	}
	void PrintTrees() {
		if (fEventTree != NULL)
			fEventTree->Print();
		if (fAnalysisTree != NULL)
		{
			fout << "=" << endl;
			fAnalysisTree->Print();
		}
	}
	void PrintObservables() {
		if (fAnalysisTree != NULL)
			fAnalysisTree->PrintObservables();
	}

	void PrintEvent() { fInputEvent->PrintEvent(); }

protected:
	//runinfo
	Int_t fRunNumber;                 //< first identificative number
	Int_t fParentRunNumber;
	TString fRunClassName;
	TString fRunType;             //< Stores bit by bit the type of run. e.g. calibration, background, pedestal, simulation, datataking 
	TString fRunUser;	          //< To identify the author it has created the run. It might be also a word describing the origin of the run (I.e. REST_Prototype, T-REX, etc)
	TString fRunTag;            //< A tag to be written to the output file
	TString fRunDescription;	  //< A word or sentence describing the run (I.e. Fe55 calibration, cosmics, etc)
	TString fExperimentName;

	//program data
	TString fInputFileName;
	TString fOutputFileName;
	TString fVersion;
	Double_t fStartTime;            ///< Event absolute starting time/date (unix timestamp)
	Double_t fEndTime;              ///< Event absolute starting time/date (unix timestamp)

	//data-like metadata objects
	vector<TRestMetadata*> fMetadataInfo;//!
	map<string, string> FileInfo;

	//temp data member
	vector<TString> fInputFileNames;//!
	TFile *fInputFile;//!
	TFile *fOutputFile;//!
	TRestEvent* fInputEvent;//!
	TTree *fEventTree;//!
	TRestAnalysisTree *fAnalysisTree;//!

	//input infomation
	TRestEventProcess* fFileProcess;//!
	int fCurrentEvent;//!
	Long64_t fBytesReaded;//!
	Long64_t fTotalBytes;//!
	int fEventBranchLoc;//!


};




#endif

