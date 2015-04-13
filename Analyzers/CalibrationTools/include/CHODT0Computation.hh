// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef CHODT0_H
#define CHODT0_H 1

class CHODT0Computation : public T0Computation {

public:

  CHODT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~CHODT0Computation() {};
};

#endif
