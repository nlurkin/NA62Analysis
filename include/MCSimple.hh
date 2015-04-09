#ifndef MCSIMPLE
#define MCSIMPLE

#include <map>
#include <vector>

#include <TLorentzVector.h>

#include "KinePart.hh"
#include "Event.hh"

#include "functions.hh"
#include "Verbose.hh"
#include "ParticleInterface.hh"
#include "ParticleTree.hh"

namespace NA62Analysis {

/// \class MCSimple
/// \Brief 
/// Class gathering and containing MC Event
/// \EndBrief 
///
/// \Detailed
/// The user defines a minimal event signature (example: one K+ decaying to pi+, pi0 and
/// the pi0 decaying to two photons, plus anything else at any generation if any). This class
/// scans the MC Event to detect this signature and if found will internally store the
/// corresponding particles. The user can then easily access them without needing to
/// search himself through the generated MC particles.
/// \EndDetailed

class MCSimple : public Verbose
{
public:
	/// Possible status for MCSimple instance. Self-explanatory
	enum MCSimpleStatus {kEmpty, kComplete, kMissing};

	MCSimple();
	MCSimple(const MCSimple& c);
	~MCSimple() {};

	//Filling from TTree
	void GetRealInfos( Event* MCTruthEvent);

	//Defining MCSimple
	int AddParticle(int parent, int type);

	//Accessing elements
	vector<KinePart*> operator[](TString);
	vector<KinePart*> operator[](int);
	int Size(int pdgID) const;
	int Size(TString name) const;
	vector<KinePart*> GetFinalState();
	vector<KinePart*> GetDecayLevel(int level, bool full);

	//Printing
	void PrintInit() const;
	void PrintDecayTree() const;

	MCSimpleStatus fStatus; ///< Status after collecting particles.
private:
	TString GetParticleName(int pdgID) const;
	int GetPdgID(TString name) const;
	void ClearParticles();
	void ReplaceID(std::multimap<std::pair<int,int>, int> &s, int seqID, int particleID);

	std::map<int, std::vector<KinePart*>* > fParticles; ///< Container for particle vectors
	std::multimap<std::pair<int,int>, int> fStruct; ///< Structure describing the particle we wand to collect
	const ParticleInterface* const fParticleInterface; ///< Pointer to the particle interface

	ParticleTree *fDecayTree; ///< Sorting of the particles in tree
};

} /* namespace NA62Analysis */

#endif
