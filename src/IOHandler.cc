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
#include <TKey.h>

namespace NA62Analysis {
namespace Core {

IOHandler::IOHandler():
	fIOType(IOHandlerType::kNOIO),
	fCurrentFileNumber(-1),
	fOutFile(0),
	fCurrentFile(NULL)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

IOHandler::IOHandler(std::string name):
	Verbose(name),
	fIOType(IOHandlerType::kNOIO),
	fCurrentFileNumber(-1),
	fOutFile(0),
	fCurrentFile(NULL)
{
	/// \MemberDescr
	/// \param name : Display name
	///
	/// Constructor with name
	/// \EndMemberDescr
}

IOHandler::IOHandler(const IOHandler& c):
	fIOType(c.GetIOType()),
	fCurrentFileNumber(c.fCurrentFileNumber),
	fOutFile(c.fOutFile),
	fOutFileName(c.fOutFileName),
	fCurrentFile(c.fCurrentFile)
{
	/// \MemberDescr
	/// \param c : Reference of the object to copy
	///
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

bool IOHandler::CheckDirExists(TString dir) {
	/// \MemberDescr
	/// \param dir : Directory to check
	/// \return True if the directory exists in the input file (and the input file is open)
	///
	/// Check if a directory exists in the input ROOT file.
	/// \EndMemberDescr
	if(!fCurrentFile) return false;

	return fCurrentFile->GetDirectory(dir)!=0;
}

bool IOHandler::CheckNewFileOpened() {
	/// \MemberDescr
	/// \return False
	/// \EndMemberDescr

	return false;
}

int IOHandler::GetCurrentFileNumber() const{
	/// \MemberDescr
	/// \return Index of the currently opened file
	/// \EndMemberDescr

	return fCurrentFileNumber;
}

std::vector<TString> IOHandler::GetListOfDirs(TString dir) {
	/// \MemberDescr
	/// \param dir : Directory to search
	/// \return A list of directories in the searched directory in the input ROOT file.
	/// \EndMemberDescr

	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	std::vector<TString> dirs;

	for(auto k : keys){
		if(k.className.CompareTo("TDirectoryFile")>=0) dirs.push_back(k.name);
	}
	return dirs;
}

std::vector<TString> IOHandler::GetListOfHistos(TString dir) {
	/// \MemberDescr
	/// \param dir : Directory to search
	/// \return A list of histograms in the searched directory in the input ROOT file.
	/// \EndMemberDescr

	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	std::vector<TString> histo;

	for(auto k : keys){
		if(k.className.CompareTo("TH1")>=0) histo.push_back(k.name);
		else if(k.className.CompareTo("TH2")>=0) histo.push_back(k.name);
		else if(k.className.CompareTo("TGraph")>=0) histo.push_back(k.name);
	}
	return histo;
}

std::vector<IOHandler::keyPair> IOHandler::GetListOfKeys(TString dir) {
	/// \MemberDescr
	/// \param dir : Directory to search
	/// \return A list of keys in the searched directory in the input ROOT file. The key contains
	/// the name of the object (key.name) and the className of the object (key.className)
	/// \EndMemberDescr

	std::vector<IOHandler::keyPair> keys;

	if(!CheckDirExists(dir)){
		std::cerr << "The requested directory " << dir << " does not exist in input file" << std::endl;
		return keys;
	}
	TList *kList = fCurrentFile->GetDirectory(dir)->GetListOfKeys();
	TKey *k;

	for(int i=0; i<kList->GetEntries(); i++){
		k = (TKey*)kList->At(i);
		keys.push_back(keyPair_t(k->GetName(), k->GetClassName()));
	}

	return keys;
}

std::vector<TString> IOHandler::GetListOfTGraph(TString dir) {
	/// \MemberDescr
	/// \param dir : Directory to search
	/// \return A list of TGraph in the searched directory in the input ROOT file.
	/// \EndMemberDescr

	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	std::vector<TString> histo;

	for(auto k : keys){
		if(k.className.CompareTo("TGraph")>=0) histo.push_back(k.name);
	}
	return histo;
}

std::vector<TString> IOHandler::GetListOfTH1(TString dir) {
	/// \MemberDescr
	/// \param dir : Directory to search
	/// \return A list of TH1 in the searched directory in the input ROOT file.
	/// \EndMemberDescr

	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	std::vector<TString> histo;

	for(auto k : keys){
		if(k.className.CompareTo("TH1")>=0) histo.push_back(k.name);
	}

	return histo;
}

std::vector<TString> IOHandler::GetListOfTH2(TString dir) {
	/// \MemberDescr
	/// \param dir : Directory to search
	/// \return A list of TH2 in the searched directory in the input ROOT file.
	/// \EndMemberDescr

	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	std::vector<TString> histo;

	for(auto k : keys){
		if(k.className.CompareTo("TH2")>=0) histo.push_back(k.name);
	}
	return histo;
}

TString IOHandler::GetOutputFileName() const{
	/// \MemberDescr
	/// \return Base name of the output file
	/// \EndMemberDescr

	return fOutFileName;
}

void IOHandler::MkOutputDir(TString name) const{
	/// \MemberDescr
	/// \param name : Name of the directory
	///
	/// Create a new directory in the output file
	/// \EndMemberDescr

	if(!fOutFile->FindKey(name)) fOutFile->mkdir(name);
}

void IOHandler::NewFileOpened(int index, TFile* currFile){
	/// \MemberDescr
	/// \param index : Index of the new file
	/// \param currFile : Pointer to the new file
	///
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	fCurrentFileNumber = index;
	fCurrentFile = currFile;

	//Print fileName in the output file for future reference
	MkOutputDir("InputFiles");
	gFile->cd("InputFiles");
	TObjString fileName(fCurrentFile->GetName());
	fileName.Write();
	gFile->cd();
}

bool IOHandler::OpenInput(TString inFileName, int nFiles){
	/// \MemberDescr
	/// \param inFileName : Path to the input file
	/// \param nFiles : Number of files to open
	/// \return true if success, else false
	///
	/// Open and register the input files.
	/// \EndMemberDescr

	int inputFileNumber = 0;

	if(inFileName.Length()==0){
		std::cerr << "AnalysisFW: No input file" << std::endl;
		return false;
	}
	if(nFiles == 0){
		std::cout << normal() << "AnalysisFW: Adding file " << inFileName << std::endl;
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
		while(inputFileName.ReadLine(inputList) && (nFiles<0 || inputFileNumber < nFiles)){
			std::cout << normal() << "AnalysisFW: Adding file " << inputFileName << std::endl;
			if(inputFileName.Contains("/castor/") && !inputFileName.Contains("root://castorpublic.cern.ch//")){
                                TString svcClass = getenv("STAGE_SVCCLASS");
                                if(svcClass=="") svcClass="na62";
			        inputFileName = "root://castorpublic.cern.ch//"+inputFileName+"?svcClass="+svcClass;
			}
			if(inputFileName.Contains("/eos/") && !inputFileName.Contains("root://eosna62.cern.ch//")){
				inputFileName = "root://eosna62.cern.ch//"+inputFileName;
			}
			fInputfiles.push_back(inputFileName);
			++inputFileNumber;
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
	/// \return true if success, else false
	///
	/// Open the output file
	/// \EndMemberDescr

	fOutFileName = outFileName;
	fOutFileName.ReplaceAll(".root", "");
	fOutFile = new TFile(outFileName, "RECREATE");

	if(!fOutFile) return false;
	return true;
}

void IOHandler::PrintInitSummary() const{
	/// \MemberDescr
	///
	/// Print the summary after initialization
	/// \EndMemberDescr
}

} /* namespace Core */
} /* namespace NA62Analysis */
