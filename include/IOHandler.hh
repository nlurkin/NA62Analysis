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
using namespace std;

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
	bool OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity);
	bool OpenOutput(TString outFileName);
	void SetReferenceFileName(TString fileName);
	bool checkInputFile(TString fileName, AnalysisFW::VerbosityLevel verbosity);
	bool CheckNewFileOpened();
	TString GetOutputFileName() const;
	int GetCurrentFileNumber() const;

	//Histogram
	TH1* GetInputHistogram(TString directory, TString name, bool append);
	TH1* GetReferenceTH1(TString name);
	TH2* GetReferenceTH2(TString name);
	TGraph* GetReferenceTGraph(TString name);
	void UpdateInputHistograms();

	//TTree
	void RequestTree(TString name, TDetectorVEvent* const evt, TString branchName="");
	bool RequestTree(TString name, TString branchName, TString className, void* const obj);
	int BranchTrees(int eventNb);
	TChain* GetTree(TString name);
	void SetAllowNonExisting(bool allowNonExisting);

	//Events
	TDetectorVEvent *GetEvent(TString name, TString branchName="");
	void* GetObject(TString name, TString branchName="");
	int FillMCTruth(AnalysisFW::VerbosityLevel verbosity);
	int FillRawHeader(AnalysisFW::VerbosityLevel verbosity);
	void LoadEvent(int iEvent);
	Event* GetMCTruthEvent();
	bool GetWithMC() const;
	RawHeader* GetRawHeaderEvent();
	bool GetWithRawHeader() const;

	//Writing
	void WriteEvent();
	void WriteTree() const;
	void MkOutputDir(TString name) const;

	//Printing
	void PrintInitSummary() const;

private:
	void FindAndBranchTree(TChain* tree, TString branchName, TString branchClass, void* const evt, Int_t &eventNb);

	/// \class ObjectTriplet
	/// \Brief
	/// Class containing an object branched to a custom TTree
	/// \EndBrief
	///
	/// \Detailed
	/// It contains everything needed to fully define the custom object:
	///	The class name, the branch name and the pointer to the object (stored as void*)
	/// \EndDetailed

	class ObjectTriplet{
	public:
		ObjectTriplet(TString c, TString branch, void* obj):
			fClassName(c),
			fBranchName(branch),
			fObject(obj)
		{
			/// \MemberDescr
			///	\param c : Class name of the object
			///	\param branch : Name of the branch
			/// \param obj : Pointer to the object
			///	Constructor
			///	\EndMemberDescr
		};
		TString fClassName; ///< Class name of the object
		TString fBranchName; ///< Branch name
		void* fObject; ///< Pointer to the object
	};

	/// \class EventTriplet
	/// \Brief
	/// Class containing an event branched to a TTree
	/// \EndBrief
	///
	/// \Detailed
	/// It contains the branch from which the eventis extracted and
	/// the pointer to the event itself.
	/// \EndDetailed
	class EventTriplet{
		public:
			EventTriplet(TString branch, TDetectorVEvent* obj):
				fBranchName(branch),
				fEvent(obj)
			{
				/// \MemberDescr
				///	\param branch : Name of the branch
				/// \param obj : Pointer to the event
				///	Constructor
				///	\EndMemberDescr
			};
			~EventTriplet(){
				delete fEvent;
			}
			TString fBranchName; ///< Branch name
			TDetectorVEvent* fEvent; ///< Pointer to the event
	};

	typedef pair<TString, TChain*> chainPair; ///< typedef for elements of map of TChain
	typedef pair<TString, EventTriplet*> eventPair; ///< typedef for elements of map of EventTriplet
	typedef pair<TString, ObjectTriplet*> objectPair; ///< typedef for elements of map of ObjectTriplet
	typedef AnalysisFW::NA62Map<TString,TChain*>::type::iterator treeIterator; ///< typedef for iterators of map of TChain
	typedef AnalysisFW::NA62MultiMap<TString,EventTriplet*>::type::iterator eventIterator; ///< typedef for iterators of map of EventTriplet
	typedef AnalysisFW::NA62MultiMap<TString,ObjectTriplet*>::type::iterator objectIterator; ///< typedef for iterators of map of ObjectTriplet

	AnalysisFW::NA62Map<TString,TChain*>::type fTree; ///< Container for the trees
	AnalysisFW::NA62MultiMap<TString,EventTriplet*>::type fEvent; ///< Container for the events
	AnalysisFW::NA62MultiMap<TString,ObjectTriplet*>::type fObject; ///< Container for the custom objects

	TChain *fMCTruthTree; ///< Container for the MC TTrees
	Event *fMCTruthEvent; ///< MC Event

	TChain *fRawHeaderTree; ///< Container for the RawHeader TTrees
	RawHeader *fRawHeaderEvent; ///< RawHeader

	AnalysisFW::NA62MultiMap<TString,TH1*>::type fInputHistoAdd; ///< Container for input histograms for which we append the values of the new files
	AnalysisFW::NA62MultiMap<TString,TH1*>::type fInputHisto; ///< Container for input histograms for which we do not append the values of the new files

	int fCurrentFileNumber; ///< Index of the current opened file in the TChain

	TString fReferenceFileName; ///< Name of the file containing reference plots to compare with

	TFile *fOutFile; ///< Output file
	TString fOutFileName; ///< Output fileName

	AnalysisFW::NA62Map<TString,TTree*>::type fExportTrees; ///< Container for TTrees for exporting

	bool fWithMC; ///< Do we have MC in the file?
	bool fWithRawHeader; ///< Do we have RawHeader in the file?

	TFile *fCurrentFile; ///< Pointer to the currently opened file in the TChain

	bool allowNonExisting; ///< Do we allow non existing trees
};

#endif /* IOHANDLER_HH_ */
