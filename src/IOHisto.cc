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

IOHisto::IOHisto(){
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

IOHisto::IOHisto(const IOHisto &c):
	fInputHistoAdd(c.fInputHistoAdd),
	fInputHisto(c.fInputHisto),
	fReferenceFileName(c.fReferenceFileName)
{
	/// \MemberDescr
	/// Copy Constructor
	/// \EndMemberDescr
}

IOHisto::~IOHisto() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr
}

TH1* IOHisto::GetReferenceTH1(TString name){
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
	///
	/// Return the reference histogram from the reference file
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
	///
	/// Return the reference histogram from the reference file
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

