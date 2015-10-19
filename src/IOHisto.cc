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

	fIOTimeCount.Start();
	TString oldDirectory = gDirectory->GetName();
	fIOTimeCount.Stop();

	if(fReferenceFileName.IsNull()) return NULL;

	fIOTimeCount.Start();
	fd = TFile::Open(fReferenceFileName, "READ");
	fIOTimeCount.Stop();
	if(!fd){
		std::cout << normal() << "[Error] Unable to open reference file "
				<< fReferenceFileName << std::endl;
		return NULL;
	}

	fIOTimeCount.Start();
	tempHisto = (TH1*)fd->Get(oldDirectory + "/" + name);

	fOutFile->cd(oldDirectory);
	fIOTimeCount.Stop();
	if(tempHisto){
		returnHisto = (TH1*)tempHisto->Clone(name + "_ref");
		delete tempHisto;
	}
	else std::cout << normal() << "Histogram " << oldDirectory << "/" << name
			<< " not found in reference file" << std::endl;
	fIOTimeCount.Start();
	fd->Close();
	fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	TString oldDirectory = gDirectory->GetName();
	fIOTimeCount.Stop();

	if(fReferenceFileName.IsNull()) return NULL;

	fIOTimeCount.Start();
	fd = TFile::Open(fReferenceFileName, "READ");
	fIOTimeCount.Stop();
	if(!fd){
		std::cout << normal() << "[Error] Unable to open reference file "
				<< fReferenceFileName << std::endl;
		return NULL;
	}

	fIOTimeCount.Start();
	tempHisto = (TH2*)fd->Get(oldDirectory + "/" + name);

	fOutFile->cd(oldDirectory);
	fIOTimeCount.Stop();

	if(tempHisto){
		returnHisto = (TH2*)tempHisto->Clone(name + "_ref");
		delete tempHisto;
	}
	else std::cout << normal() << "Histogram " << oldDirectory << "/" << name
			<< " not found in reference file" << std::endl;
	fIOTimeCount.Start();
	fd->Close();
	fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	TString oldDirectory = gDirectory->GetName();
	fIOTimeCount.Stop();

	if(fReferenceFileName.IsNull()) return NULL;

	fIOTimeCount.Start();
	fd = TFile::Open(fReferenceFileName, "READ");
	fIOTimeCount.Stop();
	if(!fd){
		std::cout << normal() << "[Error] Unable to open reference file "
				<< fReferenceFileName << std::endl;
		return NULL;
	}

	fIOTimeCount.Start();
	tempHisto = (TGraph*)fd->Get(oldDirectory + "/" + name);

	fOutFile->cd(oldDirectory);
	fIOTimeCount.Stop();

	if(tempHisto){
		returnHisto = (TGraph*)tempHisto->Clone(name + "_ref");
		delete tempHisto;
	}
	else std::cout << normal() << "Histogram " << oldDirectory <<  "/" << name
			<< " not found in reference file" << std::endl;
	fIOTimeCount.Start();
	fd->Close();
	fIOTimeCount.Stop();
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
	if((it = fInputHisto.find(fullName)) != fInputHisto.end() && !append) return it->second;
	else if((it = fInputHistoAdd.find(fullName)) != fInputHistoAdd.end() && append) return it->second;
	if(!fCurrentFile) {
		std::cout << normal() << "[Error] Unable to open reference file "
						<< fReferenceFileName << std::endl;
		return nullptr;
	}

	TH1* tempHisto, *returnHisto=nullptr;


	fIOTimeCount.Start();
	tempHisto = (TH1*)fCurrentFile->Get(fullName);
	fIOTimeCount.Stop();

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
	else std::cout << normal() << "Histogram " << fullName
				<< " not found in reference file" << std::endl;
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

	std::cout << debug() << "Updating input histograms..." << std::endl;
	NA62Analysis::NA62MultiMap<TString,TH1*>::type::iterator it;
	TString histoPath = "-1";
	TH1* histoPtr = NULL;

	//Update input histograms by appending to existing one
	for(it=fInputHistoAdd.begin(); it!=fInputHistoAdd.end(); it++){
		//If needed, fetch the histogram in file
		if(histoPath.CompareTo(it->first)!=0){
			std::cout << debug() << "Appending " << it->first << std::endl;
			if(histoPtr) delete histoPtr;
			fIOTimeCount.Start();
			histoPtr = (TH1*)fCurrentFile->Get(it->first);
			fIOTimeCount.Stop();
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
			std::cout << debug() << "Replacing " << it->first << std::endl;
			if(histoPtr) delete histoPtr;
			fIOTimeCount.Start();
			histoPtr = (TH1*)fCurrentFile->Get(it->first);
			fIOTimeCount.Stop();
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
		fIOTimeCount.Start();
		gFile = fOutFile;
		gFile->cd();
		fIOTimeCount.Stop();
		NewFileOpened(fCurrentFileNumber, fCurrentFile);
	}
	fNewFileOpened = false;
	return ret;
}

bool IOHisto::LoadEvent(Long64_t iEvent) {
	/// \MemberDescr
	/// \param iEvent : Index of the file to load
	/// \return true if the file was loaded successfully, else false
	///
	/// Load the file at index iEvent
	/// \EndMemberDescr
	if(iEvent<GetInputFileNumber()){
		if(fCurrentFile){
			fIOTimeCount.Start();
			fCurrentFile->Close();
			fIOTimeCount.Stop();
			delete fCurrentFile;
		}
		fIOTimeCount.Start();
		fCurrentFile = TFile::Open(fInputfiles[iEvent], "READ");
		fIOTimeCount.Stop();
		if(!fCurrentFile || checkBadFile()){
			FileSkipped(fInputfiles[iEvent]);
			return false;
		}
		fCurrentFileNumber = iEvent;
		fNewFileOpened = true;
	}
	return true;
}

bool IOHisto::OpenInput(TString inFileName, int nFiles) {
	/// \MemberDescr
	/// \param inFileName : Path to the input root file, or file containing a list of input root files
	/// \param nFiles : Number of files to read
	/// \return true in case of success. Else false
	///
	/// Open input files
	/// \EndMemberDescr

	fIOTimeCount.Start();
	bool ret = IOHandler::OpenInput(inFileName, nFiles);
	fIOTimeCount.Stop();
	return ret;
}

bool IOHisto::checkBadFile() {
	/// \MemberDescr
	/// \return True if the file is suspected to be empty, else false.
	///
	/// Check if the current file is suspected to be empty. It is suspected to be
	/// empty if it contains only TDirectoryFile entries.
	/// \EndMemberDescr

	std::vector<keyPair> dirs = GetListOfKeys("/");
	std::vector<keyPair> items;

	for(auto k : dirs){
		if(k.className.CompareTo("TDirectoryFile")!=0) return false; //We have an object different than a directory
		else{
			items = GetListOfKeys(k.name);
			for(auto sub : items){
				if(k.className.CompareTo("TDirectoryFile")!=0) return false; //We have an object different than a directory
			}
		}
	}
	std::cout << standard() << "The file " << fCurrentFile->GetName() << " seems to be empty: skipping." << std::endl;
	return true;
}

} /* namespace Core */
} /* namespace NA62Analysis */
