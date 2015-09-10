// ---------------------------------------------------------------
// History:
//
// Created by Chris Parkinson (cjp@hep.ph.bham.ac.uk) &
// Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-17
//
// ---------------------------------------------------------------

#ifndef SPECRICHEVENTMONITOR_HH
#define SPECRICHEVENTMONITOR_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include <TCanvas.h>
#include <TStyle.h>

#include <TLine.h>
#include <TBox.h>
#include <TArc.h>
#include <TEllipse.h>
#include <TCrown.h>
#include "TRecoRICHEvent.hh"
#include "TRecoSpectrometerEvent.hh"
#include "TLatex.h"

class TH2F;
class TGraph;
class TTree;

class SpecRICHEventMonitor : public NA62Analysis::Analyzer {

public:

  SpecRICHEventMonitor(NA62Analysis::Core::BaseAnalysis *ba);
  void InitHist();
  void InitOutput() {}
  void DefineMCSimple() {}
  void Process(int iEvent);
  void StartOfBurstUser() {}
  void EndOfBurstUser() {}
  void StartOfRunUser() {}
  void EndOfRunUser();
  void PostProcess() {}
  void DrawPlot() {}

private:

  Int_t  fMaxGraphCount;   ///< How many pages to produce in the PDF document?
  Int_t  fNTrackCutLow;    ///< Minimum number of STRAW tracks for an eligible event
  Int_t  fNTrackCutHigh;   ///< Maximum number of STRAW tracks for an eligible event
  Int_t  fGraphCount;      ///< Counter of output pages produced
  
  TString fOutputPDFName;

  TH2D     *fH2;
  TGraph   *fGHits;
  TEllipse *fAcceptance;
  TArc     *fArc;
  TCrown   *fCrown;
  TLatex   *fText;
  TLine    *fLine;
  TCanvas  *fCanvas;
};

#endif
