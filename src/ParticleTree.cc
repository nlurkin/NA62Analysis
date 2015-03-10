/*
 * ParticleTree.cc
 *
 * Created on: Jun 11, 2013
 * Author: nlurkin
 */

#include "ParticleTree.hh"
#include <iostream>
#include <iomanip>

ParticleTree::ParticleTree():
		fpdgID(0),
		fId(-1),
		fParticle(NULL)
{
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

ParticleTree::ParticleTree(int id, int pdgID, TString name):
		fpdgID(pdgID),
		fId(id),
		fName(name),
		fParticle(NULL)
{
	/// \MemberDescr
	/// \param id : Particle index
	/// \param pdgID : PDG Id of the particle
	/// \param name : Text name of the particle
	///
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
	/// \param c : Reference object to copy
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
	/// \return Particle id
	/// \EndMemberDescr

	return fId;
}

KinePart* ParticleTree::GetKinePart() const{
	/// \MemberDescr
	/// \return KinePart pointer
	/// \EndMemberDescr

	return fParticle;
}

int ParticleTree::GetParentID() const{
	/// \MemberDescr
	/// \return Parent id of the particle if KinePart is set
	/// \EndMemberDescr

	if(fParticle!=NULL) return fParticle->GetParentID();
	else return -1;
}

ParticleTree::~ParticleTree() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	std::vector<ParticleTree*>::iterator it;

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
	/// \return Particle with the specified id.
	///
	/// Can be itself, direct or non-direct children
	/// \EndMemberDescr

	std::vector<ParticleTree*>::iterator it;
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
	/// \returns Child i.
	///
	/// Access operator.
	/// \EndMemberDescr

	if(fChildrens.size()<i) return fChildrens[i];
	else return NULL;
}

void ParticleTree::PrintHorizontal(TString prefix, int cellSize, int level) const{
	/// \MemberDescr
	/// \param prefix : Prefix for each line
	/// \param cellSize : Size of each text cell
	/// \param level : Current printing level
	///
	/// Locate and print the next particle in the tree. Can be itself, direct or non direct children.\n
	/// Final result for the tree should look like\n
	/// K+    -> pi+   \n
	///       -> pi0    -> gamma \n
	///                 -> gamma
	/// \EndMemberDescr
	std::vector<ParticleTree*>::const_iterator it;

	if(level==1) std::cout << prefix;
	if(fName.Length()!=0) std::cout << std::setw(cellSize) << fName;
	else std::cout << std::setw(cellSize) << fpdgID;

	if(fChildrens.size()==0) {
		std::cout << std::endl;
		return;
	}
	for(it=fChildrens.begin(); it!=fChildrens.end(); ++it){
		if(it!=fChildrens.begin()) std::cout << prefix << std::setw((cellSize+3)*level) << " ->";
		else std::cout << " ->";
		(*it)->PrintHorizontal(prefix, cellSize, level+1);
	}
}

bool ParticleTree::GetFinalState(std::vector<KinePart*> &array) const{
	/// \MemberDescr
	/// \param array : Address of the array to be filled
	/// \return false if the pointers are not filled.
	///
	/// Fill the input array with the pointers to the particles in the leaves.
	/// \EndMemberDescr

	std::vector<ParticleTree*>::const_iterator it;
	std::vector<KinePart*> tempVector;

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

bool ParticleTree::GetLevel(std::vector<KinePart*> &array, int level, bool full) const{
	/// \MemberDescr
	/// \param array : Address of the array to be filled
	/// \param level : Level number below the current node to return
	/// \param full : If true will also return the final particles in the branches thatc stopped before the given level
	/// \return false if the pointers are not filled.
	///
	/// Fill the input array with the pointers to the particles at the given tree level below the current node.
	/// \EndMemberDescr

	std::vector<ParticleTree*>::const_iterator it;
	std::vector<KinePart*> tempVector;

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
