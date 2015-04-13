// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef HACT0_H
#define HACT0_H 1

class HACT0Computation : public T0Computation {

public:

  HACT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~HACT0Computation() {};
};

#endif
