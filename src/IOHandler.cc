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
#include <TThread.h>
#include <TRegexp.h>

#include "SvnRevision.hh"
#include "ConfigSettings.hh"
#include "TermManip.hh"

namespace NA62Analysis {
namespace Core {

IOHandler::IOHandler():
	fContinuousReading(false),
	fSignalExit(false),
	fIOType(IOHandlerType::kNOIO),
	fCurrentFileNumber(-1),
	fOutFile(0),
	fCurrentFile(NULL),
	fGraphicalMutex(NULL)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
	fCurrentDir = get_current_dir_name();
}

IOHandler::IOHandler(std::string name):
	Verbose(name),
	fContinuousReading(false),
	fSignalExit(false),
	fIOType(IOHandlerType::kNOIO),
	fCurrentFileNumber(-1),
	fOutFile(0),
	fCurrentFile(NULL),
	fGraphicalMutex(NULL)
{
	/// \MemberDescr
	/// \param name : Display name
	///
	/// Constructor with name
	/// \EndMemberDescr
	fCurrentDir = get_current_dir_name();
}

IOHandler::IOHandler(const IOHandler& c):
	Verbose(c),
	fContinuousReading(false),
	fSignalExit(false),
	fIOType(c.GetIOType()),
	fCurrentFileNumber(c.fCurrentFileNumber),
	fOutFile(c.fOutFile),
	fOutFileName(c.fOutFileName),
	fCurrentDir(c.fCurrentDir),
	fCurrentFile(c.fCurrentFile),
	fGraphicalMutex(c.fGraphicalMutex)
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

	free(fCurrentDir);
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

	if(index!=(fCurrentFileNumber+1)){
		for(int i=fCurrentFileNumber+1; i<index; i++){
			std::cout << normal() << "File " << i << ":" << fInputfiles[i] << " has been skipped" << std::endl;
			FileSkipped(fInputfiles[i]);
		}
	}
	std::cout << normal() << "Opening file " << index << ":" << currFile->GetName() << std::endl;
	fCurrentFileNumber = index;
	fCurrentFile = currFile;

	fIOTimeCount.Start();
	//Print fileName in the output file for future reference
	MkOutputDir("InputFiles");
	gFile->cd("InputFiles");
	TString fileNameT = TString(fCurrentFile->GetName());
	TObjString fileName;
	if(fileNameT.BeginsWith("/") ||
			fileNameT.BeginsWith("root://")){
		//It is an absolute path
		fileName = TObjString(fileNameT);
	}
	else{
		fileName = TObjString(TString(fCurrentDir) + "/" + fileNameT);
	}
	fileName.Write();
	gFile->cd();
	fIOTimeCount.Stop();

	int fileRevision = ReadCurrentFileRevision(); //Read revision from file
	TString persistencyRevisionString = GetCurrentSvnRevision();	  //Read revision from persistency
	if(persistencyRevisionString.Length()>0 && fileRevision!=-1){
		//Revision could be found in both the file and the persistency.
		//Can compare!
		int persistencyRev = TString(persistencyRevisionString(TRegexp("[0-9]+"))).Atoi();
		if(persistencyRev!=persistencyRevisionString){
			std::cout << manip::brown << manip::bold << normal() << "WARNING: File revision (" << fileRevision <<
					") and Persistency revision (" << persistencyRev << ") are different." << std::endl;
			std::cout << normal() << "This might lead to inconsistencies." << std::endl;
			std::cout << manip::reset;
		}
	}
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
			// Continuous reading needs a list of files, not a single file
			std::cout << standard() << "Error: Continuous reading enabled but no list file provided... Aborting" << std::endl;
			raise(SIGABRT);
		}
		// Use new address format for castor and eos
		if(inFileName.Contains("/castor/") && !inFileName.Contains("root://castorpublic.cern.ch//")){
			inFileName = "root://castorpublic.cern.ch//"+inFileName+"?svcClass="+Configuration::ConfigSettings::global::fSvcClass;
		}
		if(inFileName.Contains("/eos/") && !inFileName.Contains("root://eosna62.cern.ch//")){
			inFileName = "root://eosna62.cern.ch//"+inFileName;
		}
		std::cout << normal() << "Adding file " << inFileName << std::endl;
		fInputfiles.push_back(inFileName);
	}else{
		//Reading a list of files
		TString inputFileName;
		fIOTimeCount.Start();
		//Check it is indeed a text file and not a binary file
		if(!TestIsTextFile(inFileName)){
			std::cout << noverbose() << "Input list file " << inFileName << " cannot be read as a text file." << std::endl;
			return false;
		}
		std::ifstream inputList;
		int counter = 0;
		char roll[4] = {'|','/','-','\\'};
		do{
			//If we already read an input list, close it first and retry (has already been processed)
			if(inputList.is_open()) inputList.close();
			TThread::CancelPoint();
			inputList.open(inFileName.Data());

		    if(fContinuousReading){ // Display waiting wheel
		      std::cout << standard() << "Waiting for a valid List File to be ready " << roll[counter%4] << "\r" << std::flush;
		      counter++;
		      gSystem->Sleep(500);
		    }
		    //Try to read the file as long as we can read more in the list and that we didn't reach the limit
			while(inputFileName.ReadLine(inputList) && (nFiles<0 || inputFileNumber < nFiles)){
				fIOTimeCount.Stop();
				// Use new address format for castor and eos
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

			//If list file is empty or we did not manage to read at least 1 file, abort.
			//Unless continuous reading, in such case, we just retry until it works
			if(!fContinuousReading && inputFileNumber==0){
				std::cout << noverbose() << "No input file in the list " << inFileName << std::endl;
				return false;
			}
		// If continuous reading, loop until we read at least 1 file
		} while(!fSignalExit && fContinuousReading && (inputFileNumber==0 || !inputList.is_open()));

		//Close and eventually delete input list
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

int IOHandler::ReadCurrentFileRevision() {
	/// \MemberDescr
	/// \return Revision number embedded in the input ROOT file or -1 if not found
	/// \MemberDescr
	TList *keys = fCurrentFile->GetListOfKeys();
	int revValue = -1;
	for(int kIndex = 0; kIndex<keys->GetEntries(); kIndex++){
		TKey *k = static_cast<TKey*>(keys->At(kIndex));
		if(TString(k->GetName()).BeginsWith("Revision:")){
			revValue = TString(TString(k->GetName())(TRegexp("[0-9]+"))).Atoi();
		}
	}
	return revValue;
}

bool TestIsTextFile(TString fileName){
	/// \MemberDescr
	/// \param fileName : Path to the file to test
	/// \return True is the file has been found to be a text file, else false
	///
	/// Test all the characters in the first 1KB chunk of the file. If all of them
	/// are found to be valid text characters (ASCII or 8-bit variable length encoding),
	/// the file is considered as being a valid text file.
	/// \MemberDescr
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
	/// \MemberDescr
	/// \param c : char to test
	/// \return True if the given char is ASCII
	/// \MemberDescr
	if((c>9 && c<13) || (c>32 && c<126)) return true;
	return false;
}

bool TestMultiByteChar(unsigned char c) {
	/// \MemberDescr
	/// \param c : char to test
	/// \return True if the given char is a multibyte char
	/// \MemberDescr

	if(c>128 && c<255) return true;
	return false;
}

void IOHandler::PurgeOutput() const {
	/// \MemberDescr
	/// Purge the output file
	/// \MemberDescr

	fOutFile->Purge();
}

} /* namespace Core */
} /* namespace NA62Analysis */

