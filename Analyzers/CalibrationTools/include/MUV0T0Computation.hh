// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Computation.hh"

#ifndef MUV0T0_H
#define MUV0T0_H 1

class MUV0T0Computation : public T0Computation {

public:

  MUV0T0Computation(NA62Analysis::Core::BaseAnalysis *ba);
  ~MUV0T0Computation() {};
};

#endif
