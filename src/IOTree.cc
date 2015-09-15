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
#include <TKey.h>

#include "StringBalancedTable.hh"

namespace NA62Analysis {
namespace Core {

IOTree::IOTree():
	IOHisto("IOTree"),
	fMCTruthTree(0),
	fRawHeaderTree(0),
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
	fRawHeaderTree(c.fRawHeaderTree),
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
}


void IOTree::RequestTree(TString detectorName, TDetectorVEvent * const evt, TString outputName){
	/// \MemberDescr
	/// \param detectorName : Name of the requested Detector
	/// \param evt : Pointer to an instance of detector event (MC or Reco)
	/// \param outputName : Name of the output type to request (Reco, Hits, Digis, ...)
	///
	/// Request a branch in a tree in the input file. If the tree has already been requested before,
	/// only add the new branch.
	/// If outputName is not specified, the tree "Reco" or "Hits" will be used (depending on the
	/// TDetectorVEvent class instance). If you need a different tree, please specify the name of the tree
	/// (e.g. Digis)
	/// \EndMemberDescr

	std::cout << normal() << "Requesting Detector branch " << detectorName << std::endl;
	eventIterator it;
	std::pair<eventIterator, eventIterator> eventRange;

	//Which branch are we dealing with?
	if(outputName.CompareTo("")==0){
		if(strstr(evt->ClassName(), "Reco")!=NULL) outputName="Reco";
		else if(strstr(evt->ClassName(), "Digi")!=NULL) outputName="Digis";
		else outputName="Hits";
	}

	//Create the tree if not yet requested
	if(fTree.count(outputName)==0){
		cout << debug() << "First request... Creating TTree" << std::endl;
		fTree.insert(chainPair(outputName, new TChain(outputName)));
	}

	cout << normal() << "TTree name set to " << outputName << endl;

	//Is this branch of this tree already requested?
	//If yes delete evt and return (we already have the branching object instance)
	eventRange = fEvent.equal_range(detectorName);
	//Loop over detectors with this name
	for(it=eventRange.first; it!=eventRange.second; ++it){
		//Does it point to this TTree?
		if(it->second->fTreeName.CompareTo(outputName)==0){
			cout << debug() << "Branch already requested... Continue" << std::endl;
			delete evt;
			return;
		}
	}
	fEvent.insert(eventPair(detectorName, new EventTriplet(outputName, evt)));
}

bool IOTree::RequestTree(TString branchName, TString name, TString className, void* const obj){
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

	//Is this branch of this tree already requested?
	//If yes signal the caller that obj shoud be deleted and return (we already have the branching object instance)
	objectRange = fObject.equal_range(branchName);
	//Loop over all objects with this name
	for(it=objectRange.first; it!=objectRange.second; ++it){
		//Does it point to the same tree?
		if(it->second->fTreeName.CompareTo(name)==0){
			cout << debug() << "Branch already requested... Continue" << std::endl;
			return false;
		}
	}
	fObject.insert(objectPair(branchName,new ObjectTriplet(className, name, obj)));
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

	for(it=fTree.begin(); it!=fTree.end(); ++it){
		TObjArray * arr = it->second->GetListOfBranches();
		for(int i=0; i<arr->GetEntries(); ++i){
			TBranch *b = (TBranch*)arr->At(i);
			cout << debug() << "Disabling branch " << b->GetName() << " from tree " << it->second->GetName() << endl;
			b->SetStatus(0);
		}
	}
	//Loop over all detector branches and branch them
	for(ptr1=fEvent.begin(); ptr1!=fEvent.end(); ++ptr1){
		FindAndBranchTree(fTree.find(ptr1->second->fTreeName)->second, ptr1->first, ptr1->second->fEvent->ClassName(), &(ptr1->second->fEvent));
	}

	//Loop over all generic branches and branch them
	for(ptr2=fObject.begin(); ptr2!=fObject.end(); ++ptr2){
		FindAndBranchTree(fTree.find(ptr2->second->fTreeName)->second, ptr2->first, ptr2->second->fClassName, &(ptr2->second->fObject));
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
			std::cout << debug() << "No branch specified, only one found... Using " << eventRange.second->second->fTreeName << std::endl;
			return eventRange.second->second->fEvent;
		}
		it = eventRange.first;;
		for(it=eventRange.first; it!=eventRange.second; ++it){
			//If the branch is not specified but we find Reco or Hits, we return it
			//Or if this is the requested branch also return it
			if(( branchName.CompareTo("")==0 && (
					it->second->fTreeName.CompareTo("Reco")==0 ||
					it->second->fTreeName.CompareTo("Digis")==0 ||
					it->second->fTreeName.CompareTo("Hits")==0))
					|| it->second->fTreeName.CompareTo(branchName)==0){
				std::cout << debug() << "Using branch " << it->second->fTreeName << std::endl;
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
					it->second->fTreeName.CompareTo(branchName)==0){
				std::cout << debug() << "Using branch " << it->second->fTreeName << std::endl;
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
		//Loop over all our trees
		for (it = fTree.begin(); it != fTree.end(); it++) {
			//Loop over all event and object branch and load the corresponding entry for each of them
			for (itEvt = fEvent.begin(); itEvt != fEvent.end(); ++itEvt) {
				if (it->second->GetBranch(itEvt->first)){
					fIOTimeCount.Start();
					it->second->GetBranch(itEvt->first)->GetEntry(iEvent);
					fIOTimeCount.Stop();
				}
			}
			for (itObj = fObject.begin(); itObj != fObject.end(); ++itObj) {
				if (it->second->GetBranch(itObj->first)){
					fIOTimeCount.Start();
					it->second->GetBranch(itObj->first)->GetEntry(iEvent);
					fIOTimeCount.Stop();
				}
			}
		}
		fGraphicalMutex->UnLock();
	}
	return true;
}

Event* IOTree::GetMCTruthEvent(TString treeName){
	/// \MemberDescr
	/// \param treeName: Name of the tree from which MCTruth should be extracted
	/// \return Pointer to the MCTruthEvent
	/// \EndMemberDescr

	if(!fWithMC) return nullptr;
	objectIterator itObj;

	for(itObj = fObject.find("mcEvent"); itObj!=fObject.end(); ++itObj){
		if(itObj->second->fTreeName.CompareTo(treeName)==0) return (Event*)itObj->second->fObject;
	}
	std::cout << normal() << "MCTruth not found in tree " << treeName << std::endl;
	return nullptr;
}

bool IOTree::GetWithMC() const{
	/// \MemberDescr
	/// \return True if the Event Tree is present
	///
	/// Do we have MC available in the files?
	/// \EndMemberDescr

	return fWithMC;
}

RawHeader* IOTree::GetRawHeaderEvent(TString treeName){
	/// \MemberDescr
	/// \param treeName: Name of the tree from which RawHeader should be extracted
	/// \return Pointer to the RawHeader
	/// \EndMemberDescr

	if(!fWithRawHeader) return nullptr;
	objectIterator itObj;

	for(itObj = fObject.find("RawHeader"); itObj!=fObject.end(); ++itObj){
		if(itObj->second->fTreeName.CompareTo(treeName)==0) return (RawHeader*)itObj->second->fObject;
	}
	std::cout << standard() << "RawHeader not found in tree " << treeName << std::endl;
	return nullptr;
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

void IOTree::FindAndBranchTree(TChain* tree, TString branchName, TString branchClass, void* const evt){
	/// \MemberDescr
	/// \param tree :
	/// \param branchName : name of the branch
	/// \param branchClass : name of the branch class
	/// \param evt :
	///
	/// Branch the tree.
	/// \EndMemberDescr

	TObjArray* branchesList;
	Int_t jMax;

	std::cout << debug() << "Trying to branch " << branchName << " of type "
			<< branchClass << std::endl;

	fIOTimeCount.Start();
	std::cout << debug() << "Retrieving list of branches in " << tree->GetName() << " tree" << std::endl;
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
			//Activate branch
			cout << debug() << "Enabling branch " << branchName << " of tree " << tree->GetName() << endl;
			tree->GetBranch(branchName)->SetStatus(1);
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

	fWithMC = false;
	fWithRawHeader = false;
	for(int i=0; i<keys->GetEntries(); i++){
		TKey* k = (TKey*)keys->At(i);
		if(TString(k->GetClassName()).CompareTo("TTree")!=0) continue;
		TTree* tree = (TTree*)fd->Get(k->GetName());

		if(tree->FindBranch("mcEvent")){
			fWithMC = true;
			RequestTree("mcEvent", tree->GetName(), "Event", new Event);
			if(!fMCTruthTree) fMCTruthTree = fTree.find(tree->GetName())->second;
		}

		if(tree->FindBranch("RawHeader")){
			fWithRawHeader = true;
			RequestTree("RawHeader", tree->GetName(), "RawHeader", new RawHeader);
			if(!fRawHeaderTree) fRawHeaderTree = fTree.find(tree->GetName())->second;
		}
	}
	fd->Close();

	if(fWithMC==false){
		std::cout << normal() << "No MC data found" << std::endl;
	}
	if(fWithRawHeader==false){
		std::cout << normal() << "No Raw Header found" << std::endl;
	}

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
