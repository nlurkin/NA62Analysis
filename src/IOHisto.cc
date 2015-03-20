/*
 * IOHisto.cc
 *
 *  Created on: Mar 9, 2015
 *      Author: ncl
 */

#include "IOHisto.hh"

#include <iostream>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>

namespace NA62Analysis {
namespace Core {

IOHisto::IOHisto():
	IOHandler("IOHisto"),
	fNewFileOpened(false)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
	fIOType = IOHandlerType::kHISTO;
}

IOHisto::IOHisto(std::string name):
	IOHandler(name),
	fNewFileOpened(false)
{
	/// \MemberDescr
	/// \param name : Display name
	///
	/// Constructor with name
	/// \EndMemberDescr
	fIOType = IOHandlerType::kHISTO;
}

IOHisto::IOHisto(const IOHisto &c):
	IOHandler(c),
	fNewFileOpened(false),
	fInputHistoAdd(c.fInputHistoAdd),
	fInputHisto(c.fInputHisto),
	fReferenceFileName(c.fReferenceFileName)
{
	/// \MemberDescr
	/// \param c: Reference to the object to copy
	///
	/// Copy Constructor
	/// \EndMemberDescr

	fIOType = c.GetIOType();
}

IOHisto::~IOHisto() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr
}

TH1* IOHisto::GetReferenceTH1(TString name){
	/// \MemberDescr
	/// \param name : Name of the requested reference histogram
	/// \return Pointer to the reference histogram from the reference file
	/// \EndMemberDescr

	TFile *fd;
	TH1* tempHisto, *returnHisto=NULL;

	TString oldDirectory = gDirectory->GetName();

	if(fReferenceFileName.IsNull()) return NULL;

	fd = TFile::Open(fReferenceFileName, "READ");
	if(!fd){
		std::cerr << "Error: unable to open reference file " << fReferenceFileName << std::endl;
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
TH2* IOHisto::GetReferenceTH2(TString name){
	/// \MemberDescr
	/// \param name : Name of the requested reference histogram
	/// \return Pointer to the reference histogram from the reference file
	/// \EndMemberDescr

	TFile *fd;
	TH2* tempHisto, *returnHisto=NULL;

	TString oldDirectory = gDirectory->GetName();

	if(fReferenceFileName.IsNull()) return NULL;

	fd = TFile::Open(fReferenceFileName, "READ");
	if(!fd){
		std::cerr << "Error: unable to open reference file " << fReferenceFileName << std::endl;
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

TGraph* IOHisto::GetReferenceTGraph(TString name){
	/// \MemberDescr
	/// \param name : Name of the requested reference histogram
	/// \return Pointer to the reference histogram from the reference file
	/// \EndMemberDescr

	TFile *fd;
	TGraph* tempHisto, *returnHisto=NULL;

	TString oldDirectory = gDirectory->GetName();

	if(fReferenceFileName.IsNull()) return NULL;

	fd = TFile::Open(fReferenceFileName, "READ");
	if(!fd){
		std::cerr << "Error: unable to open reference file " << fReferenceFileName << std::endl;
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

TH1* IOHisto::GetInputHistogram(TString directory, TString name, bool append){
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file where this histogram will be searched
	/// \param name : Name of the searched histogram
	/// \param append : \n
	///  - If set to true : When a new file is opened by the TChain the value of the new histogram extracted from this file will be appended to the existing histogram.\n
	///  - If set to false : When a new file is opened by the TChain the current histogram will be replaced by the new one.
	/// \return A pointer to the requested histogram if it was found, else a null pointer.
	///
	/// Request histograms from input file. If already exists, directly return the pointer.
	/// \EndMemberDescr

	TString fullName = directory + TString("/") + name;

	NA62Analysis::NA62MultiMap<TString,TH1*>::type::iterator it;
	if((it = fInputHisto.find(fullName)) != fInputHisto.end()) return it->second;
	else if((it = fInputHistoAdd.find(fullName)) != fInputHistoAdd.end()) return it->second;
	if(!fCurrentFile) {
		std::cerr << "No input file open to extract histogram." << std::endl;
		return nullptr;
	}

	TH1* tempHisto, *returnHisto=nullptr;


	tempHisto = (TH1*)fCurrentFile->Get(fullName);

	if(tempHisto){
		returnHisto = (TH1*)tempHisto->Clone(fullName);
		delete tempHisto;
		if(append){
			fInputHistoAdd.insert(std::pair<TString, TH1*>(fullName, returnHisto));
		}
		else{
			fInputHisto.insert(std::pair<TString, TH1*>(fullName, returnHisto));
		}
	}
	return returnHisto;
}

void IOHisto::SetReferenceFileName(TString fileName) {
	/// \MemberDescr
	/// \param fileName : Path to the reference file
	///
	/// Set the path to the reference file
	/// \EndMemberDescr
	fReferenceFileName = fileName;
}

void IOHisto::UpdateInputHistograms(){
	/// \MemberDescr
	///
	/// Update the input histograms with the one coming from the current input file.
	/// \EndMemberDescr

	NA62Analysis::NA62MultiMap<TString,TH1*>::type::iterator it;
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

bool IOHisto::CheckNewFileOpened() {
	/// \MemberDescr
	/// \return true if a new file has been opened. false otherwise.
	///
	/// Check if a new file has been opened
	/// \EndMemberDescr

	bool ret = fNewFileOpened;
	if(fNewFileOpened){
		gFile = fOutFile;
		gFile->cd();
		NewFileOpened(fCurrentFileNumber, fCurrentFile);
	}
	fNewFileOpened = false;
	return ret;
}

void IOHisto::LoadEvent(int iEvent) {
	/// \MemberDescr
	/// \param iEvent : Index of the file to load
	///
	/// Load the file at index iEvent
	/// \EndMemberDescr
	if(iEvent<GetInputFileNumber()){
		if(fCurrentFile) fCurrentFile->Close();
		fCurrentFile = TFile::Open(fInputfiles[iEvent], "READ");
		fCurrentFileNumber = iEvent;
		fNewFileOpened = true;
	}
}

bool IOHisto::OpenInput(TString inFileName, int nFiles) {
	/// \MemberDescr
	/// \param inFileName : Path to the input root file, or file containing a list of input root files
	/// \param nFiles : Number of files to read
	/// \param verbosity : Verbosity level
	/// \return true in case of success. Else false
	///
	/// Open input files
	/// \EndMemberDescr

	if(!IOHandler::OpenInput(inFileName, nFiles)) return false;
	return true;
}

} /* namespace Core */
} /* namespace NA62Analysis */
