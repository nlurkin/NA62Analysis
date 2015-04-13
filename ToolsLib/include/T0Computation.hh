// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ---------------------------------------------------------------

#ifndef T0COMPUTATION_HH
#define T0COMPUTATION_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include "MCSimple.hh"
#include "DetectorAcceptance.hh"
#include <TCanvas.h>

#include "TH1D.h"
#include "TF1.h"
#include "TText.h"

class T0Computation : public NA62Analysis::Analyzer {

public:
  T0Computation(NA62Analysis::Core::BaseAnalysis *ba);

  // standard methods
  void InitOutput() {}
  void DefineMCSimple() {}
  void InitHist();
  void StartOfRunUser() {}
  void StartOfBurstUser() {}
  void Process(int iEvent) {}
  void EndOfBurstUser();
  void EndOfRunUser();
  void PostProcess() {}
  void DrawPlot() {}

  // custom methods
  void ParseConfFile();
  void EvaluateT0s(TH2D*);
  void EvaluateChannelT0(int);
  void GeneratePDFReport();
  void GenerateOutput();

private:
  void Publish();

  int    fBurstCounter, fNChannels, fNChannelsActive, ActiveChannelMap[5000];
  double fBinWidth;
  bool   fIsActive[5000];
  TH2D   *fH2, *fH2_Partial, *fH2_Integrated;
  TH1D   *fHTime[5000], *fHT0VsBurst[5000];
  TF1    *fFChannelFit[5000], *fFChannelStability[5000];
  int    fChannelID[5000];
  double fT0[5000], fDeltaT0[5000];
  bool   fUseChannelMap;

protected:

  TString fDirName;            ///< Name of directory in the input file
  TString fTH2Name;            ///< Name of the input 2D histogram
  TString fConfFileName;       ///< Configuration file name (optional, of one wants channel map in the printout)
  TString fOutTextFileName;    ///< Name of the output file with the computed T0 constants
  TString fOutPDFFileName;     ///< Name of the output PDF report file (optional, if report is required)
  int     fNFilesToAccumulate; ///< Unit of time for stability checks
  double  fFittingRange;       ///< Half-width of the fitting T0 range
  double  fHistoTimeLimit;     ///< Half-size of the X axis span for the PDF report.
};

#endif
