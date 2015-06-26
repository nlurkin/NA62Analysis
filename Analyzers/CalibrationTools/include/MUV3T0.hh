// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef MUV3T0_H
#define MUV3T0_H 1

class MUV3T0 : public T0Evaluation {

public:

  MUV3T0(NA62Analysis::Core::BaseAnalysis *ba);
  ~MUV3T0() {};

  void RequestUserHistograms();
  void GenerateUserPDFReport();

private:

  TH1D *fHTileT1T2[152];
  Bool_t fAllTileT1T2Found;
};

#endif
