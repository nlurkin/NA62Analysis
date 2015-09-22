// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-09-15
//
// ---------------------------------------------------------------

#ifndef SPECLKRPARTICLEID_HH
#define SPECLKRPARTICLEID_HH

#include "Analyzer.hh"
#include "SpecLKrAssociation.hh"

class SpecLKrParticleID : public NA62Analysis::Analyzer {

public:
  SpecLKrParticleID(NA62Analysis::Core::BaseAnalysis *ba);
  void InitHist() {}
  void InitOutput();
  void DefineMCSimple() {}
  void Process(int iEvent);
  void StartOfBurstUser() {}
  void EndOfBurstUser() {}
  void StartOfRunUser() {}
  void EndOfRunUser() {}
  void PostProcess() {}
  void DrawPlot() {}

protected:
  vector<SpecLKrAssociation> fContainer;
  Double_t fMaxTrackClusterDistance;  ///< Maximum distance to match track to cluster
};

#endif
