// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#ifndef SPECMUV3ASSOCIATION_HH
#define SPECMUV3ASSOCIATION_HH

#include "Analyzer.hh"
#include "SpecMUV3Muon.hh"

class SpecMUV3Association : public NA62Analysis::Analyzer {

public:
  SpecMUV3Association(NA62Analysis::Core::BaseAnalysis *ba);
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
  vector<SpecMUV3Muon> fMuons;

};
#endif
