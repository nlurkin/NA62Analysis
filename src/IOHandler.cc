/*
 * IOHandler.cc
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#include <signal.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include "IOHandler.hh"
#include "StringBalancedTable.hh"

IOHandler::IOHandler():
	fMCTruthTree(0),
	fMCTruthEvent(new Event()),
	fCurrentFileNumber(-1),
	fOutFile(0),
	fWithMC(false),
	fCurrentFile(NULL),
	allowNonExisting(false)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

IOHandler::IOHandler(const IOHandler& c):
	fTree(c.fTree),
	fEvent(c.fEvent),
	fObject(c.fObject),
	fMCTruthTree(c.fMCTruthTree),
	fMCTruthEvent(c.fMCTruthEvent),
	fInputHistoAdd(c.fInputHistoAdd),
	fInputHisto(c.fInputHisto),
	fCurrentFileNumber(c.fCurrentFileNumber),
	fReferenceFileName(c.fReferenceFileName),
	fOutFile(c.fOutFile),
	fOutFileName(c.fOutFileName),
	fExportTrees(c.fExportTrees),
	fWithMC(c.fWithMC),
	fCurrentFile(c.fCurrentFile),
	allowNonExisting(false)
{
	/// \MemberDescr
	/// Copy constructor
	/// \EndMemberDescr
}

IOHandler::~IOHandler(){
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	treeIterator itChain;
	objectIterator itObject;
	eventIterator itEvent;

	if(fOutFile) {
		cout << "############# Writing output file #############" << endl;
		fOutFile->Purge();
		fOutFile->Close();
		cout << "#############        DONE         #############" << endl;
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
	while(fObject.size()>0){
		itObject = fObject.begin();
		delete itObject->second;
		fObject.erase(itObject);
	}

	delete fMCTruthTree;
	delete fMCTruthEvent;
}


void IOHandler::RequestTree(TString name, TDetectorVEvent * const evt, TString branchName){
	/// \MemberDescr
	/// \param name : Name of the requested TTree
	/// \param evt : Pointer to an instance of detector event (MC or Reco)
	///
	/// Request a tree in the input file. If already requested before, do nothing.
	/// \EndMemberDescr

	if(fTree.count(name)==0){
		fTree.insert(chainPair(name, new TChain(name)));
	}
	if(branchName.CompareTo("")==0){
		if(strstr(evt->ClassName(), "Reco")!=NULL) branchName="Reco";
		else branchName="Hits";
	}
	fEvent.insert(eventPair(name, new EventTriplet(branchName, evt)));
	//else{
	//	delete evt;
	//}
}

void IOHandler::RequestTree(TString name, TString branchName, TString className, void* const obj){
	/// \MemberDescr
	/// \param name : Name of the requested TTree
	/// \param branchName : Name of the Branch to retrieve
	/// \param className : Name of the class type in this branch
	/// \param obj : Pointer to an instance of any class
	///
	/// Request a tree in the input file. If already requested before, do nothing.
	/// \EndMemberDescr

	if(fTree.count(name)==0){
		fTree.insert(chainPair(name, new TChain(name)));
	}
	fObject.insert(objectPair(name,new ObjectTriplet(className, branchName, obj)));
	//else{
	//	return false;
	//}
}

int IOHandler::BranchTrees(int eventNb){
	/// \MemberDescr
	///	\param eventNb : Number of events that should be read in the tree
	///
	/// Effectively read all the requested trees in the input file and branch them
	/// \EndMemberDescr
	treeIterator it;
	eventIterator ptr1;
	objectIterator ptr2;
	pair<eventIterator, eventIterator> eventRange;
	pair<objectIterator, objectIterator> objectRange;

	for(it=fTree.begin(); it!=fTree.end(); ++it){
		eventRange = fEvent.equal_range(it->first);
		//if((ptr1=fEvent.find(it->first))!=fEvent.end()){
		for(ptr1=eventRange.first; ptr1!=eventRange.second; ++ptr1){
			FindAndBranchTree(it->second, ptr1->second->fBranchName, ptr1->second->fEvent->ClassName(), &(ptr1->second->fEvent), eventNb);
		}
		objectRange = fObject.equal_range(it->first);
		//ptr2=fObject.find(it->first);
		for(ptr2=objectRange.first; ptr2!=objectRange.second; ++ptr2){
			FindAndBranchTree(it->second, ptr2->second->fBranchName, ptr2->second->fClassName, &(ptr2->second->fObject), eventNb);
		}
	}

	return eventNb;
}

TDetectorVEvent *IOHandler::GetEvent(TString name, TString branchName){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the event is read
	///
	/// Return the pointer to the event corresponding to the given tree
	/// \EndMemberDescr

	eventIterator it;
	pair<eventIterator, eventIterator> eventRange;

	eventRange = fEvent.equal_range(name);
	if(eventRange.first==eventRange.second){
		cerr << "Error: Unable to find event in tree " << name << endl;
		return NULL;
	}
	else{
		for(it=eventRange.first; it!=eventRange.second; ++it){
			if(( branchName.CompareTo("")==0 && (
					it->second->fBranchName.CompareTo("Reco")==0 ||
					it->second->fBranchName.CompareTo("Hits")==0))
					|| it->second->fBranchName.CompareTo(branchName)==0){
				return it->second->fEvent;
			}
		}
	}
	cerr << "Error: Unable to find event in branch " << branchName << " in tree " << name << endl;
	return NULL;
}

void *IOHandler::GetObject(TString name, TString branchName){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the object is read
	///
	/// Return the pointer to the object corresponding to the given tree
	/// \EndMemberDescr

	objectIterator it;
	pair<objectIterator, objectIterator> objectRange;

	objectRange = fObject.equal_range(name);
	if(objectRange.first==objectRange.second){
		cerr << "Error: Unable to find object in tree " << name << endl;
		return NULL;
	}
	else{
		for(it=objectRange.first; it!=objectRange.second; ++it){
			if( branchName.CompareTo("")==0 ||
					it->second->fBranchName.CompareTo(branchName)==0){
				return it->second->fObject;
			}
		}
	}
	cerr << "Error: Unable to find object in branch " << branchName << " in tree " << name << endl;
	return NULL;
}

TH1* IOHandler::GetReferenceTH1(TString name){
	/// \MemberDescr
	/// \param name : Name of the requested reference histogram
	///
	/// Return the reference histogram from the reference file
	/// \EndMemberDescr

	TFile *fd;
	TH1* tempHisto, *returnHisto=NULL;

	TString oldDirectory = gDirectory->GetName();

	if(fReferenceFileName.IsNull()) return NULL;

	fd = TFile::Open(fReferenceFileName, "READ");
	if(!fd){
		cerr << "Error: unable to open reference file " << fReferenceFileName << endl;
		return NULL;
	}

	tempHisto = (TH1*)fd->Get(oldDirectory + "/" + name);

	fOutFile->cd(oldDirectory);
	if(tempHisto){
		returnHisto = (TH1*)tempHisto->Clone(name + "_ref");
		delete tempHisto;
	}
	fd->Close();
	delete fd;
	return returnHisto;
}
TH2* IOHandler::GetReferenceTH2(TString name){
	/// \MemberDescr
	/// \param name : Name of the requested reference histogram
	///
	/// Return the reference histogram from the reference file
	/// \EndMemberDescr

	TFile *fd;
	TH2* tempHisto, *returnHisto=NULL;

	TString oldDirectory = gDirectory->GetName();

	if(fReferenceFileName.IsNull()) return NULL;

	fd = TFile::Open(fReferenceFileName, "READ");
	if(!fd){
		cerr << "Error: unable to open reference file " << fReferenceFileName << endl;
		return NULL;
	}

	tempHisto = (TH2*)fd->Get(oldDirectory + "/" + name);

	fOutFile->cd(oldDirectory);
	if(tempHisto){
		returnHisto = (TH2*)tempHisto->Clone(name + "_ref");
		delete tempHisto;
	}
	fd->Close();
	delete fd;
	return returnHisto;
}
TGraph* IOHandler::GetReferenceTGraph(TString name){
	/// \MemberDescr
	/// \param name : Name of the requested reference histogram
	///
	/// Return the reference histogram from the reference file
	/// \EndMemberDescr

	TFile *fd;
	TGraph* tempHisto, *returnHisto=NULL;

	TString oldDirectory = gDirectory->GetName();

	if(fReferenceFileName.IsNull()) return NULL;

	fd = TFile::Open(fReferenceFileName, "READ");
	if(!fd){
		cerr << "Error: unable to open reference file " << fReferenceFileName << endl;
		return NULL;
	}

	tempHisto = (TGraph*)fd->Get(oldDirectory + "/" + name);

	fOutFile->cd(oldDirectory);
	if(tempHisto){
		returnHisto = (TGraph*)tempHisto->Clone(name + "_ref");
		delete tempHisto;
	}
	fd->Close();
	delete fd;
	return returnHisto;
}

TH1* IOHandler::GetInputHistogram(TString directory, TString name, bool append){
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file where this histogram will be searched
	/// \param name : Name of the searched histogram
	/// \param append : \n
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

bool IOHandler::OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// \param inFileName : Path to the input file
	/// \param nFiles : Number of files to open
	/// \param verbosity : verbosity level
	///
	/// Open and register the input files.
	/// \EndMemberDescr

	treeIterator it;
	int inputFileNumber = 0;
	bool inputChecked = false;

	if(inFileName.Length()==0){
		cerr << "AnalysisFW: No input file" << endl;
		return false;
	}
	if(nFiles == 0){
		if(verbosity >= AnalysisFW::kNormal) cout << "AnalysisFW: Adding file " << inFileName << endl;
		if(inFileName.Contains("/castor/") && !inFileName.Contains("root://castorpublic.cern.ch//")){
			inFileName = "root://castorpublic.cern.ch//"+inFileName;
		}
		if(inFileName.Contains("/eos/") && !inFileName.Contains("root://eosna62.cern.ch//")){
			inFileName = "root://eosna62.cern.ch//"+inFileName;
		}
		checkInputFile(inFileName, verbosity);
		if(fWithMC)
			fMCTruthTree->AddFile(inFileName);
		for(it=fTree.begin(); it!=fTree.end(); it++){
			it->second->AddFile(inFileName);
		}
	}else{
		TString inputFileName;
		ifstream inputList(inFileName.Data());
		while(inputFileName.ReadLine(inputList) && inputFileNumber < nFiles){
			if(verbosity>=AnalysisFW::kNormal) cout << "AnalysisFW: Adding file " << inputFileName << endl;
			if(inputFileName.Contains("/castor/") && !inputFileName.Contains("root://castorpublic.cern.ch//")){
				inputFileName = "root://castorpublic.cern.ch//"+inputFileName;
			}
			if(inputFileName.Contains("/eos/") && !inputFileName.Contains("root://eosna62.cern.ch//")){
				inputFileName = "root://eosna62.cern.ch//"+inputFileName;
			}
			if(!inputChecked && checkInputFile(inputFileName, verbosity))
				inputChecked = kTRUE;
			if(fWithMC){
				fMCTruthTree->AddFile(inputFileName);
				inputFileNumber = fMCTruthTree->GetNtrees();
			}
			for(it=fTree.begin(); it!=fTree.end(); it++){
				it->second->AddFile(inputFileName);
				inputFileNumber = it->second->GetNtrees();
			}

		}
		if(inputFileNumber==0){
			cerr << "AnalysisFW: No input file in the list " << inFileName << endl;
			return false;
		}
	}
	return true;
}

void IOHandler::SetReferenceFileName(TString fileName) {
	/// \MemberDescr
	/// \param fileName : Path to the reference file
	///
	/// Set the path to the reference file
	/// \EndMemberDescr
	fReferenceFileName = fileName;
}

void IOHandler::LoadEvent(int iEvent){
	/// \MemberDescr
	/// \param iEvent : Index of the event
	///
	/// Load the event from the TTrees
	/// \EndMemberDescr

	treeIterator it;
	eventIterator itEvt;
	objectIterator itObj;

	pair<eventIterator, eventIterator> eventRange;
	pair<objectIterator, objectIterator> objectRange;

	if(fWithMC) fMCTruthTree->GetEntry(iEvent);
	for(it=fTree.begin(); it!=fTree.end(); it++){
		eventRange = fEvent.equal_range(it->first);
		for(itEvt=eventRange.first; itEvt!=eventRange.second; ++itEvt){
			//cout << itEvt->second->fBranchName << endl;
			//it->second->GetListOfBranches()->Print();
			it->second->GetBranch(itEvt->second->fBranchName)->GetEntry(iEvent);
		}
		objectRange = fObject.equal_range(it->first);
		for(itObj=objectRange.first; itObj!=objectRange.second; ++itObj){
			it->second->GetBranch(itObj->second->fBranchName)->GetEntry(iEvent);
		}
	}

}

Event* IOHandler::GetMCTruthEvent(){
	/// \MemberDescr
	///
	/// Return a pointer to the MCTruthEvent
	/// \EndMemberDescr

	return fMCTruthEvent;
}

bool IOHandler::GetWithMC() const{
	/// \MemberDescr
	///
	/// Do we have MC available in the files?
	/// \EndMemberDescr

	return fWithMC;
}

TChain* IOHandler::GetTree(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TTree to retrieve
	///
	///	Return a pointer to the specified TTree
	/// \EndMemberDescr
	treeIterator it;

	if((it=fTree.find(name))!= fTree.end()){
		return it->second;
	}
	else{
		cerr << "The requested TTree pointer " << name << " cannot be found" << endl;
		return NULL;
	}
}

void IOHandler::FindAndBranchTree(TChain* tree, TString branchName, TString branchClass, void* const evt, Int_t &eventNb){
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
		if(! allowNonExisting){
			cerr << "Unable to find TTree " << tree->GetName() << ". Aborting.";
			raise(SIGABRT);
		}
		else{
			cerr << "Warning: unable to find TTree " << tree->GetName() << ". Retrieved corresponding event will always be empty";
			return;
		}
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
				eventNb = tree->GetBranch(branchName)->GetEntries();
			}
			else if (eventNb != tree->GetBranch(branchName)->GetEntries())
			{
				cout << eventNb << endl;
				cerr << "Input file corrupted, bad number of entries (run) : " << tree->GetBranch(branchName)->GetEntries() << endl;
				raise(SIGABRT);
			}
			return;
		}
	}
	cerr << "Unable to find branch " << branchName << " in TTree " << tree->GetName() << endl;
}

bool IOHandler::checkInputFile(TString fileName, AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// \param fileName : Name of the file to open
	///	\param verbosity : Verbosity level
	///
	/// Open the input file to check if MC are present and if yes, what's the name of the TTree
	/// \EndMemberDescr

	TFile *fd = TFile::Open(fileName.Data(), "R");

	if(!fd)
		return kFALSE;

	TList* keys = fd->GetListOfKeys();

	fWithMC = true;
	if(keys->FindObject("Generated")) fMCTruthTree = new TChain("Generated");
	else if(keys->FindObject("Run_0")) fMCTruthTree = new TChain("Run_0");
	else if(keys->FindObject("mcEvent")) fMCTruthTree = new TChain("mcEvent");
	else{
		if(verbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: No MC data found" << endl;
		fWithMC = false;
	}
	fd->Close();
	return kTRUE;
}

int IOHandler::FillMCTruth(AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// Branch the MC trees. Name is different if the input file comes from the MC or Reconstruction.
	/// \EndMemberDescr

	//TODO find another way to fill fEventNb
	int eventNb = -1;
	if(!fWithMC) return eventNb;

	eventNb = fMCTruthTree->GetEntries();

	TObjArray* branchesList = fMCTruthTree->GetListOfBranches();
	int jMax = branchesList->GetEntries();
	TString branchName = "";

	for (Int_t j=0; j < jMax; j++)
	{
		if(verbosity >= AnalysisFW::kSomeLevel) cout << "AnalysisFW: BranchName " <<  branchesList->At(j)->GetName() << endl;
		if ( TString("event").CompareTo( branchesList->At(j)->GetName() ) == 0){
			branchName = "event";
		}
		else if(TString("mcEvent").CompareTo( branchesList->At(j)->GetName() ) == 0 ){
			branchName = "mcEvent";
		}
		if(branchName.CompareTo("") != 0)
		{
			if(verbosity >= AnalysisFW::kSomeLevel) cout << "AnalysisFW: ClassName " << ((TBranch*)branchesList->At(j))->GetClassName() << endl;
			if ( TString("Event").CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				cerr  << "Input file corrupted, bad reco class found for " << fMCTruthTree->GetTree()->GetName() << endl;
			}
			else{
				if(verbosity>=AnalysisFW::kSomeLevel) cout << "AnalysisFW: Found TRecoMCTruthEvent (" << fMCTruthTree->GetEntries() << ")" << endl;
				fMCTruthTree->SetBranchAddress(branchName, &fMCTruthEvent );
				if ( eventNb < 0 )
				{
					eventNb = fMCTruthTree->GetEntries();
				}
				else if (eventNb != fMCTruthTree->GetEntries())
				{
					cerr << "Input file corrupted, bad number of entries : " << fMCTruthTree->GetEntries() << endl;
				}
			}
		}
	}
	return eventNb;
}

void IOHandler::WriteEvent(){
	/// \MemberDescr
	/// Write the event in the output tree.
	/// \EndMemberDescr

	treeIterator it;
	AnalysisFW::NA62Map<TString,TTree*>::type::iterator itTree;

	if(fExportTrees.size()==0){
		for(it=fTree.begin(); it!= fTree.end(); it++){
			fExportTrees.insert(pair<TString,TTree*>(it->first, it->second->CloneTree(0)));
		}
		if(fWithMC) fExportTrees.insert(pair<TString,TTree*>("MC", fMCTruthTree->CloneTree(0)));
	}
	for(itTree=fExportTrees.begin(); itTree!=fExportTrees.end(); itTree++){
		itTree->second->Fill();
	}
}

void IOHandler::WriteTree() const{
	/// \MemberDescr
	/// Write the output trees in the output file
	/// \EndMemberDescr

	AnalysisFW::NA62Map<TString,TTree*>::type::const_iterator itTree;

	for(itTree=fExportTrees.begin(); itTree!=fExportTrees.end(); itTree++){
		itTree->second->Write();
	}
}

bool IOHandler::OpenOutput(TString outFileName){
	/// \MemberDescr
	/// \param outFileName : Path to the output file
	///
	/// Open the output file
	/// \EndMemberDescr

	fOutFileName = outFileName;
	fOutFileName.ReplaceAll(".root", "");
	fOutFile = new TFile(outFileName, "RECREATE");

	if(!fOutFile) return false;
	return true;
}

void IOHandler::MkOutputDir(TString name) const{
	/// \MemberDescr
	/// \param name : Name of the directory
	///
	/// Create a new directory in the output file
	/// \EndMemberDescr

	if(!fOutFile->FindKey("InputFiles")) fOutFile->mkdir(name);
}

void IOHandler::PrintInitSummary() const{
	/// \MemberDescr
	///
	/// Print the summary after initialization
	/// \EndMemberDescr

	AnalysisFW::NA62Map<TString,TChain*>::type::const_iterator itTree;
	StringBalancedTable treeTable("List of requested TTrees");

	for(itTree=fTree.begin(); itTree!=fTree.end(); itTree++){
		treeTable << itTree->first;
	}

	treeTable.Print("\t");
}

bool IOHandler::CheckNewFileOpened(){
	/// \MemberDescr
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	int openedFileNumber;
	TString histoPath = "-1";

	if(fWithMC){
		openedFileNumber = fMCTruthTree->GetTreeNumber();
		fCurrentFile = fMCTruthTree->GetFile();
	}
	else if(fTree.size()>0){
		openedFileNumber = fTree.begin()->second->GetTreeNumber();
		fCurrentFile = fTree.begin()->second->GetFile();
	}
	else return false;

	if(openedFileNumber>fCurrentFileNumber){
		fCurrentFileNumber = openedFileNumber;
		//Print fileName in the output file for future reference
		MkOutputDir("InputFiles");
		gFile->cd("InputFiles");
		TObjString fileName(fCurrentFile->GetName());
		fileName.Write();
		gFile->cd();
		return true;
	}
	return false;
}

void IOHandler::UpdateInputHistograms(){
	/// \MemberDescr
	///
	/// Update the input histograms with the one coming from the current input file.
	/// \EndMemberDescr

	AnalysisFW::NA62MultiMap<TString,TH1*>::type::iterator it;
	TString histoPath = "-1";
	TH1* histoPtr = NULL;

	//Update input histograms by appending to existing one
	for(it=fInputHistoAdd.begin(); it!=fInputHistoAdd.end(); it++){
		//If needed, fetch the histogram in file
		if(histoPath.CompareTo(it->first)!=0){
			if(histoPtr) delete histoPtr;
			histoPtr = (TH1*)fCurrentFile->Get(it->first);
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
			histoPtr = (TH1*)fCurrentFile->Get(it->first);
			histoPath = it->first;
		}
		it->second->Reset();
		it->second->Add(histoPtr, 1.0);
	}
	if(histoPtr) delete histoPtr;

}

TString IOHandler::GetOutputFileName() const{
	/// \MemberDescr
	///
	/// Return the base name of the output file
	/// \EndMemberDescr

	return fOutFileName;
}
int IOHandler::GetCurrentFileNumber() const{
	/// \MemberDescr
	///
	/// Return the index of the currently opened file
	/// \EndMemberDescr

	return fCurrentFileNumber;
}

void IOHandler::SetAllowNonExisting(bool allowNonExisting) {
	this->allowNonExisting = allowNonExisting;
}
