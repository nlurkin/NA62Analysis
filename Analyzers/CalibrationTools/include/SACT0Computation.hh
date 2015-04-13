// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef SACT0_H
#define SACT0_H 1

class SACT0Computation : public T0Computation {

public:

  SACT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~SACT0Computation() {};
};

#endif
