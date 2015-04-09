/*
 * G4ParticleInterface.h
 *
 *  Created on: Aug 13, 2013
 *      Author: nlurkin
 */

#ifndef PARTICLEINTERFACE_H_
#define PARTICLEINTERFACE_H_

#include <TString.h>
#include <TDatabasePDG.h>

namespace NA62Analysis {

/// \class ParticleInterface
/// \Brief 
/// Interface class to Root particle database
/// \EndBrief 
///
/// \Detailed
/// Use it to get particle properties from ROOT.
/// \EndDetailed

class ParticleInterface {
public:
	virtual ~ParticleInterface();
	static ParticleInterface* GetParticleInterface();

	const TParticlePDG* FindParticle(int pdgID) const;
	const TParticlePDG* FindParticle(TString name) const;

	int GetParticlePDGid(TString name) const;
	TString GetParticleName(int pdgID) const;

	void PrintTable() const;
private:
	ParticleInterface();
	ParticleInterface(const ParticleInterface&); ///< Non implemented copy-constructor (prevents copy)
	ParticleInterface& operator=(const ParticleInterface&); ///< Non implemented copy-assignment operator (prevents copy)

	TDatabasePDG *fTable; ///< Pointer to TDatabasePDG instance
	static ParticleInterface *fParticleInterface; ///< static pointer to unique instance of the class
};

} /* namespace NA62Analysis */

#endif /* G4PARTICLE_H_ */
