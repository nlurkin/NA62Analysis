/*
 * IOTree.cc
 *
 *  Created on: Mar 9, 2015
 *      Author: ncl
 */

#include "IOTree.hh"

#include <signal.h>
#include <iostream>

#include <TChain.h>

#include "StringBalancedTable.hh"

namespace NA62Analysis {
namespace Core {

IOTree::IOTree():
	IOHisto("IOTree"),
	fMCTruthTree(0),
	fMCTruthEvent(new Event()),
	fRawHeaderTree(0),
	fRawHeaderEvent(new RawHeader()),
	fWithMC(false),
	fWithRawHeader(false),
	fAllowNonExisting(false)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
	fIOType = IOHandlerType::kTREE;
}

IOTree::IOTree(const IOTree &c):
	IOHisto(c),
	fTree(c.fTree),
	fEvent(c.fEvent),
	fObject(c.fObject),
	fMCTruthTree(c.fMCTruthTree),
	fMCTruthEvent(c.fMCTruthEvent),
	fRawHeaderTree(c.fRawHeaderTree),
	fRawHeaderEvent(c.fRawHeaderEvent),
	fExportTrees(c.fExportTrees),
	fWithMC(c.fWithMC),
	fWithRawHeader(c.fWithRawHeader),
	fAllowNonExisting(false)
{
	/// \MemberDescr
	/// \param c: Reference to the object to copy
	///
	/// Copy constructor
	/// \EndMemberDescr

	fIOType = c.GetIOType();
}

IOTree::~IOTree() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	treeIterator itChain;
	objectIterator itObject;
	eventIterator itEvent;

	while(fTree.size()>0){
		itChain = fTree.begin();
		delete itChain->second;
		fTree.erase(itChain);
	}
	while(fEvent.size()>0){
		itEvent = fEvent.begin();
		delete itEvent->second;
		fEvent.erase(itEvent);
	}
	while(fObject.size()>0){
		itObject = fObject.begin();
		delete itObject->second;
		fObject.erase(itObject);
	}

	delete fMCTruthTree;
	delete fMCTruthEvent;

	delete fRawHeaderTree;
	delete fRawHeaderEvent;
}


void IOTree::RequestTree(TString name, TDetectorVEvent * const evt, TString branchName){
	/// \MemberDescr
	/// \param name : Name of the requested TTree
	/// \param evt : Pointer to an instance of detector event (MC or Reco)
	/// \param branchName : Name of the branch to request
	///
	/// Request a branch in a tree in the input file. If the tree has already been requested before,
	/// only add the new branch.
	/// If branchName is not specified, the branch "Reco" or "Hits" will be used (depending on the
	/// TDetectorVEvent class instance). If you need a different branch, please specify the name of the branch
	/// (e.g. Digis)
	/// \EndMemberDescr

	std::cout << normal() << "Requesting Detector TTree " << name << std::endl;
	eventIterator it;
	std::pair<eventIterator, eventIterator> eventRange;

	//Create the tree if not yet requested
	if(fTree.count(name)==0){
		cout << debug() << "First request... Creating TTree" << std::endl;
		fTree.insert(chainPair(name, new TChain(name)));
	}

	//Which branch are we dealing with?
	if(branchName.CompareTo("")==0){
		if(strstr(evt->ClassName(), "Reco")!=NULL) branchName="Reco";
		else if(strstr(evt->ClassName(), "Digi")!=NULL) branchName="Digis";
		else branchName="Hits";
	}
	cout << normal() << "Branch name set to " << branchName << endl;

	//Is this branch already requested?
	//If yes delete evt and return (we already have the branching object instance)
	eventRange = fEvent.equal_range(name);
	for(it=eventRange.first; it!=eventRange.second; ++it){
		if(it->second->fBranchName.CompareTo(branchName)==0){
			cout << debug() << "Branch already requested... Continue" << std::endl;
			delete evt;
			return;
		}
	}
	fEvent.insert(eventPair(name, new EventTriplet(branchName, evt)));
}

bool IOTree::RequestTree(TString name, TString branchName, TString className, void* const obj){
	/// \MemberDescr
	/// \param name : Name of the requested TTree
	/// \param branchName : Name of the Branch to retrieve
	/// \param className : Name of the class type in this branch
	/// \param obj : Pointer to an instance of any class
	/// \return False if the branch is already requested. The pointer given here should be deleted
	///
	/// Request a tree in the input file. If already requested before, do nothing.
	/// \EndMemberDescr

	std::cout << normal() << "Requesting branch " << branchName << " of generic TTree " << name << std::endl;
	std::cout << normal() << "Object class is expected to be " << className << std::endl;
	objectIterator it;
	std::pair<objectIterator, objectIterator> objectRange;

	//Create the tree if not yet requested
	if(fTree.count(name)==0){
		cout << debug() << "First request... Creating TTree" << std::endl;
		fTree.insert(chainPair(name, new TChain(name)));
	}

	//Is this branch already requested?
	//If yes signal the caller that obj shoud be deleted and return (we already have the branching object instance)
	objectRange = fObject.equal_range(name);
	for(it=objectRange.first; it!=objectRange.second; ++it){
		if(it->second->fBranchName.CompareTo(branchName)==0){
			cout << debug() << "Branch already requested... Continue" << std::endl;
			return false;
		}
	}
	fObject.insert(objectPair(name,new ObjectTriplet(className, branchName, obj)));
	return true;
}

int IOTree::BranchTrees(int eventNb){
	/// \MemberDescr
	///	\param eventNb : Number of events that should be read in the tree
	/// \return Number of events found in the Tree
	///
	/// Effectively read all the requested trees in the input file and branch them
	/// \EndMemberDescr

	treeIterator it;
	eventIterator ptr1;
	objectIterator ptr2;
	std::pair<eventIterator, eventIterator> eventRange;
	std::pair<objectIterator, objectIterator> objectRange;

	//Loop over all the trees
	for(it=fTree.begin(); it!=fTree.end(); ++it){

		//Loop over all the event branches of this tree and branch them
		eventRange = fEvent.equal_range(it->first);
		for(ptr1=eventRange.first; ptr1!=eventRange.second; ++ptr1){
			FindAndBranchTree(it->second, ptr1->second->fBranchName, ptr1->second->fEvent->ClassName(), &(ptr1->second->fEvent), eventNb);
		}

		//Loop over all the object branches of this tree and branch them
		objectRange = fObject.equal_range(it->first);
		for(ptr2=objectRange.first; ptr2!=objectRange.second; ++ptr2){
			FindAndBranchTree(it->second, ptr2->second->fBranchName, ptr2->second->fClassName, &(ptr2->second->fObject), eventNb);
		}
	}

	return eventNb;
}

TDetectorVEvent *IOTree::GetEvent(TString name, TString branchName){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the event is read
	/// \param branchName : Name of the branch
	/// \return Pointer to the event corresponding to the given tree and the given branch.
	///
	/// If branchName is left empty and there is only 1 branch requested on this tree, this
	/// single branch is returned. If there is more than 1 branch requested on this tree,
	/// return either the "Reco" or the "Hits" branch (the first one found - undefined behaviour
	/// if both "Reco" and "Hits" branches have been requested).
	/// If branchName is specified, try to return the specified branch.
	/// \EndMemberDescr

	eventIterator it;
	std::pair<eventIterator, eventIterator> eventRange;

	eventRange = fEvent.equal_range(name);
	if(eventRange.first==eventRange.second){ //Range is 0, no such event found
		std::cout << normal() << "[Error] Unable to find event in branch " << branchName << " of tree " << name << std::endl;
		return NULL;
	}
	else{
		//No branchName specified but only a single branch has been requested in this tree. Must be the one...
		it = eventRange.first;
		if(branchName.CompareTo("")==0 && (++it==eventRange.first)){
			std::cout << debug() << "No branch specified, only one found... Using " << eventRange.second->second->fBranchName << std::endl;
			return eventRange.second->second->fEvent;
		}
		it = eventRange.first;;
		for(it=eventRange.first; it!=eventRange.second; ++it){
			//If the branch is not specified but we find Reco or Hits, we return it
			//Or if this is the requested branch also return it
			if(( branchName.CompareTo("")==0 && (
					it->second->fBranchName.CompareTo("Reco")==0 ||
					it->second->fBranchName.CompareTo("Digis")==0 ||
					it->second->fBranchName.CompareTo("Hits")==0))
					|| it->second->fBranchName.CompareTo(branchName)==0){
				std::cout << debug() << "Using branch " << it->second->fBranchName << std::endl;
				return it->second->fEvent;
			}
		}
	}
	std::cout << normal() << "[Error] Unable to find event in branch " << branchName << " of tree " << name << std::endl;
	return NULL;
}

void *IOTree::GetObject(TString name, TString branchName){
	/// \MemberDescr
	/// \param name : Name of the TTree from which the object is read
	/// \param branchName : Name of the branch
	///
	///
	/// Return the pointer to the event corresponding to the given tree and the given branch.
	/// If branchName is left empty and there is only 1 branch requested on this tree, this
	/// single branch is returned. If there is more than 1 branch requested on this tree,
	/// return the first one.
	/// If branchName is specified, try to return the specified branch.
	/// \EndMemberDescr

	objectIterator it;
	std::pair<objectIterator, objectIterator> objectRange;

	objectRange = fObject.equal_range(name);
	if(objectRange.first==objectRange.second){ //Range is 0, no such object found
		std::cout << normal() << "[Error] Unable to find object in branch " << branchName << " of tree " << name << std::endl;
		return NULL;
	}
	else{
		for(it=objectRange.first; it!=objectRange.second; ++it){
			// If the branch is not specified or this is the requested one,
			// return it
			if( branchName.CompareTo("")==0 ||
					it->second->fBranchName.CompareTo(branchName)==0){
				std::cout << debug() << "Using branch " << it->second->fBranchName << std::endl;
				return it->second->fObject;
			}
		}
	}
	std::cout << "[Error] Unable to find object in branch " << branchName << " of tree " << name << std::endl;
	return NULL;
}

bool IOTree::LoadEvent(int iEvent){
	/// \MemberDescr
	/// \param iEvent : Index of the event
	/// \return true
	///
	/// Load the event from the TTrees
	/// \EndMemberDescr

	treeIterator it;
	eventIterator itEvt;
	objectIterator itObj;

	std::cout << debug() << "Loading event " << iEvent << "... " << std::endl;
	std::pair<eventIterator, eventIterator> eventRange;
	std::pair<objectIterator, objectIterator> objectRange;

	if (fGraphicalMutex->Lock() == 0) {
		fIOTimeCount.Start();
		if (fWithMC)
			fMCTruthTree->GetEntry(iEvent);
		if (fWithRawHeader)
			fRawHeaderTree->GetEntry(iEvent);
		fIOTimeCount.Stop();

		//Loop over all our trees
		for (it = fTree.begin(); it != fTree.end(); it++) {
			//Loop over all event and object branch and load the corresponding entry for each of them
			eventRange = fEvent.equal_range(it->first);
			for (itEvt = eventRange.first; itEvt != eventRange.second;
					++itEvt) {
				if (it->second->GetBranch(itEvt->second->fBranchName))
					fIOTimeCount.Start();
				it->second->GetEntry(iEvent);
				fIOTimeCount.Stop();
			}
			objectRange = fObject.equal_range(it->first);
			for (itObj = objectRange.first; itObj != objectRange.second;
					++itObj) {
				if (it->second->GetBranch(itObj->second->fBranchName))
					fIOTimeCount.Start();
				it->second->GetEntry(iEvent);
				fIOTimeCount.Stop();
			}
		}
		fGraphicalMutex->UnLock();
	}
	return true;
}

Event* IOTree::GetMCTruthEvent(){
	/// \MemberDescr
	/// \return Pointer to the MCTruthEvent
	/// \EndMemberDescr

	return fMCTruthEvent;
}

bool IOTree::GetWithMC() const{
	/// \MemberDescr
	/// \return True if the Event Tree is present
	///
	/// Do we have MC available in the files?
	/// \EndMemberDescr

	return fWithMC;
}

RawHeader* IOTree::GetRawHeaderEvent(){
	/// \MemberDescr
	/// \return Pointer to the RawHeader
	/// \EndMemberDescr

	return fRawHeaderEvent;
}

bool IOTree::GetWithRawHeader() const{
	/// \MemberDescr
	/// \return True if the RawHeader Tree is present
	///
	/// Do we have RawHeader available in the files?
	/// \EndMemberDescr

	return fWithRawHeader;
}

TChain* IOTree::GetTree(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TTree to retrieve
	///	\return Pointer to the specified TTree
	/// \EndMemberDescr
	treeIterator it;

	if((it=fTree.find(name))!= fTree.end()){
		return it->second;
	}
	else{
		std::cout << normal() << "The requested TTree pointer " << name << " cannot be found" << std::endl;
		return NULL;
	}
}

void IOTree::FindAndBranchTree(TChain* tree, TString branchName, TString branchClass, void* const evt, Int_t &eventNb){
	/// \MemberDescr
	/// \param tree :
	/// \param branchName : name of the branch
	/// \param branchClass : name of the branch class
	/// \param evt :
	/// \param eventNb : number of expected events
	///
	/// Branch the tree.
	/// \EndMemberDescr

	TObjArray* branchesList;
	Int_t jMax;

	std::cout << debug() << "Trying to branch " << branchName << " of type "
			<< branchClass << std::endl;

	fIOTimeCount.Start();
	std::cout << debug() << "Retrieving list of branches in " << branchName << " tree" << std::endl;
	branchesList = tree->GetListOfBranches();
	fIOTimeCount.Stop();
	if(!branchesList){
		if(! fAllowNonExisting){
			std::cout << normal() << "Unable to find TTree " << tree->GetName() << "... Aborting.";
			raise(SIGABRT);
		}
		else{
			std::cout << normal() << "[Warning] Unable to find TTree "
					<< tree->GetName() << ". Retrieved corresponding event will always be empty";
			return;
		}
	}
	jMax = branchesList->GetEntries();
	for (Int_t j=0; j < jMax; j++){
		if ( TString(branchName).CompareTo( branchesList->At(j)->GetName() ) == 0 )
		{
			if ( TString(branchClass).CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				std::cout << normal() << "Input file inconsistent. Bad Event class (Found: "
						<< ((TBranch*)branchesList->At(j))->GetClassName() << ", expected: "
						<< branchClass << ") for " << tree->GetTree()->GetName() << std::endl;
				raise(SIGABRT);
			}
			std::cout << normal() << "Found " << branchName << " of class " << branchClass << std::endl;
			tree->SetBranchAddress(branchName, evt);
			return;
		}
	}
	std::cout << normal() << "Unable to find branch " << branchName << " in TTree " << tree->GetName() << std::endl;
}

int IOTree::FillMCTruth(){
	/// \MemberDescr
	/// \return Number of events in the Event Tree
	///
	/// Branch the MC trees. Name is different if the input file comes from the MC or Reconstruction.
	/// \EndMemberDescr

	int eventNb = -1;
	if(!fWithMC) return eventNb;

	fIOTimeCount.Start();
	std::cout << debug() << "Retrieving number of entries in MCTruth tree" << std::endl;
	eventNb = fMCTruthTree->GetEntries();
	fIOTimeCount.Stop();
	if(eventNb==0){
		fWithMC = false;
		return -1;
	}

	std::cout << debug() << "Retrieving list of branches in MCTruth tree" << std::endl;
	fIOTimeCount.Start();
	TObjArray* branchesList = fMCTruthTree->GetListOfBranches();
	int jMax = branchesList->GetEntries();
	fIOTimeCount.Stop();
	TString branchName = "";

	for (Int_t j=0; j < jMax; j++)
	{
		std::cout << debug() << "Searching MC tree..." << std::endl;
		if ( TString("event").CompareTo( branchesList->At(j)->GetName() ) == 0){
			branchName = "event";
		}
		else if(TString("mcEvent").CompareTo( branchesList->At(j)->GetName() ) == 0 ){
			branchName = "mcEvent";
		}

		if(branchName.CompareTo("") != 0)
		{
			std::cout << debug() << "Found tree with branch " << branchName << std::endl;
			if ( TString("Event").CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				std::cout << normal() << "Input file inconsistent. Bad MC class for tree "
						<< fMCTruthTree->GetTree()->GetName() << "(Found: "
						<< ((TBranch*)branchesList->At(j))->GetClassName()
						<< ", Expected: Event)" << std::endl;
			}
			else{
				std::cout << normal() << "Found " << branchName << "(" << eventNb
						<< ")" << std::endl;
				fMCTruthTree->SetBranchAddress(branchName, &fMCTruthEvent );
			}
		}
	}
	return eventNb;
}

int IOTree::FillRawHeader(){
	/// \MemberDescr
	/// \return Number of events in the RawHeader Tree
	///
	/// Branch the RawHeader trees.
	/// \EndMemberDescr

	int eventNb = -1;
	if(!fWithRawHeader) return eventNb;

	fIOTimeCount.Start();
	std::cout << debug() << "Retrieving number of entries in RawHeader tree" << std::endl;
	eventNb = fRawHeaderTree->GetEntries();
	fIOTimeCount.Stop();
	if(eventNb==0){
		fWithRawHeader = false;
		return -1;
	}

	fIOTimeCount.Start();
	std::cout << debug() << "Retrieving list of branches in RawHeader tree" << std::endl;
	TObjArray* branchesList = fRawHeaderTree->GetListOfBranches();
	int jMax = branchesList->GetEntries();
	fIOTimeCount.Stop();
	TString branchName = "";

	for (Int_t j=0; j < jMax; j++)
	{
		std::cout << debug() << "Searching RawHeader tree..." << std::endl;
		if ( TString("RawHeader").CompareTo( branchesList->At(j)->GetName() ) == 0){
			branchName = "RawHeader";
		}
		if(branchName.CompareTo("") != 0)
		{
			std::cout << debug() << "Found tree with branch " << branchName << std::endl;
			if ( TString("RawHeader").CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				std::cout << normal() << "Input file inconsistent. Bad RawHeader class for tree "
						<< fRawHeaderTree->GetTree()->GetName() << "(Found: "
						<< ((TBranch*)branchesList->At(j))->GetClassName()
						<< ", Expected: Event)" << std::endl;

			}
			else{
				std::cout << normal() << "Found " << branchName << "(" << eventNb
						<< ")" << std::endl;
				fRawHeaderTree->SetBranchAddress(branchName, &fRawHeaderEvent );
			}
		}
	}
	return eventNb;
}


void IOTree::SetIgnoreNonExisting(bool bFlag) {
	/// \MemberDescr
	/// \param bFlag : if false, exit if one or several TTree is missing in the input file
	///
	/// Determine if the framework is allowed to run when one or several TTrees
	/// are missing in the input file.
	/// \EndMemberDescr

	if(bFlag) std::cout << normal() << "Ignoring non existing TTree" << std::endl;
	fAllowNonExisting = bFlag;
}


bool IOTree::OpenInput(TString inFileName, int nFiles){
	/// \MemberDescr
	/// \param inFileName : Path to the input file
	/// \param nFiles : Number of files to open
	/// \return True if the input files are checked and valid
	///
	/// Open and register the input files.
	/// \EndMemberDescr

	if(!IOHandler::OpenInput(inFileName, nFiles)) return false;

	treeIterator it;
	bool inputChecked = false;

	for(auto fileName : fInputfiles){
		if(!inputChecked && checkInputFile(fileName))
			inputChecked = true;
		fIOTimeCount.Start();
		if(fWithMC) fMCTruthTree->AddFile(fileName);
		if(fWithRawHeader) fRawHeaderTree->AddFile(fileName);
		for(it=fTree.begin(); it!=fTree.end(); it++) it->second->AddFile(fileName);
		fIOTimeCount.Stop();
	}
	return inputChecked;
}

bool IOTree::checkInputFile(TString fileName){
	/// \MemberDescr
	/// \param fileName : Name of the file to open
	/// \return True if the file can be opened
	///
	/// Open the input file to check if MC are present and if yes, what's the name of the TTree
	/// \EndMemberDescr

	TFile *fd;
	if(fGraphicalMutex->Lock()==0){
		fIOTimeCount.Start();
		fd = TFile::Open(fileName.Data(), "R");
		fIOTimeCount.Stop();
		fGraphicalMutex->UnLock();
	}

	if(!fd)
		return kFALSE;

	fIOTimeCount.Start();
	TList* keys = fd->GetListOfKeys();
	fIOTimeCount.Stop();

	fWithMC = true;
	if(keys->FindObject("Generated")) fMCTruthTree = new TChain("Generated");
	else if(keys->FindObject("Run_0")) fMCTruthTree = new TChain("Run_0");
	else if(keys->FindObject("mcEvent")) fMCTruthTree = new TChain("mcEvent");
	else{
		std::cout << normal() << "No MC data found" << std::endl;
		fWithMC = false;
	}

	fWithRawHeader = true;
	if(keys->FindObject("RawHeader")) fRawHeaderTree = new TChain("RawHeader");
	else{
		std::cout << normal() << "No Raw Header found" << std::endl;
		fWithRawHeader = false;
	}

	fd->Close();
	return kTRUE;
}

void IOTree::WriteEvent(){
	/// \MemberDescr
	/// Write the event in the output tree.
	/// \EndMemberDescr

	treeIterator it;
	NA62Analysis::NA62Map<TString,TTree*>::type::iterator itTree;

	cout << extended() << "Writing event in output" << std::endl;
	if(fExportTrees.size()==0){
		for(it=fTree.begin(); it!= fTree.end(); it++){
			fExportTrees.insert(std::pair<TString,TTree*>(it->first, it->second->CloneTree(0)));
		}
		if(fWithMC) fExportTrees.insert(std::pair<TString,TTree*>("MC", fMCTruthTree->CloneTree(0)));
		if(fWithRawHeader) fExportTrees.insert(std::pair<TString,TTree*>("RawHeader", fRawHeaderTree->CloneTree(0)));
	}
	for(itTree=fExportTrees.begin(); itTree!=fExportTrees.end(); itTree++){
		itTree->second->Fill();
	}
}

void IOTree::WriteTree() const{
	/// \MemberDescr
	/// Write the output trees in the output file
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TTree*>::type::const_iterator itTree;

	std::cout << normal() << "Writing output trees" << endl;
	for(itTree=fExportTrees.begin(); itTree!=fExportTrees.end(); itTree++){
		fIOTimeCount.Start();
		itTree->second->Write();
		fIOTimeCount.Stop();
	}
}

void IOTree::PrintInitSummary() const{
	/// \MemberDescr
	///
	/// Print the summary after initialization
	/// \EndMemberDescr

	IOHandler::PrintInitSummary();

	NA62Analysis::NA62Map<TString,TChain*>::type::const_iterator itTree;
	StringBalancedTable treeTable("List of requested TTrees");

	for(itTree=fTree.begin(); itTree!=fTree.end(); itTree++){
		treeTable << itTree->first;
	}

	treeTable.Print("\t");
}

bool IOTree::CheckNewFileOpened(){
	/// \MemberDescr
	/// \return True if a new file has been opened
	///
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	int openedFileNumber;
	TFile *currFile;

	if(fWithMC){
		openedFileNumber = fMCTruthTree->GetTreeNumber();
		currFile = fMCTruthTree->GetFile();
	}
	else if(fWithRawHeader){
		openedFileNumber = fRawHeaderTree->GetTreeNumber();
		currFile = fRawHeaderTree->GetFile();
	}
	else if(fTree.size()>0){
		openedFileNumber = fTree.begin()->second->GetTreeNumber();
		currFile = fTree.begin()->second->GetFile();
	}
	else return false;

	if(openedFileNumber>fCurrentFileNumber){
		IOHandler::NewFileOpened(openedFileNumber, currFile);
		return true;
	}
	return false;
}

} /* namespace Core */
} /* namespace NA62Analysis */
