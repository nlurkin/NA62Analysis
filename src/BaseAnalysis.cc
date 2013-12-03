#include "BaseAnalysis.hh"
#include <TStyle.h>
#include <sys/stat.h>
#include <fstream>
#include <ctime>
#include "Event.hh"
#include <signal.h>
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

	fOutFile = 0;
	fMCTruthTree = 0;

	fCurrentFileNumber = 0;
	fMCTruthEvent = new Event();
	fWithMC = 0;
	fEventNb = 0;
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
	map<TString, TChain*>::iterator itChain;
	map<TString, TDetectorVEvent*>::iterator itEvent;
	map<TString, TTree*>::iterator itTree;

	if(fOutFile) {
		cout << "############# Writing output file #############" << endl;
		fOutFile->Purge();
		fOutFile->Close();
		cout << "#############        DONE         #############" << endl;
	}

	//fEventFraction.clear();
	while(fEventFraction.size()>0){
		itEF = fEventFraction.begin();
		delete itEF->second;
		fEventFraction.erase(itEF);
	}
	while(fTree.size()>0){
		itChain = fTree.begin();
		delete itChain->second;
		fTree.erase(itChain);
	}
	while(fEvent.size()>0){
		itEvent = fEvent.begin();
		delete itEvent->second;
		fEvent.erase(itEvent);
	}
	/*	while(fExportTrees.size()>0){
		itTree = fExportTrees.begin();
		//cout << itTree->second << endl;
		//cout << itTree->second->GetName() << endl;
		delete itTree->second;
		fExportTrees.erase(itTree);
	}*/

	delete fMCTruthTree;
	delete fMCTruthEvent;

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

void BaseAnalysis::Init(TString inFileName, TString outFileName, TString params, TString configFile, Int_t NFiles, bool graphicMode){
	/// \MemberDescr
	/// \param inFileName : path to the input file / path to the file containing the list of input files
	/// \param outFileName : path to the output file
	/// \param mutipleFiles : Indicating if inFileName is the input file or the list of input files
	/// \param textOnly : If yes the plots are printed on screen, in no only producing output file
	///
	/// Add all the input files to TChains and to Analyzers and create branches.\n
	/// Initialize the output trees (Histograms) and create branches.
	/// \EndMemberDescr

	//##############################
	//Get data from Files
	//Check integrity
	//Check all the data are present
	//##############################
	map<TString, TChain*>::iterator it;
	TString anName, anParams;
	bool inputChecked = false;
	int inputFileNumber = 0;

	if(inFileName.Length()==0){
		cerr << "AnalysisFW: No input file" << endl;
		return;
	}
	fGraphicMode = graphicMode;
	if(NFiles == 0){
		if(fVerbosity >= AnalysisFW::kNormal) cout << "AnalysisFW: Adding file " << inFileName << endl;
		checkInputFile(inFileName);
		if(fWithMC)
			fMCTruthTree->AddFile(inFileName);
		for(it=fTree.begin(); it!=fTree.end(); it++){
			it->second->AddFile(inFileName);
		}
	}else{
		TString inputFileName;
		ifstream inputList(inFileName.Data());
		while(inputFileName.ReadLine(inputList) && inputFileNumber < NFiles){
			if(fVerbosity>=AnalysisFW::kNormal) cout << "AnalysisFW: Adding file " << inputFileName << endl;
			if(!inputChecked && checkInputFile(inputFileName))
				inputChecked = kTRUE;
			if(fWithMC){
				fMCTruthTree->AddFile(inputFileName);
				inputFileNumber = fMCTruthTree->GetNtrees();
				cout << "----------- " << inputFileNumber << endl;
			}
			for(it=fTree.begin(); it!=fTree.end(); it++){
				it->second->AddFile(inputFileName);
				inputFileNumber = it->second->GetNtrees();
			}
		}
		if(inputFileNumber==0){
			cerr << "AnalysisFW: No input file in the list " << inFileName << endl;
			return;
		}
	}

	fOutFileName = outFileName;
	fOutFileName.ReplaceAll(".root", "");
	fOutFile = new TFile(outFileName, "RECREATE");

	FillMCTruth();

	//Parse parameters from file
	ConfigParser confParser;
	confParser.ParseFile(configFile);
	//Parse parameters from commandLine
	confParser.ParseCLI(params);

	for(unsigned int i=0; i<fAnalyzerList.size(); i++){
		fOutFile->mkdir(fAnalyzerList[i]->GetAnalyzerName());
		gFile->cd(fAnalyzerList[i]->GetAnalyzerName());
		fAnalyzerList[i]->InitOutput();
		fAnalyzerList[i]->InitHist();

		confParser.ApplyParams(fAnalyzerList[i]);

		fAnalyzerList[i]->DefineMCSimple(fMCSimple[i]);
		fAnalyzerList[i]->PrintInitSummary(fMCSimple[i]);
		gFile->cd();
	}

	PrintInitSummary();
	GetTree(fEventNb);

	fInitialized = true;
}

Bool_t BaseAnalysis::checkInputFile(TString fileName){
	/// \MemberDescr
	/// \param fileName : Name of the file to open
	///
	/// Open the input file to check if MC are present and if yes, what's the name of the TTree
	/// \EndMemberDescr

	TFile *fd = TFile::Open(fileName.Data(), "R");

	if(!fd)
		return kFALSE;

	TList* keys = fd->GetListOfKeys();

	fWithMC = true;
	if(keys->FindObject("Run_0")) fMCTruthTree = new TChain("Run_0");
	else if(keys->FindObject("mcEvent")) fMCTruthTree = new TChain("mcEvent");
	else{
		fEventNb = -1;
		if(fVerbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: No MC data found" << endl;
		fWithMC = false;
	}
	fd->Close();
	return kTRUE;
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

void BaseAnalysis::FillMCTruth(){
	/// \MemberDescr
	/// Branch the MC trees. Name is different if the input file comes from the MC or Reconstruction.
	/// \EndMemberDescr

	//TODO find another way to fill fEventNb
	if(!fWithMC) return;

	fEventNb = fMCTruthTree->GetEntries();

	TObjArray* branchesList = fMCTruthTree->GetListOfBranches();
	int jMax = branchesList->GetEntries();
	TString branchName = "";

	for (Int_t j=0; j < jMax; j++)
	{
		if(fVerbosity >= AnalysisFW::kSomeLevel) cout << "AnalysisFW: BranchName " <<  branchesList->At(j)->GetName() << endl;
		if ( TString("event").CompareTo( branchesList->At(j)->GetName() ) == 0){
			branchName = "event";
		}
		else if(TString("mcEvent").CompareTo( branchesList->At(j)->GetName() ) == 0 ){
			branchName = "mcEvent";
		}
		if(branchName.CompareTo("") != 0)
		{
			if(fVerbosity >= AnalysisFW::kSomeLevel) cout << "AnalysisFW: ClassName " << ((TBranch*)branchesList->At(j))->GetClassName() << endl;
			if ( TString("Event").CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				cerr  << "Input file corrupted, bad reco class found for " << fMCTruthTree->GetTree()->GetName() << endl;
			}
			else{
				if(fVerbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: Found TRecoMCTruthEvent (" << fMCTruthTree->GetEntries() << ")" << endl;
				fMCTruthTree->SetBranchAddress(branchName, &fMCTruthEvent );
				if ( fEventNb < 0 )
				{
					fEventNb = fMCTruthTree->GetEntries();
				}
				else if (fEventNb != fMCTruthTree->GetEntries())
				{
					cerr << "Input file corrupted, bad number of entries : " << fMCTruthTree->GetEntries() << endl;
				}
			}
		}
	}
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
	map<TString, TChain*>::iterator it;
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
		if(fWithMC) fMCTruthTree->GetEntry(i);
		for(it=fTree.begin(); it!=fTree.end(); it++){
			it->second->GetEntry(i);
		}

		checkNewFileOpened();

		PreProcess();
		//Process event in Analyzer
		exportEvent = false;
		for(unsigned int j=0; j<fAnalyzerList.size(); j++){
			//Get reality
			gFile->cd(fAnalyzerList[j]->GetAnalyzerName());
			if(fWithMC) fMCSimple[j]->GetRealInfos( fMCTruthEvent, fVerbosity);

			fAnalyzerList[j]->Process(i, *fMCSimple[j], fMCTruthEvent);
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
		if(exportEvent) WriteEvent();
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
	WriteTree();
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

void BaseAnalysis::RequestTree(TString name, TDetectorVEvent *evt){
	/// \MemberDescr
	/// \param name : Name of the requested TTree
	/// \param evt : Pointer to an instance of detector event (MC or Reco)
	///
	/// Request a tree in the input file. If already requested before, do nothing.
	/// \EndMemberDescr

	if(fTree.count(name)==0){
		fTree.insert(pair<TString,TChain*>(name, new TChain(name)));
		fEvent.insert(pair<TString,TDetectorVEvent*>(name,evt));
	}
	else{
		delete evt;
	}
}

bool BaseAnalysis::RequestTree(TString name, TString branchName, TString className, void* evt){
	/// \MemberDescr
	/// \param name : Name of the requested TTree
	/// \param evt : Pointer to an instance of any class
	///
	/// Request a tree in the input file. If already requested before, do nothing.
	/// \EndMemberDescr

	if(fTree.count(name)==0){
		fTree.insert(pair<TString,TChain*>(name, new TChain(name)));
		fObject.insert(pair<TString,ObjectTriplet*>(name,new ObjectTriplet(className, branchName, evt)));
		return true;
	}
	else{
		return false;
	}
}

void BaseAnalysis::GetTree(int &eventNb){
	/// \MemberDescr
	/// Effectively read all the requested trees in the input file and branch them
	/// \EndMemberDescr

	map<TString, TChain*>::iterator it;

	TString branchName;
	for(it=fTree.begin(); it!=fTree.end(); it++){
		if(fEvent.count(it->first)){
			if(strstr(fEvent[it->first]->ClassName(), "Reco")!=NULL) FindAndGetTree(it->second, "Reco", fEvent[it->first]->ClassName(), &(fEvent[it->first]), eventNb);
			else FindAndGetTree(it->second, "Hits", fEvent[it->first]->ClassName(), &(fEvent[it->first]), eventNb);
		}
		else{
			FindAndGetTree(it->second, fObject[it->first]->fBranchName, fObject[it->first]->fClassName, &(fObject[it->first]->fObject), eventNb);
		}
	}
}

TDetectorVEvent *BaseAnalysis::GetEvent(TString name){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the event is read
	///
	/// Return the pointer to the event corresponding to the given tree
	/// \EndMemberDescr

	return fEvent[name];
}

void *BaseAnalysis::GetObject(TString name){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the object is read
	///
	/// Return the pointer to the object corresponding to the given tree
	/// \EndMemberDescr
	return fObject[name]->fObject;
}

void BaseAnalysis::WriteEvent(){
	/// \MemberDescr
	/// Write the event in the output tree.
	/// \EndMemberDescr

	map<TString,TChain*>::iterator it;
	map<TString,TTree*>::iterator itTree;

	if(fExportTrees.size()==0){
		for(it=fTree.begin(); it!= fTree.end(); it++){
			fExportTrees.insert(pair<TString,TTree*>(it->first, it->second->CloneTree(0)));
		}
		fExportTrees.insert(pair<TString,TTree*>("MC", fMCTruthTree->CloneTree(0)));
	}
	for(itTree=fExportTrees.begin(); itTree!=fExportTrees.end(); itTree++){
		itTree->second->Fill();
	}
}

void BaseAnalysis::WriteTree(){
	/// \MemberDescr
	/// Write the output trees in the output file
	/// \EndMemberDescr

	map<TString,TTree*>::iterator itTree;

	for(itTree=fExportTrees.begin(); itTree!=fExportTrees.end(); itTree++){
		itTree->second->Write();
	}
}

void BaseAnalysis::WriteEventFraction(){
	/// \MemberDescr
	/// Dump the EventFraction
	/// \EndMemberDescr

	map<TString,EventFraction*>::iterator it;

	for(it=fEventFraction.begin(); it!=fEventFraction.end(); it++){
		it->second->DumpTable();
		it->second->WriteTable(fOutFileName);
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
	map<TString, TChain*>::iterator itTree;

	StringBalancedTable anTable("List of loaded Analyzers");
	StringBalancedTable evtFracTable("List of EventFraction");
	StringBalancedTable counterTable("List of Counters");
	StringBalancedTable outputTable("List of Outputs");
	StringBalancedTable treeTable("List of requested TTrees");

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

	for(itTree=fTree.begin(); itTree!=fTree.end(); itTree++){
		treeTable << itTree->first;
	}

	cout << "================================================================================" << endl;
	cout << endl << "\t *** Global settings for AnalysisFW ***" << endl << endl;

	anTable.Print("\t");
	evtFracTable.Print("\t");
	counterTable.Print("\t");
	outputTable.Print("\t");
	treeTable.Print("\t");

	cout << "================================================================================" << endl;
}

void BaseAnalysis::FindAndGetTree(TChain* tree, TString branchName, TString branchClass, void* evt, Int_t &eventNb){
	/// \MemberDescr
	/// \param tree :
	/// \param branchName : name of the branch
	/// \param branchClass : name of the branch class
	/// \param evt :
	/// \param eventNb : number of expected events
	///
	/// Branch the tree
	/// \EndMemberDescr

	TObjArray* branchesList;
	Int_t jMax;

	branchesList = tree->GetListOfBranches();
	if(!branchesList){
		cerr << "Unable to find TTree " << tree->GetName() << ". Aborting.";
		raise(SIGABRT);
	}
	jMax = branchesList->GetEntries();
	for (Int_t j=0; j < jMax; j++){
		if ( TString(branchName).CompareTo( branchesList->At(j)->GetName() ) == 0 )
		{
			if ( TString(branchClass).CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				cerr  << "Input file corrupted, bad Event class (" << ((TBranch*)branchesList->At(j))->GetClassName() << ") found for " << tree->GetTree()->GetName() << endl;
				raise(SIGABRT);
			}
			cout << "Found " << branchName << " (" << tree->GetEntries() << ") of class " << branchClass << endl;
			tree->SetBranchAddress(branchName, evt);
			if ( eventNb < 0 )
			{
				eventNb = tree->GetEntries();
			}
			else if (eventNb != tree->GetEntries())
			{
				cerr << "Input file corrupted, bad number of entries (run) : " << tree->GetEntries() << endl;
				raise(SIGABRT);
			}
		}
	}
}

void BaseAnalysis::checkNewFileOpened(){
	/// \MemberDescr
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	int openedFileNumber;
	TFile *fd;
	multimap<TString,TH1*>::iterator it;
	TString histoPath = "-1";
	TH1* histoPtr = NULL;

	if(fWithMC){
		openedFileNumber = fMCTruthTree->GetTreeNumber();
		fd = fMCTruthTree->GetFile();
	}
	else if(fTree.size()>0){
		openedFileNumber = fTree.begin()->second->GetTreeNumber();
		fd = fTree.begin()->second->GetFile();
	}
	else return;

	if(openedFileNumber>fCurrentFileNumber){
		//New file opened
		//end of burst
		for(unsigned int i=0; i<fAnalyzerList.size(); i++){
			fAnalyzerList[i]->EndOfBurst();
		}

		//Update input histograms by appending to existing one
		for(it=fInputHistoAdd.begin(); it!=fInputHistoAdd.end(); it++){
			//If needed, fetch the histogram in file
			if(histoPath.CompareTo(it->first)!=0){
				if(histoPtr) delete histoPtr;
				histoPtr = (TH1*)fd->Get(it->first);
				histoPath = it->first;
			}
			it->second->Add(histoPtr, 1.0);
		}
		if(histoPtr) delete histoPtr;

		//Update input histograms by replacing the existing one
		histoPath = "-1";
		histoPtr = NULL;
		for(it=fInputHisto.begin(); it!=fInputHisto.end(); it++){
			//If needed, fetch the histogram in file
			if(histoPath.CompareTo(it->first)!=0){
				if(histoPtr) delete histoPtr;
				histoPtr = (TH1*)fd->Get(it->first);
				histoPath = it->first;
			}
			it->second->Reset();
			it->second->Add(histoPtr, 1.0);
		}
		if(histoPtr) delete histoPtr;

		fCurrentFileNumber = openedFileNumber;
		for(unsigned int i=0; i<fAnalyzerList.size(); i++){
			fAnalyzerList[i]->StartOfBurst();
		}
	}
}

TH1* BaseAnalysis::GetInputHistogram(TString directory, TString name, bool append){
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file where this histogram will be searched
	/// \param name : Name of the searched histogram
	/// \param appendOnNewFile : \n
	///  - If set to true : When a new file is opened by the TChain the value of the new histogram extracted from this file will be appended to the existing histogram.\n
	///  - If set to false : When a new file is opened by the TChain the current histogram will be replaced by the new one.
	/// \return A pointer to the requested histogram if it was found, else a null pointer.
	///
	/// Request histograms from input file.
	/// \EndMemberDescr

	TFile *fd;
	TH1* tempHisto, *returnHisto=NULL;
	TString fullName = directory + TString("/") + name;

	if(fWithMC){
		fd = fMCTruthTree->GetFile();
	}
	else if(fTree.size()>0){
		fd = fTree.begin()->second->GetFile();
	}
	else return returnHisto;

	tempHisto = (TH1*)fd->Get(fullName);

	if(tempHisto){
		returnHisto = (TH1*)tempHisto->Clone(fullName);
		delete tempHisto;
		if(append){
			fInputHistoAdd.insert(pair<TString, TH1*>(fullName, returnHisto));
		}
		else{
			fInputHisto.insert(pair<TString, TH1*>(fullName, returnHisto));
		}
	}
	return returnHisto;
}
