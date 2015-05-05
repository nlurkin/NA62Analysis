// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"

#ifndef IRCT0_H
#define IRCT0_H 1

class IRCT0 : public T0Evaluation {

public:

  IRCT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~IRCT0() {};
};

#endif
