/*
 * ParticleTree.hh
 *
 *  Created on: Jun 11, 2013
 *      Author: nlurkin
 */

#include <vector>
#include <TString.h>

using namespace std;

#ifndef PARTICLETREE_HH_
#define PARTICLETREE_HH_

/// \class ParticleTree
/// \Brief 
/// Class representing particle decay chain for on-screen printing.
/// \EndBrief 
///
/// \Detailed
/// Class representing particle decay chain for on-screen printing.
/// \EndDetailed

class ParticleTree {
public:
	enum state {kFirst, kChildren, kEmpty}; ///< Current state of the node

	ParticleTree();
	ParticleTree(int id, int pdgID, TString name);
	virtual ~ParticleTree();

	void SetParticleProperties(int id, int pdgID, TString name);

	void AddChildren(ParticleTree *child);
	ParticleTree *GetChildren(int id);

	ParticleTree *operator[](unsigned int i);
	state PrintNext();

private:
	vector<ParticleTree*> fChildrens; ///< Children nodes
	int fpdgID; ///< PDG id of the particle
	int fId; ///< Sequence id of the particle
	TString fName; ///< Name of the particle

	unsigned int fGiven, fPrevGiven; ///< Internal mess
};

#endif /* PARTICLETREE_HH_ */
