/*
 * G4ParticleInterface.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: nlurkin
 */

#include "ParticleInterface.hh"

using namespace std;

ParticleInterface *ParticleInterface::fParticleInterface = 0;

ParticleInterface *ParticleInterface::GetParticleInterface(){
	/// \MemberDescr
	/// Return a pointer to the unique instance of G4ParticleInterface
	/// \EndMemberDescr

	if(!fParticleInterface) fParticleInterface = new ParticleInterface();
	return fParticleInterface;
}

ParticleInterface::ParticleInterface() {
	/// \MemberDescr
	/// Default private constructor to ensure uniqueness of the instance
	/// \EndMemberDescr

	fTable = TDatabasePDG::Instance();
}

ParticleInterface::~ParticleInterface() {
	/// \MemberDescr
	/// Default destructor
	/// \EndMemberDescr

}

TParticlePDG* ParticleInterface::FindParticle(int pdgID){
	/// \MemberDescr
	/// \param pdgID : pdg code of the particle
	///
	/// Return the G4ParticleDefinition corresponding to the provided particle pdg code
	/// \EndMemberDescr

	return fTable->GetParticle(pdgID);
}

TParticlePDG* ParticleInterface::FindParticle(TString name){
	/// \MemberDescr
	/// \param name : name of the particle
	///
	/// Return the G4ParticleDefinition corresponding to the provided particle name
	/// \EndMemberDescr

	return fTable->GetParticle(name.Data());
}

TString ParticleInterface::GetParticleName(int pdgID){
	/// \MemberDescr
	/// \param pdgID : pdg code of the particle
	///
	/// Return the name of the particle with the specified pdg code
	/// \EndMemberDescr

	TParticlePDG *p = fTable->GetParticle(pdgID);
	if(p){
		return p->GetName();
	}
	return "";
}

int ParticleInterface::GetParticlePDGid(TString name){
	/// \MemberDescr
	/// \param name : name of the particle
	///
	/// Return the pdg code of the specified particle
	/// \EndMemberDescr

	TParticlePDG *p = fTable->GetParticle(name.Data());
	if(p){
		return p->PdgCode();
	}
	return 0;
}

void ParticleInterface::PrintTable(){
	/// \MemberDescr
	/// Dump the TDatabasePDG table on screen
	/// \EndMemberDescr

	fTable->Print();
}
