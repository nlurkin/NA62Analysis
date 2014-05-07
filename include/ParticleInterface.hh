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
/// Interface class to Root particle database
/// \EndDetailed

class ParticleInterface {
public:
	virtual ~ParticleInterface();
	static ParticleInterface* GetParticleInterface();

	TParticlePDG *FindParticle(int pdgID);
	TParticlePDG *FindParticle(TString name);

	int GetParticlePDGid(TString name);
	TString GetParticleName(int pdgID);

	void PrintTable();
private:
	ParticleInterface();

	TDatabasePDG *fTable; ///< Pointer to TDatabasePDG instance
	static ParticleInterface *fParticleInterface; ///< static pointer to unique instance of the class
};

#endif /* G4PARTICLE_H_ */
