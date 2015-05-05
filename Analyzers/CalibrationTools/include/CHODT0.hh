// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef CHODT0_H
#define CHODT0_H 1

class CHODT0 : public T0Evaluation {

public:

  CHODT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~CHODT0() {};
};

#endif
