/*
 * G4ParticleInterface.cpp
 *
 *  Created on: Aug 13, 2013
 *      Author: nlurkin
 */

#include "ParticleInterface.hh"

ParticleInterface *ParticleInterface::fParticleInterface = 0;

ParticleInterface *ParticleInterface::GetParticleInterface(){
	/// \MemberDescr
	/// \return Pointer to the unique instance of G4ParticleInterface
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

const TParticlePDG* ParticleInterface::FindParticle(int pdgID) const{
	/// \MemberDescr
	/// \param pdgID : pdg code of the particle
	/// \return G4ParticleDefinition corresponding to the provided particle pdg code
	/// \EndMemberDescr

	return fTable->GetParticle(pdgID);
}

const TParticlePDG* ParticleInterface::FindParticle(TString name) const{
	/// \MemberDescr
	/// \param name : name of the particle
	/// \return G4ParticleDefinition corresponding to the provided particle name
	/// \EndMemberDescr

	return fTable->GetParticle(name.Data());
}

TString ParticleInterface::GetParticleName(int pdgID) const{
	/// \MemberDescr
	/// \param pdgID : pdg code of the particle
	/// \return Name of the particle with the specified PDG code
	/// \EndMemberDescr

	TParticlePDG *p = fTable->GetParticle(pdgID);
	if(p){
		return p->GetName();
	}
	return "";
}

int ParticleInterface::GetParticlePDGid(TString name) const{
	/// \MemberDescr
	/// \param name : name of the particle
	/// \return PDG code of the specified particle
	/// \EndMemberDescr

	TParticlePDG *p = fTable->GetParticle(name.Data());
	if(p){
		return p->PdgCode();
	}
	return 0;
}

void ParticleInterface::PrintTable() const{
	/// \MemberDescr
	/// Dump the TDatabasePDG table on screen
	/// \EndMemberDescr

	fTable->Print();
}
