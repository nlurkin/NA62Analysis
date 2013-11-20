#include "TRecoLKrCandidate.hh"

#ifndef TRECOLKRCANDIDATEMC_
#define TRECOLKRCANDIDATEMC_

class TRecoLKrCandidateMC : public TRecoLKrCandidate
{
public:
	void SetMCParticleID(int id){ fMCParticleID=id;};
	int GetMCPartileID(){ return fMCParticleID;};

	void copy(TRecoLKrCandidate *c);
private:
	int fMCParticleID;
	ClassDef(TRecoLKrCandidateMC,1);
};

#endif
