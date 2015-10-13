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

#include "IOHisto.hh"
#include "RawHeader.hh"

class TChain;
class TTree;

namespace NA62Analysis {
namespace Core {

/// \class IOTree
/// \Brief
/// Class containing and handling every IO TTree
/// \EndBrief
///
/// \Detailed
/// Inherits IOHandler and implements the IO methods for TTrees\n
/// TTree management (input and output).
/// \EndDetailed
class IOTree : public IOHisto{
public:
	IOTree();
	IOTree(const IOTree& c);
	virtual ~IOTree();

	//IO Files
	bool OpenInput(TString inFileName, int nFiles);
	bool checkInputFile(TString fileName);
	bool CheckNewFileOpened();

	//TTree
	void RequestTree(TString detectorName, TDetectorVEvent* const evt, TString outputStage="");
	bool RequestTree(TString treeName, TString branchName, TString className, void* const obj);
	int BranchTrees(int eventNb);
	TChain* GetTree(TString name);
	void SetIgnoreNonExisting(bool bFlag);

	//Events
	TDetectorVEvent *GetEvent(TString name, TString branchName="");
	void* GetObject(TString name, TString branchName="");
	int FillMCTruth();
	int FillRawHeader();
	bool LoadEvent(int iEvent);
	Event* GetMCTruthEvent();
	bool GetWithMC() const;
	RawHeader* GetRawHeaderEvent(TString treeName = "Reco");
	bool GetWithRawHeader() const;
	long long GetNEvents();

	//Writing
	void WriteEvent();
	void WriteTree() const;

	//Printing
	void PrintInitSummary() const;

private:
	void FindAndBranchTree(TChain* tree, TString branchName, TString branchClass, void* const evt);

	/// \class ObjectTriplet
	/// \Brief
	/// Class containing an object branched to a custom TTree
	/// \EndBrief
	///
	/// \Detailed
	/// It contains everything needed to fully define the custom object:
	///	The class name, the tree name and the pointer to the object (stored as void*)
	/// \EndDetailed

	class ObjectTriplet{
	public:
		ObjectTriplet(TString c, TString tree, void* obj):
			fClassName(c),
			fTreeName(tree),
			fObject(obj)
		{
			/// \MemberDescr
			///	\param c : Class name of the object
			///	\param tree : Name of the TTree
			/// \param obj : Pointer to the object
			///	Constructor
			///	\EndMemberDescr
		};
		TString fClassName; ///< Class name of the object
		TString fTreeName; ///< TTree name
		void* fObject; ///< Pointer to the object
	};

	/// \class EventTriplet
	/// \Brief
	/// Class containing an event branched to a TTree
	/// \EndBrief
	///
	/// \Detailed
	/// It contains the tree from which the event is extracted and
	/// the pointer to the event itself.
	/// \EndDetailed
	class EventTriplet{
		public:
			EventTriplet(TString tree, TDetectorVEvent* obj):
				fTreeName(tree),
				fEvent(obj)
			{
				/// \MemberDescr
				///	\param tree : Name of the TTree
				/// \param obj : Pointer to the event
				///	Constructor
				///	\EndMemberDescr
			};
			~EventTriplet(){
				delete fEvent;
			}
			TString fTreeName; ///< TTree name
			TDetectorVEvent* fEvent; ///< Pointer to the event
	};

	typedef std::pair<TString, TChain*> chainPair; ///< typedef for elements of map of TChain
	typedef std::pair<TString, EventTriplet*> eventPair; ///< typedef for elements of map of EventTriplet
	typedef std::pair<TString, ObjectTriplet*> objectPair; ///< typedef for elements of map of ObjectTriplet
	typedef NA62Analysis::NA62Map<TString,TChain*>::type::iterator treeIterator; ///< typedef for iterators of map of TChain
	typedef NA62Analysis::NA62MultiMap<TString,EventTriplet*>::type::iterator eventIterator; ///< typedef for iterators of map of EventTriplet
	typedef NA62Analysis::NA62MultiMap<TString,ObjectTriplet*>::type::iterator objectIterator; ///< typedef for iterators of map of ObjectTriplet

	NA62Analysis::NA62Map<TString,TChain*>::type fTree; ///< Container for the trees (Name, pointer)
	NA62Analysis::NA62MultiMap<TString,EventTriplet*>::type fEvent; ///< Container for the events (Detector, EventTriplet)
	NA62Analysis::NA62MultiMap<TString,ObjectTriplet*>::type fObject; ///< Container for the custom objects (Branch name, ObjectTriplet)

	TChain *fMCTruthTree; ///< Pointer to (first) TTree containing MCTruth
	TChain *fRawHeaderTree; ///< Pointer to (first) TTree containing RawHeader

	NA62Analysis::NA62Map<TString,TTree*>::type fExportTrees; ///< Container for TTrees for exporting

	bool fWithMC; ///< Do we have MC in the file?
	bool fWithRawHeader; ///< Do we have RawHeader in the file?

	bool fAllowNonExisting; ///< Do we allow non existing trees
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* IOTREE_HH_ */
