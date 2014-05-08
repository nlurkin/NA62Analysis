/*
 * ParticleTree.hh
 *
 *  Created on: Jun 11, 2013
 *      Author: nlurkin
 */

#include <vector>
#include <TString.h>
#include "KinePart.hh"

using namespace std;

#ifndef PARTICLETREE_HH_
#define PARTICLETREE_HH_

/// \class ParticleTree
/// \Brief 
/// Class representing particle decay chain for on-screen printing.
/// \EndBrief 
///
/// \Detailed
/// Implementation of a node class for a tree. Each node can have multiple children.
/// The node represents a particle. When the tree is traversed for printing, the
/// result is an horizontal particle decay tree with the decay generations are aligned
/// on columns.
/// \EndDetailed

class ParticleTree {
public:
	/// Current state of the node
	enum state {kFirst, kChildren, kEmpty};

	ParticleTree();
	ParticleTree(int id, int pdgID, TString name);
	ParticleTree(KinePart *ptr);
	~ParticleTree();

	void SetParticleProperties(int id, int pdgID, TString name);
	int GetID();
	KinePart *GetKinePart();
	int GetParentID();

	void AddChildren(ParticleTree *child);
	ParticleTree *GetChildren(int id);
	bool GetFinalState(vector<KinePart*> &array);
	bool GetLevel(vector<KinePart*> &array, int level, bool full);

	ParticleTree *operator[](unsigned int i);
	state PrintNext();
	void ResetPrint();

private:
	vector<ParticleTree*> fChildrens; ///< Children nodes
	int fpdgID; ///< PDG id of the particle
	int fId; ///< Sequence id of the particle
	TString fName; ///< Name of the particle
	KinePart *fParticle; ///< Pointer to kinePart

	unsigned int fGiven; ///< Internal mess
	unsigned int fPrevGiven; ///< Internal mess
};

#endif /* PARTICLETREE_HH_ */
