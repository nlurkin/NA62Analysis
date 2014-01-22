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

	DetectorAcceptance *GetDetectorAcceptanceInstance();
	DetectorAcceptance *IsDetectorAcceptanceInstaciated();

	void PrintInitSummary();

	void checkNewFileOpened();

	void WriteEventFraction();

private:
	void PreProcess();
protected:
	int fEventNb; ///< Number of events available in the TChains
	bool fGraphicMode; ///< Indicating if we only want output file or display
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity of the program
	bool fInitialized;

	vector<Analyzer*> fAnalyzerList; ///< Container for the analyzers

	map<TString, void*> fOutput; ///< Container for outputs of all analyzers
	map<TString, Analyzer::OutputState> fOutputStates; ///< Container for output states for all analyzers
	map<TString, EventFraction*> fEventFraction; ///< Container for event fraction
	map<TString, int> fCounters; ///< Container for counters

	vector<MCSimple*> fMCSimple; ///< Container for MCSimple

	bool fWithMC; ///< Do we have MC in the file?

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Global instance of DetectorAcceptance

	IOHandler fIOHandler;
};

#endif
