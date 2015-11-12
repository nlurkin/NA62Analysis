#ifndef BASEANALYSIS_HH
#define BASEANALYSIS_HH 1

#include <sstream>

#include "Analyzer.hh"
#include "DetectorAcceptance.hh"
#include "CounterHandler.hh"
#include "IOTree.hh"
#include "containers.hh"
#include "Verbose.hh"
#include "TimeCounter.h"

#include <TSemaphore.h>

namespace NA62Analysis {
namespace Core {

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

class OMMainWindow;
class IOPrimitive;

class BaseAnalysis: public Verbose {
public:
	BaseAnalysis();
	~BaseAnalysis();

	void AddAnalyzer(Analyzer * const an);
	void StartContinuous(TString inFileList);
	void Init(TString inFileName, TString outFileName, TString params,
			TString configFile, Int_t NFiles, TString refFile,
			bool allowNonExisting);
	bool Process(Long64_t beginEvent, Long64_t maxEvent);
	void InitPrimitives();

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
	IOPrimitive * GetIOPrimitive();

	CounterHandler* GetCounterHandler();

	Long64_t GetNEvents();
	TChain* GetTree(TString name);

	bool IsHistoType() {
		/// \MemberDescr
		/// \return true if IO Handler is compatible with Histo type
		/// \EndMemberDescr
		return fIOHandler->GetIOType() == IOHandlerType::kHISTO
				|| fIOHandler->GetIOType() == IOHandlerType::kTREE;
	}
	;
	bool IsTreeType() {
		/// \MemberDescr
		/// \return true if IO Handler is compatible with Tree type
		/// \EndMemberDescr
		return fIOHandler->GetIOType() == IOHandlerType::kTREE;
	}
	;

	void SetGraphicMode(bool bVal) {
		/// \MemberDescr
		/// \param bVal : Graphic mode On/Off flag
		/// \EndMemberDescr
		fGraphicMode = bVal;
	}
	;
	void SetReadType(IOHandlerType type);
	void SetContinuousReading(bool flagContinuousReading);
	void SetDownscaling(bool flagDownscaling);
	void SetFastStart(bool bVal) {
		/// \MemberDescr
		/// \param bVal : true/false
		///
		/// Enabled/Disable the FastStart option
		/// \EndMemberDescr
		fIOHandler->SetFastStart(bVal);
	}
	void SetPrimitiveFile(TString fileName);

	void ReconfigureAnalyzer(TString analyzerName, TString parameterName,
			TString parameter);

private:
	BaseAnalysis(const BaseAnalysis&); ///< Prevents copy construction
	BaseAnalysis& operator=(const BaseAnalysis&); ///< Prevents copy assignment
	void PreProcess();
	void printCurrentEvent(Long64_t iEvent, Long64_t totalEvents,
			int defaultPrecision, std::string displayType,
			TimeCounter startTime);
	static void ContinuousLoop(void* args);
	void CreateOMWindow();

	/// \struct ThreadArgs_t
	/// \Brief
	/// Arguments to be passed to the thread function.
	/// \EndBrief
	struct ThreadArgs_t {
		BaseAnalysis* ban; ///< Pointer to the parent BaseAnalysis instance
		TString inFileList; ///< Path to the input list file
	};
protected:
	Long64_t fNEvents; ///< Number of events available in the TChains
	int fEventsDownscaling; ///< Downscaling. Read 1 out of x events
	bool fGraphicMode; ///< Indicating if we only want output file or display
	bool fInitialized; ///< Indicate if BaseAnalysis has been initialized
	bool fContinuousReading; ///< Continuous reading enabled?
	bool fSignalStop; ///< Stop signal for the Thread

	std::vector<Analyzer*> fAnalyzerList; ///< Container for the analyzers

	NA62Analysis::NA62Map<TString, const void* const >::type fOutput; ///< Container for outputs of all analyzers
	NA62Analysis::NA62Map<TString, Analyzer::OutputState>::type fOutputStates; ///< Container for output states for all analyzers

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Global instance of DetectorAcceptance

	CounterHandler fCounterHandler; ///< Handler for EventFraction and Counters
	IOHandler* fIOHandler; ///< Handler for all IO objects
	IOPrimitive* fIOPrimitive;

	TimeCounter fInitTime; ///< Time counter for the initialisation step (from constructor to end of Init())

	TMutex fGraphicalMutex; ///< Mutex to prevent TApplication and BaseAnalysis to perform graphical operation at the same time
	TThread *fRunThread; ///< Thread for Process during Online Monitor
	OMMainWindow *fOMMainWindow; ///< Online monitor GUI
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif
