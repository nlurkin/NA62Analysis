// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef MUV1T0_H
#define MUV1T0_H 1

class MUV1T0Computation : public T0Computation {

public:

  MUV1T0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~MUV1T0Computation() {};
};

#endif
