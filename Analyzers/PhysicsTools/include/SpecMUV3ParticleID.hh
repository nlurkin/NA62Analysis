// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#ifndef SPECMUV3PARTICLEID_HH
#define SPECMUV3PARTICLEID_HH

#include "Analyzer.hh"
#include "SpecMUV3Association.hh"

class SpecMUV3ParticleID : public NA62Analysis::Analyzer {

public:
  SpecMUV3ParticleID(NA62Analysis::Core::BaseAnalysis *ba);
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
  vector<SpecMUV3Association> fContainer;
  Double_t fScaleFactor; ///< Scale factor to modify the MUV3 search radius
};

#endif
