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
}

void ParticleTree::SetParticleProperties(int id, int pdgID, TString name){
	/// \MemberDescr
	/// Sets the particle properties
	/// \EndMemberDescr

	fId = id;
	fpdgID = pdgID;
	fName = name;
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
