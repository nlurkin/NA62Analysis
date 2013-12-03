#ifndef MCSIMPLE
#define MCSIMPLE

#include "TLorentzVector.h"
#include "KinePart.hh"
#include "Event.hh"
#include <map>
#include <vector>
#include "functions.hh"
#include "FWEnums.hh"
#include "ParticleInterface.hh"
#include "ParticleTree.hh"
using namespace std;

/// \class MCSimple
/// \Brief 
/// Class gathering and containing MC Event
/// \EndBrief 
///
/// \Detailed
/// Class gathering and containing MC Event
/// \EndDetailed

class MCSimple
{
public:
	enum MCSimpleStatus {kEmpty, kComplete, kMissing}; ///< Possible status for MCSimple instance. Self-explanatory

	MCSimple();
	~MCSimple() {};

	//Filling from TTree
	void GetRealInfos( Event* MCTruthEvent, AnalysisFW::VerbosityLevel verbose);

	//Defining MCSimple
	int AddParticle(int parent, int type);

	//Accessing elements
	vector<KinePart*> operator[](TString);
	vector<KinePart*> operator[](int);
	int Size(int pdgID);
	int Size(TString name);
	vector<KinePart*> GetFinalState();
	vector<KinePart*> GetDecayLevel(int level, bool full);

	//Printing
	void PrintInit();
	void PrintDecayTree();

	MCSimpleStatus fStatus; ///< Status after collecting particles.
private:
	TString GetParticleName(int pdgID);
	int GetPdgID(TString name);
	void ClearParticles();
	void ReplaceID(multimap<pair<int,int>, int> &s, int genID, int particleID);

	map<int, vector<KinePart*>* > fParticles; ///< Container for particle vectors
	multimap<pair<int,int>, int> fStruct; ///< Structure describing the particle we wand to collect
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity level
	ParticleInterface *fParticleInterface; ///< Pointer to the particle interface

	ParticleTree *fDecayTree; ///< Sorting of the particles in tree
};

#endif
