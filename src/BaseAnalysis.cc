#include "BaseAnalysis.hh"

#include <iomanip>
#include <sstream>
#include <TStyle.h>
#include <TFile.h>
#include <TThread.h>
#include <TGClient.h>

#include "ConfigAnalyzer.hh"
#include "StringBalancedTable.hh"
#include "TermManip.hh"
#include "ConfigSettings.hh"
#include "OMMainWindow.hh"

namespace NA62Analysis {
namespace Core {

BaseAnalysis::BaseAnalysis() :
		Verbose("BaseAnalysis"), fNEvents(-1), fEventsDownscaling(0), fGraphicMode(
				false), fInitialized(false), fContinuousReading(false), fSignalStop(
				false), fDetectorAcceptanceInstance(nullptr), fIOHandler(
				nullptr), fInitTime(true), fRunThread(nullptr), fOMMainWindow(
				nullptr) {
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr

	Configuration::ConfigSettings().ParseFile(
			TString(std::getenv("ANALYSISFW_USERDIR"))
					+ TString("/.settingsna62"));
	gStyle->SetOptFit(1);
	NA62Analysis::manip::enableManip =
			Configuration::ConfigSettings::global::fUseColors
					&& isatty(fileno(stdout));
}

BaseAnalysis::~BaseAnalysis() {
	/// \MemberDescr
	/// Destructor.
	/// \EndMemberDescr

	if (fRunThread) {
		fGraphicalMutex.UnLock();
		fSignalStop = true;
		fIOHandler->SignalExit();
		fRunThread->Delete();
		while (fRunThread->GetState() != TThread::kCanceledState
				&& fRunThread->GetState() != TThread::kFinishedState) {
			gSystem->Sleep(300);
		}
	}
	if (fDetectorAcceptanceInstance)
		delete fDetectorAcceptanceInstance;
}

void BaseAnalysis::Init(TString inFileName, TString outFileName, TString params,
		TString configFile, Int_t NFiles, TString refFile,
		bool ignoreNonExisting) {
	/// \MemberDescr
	/// \param inFileName : path to the input file / path to the file containing the list of input files
	/// \param outFileName : path to the output file
	///	\param params : list of command line parameters to parse and pass to analyzers
	/// \param configFile : path to a runtime configuration file to be parsed and defining parameters for analyzers.
	///	\param NFiles : Maximum number of input files to process
	/// \param refFile : Eventual name of a file containing reference plots
	/// \param allowNonExisting : Continue processing if input tree is not found
	///
	/// Add all the input files to TChains and to Analyzers and create branches.\n
	/// Initialize the output trees (Histograms) and create branches.
	/// \EndMemberDescr

	//##############################
	//Get data from Files
	//Check integrity
	//Check all the data are present
	//##############################
	TString anName, anParams;

	std::cout << debug() << "Initializing... " << std::endl;
	if (!fIOHandler->OpenInput(inFileName, NFiles))
		return;

	fIOHandler->OpenOutput(outFileName);

	if (IsTreeType()) {
		IOTree * treeHandler = static_cast<IOTree*>(fIOHandler);

		treeHandler->SetReferenceFileName(refFile);
		treeHandler->SetIgnoreNonExisting(ignoreNonExisting);

		fNEvents = std::max(treeHandler->FillMCTruth(),
				treeHandler->FillRawHeader());

		std::cout << debug() << "Using " << fNEvents << " events" << std::endl;
	}

	if (IsTreeType())
		fNEvents = GetIOTree()->BranchTrees(fNEvents);
	else if (IsHistoType())
		fNEvents = fIOHandler->GetInputFileNumber();

	int testEvent = 0;
	while (!fIOHandler->LoadEvent(testEvent) && testEvent < fNEvents)
		testEvent++;
	if (testEvent == fNEvents) {
		std::cout << "Unable to load any event/file. Aborting processing"
				<< std::endl;
		raise(SIGABRT);
	}
	fIOHandler->CheckNewFileOpened();

	std::cout << debug() << "Parsing parameters" << std::endl;
	//Parse parameters from file
	Configuration::ConfigAnalyzer confParser;
	confParser.ParseFile(configFile);
	//Parse parameters from commandLine
	confParser.ParseCLI(params);

	std::cout << debug() << "Parsed parameters: " << std::endl;
	if (TestLevel(Verbosity::kDebug))
		confParser.Print();

	for (unsigned int i = 0; i < fAnalyzerList.size(); i++) {
		fIOHandler->MkOutputDir(fAnalyzerList[i]->GetAnalyzerName());
		gFile->cd(fAnalyzerList[i]->GetAnalyzerName());

		confParser.ApplyParams(fAnalyzerList[i]);

		fAnalyzerList[i]->InitOutput();
		fAnalyzerList[i]->InitHist();

		fAnalyzerList[i]->DefineMCSimple();
		fAnalyzerList[i]->PrintInitSummary();
		gFile->cd();
	}

	PrintInitSummary();

	fInitialized = true;
	fInitTime.Stop();
}

void BaseAnalysis::AddAnalyzer(Analyzer* an) {
	/// \MemberDescr
	/// \param an : Pointer to the analyzer
	///
	/// Add an analyzer to the Analyzer lists
	/// \EndMemberDescr

	std::cout << normal() << "Adding analyzer " << an->GetAnalyzerName()
			<< std::endl;
	an->SetVerbosity(GetVerbosityLevel());
	fAnalyzerList.push_back(an);
}

void BaseAnalysis::RegisterOutput(TString name, const void * const address) {
	/// \MemberDescr
	/// \param name : Name of the output
	/// \param address : pointer to the output variable
	///
	/// Register an output
	/// \EndMemberDescr

	std::cout << normal() << "Registering output " << name << std::endl;
	std::cout << debug() << " at address " << address << std::endl;
	fOutput.insert(std::pair<TString, const void* const >(name, address));
	fOutputStates.insert(
			std::pair<TString, Analyzer::OutputState>(name,
					Analyzer::kOUninit));
}

void BaseAnalysis::SetOutputState(TString name, Analyzer::OutputState state) {
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : state to be set
	///
	/// Set the state of the output
	/// \EndMemberDescr

	fOutputStates[name] = state;
}

const void *BaseAnalysis::GetOutput(TString name,
		Analyzer::OutputState &state) const {
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : is filled with the current state of the output
	///
	/// Return an output variable and the corresponding state
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString, const void* const >::type::const_iterator ptr;

	if ((ptr = fOutput.find(name)) != fOutput.end()) {
		state = fOutputStates.find(name)->second;
		return ptr->second;
	} else {
		state = Analyzer::kOUninit;
		std::cout << normal() << "Output " << name << " not found" << std::endl;
		return 0;
	}
}

void BaseAnalysis::PreProcess() {
	/// \MemberDescr
	/// Pre-processing method. Reset the states of the output
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString, Analyzer::OutputState>::type::iterator itState;
	NA62Analysis::NA62Map<TString, void*>::type::iterator itOut;

	for (itState = fOutputStates.begin(); itState != fOutputStates.end();
			itState++) {
		itState->second = Analyzer::kOInvalid;
	}

	for (unsigned int j = 0; j < fAnalyzerList.size(); j++) {
		fAnalyzerList[j]->PreProcess();
	}
}

bool BaseAnalysis::Process(Long64_t beginEvent, Long64_t maxEvent) {
	/// \MemberDescr
	/// \param beginEvent : index of the first event to be processed
	/// \param maxEvent : maximum number of events to be processed
	/// \return True if successful
	///
	/// Main process loop. Read the files event by event and process each analyzer in turn for each event
	/// \EndMemberDescr

	if (!fInitialized)
		return false;

	TimeCounter processLoopTime;
	TimeCounter processTime;
	int i_offset;
	bool exportEvent = false;

	processLoopTime.Start();

	std::string displayType;
	if (IsTreeType())
		displayType = "Event";
	else if (IsHistoType())
		displayType = "File";

	//Print event processing summary
	if (maxEvent > fNEvents || maxEvent <= 0)
		maxEvent = fNEvents;
	std::cout << normal() << "Treating " << maxEvent << " " << displayType
			<< "s, beginning with " << displayType << " " << beginEvent
			<< std::endl;

	if (fIOHandler->IsFastStart())
		i_offset = 1000;
	else
		i_offset = maxEvent / 100.;
	if (i_offset == 0)
		i_offset = 1;
	std::cout << extended() << "i_offset : " << i_offset << std::endl;

	for (unsigned int j = 0; j < fAnalyzerList.size(); j++) {
		gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
		fAnalyzerList[j]->StartOfRun();
		if (IsTreeType())
			fAnalyzerList[j]->StartOfBurst();
	}

	//##############################
	//Begin event loop
	//##############################
	int defaultPrecision = std::cout.precision();
	Long64_t processEvents = std::min(beginEvent + maxEvent, fNEvents);

	for (Long64_t i = beginEvent; (i < processEvents || processEvents < 0);
			i++) {
		//Print current event
		if (i % i_offset == 0) {
			printCurrentEvent(i, processEvents, defaultPrecision, displayType,
					processLoopTime);
		}
		if (fEventsDownscaling > 0 && (i % fEventsDownscaling != 0))
			continue;

		// Load event infos
		if (!fIOHandler->LoadEvent(i))
			std::cout << normal() << "Unable to read event " << i << std::endl;
		CheckNewFileOpened();

		processTime.Start();
		PreProcess();
		//Process event in Analyzer
		exportEvent = false;
		for (unsigned int j = 0; j < fAnalyzerList.size(); j++) {
			//Get MCSimple
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			if (IsTreeType() && static_cast<IOTree*>(fIOHandler)->GetWithMC())
				fAnalyzerList[j]->FillMCSimple(
						static_cast<IOTree*>(fIOHandler)->GetMCTruthEvent());

			fAnalyzerList[j]->Process(i);
			if (fGraphicalMutex.Lock() == 0) {
				fAnalyzerList[j]->UpdatePlots(i);
				fGraphicalMutex.UnLock();
			}
			exportEvent = exportEvent || fAnalyzerList[j]->GetExportEvent();
			gFile->cd();
		}

		for (unsigned int j = 0; j < fAnalyzerList.size(); j++) {
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			fAnalyzerList[j]->PostProcess();
			gFile->cd();
		}
		processTime.Stop();

		if (IsTreeType() && exportEvent)
			static_cast<IOTree*>(fIOHandler)->WriteEvent();

		//We finally know the total number of events in the sample
		if (fIOHandler->IsFastStart() && fNEvents < processEvents)
			processEvents = fNEvents;
	}

	printCurrentEvent(processEvents - 1, processEvents, defaultPrecision,
			displayType, processLoopTime);
	std::cout << std::endl;

	//Ask the analyzer to export and draw the plots
	for (unsigned int j = 0; j < fAnalyzerList.size(); j++) {
		gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
		fAnalyzerList[j]->EndOfBurst();
		fAnalyzerList[j]->EndOfRun();

		fAnalyzerList[j]->ExportPlot();
		if (fGraphicalMutex.Lock() == 0) {
			if (fGraphicMode)
				fAnalyzerList[j]->DrawPlot();
			fGraphicalMutex.UnLock();
		}
		fAnalyzerList[j]->WriteTrees();
		gFile->cd();
	}
	if (IsTreeType())
		static_cast<IOTree*>(fIOHandler)->WriteTree();
	fIOHandler->Finalise();
	fCounterHandler.WriteEventFraction(fIOHandler->GetOutputFileName());

	//Complete the analysis
	using NA62Analysis::operator -;
	float totalTime = fInitTime.GetTime() - fInitTime.GetStartTime();
	std::cout << std::setprecision(2);
	std::cout << std::endl << "###################################"
			<< std::endl;
	std::cout << "Total time: " << std::setw(17) << std::fixed << totalTime
			<< " seconds" << std::endl;
	std::cout << " - Init time: " << std::setw(15) << std::fixed
			<< fInitTime.GetTotalTime() << " seconds" << std::endl;
	std::cout << " - Process loop time: " << std::setw(7) << std::fixed
			<< processLoopTime.GetTotalTime() << " seconds" << std::endl;
	std::cout << "   - Processing time: " << std::setw(7)
			<< processTime.GetTotalTime() << " seconds" << std::endl;
	std::cout << "IO time: " << std::setw(20)
			<< fIOHandler->GetIoTimeCount().GetTotalTime() << " seconds"
			<< std::endl;
	std::cout << std::endl << "Analysis complete" << std::endl
			<< "###################################" << std::endl;

	return true;
}

DetectorAcceptance* BaseAnalysis::GetDetectorAcceptanceInstance() {
	/// \MemberDescr
	/// Return a pointer to the unique global instance of DetectorAcceptance.\n
	/// If not yet instantiated, instantiate it.
	/// \return Pointer to the unique global instance of DetectorAcceptance
	/// \EndMemberDescr

	if (fDetectorAcceptanceInstance == NULL) {
		fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.root");
	}

	return fDetectorAcceptanceInstance;
}

DetectorAcceptance* BaseAnalysis::IsDetectorAcceptanceInstanciated() const {
	/// \MemberDescr
	/// \return Pointer to the unique global instance of DetectorAcceptance if instantiated. Otherwise return null.
	/// \EndMemberDescr

	return fDetectorAcceptanceInstance;
}

void BaseAnalysis::PrintInitSummary() const {
	/// \MemberDescr
	/// Print summary after initialization.
	/// \EndMemberDescr

	if (!TestLevel(Verbosity::kStandard))
		return;

	std::vector<Analyzer*>::const_iterator itAn;
	NA62Analysis::NA62Map<TString, const void* const >::type::const_iterator itOutput;

	StringBalancedTable anTable("List of loaded Analyzers");
	StringBalancedTable outputTable("List of Outputs");

	for (itAn = fAnalyzerList.begin(); itAn != fAnalyzerList.end(); itAn++) {
		anTable << (*itAn)->GetAnalyzerName();
	}

	for (itOutput = fOutput.begin(); itOutput != fOutput.end(); itOutput++) {
		outputTable << itOutput->first;
	}

	std::cout
			<< "================================================================================"
			<< std::endl;
	std::cout << std::endl << "\t *** Global settings for AnalysisFW ***"
			<< std::endl << std::endl;

	anTable.Print("\t");
	fCounterHandler.PrintInitSummary();
	outputTable.Print("\t");
	fIOHandler->PrintInitSummary();
	std::cout
			<< "================================================================================"
			<< std::endl;
}

void BaseAnalysis::CheckNewFileOpened() {
	/// \MemberDescr
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	if (!fIOHandler->CheckNewFileOpened())
		return;
	//New file opened
	std::cout << debug() << "New file opened" << std::endl;
	//first burst or not? Call end of burst only if it's not
	if (fIOHandler->GetCurrentFileNumber() > 0) {
		//end of burst
		for (unsigned int i = 0; i < fAnalyzerList.size(); i++) {
			fAnalyzerList[i]->EndOfBurst();
		}
		if (IsHistoType())
			GetIOHisto()->UpdateInputHistograms();
	}

	for (unsigned int i = 0; i < fAnalyzerList.size(); i++) {
		// Update number of events
		if (fIOHandler->IsFastStart())
			fNEvents = fIOHandler->GetNEvents();
		fAnalyzerList[i]->StartOfBurst();
	}
}

IOHandler* BaseAnalysis::GetIOHandler() {
	/// \MemberDescr
	///	\return Pointer to the IOHandler instance
	/// \EndMemberDescr

	return fIOHandler;
}

CounterHandler* BaseAnalysis::GetCounterHandler() {
	/// \MemberDescr
	///	\return Pointer to the CounterHandler instance
	/// \EndMemberDescr

	return &fCounterHandler;
}

Long64_t BaseAnalysis::GetNEvents() {
	/// \MemberDescr
	///	\return Total number of events loaded from the input files
	/// \EndMemberDescr
	return fNEvents;
}

TChain* BaseAnalysis::GetTree(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TChain
	///
	///	\return Pointer to the TChain
	/// \EndMemberDescr

	if (IsTreeType())
		return GetIOTree()->GetTree(name);
	else
		return nullptr;
}

IOTree* BaseAnalysis::GetIOTree() {
	/// \MemberDescr
	/// \return Pointer to IOTree ifIOHandler is of Tree Type
	/// \EndMemberDescr

	if (IsTreeType())
		return static_cast<IOTree*>(fIOHandler);
	else
		return nullptr;
}

IOHisto* BaseAnalysis::GetIOHisto() {
	/// \MemberDescr
	/// \return Pointer to IOHisto ifIOHandler is of Histo Type
	/// \EndMemberDescr

	if (IsHistoType())
		return static_cast<IOHisto*>(fIOHandler);
	else
		return nullptr;
}

void BaseAnalysis::SetReadType(IOHandlerType type) {
	/// \MemberDescr
	/// \param type: Type of IOHandler to use
	///
	/// Create the correct instance of IOHandler
	/// \EndMemberDescr

	std::cout << normal() << "Creating IOHandler of type "
			<< (type == IOHandlerType::kHISTO ? "kHisto" : "kTree")
			<< std::endl;
	if (type == IOHandlerType::kHISTO)
		fIOHandler = new IOHisto();
	else
		fIOHandler = new IOTree();

	fIOHandler->SetMutex(&fGraphicalMutex);
}

void BaseAnalysis::printCurrentEvent(Long64_t iEvent, Long64_t totalEvents,
		int defaultPrecision, std::string displayType, TimeCounter startTime) {
	/// \MemberDescr
	/// \param iEvent: currently processed object
	/// \param totalEvents: total number of objects
	/// \param defaultPrecision: default floating point number precision in cout
	/// \param displayType: Type of object (event, file)
	/// \param startTime: start time of the processing
	///
	/// Print the currently processed object. Formatting done according to settings.
	/// Also print the percentage of completion and the estimated remaining time.
	/// \EndMemberDescr

	if (!TestLevel(Verbosity::kStandard))
		return;
	std::stringstream ss;

	//Print current event
	if (Configuration::ConfigSettings::global::fUseColors)
		std::cout << manip::red << manip::bold;

	float elapsed = startTime.GetTotalTime();
	float eta = 0;
	if (iEvent > 0)
		eta = (elapsed) * ((totalEvents - iEvent) / (double) iEvent);
	float totalTime = iEvent > 0 ? elapsed + eta : elapsed;

	//Processing what current/total =>
	ss << "*** Processing " << displayType << " " << iEvent << "/"
			<< totalEvents;
	std::cout << std::setw(35) << std::left << ss.str() << " => ";
	// percentage%
	std::cout << std::setprecision(2) << std::fixed << std::setw(6)
			<< std::right << ((double) iEvent / (double) totalEvents) * 100
			<< "%";
	// ETA: 123s
	if (iEvent == 0)
		std::cout << std::setw(10) << "ETA: " << "----s";
	else
		std::cout << std::setw(10) << "ETA: " << eta << "s";

	// Elapsed: 123s
	std::cout << std::setw(14) << "Elapsed: " << elapsed << "s";
	// Total: 123s
	std::cout << std::setw(12) << "Total: " << totalTime << "s";

	if (Configuration::ConfigSettings::global::fUseColors)
		std::cout << manip::reset;
	if (Configuration::ConfigSettings::global::fProcessOutputNewLine)
		std::cout << std::endl;
	else
		std::cout << std::setw(10) << "\r" << std::flush;

	//Reset to default
	std::cout.precision(defaultPrecision);
	std::cout.unsetf(std::ios_base::floatfield);
}

void BaseAnalysis::SetContinuousReading(bool flagContinuousReading) {
	/// \MemberDescr
	/// \param flagContinuousReading: enable/disable flag
	///
	/// Enable/Disable the continuousReading mode in both BaseAnalysis and the IOHandler
	/// \EndMemberDescr

	fContinuousReading = flagContinuousReading;
	fIOHandler->SetContinuousReading(flagContinuousReading);
}

void BaseAnalysis::SetDownscaling(bool flagDownscaling) {
	/// \MemberDescr
	/// \param flagDownscaling: enable/disable flag
	///
	/// Enable/Disable the downscaling
	/// \EndMemberDescr

	if (flagDownscaling)
		fEventsDownscaling =
				Configuration::ConfigSettings::global::fEventsDownscaling;
	else
		fEventsDownscaling = 0;
}

void BaseAnalysis::StartContinuous(TString inFileList) {
	/// \MemberDescr
	/// \param inFileList: Path to a text file containing a list of input root files
	///
	/// Start the continuous reading loop:
	/// - Setup the GUI
	/// - Start the Processing loop in its own thread to decouple the GUI from the processing
	/// \EndMemberDescr

	//Prepare TThread arguments (needs reference to this and input file list
	ThreadArgs_t *args = new ThreadArgs_t();
	args->ban = this;
	args->inFileList = inFileList;

	//Create GUI
	CreateOMWindow();

	//Start the thread
	fRunThread = new TThread("t0", (void (*)(void*))&ContinuousLoop, (void*) args);
	//Allow cancelation of thread only at specific points
	fRunThread->SetCancelOn();
	fRunThread->SetCancelDeferred();
	fRunThread->Run();

	while (1) {
		//Graphical loop. Only process GUI events when the Process loop is not touching graphical objects.
		//Else crashes occurs
		if (fGraphicalMutex.Lock() == 0) {
			fIOHandler->SetOutputFileAsCurrent();
			gSystem->ProcessEvents();
			fGraphicalMutex.UnLock();
		}
	}
}

void BaseAnalysis::CreateOMWindow() {
	/// \MemberDescr
	/// Create GUI.
	/// - Create main window
	/// - Create one tab for each Analyzer
	/// - Within the analyzer tab, create one tab for each CanvasOrganizer
	///   and pass the created canvas to the Organizer
	/// \EndMemberDescr

	fOMMainWindow = new OMMainWindow(gClient->GetRoot(),
			gClient->GetDisplayHeight(), gClient->GetDisplayWidth());
	for (auto it : fAnalyzerList) {
		fOMMainWindow->AddAnalyzerTab(it->GetAnalyzerName());
		for (auto itCanvas : it->GetCanvases()) {
			itCanvas.second->SetCanvas(
					fOMMainWindow->AddAnalyzerCanvas(it->GetAnalyzerName(),
							itCanvas.first));
		}
	}
	fOMMainWindow->Create();
}

void BaseAnalysis::ContinuousLoop(void* args) {
	/// \MemberDescr
	/// \param args: arguments passed to the TThread. Expected a pointer to ThreadArgs_t struct.
	///
	/// Process loop:
	/// - Wait for valid input files list
	/// - Process the root files
	/// - Start again until main thread signal end of processing through fSignalStop
	/// \EndMemberDescr

	BaseAnalysis* ban = ((ThreadArgs_t*) args)->ban;
	TString inFileList = ((ThreadArgs_t*) args)->inFileList;
	while (!ban->fSignalStop) {
		ban->GetIOHandler()->OpenInput(inFileList, -1);
		ban->GetIOHandler()->SetOutputFileAsCurrent();
		ban->Process(0, -1);
		TThread::CancelPoint();
	}
	ban->fSignalStop = false;
}

void BaseAnalysis::ReconfigureAnalyzer(TString analyzerName,
		TString parameterName, TString parameter) {
	/// \MemberDescr
	/// \param analyzerName : Analyzer to reconfigure
	/// \param parameterName : Parameter to modify
	/// \param parameter : Value to set
	///
	/// Modify parameter of an analyzer during the processing
	/// \EndMemberDescr
	for (auto it : fAnalyzerList) {
		if (it->GetAnalyzerName().CompareTo(analyzerName) == 0) {
			it->ApplyParam(parameterName, parameter);
		}
	}
}

} /* namespace Core */
} /* namespace NA62Analysis */

