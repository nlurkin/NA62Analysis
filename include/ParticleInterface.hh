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

	const TParticlePDG* const FindParticle(int pdgID) const;
	const TParticlePDG* const FindParticle(TString name) const;

	int GetParticlePDGid(TString name) const;
	TString GetParticleName(int pdgID) const;

	void PrintTable() const;
private:
	ParticleInterface();
	ParticleInterface(const ParticleInterface&);
	ParticleInterface& operator=(const ParticleInterface&);

	TDatabasePDG *fTable; ///< Pointer to TDatabasePDG instance
	static ParticleInterface *fParticleInterface; ///< static pointer to unique instance of the class
};

#endif /* G4PARTICLE_H_ */
