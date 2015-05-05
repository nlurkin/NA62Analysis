// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef LAVT0_H
#define LAVT0_H 1

class LAVT0 : public T0Evaluation {

public:

  LAVT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~LAVT0() {};
};

#endif
