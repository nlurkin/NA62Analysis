#include "BaseAnalysis.hh"

#include <sstream>
#include <TStyle.h>
#include <TFile.h>

#include "ConfigAnalyzer.hh"
#include "StringBalancedTable.hh"
#include "TermManip.hh"
#include "ConfigSettings.hh"

namespace NA62Analysis {
namespace Core {

BaseAnalysis::BaseAnalysis():
	Verbose("BaseAnalysis"),
	fNEvents(-1),
	fGraphicMode(false),
	fInitialized(false),
	fDetectorAcceptanceInstance(nullptr),
	fIOHandler(nullptr),
	fInitTime(true)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr


	Configuration::ConfigSettings().ParseFile(TString(std::getenv("ANALYSISFW_USERDIR")) + TString("/.settingsna62"));
	gStyle->SetOptFit(1);
	NA62Analysis::manip::enableManip = Configuration::ConfigSettings::global::fUseColors && isatty(fileno(stdout));
}

BaseAnalysis::~BaseAnalysis(){
	/// \MemberDescr
	/// Destructor.
	/// \EndMemberDescr

	if(fDetectorAcceptanceInstance) delete fDetectorAcceptanceInstance;
}

void BaseAnalysis::Init(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, TString refFile, bool ignoreNonExisting){
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
	if(!fIOHandler->OpenInput(inFileName, NFiles)) return;

	fIOHandler->OpenOutput(outFileName);

	if(IsTreeType()){
		IOTree * treeHandler = static_cast<IOTree*>(fIOHandler);

		treeHandler->SetReferenceFileName(refFile);
		treeHandler->SetIgnoreNonExisting(ignoreNonExisting);

		fNEvents = std::max(treeHandler->FillMCTruth(), treeHandler->FillRawHeader());

		std::cout << debug() << "Using " << fNEvents << " events" << endl;
	}

	if(IsTreeType()) fNEvents = GetIOTree()->BranchTrees(fNEvents);
	else if (IsHistoType()) fNEvents = fIOHandler->GetInputFileNumber();

	int testEvent=0;
	while(!fIOHandler->LoadEvent(testEvent) && testEvent < fNEvents) testEvent++;
	if(testEvent==fNEvents){
		std::cout << "Unable to load any event/file. Aborting processing" << std::endl;
		raise(SIGABRT);
	}
	fIOHandler->CheckNewFileOpened();

	std::cout << debug() << "Parsing parameters" << std::endl;
	//Parse parameters from file
	Configuration::ConfigAnalyzer confParser;
	confParser.ParseFile(configFile);
	//Parse parameters from commandLine
	confParser.ParseCLI(params);

	for(unsigned int i=0; i<fAnalyzerList.size(); i++){
		fIOHandler->MkOutputDir(fAnalyzerList[i]->GetAnalyzerName());
		gFile->cd(fAnalyzerList[i]->GetAnalyzerName());
		fAnalyzerList[i]->InitOutput();
		fAnalyzerList[i]->InitHist();

		confParser.ApplyParams(fAnalyzerList[i]);

		fAnalyzerList[i]->DefineMCSimple();
		fAnalyzerList[i]->PrintInitSummary();
		gFile->cd();
	}

	PrintInitSummary();

	fInitialized = true;
	fInitTime.Stop();
}

void BaseAnalysis::AddAnalyzer(Analyzer* an){
	/// \MemberDescr
	/// \param an : Pointer to the analyzer
	///
	/// Add an analyzer to the Analyzer lists
	/// \EndMemberDescr

	std::cout << normal() << "Adding analyzer " << an->GetAnalyzerName() << std::endl;
	an->SetVerbosity(GetVerbosityLevel());
	fAnalyzerList.push_back(an);
}

void BaseAnalysis::RegisterOutput(TString name, const void * const address){
	/// \MemberDescr
	/// \param name : Name of the output
	/// \param address : pointer to the output variable
	///
	/// Register an output
	/// \EndMemberDescr

	std::cout << normal() << "Registering output " << name << endl;
	std::cout << debug() << " at address " << address << endl;
	fOutput.insert(std::pair<TString, const void* const>(name, address));
	fOutputStates.insert(std::pair<TString, Analyzer::OutputState>(name, Analyzer::kOUninit));
}

void BaseAnalysis::SetOutputState(TString name, Analyzer::OutputState state){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : state to be set
	///
	/// Set the state of the output
	/// \EndMemberDescr

	fOutputStates[name] = state;
}

const void *BaseAnalysis::GetOutput(TString name, Analyzer::OutputState &state) const{
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : is filled with the current state of the output
	///
	/// Return an output variable and the corresponding state
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,const void* const>::type::const_iterator ptr;

	if((ptr=fOutput.find(name))!=fOutput.end()){
		state = fOutputStates.find(name)->second;
		return ptr->second;
	}
	else{
		state = Analyzer::kOUninit;
		std::cout << normal() << "Output " << name << " not found" << std::endl;
		return 0;
	}
}

void BaseAnalysis::PreProcess(){
	/// \MemberDescr
	/// Pre-processing method. Reset the states of the output
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,Analyzer::OutputState>::type::iterator itState;
	NA62Analysis::NA62Map<TString,void*>::type::iterator itOut;

	for(itState = fOutputStates.begin(); itState!=fOutputStates.end(); itState++){
		itState->second = Analyzer::kOInvalid;
	}

	for(unsigned int j=0; j<fAnalyzerList.size(); j++){
		fAnalyzerList[j]->PreProcess();
	}
}


void BaseAnalysis::Process(int beginEvent, int maxEvent){
	/// \MemberDescr
	/// \param beginEvent : index of the first event to be processed
	/// \param maxEvent : maximum number of events to be processed
	///
	/// Main process loop. Read the files event by event and process each analyzer in turn for each event
	/// \EndMemberDescr

	if(!fInitialized) return;

	TimeCounter processLoopTime;
	TimeCounter processTime;
	int i_offset;
	bool exportEvent = false;

	processLoopTime.Start();

	std::string displayType;
	if(IsTreeType()) displayType = "Event";
	else if (IsHistoType()) displayType = "File";

	//Print event processing summary
	if ( maxEvent > fNEvents || maxEvent <= 0 ) maxEvent = fNEvents;
	std::cout << normal() << "Treating " << maxEvent << " " << displayType << "s, beginning with " << displayType << " " << beginEvent << std::endl;

	i_offset = maxEvent/100.;
	if(i_offset==0) i_offset=1;
	std::cout << extended() << "i_offset : " << i_offset << std::endl;

	for(unsigned int j=0; j<fAnalyzerList.size(); j++){
		gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
		fAnalyzerList[j]->StartOfRun();
		if(IsTreeType()) fAnalyzerList[j]->StartOfBurst();
	}

	//##############################
	//Begin event loop
	//##############################
	int defaultPrecision = std::cout.precision();
	int processEvents = std::min(beginEvent+maxEvent, fNEvents);

	for (int i=beginEvent; i < processEvents; i++)
	{
		//Print current event
		if ( i % i_offset == 0 ){
			printCurrentEvent(i, processEvents, defaultPrecision, displayType, processLoopTime);
		}

		// Load event infos
		if(!fIOHandler->LoadEvent(i)) std::cout << normal() << "Unable to read event " << i << std::endl;
		CheckNewFileOpened();


		processTime.Start();
		PreProcess();
		//Process event in Analyzer
		exportEvent = false;
		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			//Get reality
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			if(IsTreeType() && static_cast<IOTree*>(fIOHandler)->GetWithMC()) fAnalyzerList[j]->FillMCSimple( static_cast<IOTree*>(fIOHandler)->GetMCTruthEvent());

			fAnalyzerList[j]->Process(i);
			fAnalyzerList[j]->UpdatePlots(i);
			exportEvent = exportEvent || fAnalyzerList[j]->GetExportEvent();
			gFile->cd();
		}

		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			fAnalyzerList[j]->PostProcess();
			gFile->cd();
		}
		processTime.Stop();

		if(IsTreeType() && exportEvent) static_cast<IOTree*>(fIOHandler)->WriteEvent();
	}

	printCurrentEvent(processEvents-1, processEvents, defaultPrecision, displayType, processLoopTime);
	std::cout << std::endl;

	//Ask the analyzer to export and draw the plots
	for(unsigned int j=0; j<fAnalyzerList.size(); j++){
		gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
		fAnalyzerList[j]->EndOfBurst();
		fAnalyzerList[j]->EndOfRun();

		fAnalyzerList[j]->ExportPlot();
		if(fGraphicMode) fAnalyzerList[j]->DrawPlot();
		fAnalyzerList[j]->WriteTrees();
		gFile->cd();
	}
	if(IsTreeType()) static_cast<IOTree*>(fIOHandler)->WriteTree();
	fCounterHandler.WriteEventFraction(fIOHandler->GetOutputFileName());

	//Complete the analysis
	using NA62Analysis::operator -;
	float totalTime = fInitTime.GetTime() - fInitTime.GetStartTime();
	std::cout << setprecision(2);
	std::cout << std::endl << "###################################" << std::endl;
	std::cout << "Total time: " << std::setw(17) << std::fixed << totalTime << " seconds" << std::endl;
	std::cout << " - Init time: " << std::setw(15) << std::fixed << fInitTime.GetTotalTime() << " seconds" << std::endl;
	std::cout << " - Process loop time: " << std::setw(7) << std::fixed << processLoopTime.GetTotalTime() << " seconds" << std::endl;
	std::cout << "   - Processing time: " << std::setw(7) << processTime.GetTotalTime() << " seconds" << std::endl;
	std::cout << "IO time: " << std::setw(20) << fIOHandler->GetIoTimeCount().GetTotalTime() << " seconds" << std::endl;
	std::cout << std::endl << "Analysis complete" << std::endl << "###################################" << std::endl;
}

DetectorAcceptance* BaseAnalysis::GetDetectorAcceptanceInstance(){
	/// \MemberDescr
	/// Return a pointer to the unique global instance of DetectorAcceptance.\n
	/// If not yet instantiated, instantiate it.
	/// \return Pointer to the unique global instance of DetectorAcceptance
	/// \EndMemberDescr

	if(fDetectorAcceptanceInstance==NULL){
		fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.root");
	}

	return fDetectorAcceptanceInstance;
}

DetectorAcceptance* BaseAnalysis::IsDetectorAcceptanceInstanciated() const{
	/// \MemberDescr
	/// \return Pointer to the unique global instance of DetectorAcceptance if instantiated. Otherwise return null.
	/// \EndMemberDescr

	return fDetectorAcceptanceInstance;
}

void BaseAnalysis::PrintInitSummary() const{
	/// \MemberDescr
	/// Print summary after initialization.
	/// \EndMemberDescr

	std::vector<Analyzer*>::const_iterator itAn;
	NA62Analysis::NA62Map<TString,const void* const>::type::const_iterator itOutput;

	StringBalancedTable anTable("List of loaded Analyzers");
	StringBalancedTable outputTable("List of Outputs");

	for(itAn=fAnalyzerList.begin(); itAn!=fAnalyzerList.end(); itAn++){
		anTable << (*itAn)->GetAnalyzerName();
	}

	for(itOutput=fOutput.begin(); itOutput!=fOutput.end(); itOutput++){
		outputTable << itOutput->first;
	}


	std::cout << "================================================================================" << std::endl;
	std::cout << std::endl << "\t *** Global settings for AnalysisFW ***" << std::endl << std::endl;

	anTable.Print("\t");
	fCounterHandler.PrintInitSummary();
	outputTable.Print("\t");
	fIOHandler->PrintInitSummary();
	std::cout << "================================================================================" << std::endl;
}

void BaseAnalysis::CheckNewFileOpened(){
	/// \MemberDescr
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	if(!fIOHandler->CheckNewFileOpened()) return;
	//New file opened
	//first burst or not? Call end of burst only if it's not
	if(fIOHandler->GetCurrentFileNumber()>0){
		//end of burst
		for(unsigned int i=0; i<fAnalyzerList.size(); i++){
			fAnalyzerList[i]->EndOfBurst();
		}
		if(IsHistoType()) GetIOHisto()->UpdateInputHistograms();
	}

	for(unsigned int i=0; i<fAnalyzerList.size(); i++){
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

int BaseAnalysis::GetNEvents(){
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

	if(IsTreeType()) return GetIOTree()->GetTree(name);
	else return nullptr;
}

IOTree* BaseAnalysis::GetIOTree() {
	/// \MemberDescr
	/// \return Pointer to IOTree ifIOHandler is of Tree Type
	/// \EndMemberDescr

	if(IsTreeType()) return static_cast<IOTree*>(fIOHandler);
	else return nullptr;
}

IOHisto* BaseAnalysis::GetIOHisto() {
	/// \MemberDescr
	/// \return Pointer to IOHisto ifIOHandler is of Histo Type
	/// \EndMemberDescr

	if(IsHistoType()) return static_cast<IOHisto*>(fIOHandler);
	else return nullptr;
}

void BaseAnalysis::SetReadType(IOHandlerType type) {
	/// \MemberDescr
	/// \param type: Type of IOHandler to use
	///
	/// Create the correct instance of IOHandler
	/// \EndMemberDescr

	std::cout << normal() << "Creating IOHandler of type "
			<< (type==IOHandlerType::kHISTO ? "kHisto" : "kTree") << std::endl;
	if(type==IOHandlerType::kHISTO) fIOHandler = new IOHisto();
	else fIOHandler = new IOTree();
}

void BaseAnalysis::printCurrentEvent(int iEvent, int totalEvents, int defaultPrecision, std::string displayType, TimeCounter startTime) {
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

	std::stringstream ss;

	//Print current event
	if(Configuration::ConfigSettings::global::fUseColors) std::cout << manip::red << manip::bold;

	float elapsed = startTime.GetTotalTime();
	float eta = 0;
	if(iEvent>0) eta = (elapsed)*((totalEvents-iEvent)/(double)iEvent);
	float totalTime = iEvent>0 ? elapsed+eta : elapsed;

	//Processing what current/total =>
	ss << "*** Processing " << displayType << " " << iEvent << "/" << totalEvents;
	std::cout << std::setw(35) << std::left << ss.str() << " => ";
	// percentage%
	std::cout << std::setprecision(2) << std::fixed << std::setw(6) << std::right << ((double)iEvent/(double)totalEvents)*100 << "%";
	// ETA: 123s
	if(iEvent==0) std::cout << std::setw(10) << "ETA: " << "----s";
	else std::cout << std::setw(10) << "ETA: " << eta << "s";

	// Elapsed: 123s
	std::cout << std::setw(14) << "Elapsed: " << elapsed << "s";
	// Total: 123s
	std::cout << std::setw(12) << "Total: " << totalTime << "s";

	if(Configuration::ConfigSettings::global::fUseColors) std::cout << manip::reset;
	if(Configuration::ConfigSettings::global::fProcessOutputNewLine) std::cout << std::endl;
	else std::cout << std::setw(10) << "\r" << std::flush;

	//Reset to default
	std::cout.precision(defaultPrecision);
	std::cout.unsetf(std::ios_base::floatfield);
}

} /* namespace Core */
} /* namespace NA62Analysis */

