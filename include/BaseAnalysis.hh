#ifndef BASEANALYSIS_HH
#define BASEANALYSIS_HH 1

#include "TString.h"
#include "TFile.h"

#include "Analyzer.hh"
#include "MCSimple.hh"
#include "DetectorAcceptance.hh"
#include "EventFraction.hh"
#include "functions.hh"
#include "FWEnums.hh"

/// \class BaseAnalysis
/// \Brief 
/// Is taking care of the initialization and closing procedure. Process the analyzers in the required order.
/// \EndBrief 
///
/// \Detailed
/// Is taking care of the initialization and closing procedure. Process the analyzers in the required order.
/// \EndDetailed

class BaseAnalysis
{
public:
	BaseAnalysis();
	~BaseAnalysis();

	void AddAnalyzer(Analyzer *an);
	void SetVerbosity(AnalysisFW::VerbosityLevel v);
	void Init(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, bool textonly, TString refFile);
	void Process(int beginEvent, int maxEvent);

	//Output methods
	void RegisterOutput(TString name, void* address);
	void SetOutputState(TString name, Analyzer::OutputState state);
	const void *GetOutput(TString name, Analyzer::OutputState &state);

	//Event fraction methods
	void NewEventFraction(TString name);
	void AddCounterToEventFraction(TString efName, TString cName);
	void DefineSampleSizeCounter(TString efName, TString cName);
	void SetSignificantDigits(TString efName, int v);

	//Counter methods
	void BookCounter(TString name);
	void IncrementCounter(TString name);
	void IncrementCounter(TString name, int v);
	void DecrementCounter(TString name);
	void DecrementCounter(TString name, int v);
	void SetCounterValue(TString name, int v);
	int GetCounterValue(TString);

	//Histogram Retrieving methods
	TH1* GetInputHistogram(TString directory, TString name, bool append);
	TH1* GetReferenceHistogram(TString name);

	//Writing methods
	void WriteEvent();
	void WriteTree();
	void WriteEventFraction();

	void RequestTree(TString name, TDetectorVEvent *evt);
	bool RequestTree(TString name, TString branchName, TString className, void* obj);

	TDetectorVEvent *GetEvent(TString name);
	void* GetObject(TString name);

	DetectorAcceptance *GetDetectorAcceptanceInstance();
	DetectorAcceptance *IsDetectorAcceptanceInstaciated();

	void PrintInitSummary();

	void checkNewFileOpened();

private:
	void FillMCTruth();
	void PreProcess();
	void GetTree(int &eventNb);
	void FindAndGetTree(TChain* tree, TString branchName, TString branchClass, void* evt, Int_t &eventNb);
	Bool_t checkInputFile(TString fileName);
protected:
	class ObjectTriplet{
	public:
		ObjectTriplet(TString c, TString branch, void* obj){
			fClassName = c;
			fBranchName = branch;
			fObject = obj;
		};
		TString fClassName;
		TString fBranchName;
		void* fObject;
	};
	int fEventNb; ///< Number of events available in the TChains
	bool fGraphicMode; ///< Indicating if we only want output file or display
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity of the program
	bool fInitialized;


	//TChain *fMCTruthTree[2]; ///< Container for the MC TTrees
	TChain *fMCTruthTree; ///< Container for the MC TTrees
	Event *fMCTruthEvent; ///< MC Event

	vector<Analyzer*> fAnalyzerList; ///< Container for the analyzers

	TFile *fOutFile; ///< Output file
	TString fOutFileName; ///< Output fileName

	map<TString, TChain*> fTree; ///< Container for the trees
	map<TString, TDetectorVEvent*> fEvent; ///< Container for the events
	map<TString, ObjectTriplet*> fObject; ///< Container for the events
	map<TString, TTree*> fExportTrees; ///< Container for TTrees for exporting
	map<TString, void*> fOutput; ///< Container for outputs of all analyzers
	map<TString, Analyzer::OutputState> fOutputStates; ///< Container for output states for all analyzers
	map<TString, EventFraction*> fEventFraction; ///< Container for event fraction
	map<TString, int> fCounters; ///< Container for counters
	multimap<TString, TH1*> fInputHistoAdd; ///< Container for input histograms for which we append the values of the new files
	multimap<TString, TH1*> fInputHisto; ///< Container for input histograms for which we do not append the values of the new files

	int fCurrentFileNumber; ///< Index of the current opened file in the TChain

	vector<MCSimple*> fMCSimple; ///< Container for MCSimple

	bool fWithMC; ///< Do we have MC in the file?

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Global instance of DetectorAcceptance

	TString fReferenceFileName; ///< Name of the file containing reference plots to compare with
};

#endif
