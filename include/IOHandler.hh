/*
  * IOHandler.hh
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#ifndef IOHANDLER_HH_
#define IOHANDLER_HH_

#include <TString.h>
#include <TFile.h>
#include "FWEnums.hh"

/// \class IOHandler
/// \Brief
/// Class containing and handling every IO object
/// \EndBrief
///
/// \Detailed
/// Implements the IO methods for histograms, TTrees, ...\n
/// Manage input event files (loading, specific processing before closing a file or after opening it)\n
/// Load input files with reference histograms and give access to these histograms, update them.\n
/// TTree management (input and output).
/// \EndDetailed
class IOHandler {
public:
	IOHandler();
	IOHandler(const IOHandler& c);
	virtual ~IOHandler();

	//IO Files
	virtual bool OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity);
	virtual bool OpenOutput(TString outFileName);
	virtual bool CheckNewFileOpened();
	TString GetOutputFileName() const;
	int GetCurrentFileNumber() const;

	//Writing
	void MkOutputDir(TString name) const;

	//Printing
	virtual void PrintInitSummary() const;

protected:
	void NewFileOpened();

	int fCurrentFileNumber; ///< Index of the current opened file in the TChain

	TFile *fOutFile; ///< Output file
	TString fOutFileName; ///< Output fileName

	TFile *fCurrentFile; ///< Pointer to the currently opened file in the TChain

	std::vector<TString> fInputfiles;
};

#endif /* IOHANDLER_HH_ */
