#ifndef BASEANALYSIS_HH
#define BASEANALYSIS_HH 1

#include "Analyzer.hh"
#include "DetectorAcceptance.hh"
#include "CounterHandler.hh"
#include "IOTree.hh"
#include "containers.hh"

/// \class BaseAnalysis
/// \Brief 
/// Is taking care of the initialization and closing procedure. Process the analyzers in the required order.
/// \EndBrief 
///
/// \Detailed
/// Main class of the framework. It takes care of initializing everything properly, processing the events,
/// keeping track of every object created by the framework (including user analyzers) and passing them to
/// the user when requested, communication between different parts of the framework, writing the output files.
/// \EndDetailed

class BaseAnalysis
{
public:
	BaseAnalysis();
	~BaseAnalysis();

	void AddAnalyzer(Analyzer * const an);
	void SetVerbosity(AnalysisFW::VerbosityLevel v);
	void Init(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, TString refFile, bool allowNonExisting);
	void Process(int beginEvent, int maxEvent);
	void ProcessWithTree(int beginEvent, int maxEvent);
	void ProcessWithHisto(int beginEvent, int maxEvent);

	//Output methods
	void RegisterOutput(TString name, const void* const address);
	void SetOutputState(TString name, Analyzer::OutputState state);
	const void *GetOutput(TString name, Analyzer::OutputState &state) const;

	DetectorAcceptance *GetDetectorAcceptanceInstance();
	DetectorAcceptance *IsDetectorAcceptanceInstanciated() const;

	void PrintInitSummary() const;

	void CheckNewFileOpened();

	IOHandler * GetIOHandler();
	IOTree * GetIOTree();
	IOHisto * GetIOHisto();

	CounterHandler* GetCounterHandler();

	int GetNEvents();
	TChain* GetTree(TString name);

	bool IsHistoType() {
		/// \MemberDescr
		/// \return true if IO Handler is compatible with Histo type
		/// \EndMemberDescr
		return fIOHandler->GetIOType()==IOHandlerType::kHISTO || fIOHandler->GetIOType()==IOHandlerType::kTREE;
	};
	bool IsTreeType() {
		/// \MemberDescr
		/// \return true if IO Handler is compatible with Tree type
		/// \EndMemberDescr
		return fIOHandler->GetIOType()==IOHandlerType::kTREE;
	};

	void SetGraphicMode(bool bVal) {
		/// \MemberDescr
		/// \param bVal : Graphic mode On/Off flag
		/// \EndMemberDescr
		fGraphicMode = bVal;
	};
	void SetReadType(IOHandlerType type);

private:
	BaseAnalysis(const BaseAnalysis&); ///< Prevents copy construction
	BaseAnalysis& operator=(const BaseAnalysis&); ///< Prevents copy assignment
	void PreProcess();
protected:
	int fNEvents; ///< Number of events available in the TChains
	bool fGraphicMode; ///< Indicating if we only want output file or display
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity of the program
	bool fInitialized; ///< Indicate if BaseAnalysis has been initialized

	vector<Analyzer*> fAnalyzerList; ///< Container for the analyzers

	AnalysisFW::NA62Map<TString, const void* const>::type fOutput; ///< Container for outputs of all analyzers
	AnalysisFW::NA62Map<TString, Analyzer::OutputState>::type fOutputStates; ///< Container for output states for all analyzers

	//AnalysisFW::NA62Map<TString, MCSimple*>::type fMCSimple; ///< Container for MCSimple

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Global instance of DetectorAcceptance

	CounterHandler fCounterHandler; ///< Handler for EventFraction and Counters
	IOHandler* fIOHandler; ///< Handler for all IO objects
};

#endif
