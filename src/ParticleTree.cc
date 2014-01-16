/*
 * ParticleTree.cc
 *
 * Created on: Jun 11, 2013
 * Author: nlurkin
 */

#include "ParticleTree.hh"
#include <iostream>
#include <iomanip>
using namespace std;

ParticleTree::ParticleTree(){
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr

	fId = -1;
	fPrevGiven = 0;
	fGiven = 0;
	fpdgID = 0;
	fParticle = NULL;
}

ParticleTree::ParticleTree(int id, int pdgId, TString name) {
	/// \MemberDescr
	/// Constructor setting particle properties
	/// \EndMemberDescr

	fPrevGiven = 0;
	fGiven = 0;
	fId = id;
	fpdgID = pdgId;
	fName = name;
	fParticle = NULL;
}

ParticleTree::ParticleTree(KinePart *ptr) {
	/// \MemberDescr
	/// \param ptr : Pointer to the KinePart
	///
	/// Constructor setting particle properties
	/// \EndMemberDescr

	fPrevGiven = 0;
	fGiven = 0;
	fParticle = ptr;
	fId = fParticle->GetID();
	fpdgID = fParticle->GetPDGcode();
	fName = fParticle->GetParticleName();
}

void ParticleTree::SetParticleProperties(int id, int pdgID, TString name){
	/// \MemberDescr
	/// \param id : ID of the particle
	/// \param pdgID : PDG code of the particle
	/// \param name : text name of the particle
	///
	/// Sets the particle properties
	/// \EndMemberDescr

	fId = id;
	fpdgID = pdgID;
	fName = name;
}

int ParticleTree::GetID(){
	/// \MemberDescr
	/// Return the particle id
	/// \EndMemberDescr

	return fId;
}

KinePart *ParticleTree::GetKinePart(){
	/// \MemberDescr
	/// Return the KinePart pointer
	/// \EndMemberDescr

	return fParticle;
}

int ParticleTree::GetParentID(){
	/// \MemberDescr
	/// Return the parent id of the particle if KinePart is set
	/// \EndMemberDescr

	if(fParticle!=NULL) return fParticle->GetParentID();
	else return -1;
}

ParticleTree::~ParticleTree() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	vector<ParticleTree*>::iterator it;

	for(it=fChildrens.begin(); it!=fChildrens.end(); it++){
		delete (*it);
	}
	fChildrens.clear();
}

void ParticleTree::AddChildren(ParticleTree *child){
	/// \MemberDescr
	/// \param child : children to add
	///
	/// Add a children tree to the node
	/// \EndMemberDescr

	fChildrens.push_back(child);
}

ParticleTree *ParticleTree::GetChildren(int id){
	/// \MemberDescr
	/// \param id : id of the particle to be found and returned
	///
	/// Return particle with the specified id. Can be itself, direct or non-direct children
	/// \EndMemberDescr

	vector<ParticleTree*>::iterator it;
	ParticleTree *r = NULL;
	if(fId==id) return this;
	else{
		for(it=fChildrens.begin(); it!=fChildrens.end() && r==NULL; it++){
			r = (*it)->GetChildren(id);
		}
		return r;
	}
}

ParticleTree *ParticleTree::operator [](unsigned int i){
	/// \MemberDescr
	/// \param i : number of the children to be returned
	///
	/// Access operator. Returns the children i.
	/// \EndMemberDescr

	if(fChildrens.size()<i) return fChildrens[i];
	else return NULL;
}

ParticleTree::state ParticleTree::PrintNext(){
	/// \MemberDescr
	/// Locate and print the next particle in the tree. Can be itself, direct or non direct children.
	/// \EndMemberDescr

	int r=kEmpty;
	int spaceSize = 10;

	if(fGiven==0){
		fGiven++;
		if(fName.Length()!=0) cout << setw(spaceSize) << fName;
		else cout << setw(spaceSize) << fpdgID;
		if(fChildrens.size()>0){
			cout << " ->";
			r = fChildrens[fGiven-1]->PrintNext();
		}
		else{
			cout << endl;
			return kEmpty;
		}
	}
	else if(fGiven<=fChildrens.size()){
		if(fPrevGiven!=fGiven) cout << setw(spaceSize+3) << " ->";
		else cout << setw(spaceSize+3) << "";
		r = fChildrens[fGiven-1]->PrintNext();
	}

	fPrevGiven = fGiven;
	if(r==kEmpty) fGiven++;

	if(fGiven>fChildrens.size()) return kEmpty;
	else return kChildren;
}

void ParticleTree::ResetPrint(){
	/// \MemberDescr
	/// Reset the printing state.
	/// \EndMemberDescr

	vector<ParticleTree*>::iterator it;

	fGiven = 0;
	fPrevGiven = 0;

	for(it=fChildrens.begin(); it!=fChildrens.end(); it++){
		(*it)->ResetPrint();
	}
}

bool ParticleTree::GetFinalState(vector<KinePart*> &array){
	/// \MemberDescr
	/// \param array : Address of the array to be filled
	///
	/// Fill the input array with the pointers to the particles in the leaves.
	/// Return false if the pointers are not filled.
	/// \EndMemberDescr

	vector<ParticleTree*>::iterator it;
	vector<KinePart*> tempVector;

	if(!fParticle) return false;

	if(fChildrens.size()==0){
		array.push_back(fParticle);
	}
	else{
		for(it=fChildrens.begin(); it!=fChildrens.end(); it++){
			(*it)->GetFinalState(array);
		}
	}
	return true;
}

bool ParticleTree::GetLevel(vector<KinePart*> &array, int level, bool full){
	/// \MemberDescr
	/// \param array : Address of the array to be filled
	/// \param level : Level number below the current node to return
	/// \param full : If true will also return the final particles in the branches thatc stopped before the given level
	///
	/// Fill the input array with the pointers to the particles at the given tree level below the current node.
	/// Return false if the pointers are not filled.
	/// \EndMemberDescr

	vector<ParticleTree*>::iterator it;
	vector<KinePart*> tempVector;

	if(!fParticle) return false;

	if(level==0) array.push_back(fParticle); //Level reached
	else if(level>0){ //Level not yet reached
		if(fChildrens.size()==0){ //No children. This branch died too early
			if(full) array.push_back(fParticle); // We return the leave
			else return true; //We don't return the leave
		}
		else{
			for(it=fChildrens.begin(); it!=fChildrens.end(); it++){
				(*it)->GetLevel(array, level-1, full);
			}
		}
	}
	return true;
}
