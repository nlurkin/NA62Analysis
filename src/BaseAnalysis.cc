#include "BaseAnalysis.hh"
#include <TStyle.h>
#include <sys/stat.h>
#include <fstream>
#include <ctime>
#include "Event.hh"
#include <TCanvas.h>
#include "Analyzer.hh"

#include "functions.hh"
#include "MCSimple.hh"
#include "ConfigParser.hh"
#include "StringBalancedTable.hh"

BaseAnalysis::BaseAnalysis(){
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr

	fEventNb = -1;
	fVerbosity = AnalysisFW::kNo;
	fGraphicMode = false;

	gStyle->SetOptFit(1);

	fDetectorAcceptanceInstance = NULL;

	fInitialized = false;
}

BaseAnalysis::~BaseAnalysis(){
	/// \MemberDescr
	/// Destructor.
	/// \EndMemberDescr

	map<TString, EventFraction*>::iterator itEF;

	//fEventFraction.clear();
	while(fEventFraction.size()>0){
		itEF = fEventFraction.begin();
		delete itEF->second;
		fEventFraction.erase(itEF);
	}
	/*	while(fExportTrees.size()>0){
		itTree = fExportTrees.begin();
		//cout << itTree->second << endl;
		//cout << itTree->second->GetName() << endl;
		delete itTree->second;
		fExportTrees.erase(itTree);
	}*/

	while(fMCSimple.size()>0){
		delete fMCSimple.back();
		fMCSimple.pop_back();
	}
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

void BaseAnalysis::Init(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, bool graphicMode, TString refFile){
	/// \MemberDescr
	/// \param inFileName : path to the input file / path to the file containing the list of input files
	/// \param outFileName : path to the output file
	/// \param mutipleFiles : Indicating if inFileName is the input file or the list of input files
	/// \param textOnly : If yes the plots are printed on screen, in no only producing output file
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

	if(!fIOHandler.OpenInput(inFileName, NFiles, fVerbosity)) return;

	fGraphicMode = graphicMode;
	fIOHandler.OpenOutput(outFileName);
	fIOHandler.SetReferenceFileName(refFile);

	fEventNb = fIOHandler.FillMCTruth(fVerbosity);

	//Parse parameters from file
	ConfigParser confParser;
	confParser.ParseFile(configFile);
	//Parse parameters from commandLine
	confParser.ParseCLI(params);

	for(unsigned int i=0; i<fAnalyzerList.size(); i++){
		fIOHandler.MkOutputDir(fAnalyzerList[i]->GetAnalyzerName());
		gFile->cd(fAnalyzerList[i]->GetAnalyzerName());
		fAnalyzerList[i]->InitOutput();
		fAnalyzerList[i]->InitHist();

		confParser.ApplyParams(fAnalyzerList[i]);

		fAnalyzerList[i]->DefineMCSimple(fMCSimple[i]);
		fAnalyzerList[i]->PrintInitSummary(fMCSimple[i]);
		gFile->cd();
	}

	PrintInitSummary();
	fEventNb = fIOHandler.GetTree(fEventNb);

	fInitialized = true;
}

void BaseAnalysis::AddAnalyzer(Analyzer* an){
	/// \MemberDescr
	/// \param an : Pointer to the analyzer
	///
	/// Add an analyzer to the Analyzer lists
	/// \EndMemberDescr

	fAnalyzerList.push_back(an);
	fMCSimple.push_back(new MCSimple());
}

void BaseAnalysis::RegisterOutput(TString name, void *address){
	/// \MemberDescr
	/// \param name : Name of the output
	/// \param address : pointer to the output variable
	///
	/// Register an output
	/// \EndMemberDescr

	fOutput.insert(pair<TString, void*>(name, address));
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

const void *BaseAnalysis::GetOutput(TString name, Analyzer::OutputState &state){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : is filled with the current state of the output
	///
	/// Return an output variable and the corresponding state
	/// \EndMemberDescr

	if(fOutput.count(name)==0){
		state = Analyzer::kOUninit;
		cerr << "Output " << name << " not found" << endl;
		return 0;
	}
	state = fOutputStates[name];
	return fOutput[name];
}

void BaseAnalysis::PreProcess(){
	/// \MemberDescr
	/// Pre-processing method. Reset the states of the output
	/// \EndMemberDescr

	map<TString, Analyzer::OutputState>::iterator itState;
	map<TString, void*>::iterator itOut;

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

	int i_offset;
	clock_t timing;
	bool exportEvent = false;

	if(!fInitialized) return;

	timing = clock();

	//Print event processing summary
	if ( maxEvent > fEventNb || maxEvent <= 0 ) maxEvent = fEventNb;
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
				printf(SHELL_COLOR_LRED"*** Processing Event %i/%i => %.2f%%\r"SHELL_COLOR_NONE, i, beginEvent+maxEvent, ((double)i/(double)(beginEvent+maxEvent))*100); fflush(stdout);
			}
			else{
				printf("*** Processing Event %i/%i => %.2f%%\n", i, beginEvent+maxEvent, ((double)i/(double)(beginEvent+maxEvent))*100);
			}
		}

		// Load event infos
		fIOHandler.LoadEvent(i);
		CheckNewFileOpened();

		PreProcess();
		//Process event in Analyzer
		exportEvent = false;
		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			//Get reality
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			if(fIOHandler.GetWithMC()) fMCSimple[j]->GetRealInfos( fIOHandler.GetMCTruthEvent(), fVerbosity);

			fAnalyzerList[j]->Process(i, *fMCSimple[j], fIOHandler.GetMCTruthEvent());
			fAnalyzerList[j]->UpdatePlots(i);
			exportEvent = exportEvent || fAnalyzerList[j]->GetExportEvent();
			gFile->cd();
		}

		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			if(exportEvent) fAnalyzerList[j]->FillTrees();
			fAnalyzerList[j]->PostProcess();
			gFile->cd();
		}
		if(exportEvent) fIOHandler.WriteEvent();
	}

	if(fGraphicMode){
		printf(SHELL_COLOR_LRED"*** Processing Event %i/%i => 100.00%%\n"SHELL_COLOR_NONE, beginEvent+maxEvent, beginEvent+maxEvent);
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
	fIOHandler.WriteTree();
	WriteEventFraction();

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

DetectorAcceptance* BaseAnalysis::IsDetectorAcceptanceInstaciated(){
	/// \MemberDescr
	/// Return a pointer to the unique global instance of DetectorAcceptance if instantiated. Otherwise return null.
	/// \EndMemberDescr

	return fDetectorAcceptanceInstance;
}

void BaseAnalysis::WriteEventFraction(){
	/// \MemberDescr
	/// Dump the EventFraction
	/// \EndMemberDescr

	map<TString,EventFraction*>::iterator it;

	for(it=fEventFraction.begin(); it!=fEventFraction.end(); it++){
		it->second->DumpTable();
		//it->second->WriteTable(fOutFileName);
	}
}

void BaseAnalysis::NewEventFraction(TString name){
	/// \MemberDescr
	/// \param name : Name of the EventFraction table
	///
	/// Create a new EventFraction instance
	/// \EndMemberDescr

	fEventFraction.insert(pair<TString,EventFraction*>(name, new EventFraction(name)));
}
void BaseAnalysis::AddCounterToEventFraction(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Add a counter in the specified EventFraction table
	/// \EndMemberDescr

	if(fEventFraction.count(efName)>0){
		if(fCounters.count(cName)>0){
			fEventFraction[efName]->AddCounter(cName, &fCounters[cName]);
		}
		else cerr << "Counter " << cName << " doesn't exist." << endl;
	}
	else cerr << "EventFraction table " << efName << " doesn't exist." << endl;
}
void BaseAnalysis::DefineSampleSizeCounter(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Define counter as SampleSize in the specified EventFraction table
	/// \EndMemberDescr

	if(fEventFraction.count(efName)>0){
		if(fCounters.count(cName)>0){
			fEventFraction[efName]->DefineSampleSizeCounter(cName);
		}
		else cerr << "Counter " << cName << " doesn't exist." << endl;
	}
	else cerr << "EventFraction table " << efName << " doesn't exist." << endl;
}
void BaseAnalysis::SetSignificantDigits(TString efName, int v){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param v : number of significant digits
	///
	/// Set number of significant digits for the specified EventFraction table
	/// \EndMemberDescr

	if(fEventFraction.count(efName)>0){
		fEventFraction[efName]->SetPrecision(v);
	}
	else cerr << "EventFraction table " << efName << " doesn't exist." << endl;
}

void BaseAnalysis::BookCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the Counter
	///
	/// Book a new counter
	/// \EndMemberDescr

	fCounters.insert(pair<TString,int>(name, 0));
}
void BaseAnalysis::IncrementCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the counter
	///
	/// Increment a previously booked counter by 1
	/// \EndMemberDescr

	IncrementCounter(name, 1);
}
void BaseAnalysis::IncrementCounter(TString name, int v){
	/// \MemberDescr
	/// \param name : Name of the counter
	/// \param v : value
	///
	/// Increment a previously booked counter by v
	/// \EndMemberDescr

	if(fCounters.count(name)>0){
		fCounters[name] = fCounters[name]+v;
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
}
void BaseAnalysis::DecrementCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the counter
	///
	/// Decrement a previously booked counter by 1
	/// \EndMemberDescr

	DecrementCounter(name,1);
}
void BaseAnalysis::DecrementCounter(TString name, int v){
	/// \MemberDescr
	/// \param name : Name of the counter
	/// \param v : value
	///
	/// Decrement a previously booked counter by v
	/// \EndMemberDescr

	if(fCounters.count(name)>0){
		fCounters[name] = fCounters[name]-v;
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
}
void BaseAnalysis::SetCounterValue(TString name, int v){
	/// \MemberDescr
	/// \param name : Name of the counter
	/// \param v : value
	///
	/// Set the value of a previously booked counter
	/// \EndMemberDescr

	if(fCounters.count(name)>0){
		fCounters[name] = v;
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
}
int BaseAnalysis::GetCounterValue(TString name){
	/// \MemberDescr
	/// \param name : Name of the counter
	///
	/// Get the value of a previously booked counter
	/// \EndMemberDescr

	if(fCounters.count(name)>0){
		return fCounters[name];
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
	return -1;
}

void BaseAnalysis::PrintInitSummary(){
	/// \MemberDescr
	/// Print summary after initialization.
	/// \EndMemberDescr

	vector<Analyzer*>::iterator itAn;
	map<TString, EventFraction*>::iterator itEvtFrac;
	map<TString, int>::iterator itCounter;
	map<TString, void*>::iterator itOutput;

	StringBalancedTable anTable("List of loaded Analyzers");
	StringBalancedTable evtFracTable("List of EventFraction");
	StringBalancedTable counterTable("List of Counters");
	StringBalancedTable outputTable("List of Outputs");

	for(itAn=fAnalyzerList.begin(); itAn!=fAnalyzerList.end(); itAn++){
		anTable << (*itAn)->GetAnalyzerName();
	}

	for(itEvtFrac=fEventFraction.begin(); itEvtFrac!=fEventFraction.end(); itEvtFrac++){
		evtFracTable << itEvtFrac->first;
	}

	for(itCounter=fCounters.begin(); itCounter!=fCounters.end(); itCounter++){
		counterTable << itCounter->first;
	}

	for(itOutput=fOutput.begin(); itOutput!=fOutput.end(); itOutput++){
		outputTable << itOutput->first;
	}


	cout << "================================================================================" << endl;
	cout << endl << "\t *** Global settings for AnalysisFW ***" << endl << endl;

	anTable.Print("\t");
	evtFracTable.Print("\t");
	counterTable.Print("\t");
	outputTable.Print("\t");
	fIOHandler.PrintInitSummary();
	cout << "================================================================================" << endl;
}

void BaseAnalysis::CheckNewFileOpened(){
	/// \MemberDescr
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	if(!fIOHandler.CheckNewFileOpened()) return;
	//New file opened
	//end of burst
	for(unsigned int i=0; i<fAnalyzerList.size(); i++){
		fAnalyzerList[i]->EndOfBurst();
	}

	fIOHandler.UpdateInputHistograms();
	for(unsigned int i=0; i<fAnalyzerList.size(); i++){
		fAnalyzerList[i]->StartOfBurst();
	}
}

IOHandler* BaseAnalysis::GetIOHandler() {
	return &fIOHandler;
}
