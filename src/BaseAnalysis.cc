#include "BaseAnalysis.hh"

#include <TStyle.h>
#include "ConfigParser.hh"
#include "StringBalancedTable.hh"
#include <TFile.h>

BaseAnalysis::BaseAnalysis():
	fNEvents(-1),
	fGraphicMode(false),
	fVerbosity(AnalysisFW::kNo),
	fInitialized(false),
	fDetectorAcceptanceInstance(nullptr),
	fIOHandler(nullptr)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr

	gStyle->SetOptFit(1);
}

BaseAnalysis::~BaseAnalysis(){
	/// \MemberDescr
	/// Destructor.
	/// \EndMemberDescr
	//AnalysisFW::NA62Map<TString, MCSimple*>::type::iterator it;

	if(fDetectorAcceptanceInstance) delete fDetectorAcceptanceInstance;
}

void BaseAnalysis::SetVerbosity(AnalysisFW::VerbosityLevel v){
	/// \MemberDescr
	/// \param v : value of the verbosity
	///
	/// Change verbosity
	/// \EndMemberDescr

	if(v >= AnalysisFW::kNormal) cout << "AnalysisFW: Setting verbosity to " << v << endl;
	fVerbosity = v;
}

void BaseAnalysis::Init(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, TString refFile, bool allowNonExisting){
	/// \MemberDescr
	/// \param inFileName : path to the input file / path to the file containing the list of input files
	/// \param outFileName : path to the output file
	///	\param params : list of command line parameters to parse and pass to analyzers
	/// \param configFile : path to a runtime configuration file to be parsed and defining parameters for analyzers.
	///	\param NFiles : Maximum number of input files to process
	/// \param graphicMode : Graphical mode. Display plots on screen (call DrawPlots on analyzers).
	/// \param refFile : Eventual name of a file containing reference plots
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

	if(!fIOHandler->OpenInput(inFileName, NFiles, fVerbosity)) return;

	fIOHandler->OpenOutput(outFileName);

	if(IsTreeType()) InitWithTree(inFileName, outFileName, params, configFile, NFiles, refFile, allowNonExisting);

	CheckNewFileOpened();
	//Parse parameters from file
	ConfigParser confParser;
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
	if(IsTreeType()) fNEvents = GetIOTree()->BranchTrees(fNEvents);
	else if (IsHistoType()) fNEvents = fIOHandler->GetInputFileNumber();

	fInitialized = true;
}

void BaseAnalysis::InitWithTree(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, TString refFile, bool allowNonExisting){
	/// \MemberDescr
	/// \param inFileName : path to the input file / path to the file containing the list of input files
	/// \param outFileName : path to the output file
	///	\param params : list of command line parameters to parse and pass to analyzers
	/// \param configFile : path to a runtime configuration file to be parsed and defining parameters for analyzers.
	///	\param NFiles : Maximum number of input files to process
	/// \param graphicMode : Graphical mode. Display plots on screen (call DrawPlots on analyzers).
	/// \param refFile : Eventual name of a file containing reference plots
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

	IOTree * treeHandler = static_cast<IOTree*>(fIOHandler);

	treeHandler->SetReferenceFileName(refFile);
	treeHandler->SetAllowNonExisting(allowNonExisting);

	fNEvents = std::max(treeHandler->FillMCTruth(fVerbosity), treeHandler->FillRawHeader(fVerbosity));
}

void BaseAnalysis::AddAnalyzer(Analyzer* an){
	/// \MemberDescr
	/// \param an : Pointer to the analyzer
	///
	/// Add an analyzer to the Analyzer lists
	/// \EndMemberDescr

	an->SetVerbosity(fVerbosity);
	fAnalyzerList.push_back(an);
}

void BaseAnalysis::RegisterOutput(TString name, const void * const address){
	/// \MemberDescr
	/// \param name : Name of the output
	/// \param address : pointer to the output variable
	///
	/// Register an output
	/// \EndMemberDescr

	fOutput.insert(pair<TString, const void* const>(name, address));
	fOutputStates.insert(pair<TString, Analyzer::OutputState>(name, Analyzer::kOUninit));
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

	AnalysisFW::NA62Map<TString,const void* const>::type::const_iterator ptr;

	if((ptr=fOutput.find(name))!=fOutput.end()){
		state = fOutputStates.find(name)->second;
		return ptr->second;
	}
	else{
		state = Analyzer::kOUninit;
		cerr << "Output " << name << " not found" << endl;
		return 0;
	}
}

void BaseAnalysis::PreProcess(){
	/// \MemberDescr
	/// Pre-processing method. Reset the states of the output
	/// \EndMemberDescr

	AnalysisFW::NA62Map<TString,Analyzer::OutputState>::type::iterator itState;
	AnalysisFW::NA62Map<TString,void*>::type::iterator itOut;

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

	if(IsTreeType()) ProcessWithTree(beginEvent, maxEvent);
	else if(IsHistoType()) ProcessWithHisto(beginEvent, maxEvent);
}

void BaseAnalysis::ProcessWithTree(int beginEvent, int maxEvent){
	/// \MemberDescr
	/// \param beginEvent : index of the first event to be processed
	/// \param maxEvent : maximum number of events to be processed
	///
	/// Main process loop. Read the files event by event and process each analyzer in turn for each event
	/// \EndMemberDescr

	int i_offset;
	clock_t timing;
	bool exportEvent = false;

	if(!fInitialized) return;

	IOTree *treeIO = static_cast<IOTree*>(fIOHandler);

	timing = clock();

	//Print event processing summary
	if ( maxEvent > fNEvents || maxEvent <= 0 ) maxEvent = fNEvents;
	if(fVerbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: Treating " << maxEvent << " events, beginning with event " << beginEvent << endl;

	i_offset = maxEvent/100.;
	if(i_offset==0) i_offset=1;
	if(fVerbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: i_offset : " << i_offset << endl;

	for(unsigned int j=0; j<fAnalyzerList.size(); j++){
		gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
		fAnalyzerList[j]->StartOfRun();
		fAnalyzerList[j]->StartOfBurst();
	}

	//##############################
	//Begin event loop
	//##############################
	for (int i=beginEvent; i < beginEvent+maxEvent; i++)
	{
		//Print current event
		if ( i % i_offset == 0 ){
			if(fGraphicMode){
				printf(SHELL_COLOR_LRED "*** Processing Event %i/%i => %.2f%%\r" SHELL_COLOR_NONE, i, beginEvent+maxEvent, ((double)i/(double)(beginEvent+maxEvent))*100); fflush(stdout);
			}
			else{
				printf("*** Processing Event %i/%i => %.2f%%\n", i, beginEvent+maxEvent, ((double)i/(double)(beginEvent+maxEvent))*100);
			}
		}

		// Load event infos
		treeIO->LoadEvent(i);
		CheckNewFileOpened();

		PreProcess();
		//Process event in Analyzer
		exportEvent = false;
		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			//Get reality
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			if(treeIO->GetWithMC()) fAnalyzerList[j]->FillMCSimple( treeIO->GetMCTruthEvent(), fVerbosity);

			fAnalyzerList[j]->Process(i);
			fAnalyzerList[j]->UpdatePlots(i);
			exportEvent = exportEvent || fAnalyzerList[j]->GetExportEvent();
			gFile->cd();
		}

		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			//if(exportEvent) fAnalyzerList[j]->FillTrees();
			fAnalyzerList[j]->PostProcess();
			gFile->cd();
		}
		if(exportEvent) treeIO->WriteEvent();
	}

	if(fGraphicMode){
		printf(SHELL_COLOR_LRED "*** Processing Event %i/%i => 100.00%%\n" SHELL_COLOR_NONE, beginEvent+maxEvent, beginEvent+maxEvent);
	}
	else{
		printf("*** Processing Event %i/%i => 100.00%%\n", beginEvent+maxEvent, beginEvent+maxEvent);
	}

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
	treeIO->WriteTree();
	fCounterHandler.WriteEventFraction(treeIO->GetOutputFileName());

	//Complete the analysis
	timing = clock()-timing;
	cout << endl << "###################################" << endl << "Processing time : " << ((float)timing/CLOCKS_PER_SEC) << " seconds";
	cout << endl << "Analysis complete" << endl << "###################################" << endl;
}

void BaseAnalysis::ProcessWithHisto(int beginEvent, int maxEvent){
	/// \MemberDescr
	/// \param beginEvent : index of the first event to be processed
	/// \param maxEvent : maximum number of events to be processed
	///
	/// Main process loop. Read the files event by event and process each analyzer in turn for each event
	/// \EndMemberDescr

	int i_offset;
	clock_t timing;

	if(!fInitialized) return;

	IOHisto *HistoIO = static_cast<IOHisto*>(fIOHandler);

	timing = clock();

	//Print event processing summary
	if ( maxEvent > fNEvents || maxEvent <= 0 ) maxEvent = fNEvents;
	if(fVerbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: Treating " << maxEvent << " files, beginning with file " << beginEvent << endl;

	i_offset = maxEvent/100.;
	if(i_offset==0) i_offset=1;
	if(fVerbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: i_offset : " << i_offset << endl;

	for(unsigned int j=0; j<fAnalyzerList.size(); j++){
		gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
		fAnalyzerList[j]->StartOfRun();
		fAnalyzerList[j]->StartOfBurst();
	}

	//##############################
	//Begin event loop
	//##############################
	for (int i=beginEvent; i < beginEvent+maxEvent; i++)
	{
		//Print current event
		if ( i % i_offset == 0 ){
			if(fGraphicMode){
				printf(SHELL_COLOR_LRED "*** Processing File %i/%i => %.2f%%\r" SHELL_COLOR_NONE, i, beginEvent+maxEvent, ((double)i/(double)(beginEvent+maxEvent))*100); fflush(stdout);
			}
			else{
				printf("*** Processing File %i/%i => %.2f%%\n", i, beginEvent+maxEvent, ((double)i/(double)(beginEvent+maxEvent))*100);
			}
		}


		// Load event infos

		HistoIO->LoadEvent(i);
		CheckNewFileOpened();

		PreProcess();
		//Process event in Analyzer
		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			//Get reality
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());

			fAnalyzerList[j]->Process(i);
			fAnalyzerList[j]->UpdatePlots(i);
			gFile->cd();
		}

		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			//if(exportEvent) fAnalyzerList[j]->FillTrees();
			fAnalyzerList[j]->PostProcess();
			gFile->cd();
		}
	}

	if(fGraphicMode){
		printf(SHELL_COLOR_LRED "*** Processing File %i/%i => 100.00%%\n" SHELL_COLOR_NONE, beginEvent+maxEvent, beginEvent+maxEvent);
	}
	else{
		printf("*** Processing File %i/%i => 100.00%%\n", beginEvent+maxEvent, beginEvent+maxEvent);
	}

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
	fCounterHandler.WriteEventFraction(HistoIO->GetOutputFileName());

	//Complete the analysis
	timing = clock()-timing;
	cout << endl << "###################################" << endl << "Processing time : " << ((float)timing/CLOCKS_PER_SEC) << " seconds";
	cout << endl << "Analysis complete" << endl << "###################################" << endl;
}

DetectorAcceptance* BaseAnalysis::GetDetectorAcceptanceInstance(){
	/// \MemberDescr
	/// Return a pointer to the unique global instance of DetectorAcceptance.\n
	/// If not yet instantiated, instantiate it.
	/// \EndMemberDescr

	if(fDetectorAcceptanceInstance==NULL){
		fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.root");
	}

	return fDetectorAcceptanceInstance;
}

DetectorAcceptance* BaseAnalysis::IsDetectorAcceptanceInstanciated() const{
	/// \MemberDescr
	/// Return a pointer to the unique global instance of DetectorAcceptance if instantiated. Otherwise return null.
	/// \EndMemberDescr

	return fDetectorAcceptanceInstance;
}

void BaseAnalysis::PrintInitSummary() const{
	/// \MemberDescr
	/// Print summary after initialization.
	/// \EndMemberDescr

	vector<Analyzer*>::const_iterator itAn;
	AnalysisFW::NA62Map<TString,const void* const>::type::const_iterator itOutput;

	StringBalancedTable anTable("List of loaded Analyzers");
	StringBalancedTable outputTable("List of Outputs");

	for(itAn=fAnalyzerList.begin(); itAn!=fAnalyzerList.end(); itAn++){
		anTable << (*itAn)->GetAnalyzerName();
	}

	for(itOutput=fOutput.begin(); itOutput!=fOutput.end(); itOutput++){
		outputTable << itOutput->first;
	}


	cout << "================================================================================" << endl;
	cout << endl << "\t *** Global settings for AnalysisFW ***" << endl << endl;

	anTable.Print("\t");
	fCounterHandler.PrintInitSummary();
	outputTable.Print("\t");
	fIOHandler->PrintInitSummary();
	cout << "================================================================================" << endl;
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
	///	Return a pointer to the IOHandler instance
	/// \EndMemberDescr

	return fIOHandler;
}

CounterHandler* BaseAnalysis::GetCounterHandler() {
	/// \MemberDescr
	///	Return a pointer to the CounterHandler instance
	/// \EndMemberDescr

	return &fCounterHandler;
}

int BaseAnalysis::GetNEvents(){
	/// \MemberDescr
	///	Return the total number of events loaded from the input files
	/// \EndMemberDescr
	return fNEvents;
}

TChain* BaseAnalysis::GetTree(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TChain
	///
	///	Return a pointer to the TChain
	/// \EndMemberDescr

	if(IsTreeType()) return GetIOTree()->GetTree(name);
	else return nullptr;
}

IOTree* BaseAnalysis::GetIOTree() {
	if(IsTreeType()) return static_cast<IOTree*>(fIOHandler);
	else return nullptr;
}

IOHisto* BaseAnalysis::GetIOHisto() {
	if(IsHistoType()) return static_cast<IOHisto*>(fIOHandler);
	else return nullptr;
}

void BaseAnalysis::SetReadType(IOHandlerType type) {
	if(type==IOHandlerType::kHISTO) fIOHandler = new IOHisto();
	else fIOHandler = new IOTree();
}
