#ifndef BASEANALYSIS_HH
#define BASEANALYSIS_HH 1

#include "Analyzer.hh"
#include "DetectorAcceptance.hh"
#include "CounterHandler.hh"
#include "IOHandler.hh"

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

	DetectorAcceptance *GetDetectorAcceptanceInstance();
	DetectorAcceptance *IsDetectorAcceptanceInstaciated();

	void PrintInitSummary();

	void CheckNewFileOpened();

	void WriteEventFraction();

	IOHandler * GetIOHandler();

	CounterHandler* GetCounterHandler();

	int GetNEvents();

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

	vector<MCSimple*> fMCSimple; ///< Container for MCSimple

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Global instance of DetectorAcceptance

	CounterHandler fCounterHandler; ///< Handler for EventFraction and Counters
	IOHandler fIOHandler; ///< Handler for all IO objects
};

#endif
