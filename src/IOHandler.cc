/*
 * IOHandler.cc
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#include "IOHandler.hh"

#include <iostream>
#include <signal.h>

#include <TFile.h>
#include <TObjString.h>
#include <TKey.h>
#include <TSystem.h>

#include "ConfigSettings.hh"

namespace NA62Analysis {
namespace Core {

IOHandler::IOHandler():
	fContinuousReading(false),
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
	fContinuousReading(false),
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
	fContinuousReading(false),
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

	if(fSkippedFD.is_open()){
		fSkippedFD.close();
	}
	if(fOutFile) {
		fIOTimeCount.Start();
		std::cout << "############# Writing output file #############" << std::endl;
		fOutFile->Purge();
		fOutFile->Close();
		std::cout << "#############        DONE         #############" << std::endl;
		fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	bool retVal = fCurrentFile->GetDirectory(dir);
	fIOTimeCount.Stop();
	return retVal!=0;
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

	fIOTimeCount.Start();
	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	fIOTimeCount.Stop();
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
		std::cout << normal() << "The requested directory " << dir <<
				" does not exist in input file" << std::endl;
		return keys;
	}
	fIOTimeCount.Start();
	TList *kList = fCurrentFile->GetDirectory(dir)->GetListOfKeys();
	fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	std::vector<IOHandler::keyPair> keys = GetListOfKeys(dir);
	fIOTimeCount.Stop();
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

	fIOTimeCount.Start();
	if(!fOutFile->FindKey(name)) fOutFile->mkdir(name);
	fIOTimeCount.Stop();
}

void IOHandler::NewFileOpened(int index, TFile* currFile){
	/// \MemberDescr
	/// \param index : Index of the new file
	/// \param currFile : Pointer to the new file
	///
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	std::cout << normal() << "Opening file " << index << ":" << currFile->GetName() << std::endl;
	fCurrentFileNumber = index;
	fCurrentFile = currFile;

	fIOTimeCount.Start();
	//Print fileName in the output file for future reference
	MkOutputDir("InputFiles");
	gFile->cd("InputFiles");
	TObjString fileName(fCurrentFile->GetName());
	fileName.Write();
	gFile->cd();
	fIOTimeCount.Stop();
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
		std::cout << noverbose() << "No input file specified" << std::endl;
		return false;
	}
	if(nFiles == 0){
		if(fContinuousReading){
			std::cout << standard() << "Error: Continuous reading enabled but no list file provided... Aborting" << std::endl;
			raise(SIGABRT);
		}
		if(inFileName.Contains("/castor/") && !inFileName.Contains("root://castorpublic.cern.ch//")){
			inFileName = "root://castorpublic.cern.ch//"+inFileName+"?svcClass="+Configuration::ConfigSettings::global::fSvcClass;
		}
		if(inFileName.Contains("/eos/") && !inFileName.Contains("root://eosna62.cern.ch//")){
			inFileName = "root://eosna62.cern.ch//"+inFileName;
		}
		std::cout << normal() << "Adding file " << inFileName << std::endl;
		fInputfiles.push_back(inFileName);
	}else{
		TString inputFileName;
		fIOTimeCount.Start();
		if(!TestIsTextFile(inFileName)){
			std::cout << noverbose() << "Input list file " << inFileName << " cannot be read as a text file." << std::endl;
			return false;
		}
		std::ifstream inputList;
		int counter = 0;
		char roll[4] = {'|','/','-','\\'};
		do{
			if(inputList.is_open()) inputList.close();
			inputList.open(inFileName.Data());
		    if(fContinuousReading){
		      std::cout << standard() << "Waiting for a valid List File to be ready " << roll[counter%4] << "\r" << std::flush;
		      counter++;
		      gSystem->Sleep(500);
		    }
			while(inputFileName.ReadLine(inputList) && (nFiles<0 || inputFileNumber < nFiles)){
				fIOTimeCount.Stop();
				if(inputFileName.Contains("/castor/") && !inputFileName.Contains("root://castorpublic.cern.ch//")){
						inputFileName = "root://castorpublic.cern.ch//"+inputFileName+"?svcClass="+Configuration::ConfigSettings::global::fSvcClass;
				}
				if(inputFileName.Contains("/eos/") && !inputFileName.Contains("root://eosna62.cern.ch//")){
					inputFileName = "root://eosna62.cern.ch//"+inputFileName;
				}
				std::cout << normal() << "Adding file " << inputFileName << std::endl;
				fInputfiles.push_back(inputFileName);
				++inputFileNumber;
				fIOTimeCount.Start();
			}
			fIOTimeCount.Stop();
			if(!fContinuousReading && inputFileNumber==0){
				std::cout << noverbose() << "No input file in the list " << inFileName << std::endl;
				return false;
			}
		} while(fContinuousReading && (inputFileNumber==0 || !inputList.is_open()));
		inputList.close();
		if(fContinuousReading) unlink(inFileName.Data());
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

	std::cout << normal() << "Opening output file " << outFileName << std::endl;
	fOutFileName = outFileName;
	fOutFileName.ReplaceAll(".root", "");
	fIOTimeCount.Start();
	fOutFile = new TFile(outFileName, "RECREATE");
	fIOTimeCount.Stop();

	if(!fOutFile) return false;
	return true;
}

void IOHandler::PrintInitSummary() const{
	/// \MemberDescr
	///
	/// Print the summary after initialization
	/// \EndMemberDescr
}

void IOHandler::FileSkipped(TString fileName) {
	/// \MemberDescr
	/// \param fileName: Name of the skipped file
	///
	/// File has been skipped for whatever reason. Notify it in the .skipped file
	/// \EndMemberDescr
	fIOTimeCount.Start();
	if(!fSkippedFD.is_open()) fSkippedFD.open((Configuration::ConfigSettings::global::fSkippedName+".skipped").data(), std::ios::out);
	if(!fSkippedFD.is_open()) std::cout << normal() << "Unable to open skipped file "
			<< Configuration::ConfigSettings::global::fSkippedName << ".skipped" << std::endl;
	else fSkippedFD << fileName << std::endl;
	fIOTimeCount.Stop();
}

bool TestIsTextFile(TString fileName){
	unsigned char buffer[1000];
	std::ifstream fd(fileName.Data(), std::ifstream::binary);

	fd.read((char*)buffer, 1000);
	int nread = fd.gcount();
	fd.close();
	for(int i=0; i<1000 && i<nread; i++){
		//Is it pure ASCII
		if(!TestASCIIChar(buffer[i])){
			//No, test 8-bit or variable length encoding
			if(!TestMultiByteChar(buffer[i])) return false; //Still some other special text encoding possible but we don't care at this point (UTF-16, UTF-32, ...)
		}
	}

	// Every single byte in the first 1K chunk of the file is compatible with either pure ascii or
	// 8-bit or variable lenght encoding. It is therefore considered as a text file.
	return true;
}

bool TestASCIIChar(unsigned char c) {
	if((c>9 && c<13) || (c>32 && c<126)) return true;
	return false;
}

bool TestMultiByteChar(unsigned char c) {
	if(c>128 && c<255) return true;
	return false;
}

} /* namespace Core */
} /* namespace NA62Analysis */

