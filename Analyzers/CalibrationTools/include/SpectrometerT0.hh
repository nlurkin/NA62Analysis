// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-30
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef SPECTROMETERT0_H
#define SPECTROMETERT0_H 1

class SpectrometerT0 : public T0Evaluation {

public:

  SpectrometerT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~SpectrometerT0() {};
};

#endif
