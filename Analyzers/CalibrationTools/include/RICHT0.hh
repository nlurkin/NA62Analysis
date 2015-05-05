// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef RICHT0_H
#define RICHT0_H 1

class RICHT0 : public T0Evaluation {

public:

  RICHT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~RICHT0() {};
};

#endif
