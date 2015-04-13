// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef IRCT0_H
#define IRCT0_H 1

class IRCT0Computation : public T0Computation {

public:

  IRCT0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~IRCT0Computation() {};
};

#endif
