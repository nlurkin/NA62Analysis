// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef MUV2T0_H
#define MUV2T0_H 1

class MUV2T0Computation : public T0Computation {

public:

  MUV2T0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~MUV2T0Computation() {};
};

#endif
