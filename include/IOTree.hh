/*
 * IOTree.hh
 *
 *  Created on: Mar 9, 2015
 *      Author: ncl
 */

#ifndef IOTREE_HH_
#define IOTREE_HH_

#include "TDetectorVEvent.hh"
#include "Event.hh"

#include "IOHandler.hh"
#include "IOHisto.hh"
#include "RawHeader.hh"

class TChain;
class TTree;

class IOTree : public IOHisto{
public:
	IOTree();
	IOTree(const IOTree& c);
	virtual ~IOTree();

	//IO Files
	bool OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity);
	bool checkInputFile(TString fileName, AnalysisFW::VerbosityLevel verbosity);
	bool CheckNewFileOpened();

	//TTree
	void RequestTree(TString name, TDetectorVEvent* const evt, TString branchName="");
	bool RequestTree(TString name, TString branchName, TString className, void* const obj);
	int BranchTrees(int eventNb);
	TChain* GetTree(TString name);
	void SetIgnoreNonExisting(bool bFlag);

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

	typedef std::pair<TString, TChain*> chainPair; ///< typedef for elements of map of TChain
	typedef std::pair<TString, EventTriplet*> eventPair; ///< typedef for elements of map of EventTriplet
	typedef std::pair<TString, ObjectTriplet*> objectPair; ///< typedef for elements of map of ObjectTriplet
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

	AnalysisFW::NA62Map<TString,TTree*>::type fExportTrees; ///< Container for TTrees for exporting

	bool fWithMC; ///< Do we have MC in the file?
	bool fWithRawHeader; ///< Do we have RawHeader in the file?

	bool fAllowNonExisting; ///< Do we allow non existing trees
};

#endif /* IOTREE_HH_ */
