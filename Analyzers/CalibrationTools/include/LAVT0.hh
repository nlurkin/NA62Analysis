// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ------------------------------------------------------------------

#include "T0Evaluation.hh"
#include "TGraphErrors.h"

#ifndef LAVT0_H
#define LAVT0_H 1

class  LAVFitFunctions {
public: 
  LAVFitFunctions(){};
  ~LAVFitFunctions(){};
  Double_t EvaluateFitf(Double_t *x, Double_t *p);
  Double_t EvaluateResidualSlewing(Double_t *x, Double_t *p);
};


class LAVT0 : public T0Evaluation {

public:

  LAVT0(NA62Analysis::Core::BaseAnalysis *ba);
  ~LAVT0() {};
  void RequestUserHistograms();
  void GenerateUserPDFReport();
  void evaluateGlobalTZeroFit(Int_t, Double_t , Int_t , Int_t , Int_t );
  TGraphErrors* correctSlewing(Int_t, TH2D**, Int_t, Double_t, Int_t, Int_t, Int_t,
			       Double_t*, Double_t*,
			       Double_t*, Double_t*,
			       Double_t*, Double_t*,
			       Double_t*, Double_t*,
			       Double_t*);

private:

  TH2D *fDeltaLAV[2];
  Bool_t fSlewingFound;
  LAVFitFunctions* fFunctions;
};

#endif
