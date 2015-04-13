// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef CHANTIT0_H
#define CHANTIT0_H 1

class CHANTIT0Computation : public T0Computation {

public:

  CHANTIT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~CHANTIT0Computation() {};
};

#endif
