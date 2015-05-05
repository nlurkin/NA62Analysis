// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef HACT0_H
#define HACT0_H 1

class HACT0 : public T0Evaluation {

public:

  HACT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~HACT0() {};
};

#endif
