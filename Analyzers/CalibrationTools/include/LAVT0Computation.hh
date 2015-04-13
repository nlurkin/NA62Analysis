// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef LAVT0_H
#define LAVT0_H 1

class LAVT0Computation : public T0Computation {

public:

  LAVT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~LAVT0Computation() {};
};

#endif
