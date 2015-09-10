// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-27
//
// ---------------------------------------------------------------

#ifndef SPECRICHPARTICLEID_HH
#define SPECRICHPARTICLEID_HH

#include "Analyzer.hh"
#include "SpecRICHAssociation.hh"
#include "TF1.h"

class SpecRICHParticleID : public NA62Analysis::Analyzer {

public:
  SpecRICHParticleID(NA62Analysis::Core::BaseAnalysis *ba);
  void InitHist() {}
  void InitOutput();
  void DefineMCSimple() {}
  void Process(int iEvent);
  void StartOfBurstUser() {}
  void EndOfBurstUser() {}
  void StartOfRunUser() {}
  void EndOfRunUser() {}
  void PostProcess() {}
  void DrawPlot() {}

protected:
  Double_t fFocalLength;    ///< RICH focal length
  Double_t fRefIndex;       ///< RICH radiator refractive index
  Double_t fBetaThreshold;  ///< Cherenkov threshold velocity
  Double_t fThetaCMax;      ///< Maximum Cherenkov angle
  Double_t fMomThresholdEl;
  Double_t fMomThresholdMu;
  Double_t fMomThresholdPi;

  Double_t fMaxMeanNHits;
  Double_t fRadiusMargin;

  TF1 *fPoisson;

  vector<SpecRICHAssociation> fContainer;
};

#endif
