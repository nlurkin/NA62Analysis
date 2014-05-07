#ifndef BASEANALYSIS_HH
#define BASEANALYSIS_HH 1

#include "Analyzer.hh"
#include "DetectorAcceptance.hh"
#include "CounterHandler.hh"
#include "IOHandler.hh"
#include "containers.hh"

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

	void AddAnalyzer(Analyzer * const an);
	void SetVerbosity(AnalysisFW::VerbosityLevel v);
	void Init(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, bool graphicMode, TString refFile);
	void Process(int beginEvent, int maxEvent);

	//Output methods
	void RegisterOutput(TString name, const void* const address);
	void SetOutputState(TString name, Analyzer::OutputState state);
	const void *GetOutput(TString name, Analyzer::OutputState &state) const;

	DetectorAcceptance *GetDetectorAcceptanceInstance();
	DetectorAcceptance *IsDetectorAcceptanceInstanciated() const;

	void PrintInitSummary() const;

	void CheckNewFileOpened();

	IOHandler * GetIOHandler();

	CounterHandler* GetCounterHandler();

	int GetNEvents();
	TChain* GetTree(TString name);

private:
	BaseAnalysis(const BaseAnalysis&);
	BaseAnalysis& operator=(const BaseAnalysis&);
	void PreProcess();
protected:
	int fNEvents; ///< Number of events available in the TChains
	bool fGraphicMode; ///< Indicating if we only want output file or display
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity of the program
	bool fInitialized; ///< Indicate if BaseAnalysis has been initialized

	vector<Analyzer*> fAnalyzerList; ///< Container for the analyzers

	AnalysisFW::NA62Map<TString, const void* const>::type fOutput; ///< Container for outputs of all analyzers
	AnalysisFW::NA62Map<TString, Analyzer::OutputState>::type fOutputStates; ///< Container for output states for all analyzers

	vector<MCSimple*> fMCSimple; ///< Container for MCSimple

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Global instance of DetectorAcceptance

	CounterHandler fCounterHandler; ///< Handler for EventFraction and Counters
	IOHandler fIOHandler; ///< Handler for all IO objects
};

#endif
