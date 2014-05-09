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

ParticleTree::ParticleTree():
		fpdgID(0),
		fId(-1),
		fParticle(NULL)
{
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

ParticleTree::ParticleTree(int id, int pdgId, TString name):
		fpdgID(pdgId),
		fId(id),
		fName(name),
		fParticle(NULL)
{
	/// \MemberDescr
	/// Constructor setting particle properties
	/// \EndMemberDescr
}

ParticleTree::ParticleTree(KinePart* const ptr):
		fpdgID(ptr->GetPDGcode()),
		fId(ptr->GetID()),
		fName(ptr->GetParticleName()),
		fParticle(ptr)
{
	/// \MemberDescr
	/// \param ptr : Pointer to the KinePart
	///
	/// Constructor setting particle properties
	/// \EndMemberDescr
}

ParticleTree::ParticleTree(const ParticleTree& c):
		fChildrens(c.fChildrens),
		fpdgID(c.fpdgID),
		fId(c.fId),
		fName(c.fName),
		fParticle(c.fParticle)
{
	/// \MemberDescr
	/// \param ptr : Pointer to the KinePart
	///
	/// Constructor setting particle properties
	/// \EndMemberDescr
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

int ParticleTree::GetID() const{
	/// \MemberDescr
	/// Return the particle id
	/// \EndMemberDescr

	return fId;
}

KinePart* ParticleTree::GetKinePart() const{
	/// \MemberDescr
	/// Return the KinePart pointer
	/// \EndMemberDescr

	return fParticle;
}

int ParticleTree::GetParentID() const{
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
	if(fId==id) return this; //If this is the right particle, return it
	else{
		//Loop over childrens recursively and get the result. While NULL, requested
		//particle is not yet found
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

void ParticleTree::PrintHorizontal(TString prefix, int cellSize, int level) const{
	/// \MemberDescr
	/// Locate and print the next particle in the tree. Can be itself, direct or non direct children.\n
	/// Final result for the tree should look like\n
	/// K+    -> pi+   \n
	///       -> pi0    -> gamma \n
	///                 -> gamma
	/// \EndMemberDescr
	vector<ParticleTree*>::const_iterator it;

	if(level==1) cout << prefix;
	if(fName.Length()!=0) cout << setw(cellSize) << fName;
	else cout << setw(cellSize) << fpdgID;

	if(fChildrens.size()==0) {
		cout << endl;
		return;
	}
	for(it=fChildrens.begin(); it!=fChildrens.end(); ++it){
		if(it!=fChildrens.begin()) cout << prefix << setw((cellSize+3)*level) << " ->";
		else cout << " ->";
		(*it)->PrintHorizontal(prefix, cellSize, level+1);
	}
}

bool ParticleTree::GetFinalState(vector<KinePart*> &array) const{
	/// \MemberDescr
	/// \param array : Address of the array to be filled
	///
	/// Fill the input array with the pointers to the particles in the leaves.
	/// Return false if the pointers are not filled.
	/// \EndMemberDescr

	vector<ParticleTree*>::const_iterator it;
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

bool ParticleTree::GetLevel(vector<KinePart*> &array, int level, bool full) const{
	/// \MemberDescr
	/// \param array : Address of the array to be filled
	/// \param level : Level number below the current node to return
	/// \param full : If true will also return the final particles in the branches thatc stopped before the given level
	///
	/// Fill the input array with the pointers to the particles at the given tree level below the current node.
	/// Return false if the pointers are not filled.
	/// \EndMemberDescr

	vector<ParticleTree*>::const_iterator it;
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
