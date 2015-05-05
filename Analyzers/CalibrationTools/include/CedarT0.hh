// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef CEDART0_H
#define CEDART0_H 1

class CedarT0 : public T0Evaluation {

public:

  CedarT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~CedarT0() {};
};

#endif
