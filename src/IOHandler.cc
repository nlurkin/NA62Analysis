/*
 * IOHandler.cc
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#include "IOHandler.hh"
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TObjString.h>

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
	fCurrentFileNumber(c.fCurrentFileNumber),
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
		std::cout << "############# Writing output file #############" << std::endl;
		fOutFile->Purge();
		fOutFile->Close();
		std::cout << "#############        DONE         #############" << std::endl;
	}
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
		std::cerr << "AnalysisFW: No input file" << std::endl;
		return false;
	}
	if(nFiles == 0){
		if(verbosity >= AnalysisFW::kNormal) std::cout << "AnalysisFW: Adding file " << inFileName << std::endl;
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
		std::ifstream inputList(inFileName.Data());
		while(inputFileName.ReadLine(inputList) && inputFileNumber < nFiles){
			if(verbosity>=AnalysisFW::kNormal) std::cout << "AnalysisFW: Adding file " << inputFileName << std::endl;
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
			std::cerr << "AnalysisFW: No input file in the list " << inFileName << std::endl;
			return false;
		}
	}
	return true;
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

