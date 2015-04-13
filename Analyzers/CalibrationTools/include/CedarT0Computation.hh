// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef CEDART0_H
#define CEDART0_H 1

class CedarT0Computation : public T0Computation {

public:

  CedarT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~CedarT0Computation() {};
};

#endif
