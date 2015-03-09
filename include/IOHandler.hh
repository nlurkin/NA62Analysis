/*
  * IOHandler.hh
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#ifndef IOHANDLER_HH_
#define IOHANDLER_HH_

#include <map>
#include <TH1.h>
#include <TFile.h>
#include <TChain.h>
#include <TDetectorVEvent.hh>
#include "FWEnums.hh"
#include "Event.hh"
#include "RawHeader.hh"
#include "containers.hh"
#include <TH2.h>
#include <TGraph.h>

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
	void SetReferenceFileName(TString fileName);
	TString GetOutputFileName() const;
	int GetCurrentFileNumber() const;

	//Histogram
	TH1* GetInputHistogram(TString directory, TString name, bool append);
	TH1* GetReferenceTH1(TString name);
	TH2* GetReferenceTH2(TString name);
	TGraph* GetReferenceTGraph(TString name);
	void UpdateInputHistograms();

	//Writing
	void MkOutputDir(TString name) const;

	//Printing
	virtual void PrintInitSummary() const;

protected:
	void NewFileOpened();

	AnalysisFW::NA62MultiMap<TString,TH1*>::type fInputHistoAdd; ///< Container for input histograms for which we append the values of the new files
	AnalysisFW::NA62MultiMap<TString,TH1*>::type fInputHisto; ///< Container for input histograms for which we do not append the values of the new files

	int fCurrentFileNumber; ///< Index of the current opened file in the TChain

	TString fReferenceFileName; ///< Name of the file containing reference plots to compare with

	TFile *fOutFile; ///< Output file
	TString fOutFileName; ///< Output fileName

	TFile *fCurrentFile; ///< Pointer to the currently opened file in the TChain

	std::vector<TString> fInputfiles;
};

#endif /* IOHANDLER_HH_ */
