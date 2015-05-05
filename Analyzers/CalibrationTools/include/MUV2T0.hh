// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef MUV2T0_H
#define MUV2T0_H 1

class MUV2T0 : public T0Evaluation {

public:

  MUV2T0(NA62Analysis::Core::BaseAnalysis *ba);
  ~MUV2T0() {};
};

#endif
