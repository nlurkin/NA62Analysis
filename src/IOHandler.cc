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
	fCurrentFileNumber(-1),
	fOutFile(0),
	fCurrentFile(NULL)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

IOHandler::IOHandler(const IOHandler& c):
	fInputHistoAdd(c.fInputHistoAdd),
	fInputHisto(c.fInputHisto),
	fCurrentFileNumber(c.fCurrentFileNumber),
	fReferenceFileName(c.fReferenceFileName),
	fOutFile(c.fOutFile),
	fOutFileName(c.fOutFileName),
	fCurrentFile(c.fCurrentFile)
{
	/// \MemberDescr
	/// Copy constructor
	/// \EndMemberDescr
}

IOHandler::~IOHandler(){
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	if(fOutFile) {
		cout << "############# Writing output file #############" << endl;
		fOutFile->Purge();
		fOutFile->Close();
		cout << "#############        DONE         #############" << endl;
	}
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

	/*TFile *fd;
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
	return returnHisto;*/
	return 0;
}

bool IOHandler::OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// \param inFileName : Path to the input file
	/// \param nFiles : Number of files to open
	/// \param verbosity : verbosity level
	///
	/// Open and register the input files.
	/// \EndMemberDescr

	int inputFileNumber = 0;

	if(inFileName.Length()==0){
		cerr << "AnalysisFW: No input file" << endl;
		return false;
	}
	if(nFiles == 0){
		if(verbosity >= AnalysisFW::kNormal) cout << "AnalysisFW: Adding file " << inFileName << endl;
		if(inFileName.Contains("/castor/") && !inFileName.Contains("root://castorpublic.cern.ch//")){
                        TString svcClass = getenv("STAGE_SVCCLASS");
                        if(svcClass=="") svcClass="na62";
			inFileName = "root://castorpublic.cern.ch//"+inFileName+"?svcClass="+svcClass;
		}
		if(inFileName.Contains("/eos/") && !inFileName.Contains("root://eosna62.cern.ch//")){
			inFileName = "root://eosna62.cern.ch//"+inFileName;
		}
		fInputfiles.push_back(inFileName);
	}else{
		TString inputFileName;
		ifstream inputList(inFileName.Data());
		while(inputFileName.ReadLine(inputList) && inputFileNumber < nFiles){
			if(verbosity>=AnalysisFW::kNormal) cout << "AnalysisFW: Adding file " << inputFileName << endl;
			if(inputFileName.Contains("/castor/") && !inputFileName.Contains("root://castorpublic.cern.ch//")){
                                TString svcClass = getenv("STAGE_SVCCLASS");
                                if(svcClass=="") svcClass="na62";
			        inputFileName = "root://castorpublic.cern.ch//"+inputFileName+"?svcClass="+svcClass;
			}
			if(inputFileName.Contains("/eos/") && !inputFileName.Contains("root://eosna62.cern.ch//")){
				inputFileName = "root://eosna62.cern.ch//"+inputFileName;
			}
			fInputfiles.push_back(inputFileName);
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
}

void IOHandler::NewFileOpened(){
	/// \MemberDescr
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	//Print fileName in the output file for future reference
	MkOutputDir("InputFiles");
	gFile->cd("InputFiles");
	TObjString fileName(fCurrentFile->GetName());
	fileName.Write();
	gFile->cd();
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

