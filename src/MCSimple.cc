#include "MCSimple.hh"

#include <iostream>
#include <map>
#include <list>

#include "KinePart.hh"
#include "functions.hh"
#include "ParticleTree.hh"

namespace NA62Analysis {


MCSimple::MCSimple():
	Verbose("MCSimple"),
	fStatus(kEmpty),
	fParticleInterface(ParticleInterface::GetParticleInterface()),
	fDecayTree(NULL)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

MCSimple::MCSimple(const MCSimple& c):
		fStatus(c.fStatus),
		fParticles(c.fParticles),
		fStruct(c.fStruct),
		fParticleInterface(c.fParticleInterface),
		fDecayTree(c.fDecayTree)
{
	/// \MemberDescr
	/// \param c : Reference of the object to copy
	///
	/// Copy constructor
	/// \EndMemberDescr
}

void MCSimple::GetRealInfos( Event* MCTruthEvent){
	/// \MemberDescr
	/// \param MCTruthEvent : Is the event coming from the TTree. Is extracted in BaseAnalysis
	/// \param verbose : If set to 1, will print the missing particles. If set to 2, will print also the MC particles found in the event.
	/// \todo Possibility to get random event types and just specify the number of generation we want
	///
	/// Extract informations from current Event and store them internally for later easy access
	/// \EndMemberDescr

	multimap<pair<int,int>,int>::iterator it;
	vector<KinePart*> tempPart;
	vector<KinePart*>::iterator itList;
	multimap<pair<int,int>,int> testStruct(fStruct);
	vector<ParticleTree*> tempParticleTree;
	vector<ParticleTree*>::iterator itPTree;
	vector<ParticleTree*>::reverse_iterator ritPTree;
	ParticleTree *pTreeNode;

	ClearParticles();
	//Loop over particles and keep them if correspond to the asked signatures (Parent,PDGcode)
	cout << debug() << MCTruthEvent->GetNKineParts() << " MC particles found" << endl;
	for (Int_t i=0; i < MCTruthEvent->GetNKineParts(); i++)	{
		KinePart* kinePart = (KinePart*)MCTruthEvent->GetKineParts()->At(i);
		cout << debug() << "Found in MCTruth : (ID: " << kinePart->GetID() << ", ParentID: " << kinePart->GetParentID() << ", PDGCode: " << kinePart->GetPDGcode() << endl;

		//Building particle tree.
		//Loop over all the existing trees (first is the main tree)
		pTreeNode = NULL;
		for(itPTree=tempParticleTree.begin(); itPTree!=tempParticleTree.end(); itPTree++){
			pTreeNode = (*itPTree)->GetChildren(kinePart->GetParentID());
			if(pTreeNode!=NULL){
				//Parent found in one of the existing tree. Add this particle as children.
				cout << debug() << "Found parent tree" << endl;
				pTreeNode->AddChildren(new ParticleTree(kinePart));
				break;
			}
		}
		if(pTreeNode==NULL){
			//Parent not found in existing trees. Create a new tree with this particle
			cout << debug() << "Creating new tree" << endl;
			tempParticleTree.push_back(new ParticleTree(kinePart));
		}

		//Test signature
		cout << debug() << "Signature : " << kinePart->GetParentID() << "," <<  kinePart->GetPDGcode() << endl;
		cout << debug() << "Available signatures :" << endl;
		if(TestLevel(Verbosity::kDebug)){
			for(it = testStruct.begin(); it!=testStruct.end(); it++){
				cout << it->first.first << "," << it->first.second << "," << it->second << endl;
			}
		}
		if((it = testStruct.find(pair<int,int>(kinePart->GetParentID(), kinePart->GetPDGcode())))!=testStruct.end()){
			//Signature found. Keep particle and remove from searched signature
			cout << debug() << "Signature found" << endl;
			tempPart.push_back(kinePart);
			ReplaceID(testStruct, (*it).second, kinePart->GetID());
			testStruct.erase(it);
		}
	}

	//Merge temporary trees
	for(ritPTree=tempParticleTree.rbegin(); (ritPTree!=tempParticleTree.rend()) && (tempParticleTree.size()>1);){
		for(itPTree=tempParticleTree.begin(); itPTree!=tempParticleTree.end(); itPTree++){
			pTreeNode = (*itPTree)->GetChildren((*ritPTree)->GetParentID());
			if(pTreeNode!=NULL){
				//Parent found, add the ritPTree as children of itPTree
				(*itPTree)->AddChildren(*ritPTree);
				tempParticleTree.pop_back();
				ritPTree=tempParticleTree.rbegin();
				break;
			}
			else{
				//Parent not found. Should not happen
				cout << normal() << "Unable to build decay tree. Particle with ID " << (*ritPTree)->GetParentID() << endl;
				ritPTree++;
			}
		}
	}

	if(tempParticleTree.size()!=0) fDecayTree = tempParticleTree[0];
	else fDecayTree=NULL;
	tempParticleTree.clear();

	//If testStruct is empty, we found all the particles we wanted. Event is complete.
	//Else, print which particle is missing
	if(testStruct.size()==0) fStatus = kComplete;
	else{
		fStatus = kMissing;
		for(it=testStruct.begin(); it!=testStruct.end(); it++){
			cout << normal() << "Missing particle " << (*it).first.second << " with parent " << (*it).first.first << endl;
		}
	}

	//From the kept particles, populate kaon, pip, pi0 and gamma lists.
	for(itList = tempPart.begin(); itList!=tempPart.end(); itList++){
		KinePart *kinePart = *itList;

		if(kinePart!=NULL){
			//Kaon
			if(fParticles.count(kinePart->GetPDGcode())==0) fParticles[kinePart->GetPDGcode()] = new vector<KinePart*>;
			fParticles[kinePart->GetPDGcode()]->push_back(kinePart);
		}
	}
}

int MCSimple::AddParticle(int parent, int type){
	/// \MemberDescr
	/// \param parent : id of the parent particle. This id is not the MC id found in the root file but the sequence id of the parent particle.
	/// \param type : PDG code of the particle
	/// \return Index of the added particle
	///
	/// Insert a new particle signature. Insertion has to be done generation by generation.\n
	/// The sequence id are inserted with a minus sign to later avoid confusion between sequence id and MC id.\n
	/// \n
	/// Example : you want to retrieve the kaon from the beam, the pi0 an pi+ from the beam kaon and the 2 photons coming from the previous pi0 decay :\n
	/// AddParticle(0, 321) //Ask beam kaon (sequence ID=1)\n
	/// AddParticle(1, 211) //Ask pi+ from previous kaon (sequence ID=2)\n
	/// AddParticle(1, 111) //Ask pi0 from previous kaon (sequence ID=3)\n
	/// AddParticle(3, 22) //Ask first gamma from previous pi0 (sequence ID=4)\n
	/// AddParticle(3, 22) //Ask second gamma from previous pi0 (sequence ID=4)
	/// \EndMemberDescr

	fStruct.insert(pair<pair<int,int>, int>(pair<int,int>(-parent,type), fStruct.size()+1));
	return fStruct.size();
}

void MCSimple::ReplaceID(multimap<pair<int,int>,int> &s, int seqID, int particleID){
	/// \MemberDescr
	/// \param s : structure in which the change is made
	/// \param seqID : sequence id to be replaced
	/// \param particleID : MC id to replace with
	///
	/// Replace the sequence ID by the MC ID in the structure
	/// \EndMemberDescr

	multimap<pair<int,int>,int>::iterator it, it2;
	for(it=s.begin(); it != s.end() && it->first.first<0;){
		if((*it).first.first == -seqID){
			s.insert(pair<pair<int,int>, int>(pair<int,int>(particleID,it->first.second), it->second));
			it2 = it;
			if(it!=s.begin()){
				it++;
				s.erase(it2);
			}
			else{
				s.erase(it2);
				it=s.begin();
			}
		}
		else it++;
	}
}

TString MCSimple::GetParticleName(int pdgID) const {
	/// \MemberDescr
	/// \param pdgID : PDG ID of the particle
	/// \return Name of the particle corresponding to the given PDG ID.
	/// \EndMemberDescr

	TString name;
	name = fParticleInterface->GetParticleName(pdgID);
	if(name.CompareTo("")==0) cout << normal() << "Unknown particle PDG ID : " << pdgID << endl;
	return name;
}

int MCSimple::GetPdgID(TString name) const{
	/// \MemberDescr
	/// \param name : Name of the particle
	/// \return PDG ID of the particle corresponding to the given name.
	/// \EndMemberDescr

	int id;

	id = fParticleInterface->GetParticlePDGid(name);
	if(id==0) cout << normal() << "Unknown particle name : " << name << endl;
	return id;
}

void MCSimple::ClearParticles(){
	/// \MemberDescr
	/// Clear all the output vectors
	/// \EndMemberDescr

	map<int, vector<KinePart*>* >::iterator it;

	for(it=fParticles.begin(); it != fParticles.end(); it++){
		it->second->clear();
	}
	if(fDecayTree) delete fDecayTree;
}

vector<KinePart*> MCSimple::operator [](TString name){
	/// \MemberDescr
	/// \param name : name of the requested particle
	/// \return Corresponding particle vector
	/// \EndMemberDescr

	int pdgID;

	pdgID=GetPdgID(name);
	return *fParticles[pdgID];
}

vector<KinePart*> MCSimple::operator [](int pdgCode){
	/// \MemberDescr
	/// \param pdgCode : PDG id of the requested particle
	/// \return Corresponding particle vector
	/// \EndMemberDescr

	return *fParticles[pdgCode];
}

int MCSimple::Size(int pdgID) const{
	/// \MemberDescr
	/// \param pdgID : pdg ID of the particle
	/// \return Number of particles
	/// \EndMemberDescr

	if(fParticles.count(pdgID)==0) return 0;
	else return fParticles.find(pdgID)->second->size();
}

int MCSimple::Size(TString name) const{
	/// \MemberDescr
	/// \param name : Name of the particle
	/// \return Number of particles
	/// \EndMemberDescr

	return Size(GetPdgID(name));
}

void MCSimple::PrintInit() const{
	/// \MemberDescr
	/// Do some printing of the requested structure
	/// \EndMemberDescr

	map<pair<int,int>, int >::const_reverse_iterator it;

	ParticleTree root;

	for(it=fStruct.rbegin(); it!=fStruct.rend(); it++){
		if(it->first.first==0) root.SetParticleProperties(1, it->first.second, GetParticleName(it->first.second));
		else{
			root.GetChildren(-it->first.first)->AddChildren(new ParticleTree(it->second, it->first.second, GetParticleName(it->first.second)));
		}
	}

	if(fStruct.size()>0){
		cout << "\tRequested MCSimple structure: " << endl << endl;;
		root.PrintHorizontal("\t");
		cout << endl;
	}
}

void MCSimple::PrintDecayTree() const{
	/// \MemberDescr
	/// Print the current decay tree.
	/// \EndMemberDescr

	if(fDecayTree==NULL) return;
	cout << endl;
	fDecayTree->PrintHorizontal("\t");
	cout << endl;
}

vector<KinePart*> MCSimple::GetFinalState(){
	/// \MemberDescr
	/// \return Vector containing a pointer to every KinePart with no daughters (Final State)
	/// \EndMemberDescr

	vector<KinePart*> v;

	fDecayTree->GetFinalState(v);
	return v;
}

vector<KinePart*> MCSimple::GetDecayLevel(int level, bool full){
	/// \MemberDescr
	/// \param level : Level to fetch in the decay tree
	/// \param full : If true, will also return the final particles that did not reach the requested level.
	/// \return Vector containing a pointer to every KinePart at the requested level in the decay tree.
	/// \EndMemberDescr

	vector<KinePart*> v;

	fDecayTree->GetLevel(v, level, full);
	return v;
}

} /* namespace NA62Analysis */
