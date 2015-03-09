/*
 * IOTree.cc
 *
 *  Created on: Mar 9, 2015
 *      Author: ncl
 */

#include "IOTree.hh"
#include <TChain.h>
#include <iostream>
#include <signal.h>

IOTree::IOTree():
	fMCTruthTree(0),
	fMCTruthEvent(new Event()),
	fRawHeaderTree(0),
	fRawHeaderEvent(new RawHeader()),
	fWithMC(false),
	fWithRawHeader(false),
	allowNonExisting(false)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

IOTree::IOTree(const IOTree &c):
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
	allowNonExisting(false)
{
	/// \MemberDescr
	/// Copy constructor
	/// \EndMemberDescr
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

	eventIterator it;
	std::pair<eventIterator, eventIterator> eventRange;

	//Create the tree if not yet requested
	if(fTree.count(name)==0){
		fTree.insert(chainPair(name, new TChain(name)));
	}

	//Which branch are we dealing with?
	if(branchName.CompareTo("")==0){
		if(strstr(evt->ClassName(), "Reco")!=NULL) branchName="Reco";
		else if(strstr(evt->ClassName(), "Digi")!=NULL) branchName="Digis";
		else branchName="Hits";
	}

	//Is this branch already requested?
	//If yes delete evt and return (we already have the branching object instance)
	eventRange = fEvent.equal_range(name);
	for(it=eventRange.first; it!=eventRange.second; ++it){
		if(it->second->fBranchName.CompareTo(branchName)==0){
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
	///
	/// Request a tree in the input file. If already requested before, do nothing.
	/// \EndMemberDescr

	objectIterator it;
	std::pair<objectIterator, objectIterator> objectRange;

	//Create the tree if not yet requested
	if(fTree.count(name)==0){
		fTree.insert(chainPair(name, new TChain(name)));
	}

	//Is this branch already requested?
	//If yes signal the caller that obj shoud be deleted and return (we already have the branching object instance)
	objectRange = fObject.equal_range(name);
	for(it=objectRange.first; it!=objectRange.second; ++it){
		if(it->second->fBranchName.CompareTo(branchName)==0){
			return false;
		}
	}
	fObject.insert(objectPair(name,new ObjectTriplet(className, branchName, obj)));
	return true;
}

int IOTree::BranchTrees(int eventNb){
	/// \MemberDescr
	///	\param eventNb : Number of events that should be read in the tree
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
	///
	/// Return the pointer to the event corresponding to the given tree and the given branch.
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
		std::cerr << "Error: Unable to find event in tree " << name << std::endl;
		return NULL;
	}
	else{
		//No branchName specified but only a single branch has been requested in this tree. Must be the one...
		it = eventRange.first;
		if(branchName.CompareTo("")==0 && (++it==eventRange.first)){
			return eventRange.second->second->fEvent;
		}
		it = eventRange.first;;
		for(it=eventRange.first; it!=eventRange.second; ++it){
			//If the branch is not specified but we find Reco or Hits, we return it
			//Or if this is the requested branchm also return it
			if(( branchName.CompareTo("")==0 && (
					it->second->fBranchName.CompareTo("Reco")==0 ||
					it->second->fBranchName.CompareTo("Digis")==0 ||
					it->second->fBranchName.CompareTo("Hits")==0))
					|| it->second->fBranchName.CompareTo(branchName)==0){
				return it->second->fEvent;
			}
		}
	}
	std::cerr << "Error: Unable to find event in branch " << branchName << " in tree " << name << std::endl;
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
		std::cerr << "Error: Unable to find object in tree " << name << std::endl;
		return NULL;
	}
	else{
		for(it=objectRange.first; it!=objectRange.second; ++it){
			// If the branch is not specified or this is the requested one,
			// return it
			if( branchName.CompareTo("")==0 ||
					it->second->fBranchName.CompareTo(branchName)==0){
				return it->second->fObject;
			}
		}
	}
	std::cerr << "Error: Unable to find object in branch " << branchName << " in tree " << name << std::endl;
	return NULL;
}

void IOTree::LoadEvent(int iEvent){
	/// \MemberDescr
	/// \param iEvent : Index of the event
	///
	/// Load the event from the TTrees
	/// \EndMemberDescr

	treeIterator it;
	eventIterator itEvt;
	objectIterator itObj;

	std::pair<eventIterator, eventIterator> eventRange;
	std::pair<objectIterator, objectIterator> objectRange;

	if(fWithMC) fMCTruthTree->GetEntry(iEvent);
	if(fWithRawHeader) fRawHeaderTree->GetEntry(iEvent);

	//Loop over all our trees
	for(it=fTree.begin(); it!=fTree.end(); it++){
		//Loop over all event and object branch and load the corresponding entry for each of them
		eventRange = fEvent.equal_range(it->first);
		for(itEvt=eventRange.first; itEvt!=eventRange.second; ++itEvt){
			if(it->second->GetBranch(itEvt->second->fBranchName))
				it->second->GetEntry(iEvent);
		}
		objectRange = fObject.equal_range(it->first);
		for(itObj=objectRange.first; itObj!=objectRange.second; ++itObj){
			if(it->second->GetBranch(itObj->second->fBranchName))
				it->second->GetEntry(iEvent);
		}
	}

}

Event* IOTree::GetMCTruthEvent(){
	/// \MemberDescr
	///
	/// Return a pointer to the MCTruthEvent
	/// \EndMemberDescr

	return fMCTruthEvent;
}

bool IOTree::GetWithMC() const{
	/// \MemberDescr
	///
	/// Do we have MC available in the files?
	/// \EndMemberDescr

	return fWithMC;
}

RawHeader* IOTree::GetRawHeaderEvent(){
	/// \MemberDescr
	///
	/// Return a pointer to the RawHeader
	/// \EndMemberDescr

	return fRawHeaderEvent;
}

bool IOTree::GetWithRawHeader() const{
	/// \MemberDescr
	///
	/// Do we have RawHeader available in the files?
	/// \EndMemberDescr

	return fWithRawHeader;
}

TChain* IOTree::GetTree(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TTree to retrieve
	///
	///	Return a pointer to the specified TTree
	/// \EndMemberDescr
	treeIterator it;

	if((it=fTree.find(name))!= fTree.end()){
		return it->second;
	}
	else{
		std::cerr << "The requested TTree pointer " << name << " cannot be found" << std::endl;
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

	branchesList = tree->GetListOfBranches();
	if(!branchesList){
		if(! allowNonExisting){
			std::cerr << "Unable to find TTree " << tree->GetName() << ". Aborting.";
			raise(SIGABRT);
		}
		else{
			std::cerr << "Warning: unable to find TTree " << tree->GetName() << ". Retrieved corresponding event will always be empty";
			return;
		}
	}
	jMax = branchesList->GetEntries();
	for (Int_t j=0; j < jMax; j++){
		if ( TString(branchName).CompareTo( branchesList->At(j)->GetName() ) == 0 )
		{
			if ( TString(branchClass).CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				std::cerr << "Input file corrupted, bad Event class (" << ((TBranch*)branchesList->At(j))->GetClassName() << ") found for " << tree->GetTree()->GetName() << std::endl;
				raise(SIGABRT);
			}
			std::cout << "Found " << branchName << " (" << tree->GetEntries() << ") of class " << branchClass << std::endl;
			tree->SetBranchAddress(branchName, evt);
			if ( eventNb <= 0 )
			{
				eventNb = tree->GetEntries();
			}
			else if (eventNb != tree->GetEntries())
			{
				std::cout << eventNb << std::endl;
				std::cerr << "Input file corrupted, bad number of entries (run) : " << tree->GetEntries() << std::endl;
				raise(SIGABRT);
			}
			return;
		}
	}
	std::cerr << "Unable to find branch " << branchName << " in TTree " << tree->GetName() << std::endl;
}

int IOTree::FillMCTruth(AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// Branch the MC trees. Name is different if the input file comes from the MC or Reconstruction.
	/// \EndMemberDescr

	//TODO find another way to fill fEventNb
	int eventNb = -1;
	if(!fWithMC) return eventNb;

	eventNb = fMCTruthTree->GetEntries();
	if(eventNb==0){
		fWithMC = false;
		return -1;
	}

	TObjArray* branchesList = fMCTruthTree->GetListOfBranches();
	int jMax = branchesList->GetEntries();
	TString branchName = "";

	for (Int_t j=0; j < jMax; j++)
	{
		if(verbosity >= AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: BranchName " <<  branchesList->At(j)->GetName() << std::endl;
		if ( TString("event").CompareTo( branchesList->At(j)->GetName() ) == 0){
			branchName = "event";
		}
		else if(TString("mcEvent").CompareTo( branchesList->At(j)->GetName() ) == 0 ){
			branchName = "mcEvent";
		}
		if(branchName.CompareTo("") != 0)
		{
			if(verbosity >= AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: ClassName " << ((TBranch*)branchesList->At(j))->GetClassName() << std::endl;
			if ( TString("Event").CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				std::cerr  << "Input file corrupted, bad reco class found for " << fMCTruthTree->GetTree()->GetName() << std::endl;
			}
			else{
				if(verbosity>=AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: Found TRecoMCTruthEvent (" << fMCTruthTree->GetEntries() << ")" << std::endl;
				fMCTruthTree->SetBranchAddress(branchName, &fMCTruthEvent );
				if ( eventNb < 0 )
				{
					eventNb = fMCTruthTree->GetEntries();
				}
				else if (eventNb != fMCTruthTree->GetEntries())
				{
					std::cerr << "Input file corrupted, bad number of entries : " << fMCTruthTree->GetEntries() << std::endl;
				}
			}
		}
	}
	return eventNb;
}

int IOTree::FillRawHeader(AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// Branch the RawHeader trees.
	/// \EndMemberDescr

	//TODO find another way to fill fEventNb
	int eventNb = -1;
	if(!fWithRawHeader) return eventNb;

	eventNb = fRawHeaderTree->GetEntries();
	if(eventNb==0){
		fWithRawHeader = false;
		return -1;
	}

	TObjArray* branchesList = fRawHeaderTree->GetListOfBranches();
	int jMax = branchesList->GetEntries();
	TString branchName = "";

	for (Int_t j=0; j < jMax; j++)
	{
		if(verbosity >= AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: BranchName " <<  branchesList->At(j)->GetName() << std::endl;
		if ( TString("RawHeader").CompareTo( branchesList->At(j)->GetName() ) == 0){
			branchName = "RawHeader";
		}
		if(branchName.CompareTo("") != 0)
		{
			if(verbosity >= AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: ClassName " << ((TBranch*)branchesList->At(j))->GetClassName() << std::endl;
			if ( TString("RawHeader").CompareTo( ((TBranch*)branchesList->At(j))->GetClassName() ) != 0 )
			{
				std::cerr  << "Input file corrupted, bad RawHeader class found for " << fRawHeaderTree->GetTree()->GetName() << std::endl;
			}
			else{
				if(verbosity>=AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: Found Rawheader (" << fRawHeaderTree->GetEntries() << ")" << std::endl;
				fRawHeaderTree->SetBranchAddress(branchName, &fRawHeaderEvent );
				if ( eventNb < 0 )
				{
					eventNb = fRawHeaderTree->GetEntries();
				}
				else if (eventNb != fRawHeaderTree->GetEntries())
				{
					std::cerr << "Input file corrupted, bad number of entries : " << fRawHeaderTree->GetEntries() << std::endl;
				}
			}
		}
	}
	return eventNb;
}


void IOTree::SetAllowNonExisting(bool allowNonExisting) {
	/// \MemberDescr
	/// allowNonExisting : if false, exit if one or several TTree is missing in the input file
	///
	/// Determine if the framework is allowed to run when one or several TTrees
	/// are missing in the input file.
	/// \EndMemberDescr
	this->allowNonExisting = allowNonExisting;
}


bool IOTree::OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// \param inFileName : Path to the input file
	/// \param nFiles : Number of files to open
	/// \param verbosity : verbosity level
	///
	/// Open and register the input files.
	/// \EndMemberDescr

	if(!IOHandler::OpenInput(inFileName, nFiles, verbosity)) return false;

	treeIterator it;
	bool inputChecked = false;

	for(auto fileName : fInputfiles){
		if(!inputChecked && checkInputFile(fileName, verbosity))
			inputChecked = true;
		if(fWithMC) fMCTruthTree->AddFile(fileName);
		if(fWithRawHeader) fRawHeaderTree->AddFile(fileName);
		for(it=fTree.begin(); it!=fTree.end(); it++) it->second->AddFile(fileName);
	}
	return inputChecked;
}

bool IOTree::checkInputFile(TString fileName, AnalysisFW::VerbosityLevel verbosity){
	/// \MemberDescr
	/// \param fileName : Name of the file to open
	///	\param verbosity : Verbosity level
	///
	/// Open the input file to check if MC are present and if yes, what's the name of the TTree
	/// \EndMemberDescr

	TFile *fd = TFile::Open(fileName.Data(), "R");

	if(!fd)
		return kFALSE;

	TList* keys = fd->GetListOfKeys();

	fWithMC = true;
	if(keys->FindObject("Generated")) fMCTruthTree = new TChain("Generated");
	else if(keys->FindObject("Run_0")) fMCTruthTree = new TChain("Run_0");
	else if(keys->FindObject("mcEvent")) fMCTruthTree = new TChain("mcEvent");
	else{
		if(verbosity>=AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: No MC data found" << std::endl;
		fWithMC = false;
	}

	fWithRawHeader = true;
	if(keys->FindObject("RawHeader")) fRawHeaderTree = new TChain("RawHeader");
	else{
		if(verbosity>=AnalysisFW::kSomeLevel) std::cout << "AnalysisFW: No Raw Header found" << std::endl;
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
	AnalysisFW::NA62Map<TString,TTree*>::type::iterator itTree;

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

	AnalysisFW::NA62Map<TString,TTree*>::type::const_iterator itTree;

	for(itTree=fExportTrees.begin(); itTree!=fExportTrees.end(); itTree++){
		itTree->second->Write();
	}
}

void IOTree::PrintInitSummary() const{
	/// \MemberDescr
	///
	/// Print the summary after initialization
	/// \EndMemberDescr

	IOHandler::PrintInitSummary();

	AnalysisFW::NA62Map<TString,TChain*>::type::const_iterator itTree;
	StringBalancedTable treeTable("List of requested TTrees");

	for(itTree=fTree.begin(); itTree!=fTree.end(); itTree++){
		treeTable << itTree->first;
	}

	treeTable.Print("\t");
}

bool IOTree::CheckNewFileOpened(){
	/// \MemberDescr
	/// Method called by TChain when opening a new file.\n
	/// It will signal a new burst to the analyzers
	/// \EndMemberDescr

	int openedFileNumber;

	if(fWithMC){
		openedFileNumber = fMCTruthTree->GetTreeNumber();
		fCurrentFile = fMCTruthTree->GetFile();
	}
	else if(fWithRawHeader){
		openedFileNumber = fRawHeaderTree->GetTreeNumber();
		fCurrentFile = fRawHeaderTree->GetFile();
	}
	else if(fTree.size()>0){
		openedFileNumber = fTree.begin()->second->GetTreeNumber();
		fCurrentFile = fTree.begin()->second->GetFile();
	}
	else return false;

	if(openedFileNumber>fCurrentFileNumber){
		fCurrentFileNumber = openedFileNumber;
		IOHandler::NewFileOpened();
		return true;
	}
	return false;
}
