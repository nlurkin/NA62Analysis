// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef RICHT0_H
#define RICHT0_H 1

class RICHT0Computation : public T0Computation {

public:

  RICHT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~RICHT0Computation() {};
};

#endif
