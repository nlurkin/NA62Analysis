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

IOHandler::IOHandler(){
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr

	fWithMC = false;
	fOutFile = 0;

	fMCTruthTree = 0;

	fCurrentFileNumber = -1;
	fCurrentFile = NULL;
	fMCTruthEvent = new Event();
}

IOHandler::~IOHandler(){
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	map<TString, TChain*>::iterator itChain;
	map<TString, TDetectorVEvent*>::iterator itEvent;
	map<TString, TTree*>::iterator itTree;

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

	delete fMCTruthTree;
	delete fMCTruthEvent;
}


void IOHandler::RequestTree(TString name, TDetectorVEvent *evt){
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

bool IOHandler::RequestTree(TString name, TString branchName, TString className, void* evt){
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

int IOHandler::GetTree(int eventNb){
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

	return eventNb;
}

TDetectorVEvent *IOHandler::GetEvent(TString name){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the event is read
	///
	/// Return the pointer to the event corresponding to the given tree
	/// \EndMemberDescr

	return fEvent[name];
}

void *IOHandler::GetObject(TString name){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the object is read
	///
	/// Return the pointer to the object corresponding to the given tree
	/// \EndMemberDescr
	return fObject[name]->fObject;
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

bool IOHandler::OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// \param inFileName : Path to the input file
	/// \param nFiles : Number of files to open
	/// \param verbosity : verbosity level
	///
	/// Open and register the input files.
	/// \EndMemberDescr

	map<TString, TChain*>::iterator it;
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

	map<TString, TChain*>::iterator it;

	if(fWithMC) fMCTruthTree->GetEntry(iEvent);
	for(it=fTree.begin(); it!=fTree.end(); it++){
		it->second->GetEntry(iEvent);
	}

}

Event* IOHandler::GetMCTruthEvent(){
	/// \MemberDescr
	///
	/// Return a pointer to the MCTruthEvent
	/// \EndMemberDescr

	return fMCTruthEvent;
}

bool IOHandler::GetWithMC(){
	/// \MemberDescr
	///
	/// Do we have MC available in the files?
	/// \EndMemberDescr

	return fWithMC;
}

void IOHandler::FindAndGetTree(TChain* tree, TString branchName, TString branchClass, void* evt, Int_t &eventNb){
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
				cout << eventNb << endl;
				cerr << "Input file corrupted, bad number of entries (run) : " << tree->GetEntries() << endl;
				raise(SIGABRT);
			}
		}
	}
}

bool IOHandler::checkInputFile(TString fileName, AnalysisFW::VerbosityLevel verbosity){
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

	map<TString,TChain*>::iterator it;
	map<TString,TTree*>::iterator itTree;

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

void IOHandler::WriteTree(){
	/// \MemberDescr
	/// Write the output trees in the output file
	/// \EndMemberDescr

	map<TString,TTree*>::iterator itTree;

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

void IOHandler::MkOutputDir(TString name){
	/// \MemberDescr
	/// \param name : Name of the directory
	///
	/// Create a new directory in the output file
	/// \EndMemberDescr

	if(!fOutFile->FindKey("InputFiles")) fOutFile->mkdir(name);
}

void IOHandler::PrintInitSummary(){
	/// \MemberDescr
	///
	/// Print the summary after initialization
	/// \EndMemberDescr

	map<TString, TChain*>::iterator itTree;
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
	multimap<TString,TH1*>::iterator it;
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

	multimap<TString,TH1*>::iterator it;
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

TString IOHandler::GetOutputFileName(){
	/// \MemberDescr
	///
	/// Return the base name of the output file
	/// \EndMemberDescr

	return fOutFileName;
}
int IOHandler::GetCurrentFileNumber(){
	/// \MemberDescr
	///
	/// Return the index of the currently opened file
	/// \EndMemberDescr

	return fCurrentFileNumber;
}
