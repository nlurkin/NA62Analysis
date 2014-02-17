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
using namespace std;

/// \class IOHandler
/// \Brief
/// Class containing and handling every IO object
/// \EndBrief
///
/// \Detailed
/// Implements the IO methods for histograms, TTrees, ...
/// \EndDetailed
class IOHandler {
public:
	IOHandler();
	virtual ~IOHandler();

	//IO Files
	bool OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity);
	bool OpenOutput(TString outFileName);
	void SetReferenceFileName(TString fileName);
	bool checkInputFile(TString fileName, AnalysisFW::VerbosityLevel verbosity);
	bool CheckNewFileOpened();
	TString GetOutputFileName();
	int GetCurrentFileNumber();

	//Histogram
	TH1* GetInputHistogram(TString directory, TString name, bool append);
	TH1* GetReferenceTH1(TString name);
	TH2* GetReferenceTH2(TString name);
	TGraph* GetReferenceTGraph(TString name);
	void UpdateInputHistograms();

	//TTree
	void RequestTree(TString name, TDetectorVEvent *evt);
	bool RequestTree(TString name, TString branchName, TString className, void* obj);
	int GetTree(int eventNb);

	//Events
	TDetectorVEvent *GetEvent(TString name);
	void* GetObject(TString name);
	int FillMCTruth(AnalysisFW::VerbosityLevel verbosity);
	void LoadEvent(int iEvent);
	Event* GetMCTruthEvent();
	bool GetWithMC();

	//Writing
	void WriteEvent();
	void WriteTree();
	void MkOutputDir(TString name);

	//Printing
	void PrintInitSummary();

private:
	void FindAndGetTree(TChain* tree, TString branchName, TString branchClass, void* evt, Int_t &eventNb);

	class ObjectTriplet{
	public:
		ObjectTriplet(TString c, TString branch, void* obj){
			fClassName = c;
			fBranchName = branch;
			fObject = obj;
		};
		TString fClassName;
		TString fBranchName;
		void* fObject;
	};
	map<TString, TChain*> fTree; ///< Container for the trees
	map<TString, TDetectorVEvent*> fEvent; ///< Container for the events
	map<TString, ObjectTriplet*> fObject; ///< Container for the events

	TChain *fMCTruthTree; ///< Container for the MC TTrees
	Event *fMCTruthEvent; ///< MC Event

	multimap<TString, TH1*> fInputHistoAdd; ///< Container for input histograms for which we append the values of the new files
	multimap<TString, TH1*> fInputHisto; ///< Container for input histograms for which we do not append the values of the new files

	int fCurrentFileNumber; ///< Index of the current opened file in the TChain

	TString fReferenceFileName; ///< Name of the file containing reference plots to compare with

	TFile *fOutFile; ///< Output file
	TString fOutFileName; ///< Output fileName

	map<TString, TTree*> fExportTrees; ///< Container for TTrees for exporting

	bool fWithMC; ///< Do we have MC in the file?

	TFile *fCurrentFile; ///< Pointer to the currently opened file in the TChain
};

#endif /* IOHANDLER_HH_ */
