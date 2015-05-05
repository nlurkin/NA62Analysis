// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef CHANTIT0_H
#define CHANTIT0_H 1

class CHANTIT0 : public T0Evaluation {

public:

  CHANTIT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~CHANTIT0() {};
};

#endif
