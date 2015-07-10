// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-06-25
//
// ---------------------------------------------------------------

#ifndef MUV3DATAQUALITYMONITOR_HH
#define MUV3DATAQUALITYMONITOR_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include <TCanvas.h>
#include <TStyle.h>

#include "TH1D.h"
#include "TProfile.h"
#include "TF1.h"
#include "TText.h"
#include "TLine.h"
#include "TLegend.h"

class MUV3DataQualityMonitor : public NA62Analysis::Analyzer {

public:
  MUV3DataQualityMonitor(NA62Analysis::Core::BaseAnalysis *ba);

  // standard methods
  void InitOutput() {}
  void DefineMCSimple() {}
  void InitHist();
  void StartOfRunUser() {}
  void StartOfBurstUser() {}
  void Process(int iEvent) {}
  void EndOfBurstUser() {}
  void EndOfRunUser();
  void PostProcess() {}
  void DrawPlot() {}

private:
  TString fOutPDFFileName, fDirName;
  TH1D *fHChannelProfile, *fHChannelProfileEOB, *fHTileOR, *fHTileAND;
  TH2D *fHChannelProfileVsBurst, *fHChannelProfileVsBurstEOB;
  TH1D *fHNEventsProcessedPerBurst;
  TH1D *fHTileAsymmetry, *fHTileAsymmetryEOB;
  std::vector<TH2D*> fHList;

  void GenerateTileAsymmetryVsBurstIDPlots();
};

#endif
