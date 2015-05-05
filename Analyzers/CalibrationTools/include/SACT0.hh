// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef SACT0_H
#define SACT0_H 1

class SACT0 : public T0Evaluation {

public:

  SACT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~SACT0() {};
};

#endif
