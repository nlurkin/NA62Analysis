// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ---------------------------------------------------------------

#ifndef T0EVALUATION_HH
#define T0EVALUATION_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include "MCSimple.hh"
#include "DetectorAcceptance.hh"
#include <TCanvas.h>

#include "TH1D.h"
#include "TProfile.h"
#include "TF1.h"
#include "TText.h"
#include "TLine.h"

class T0Evaluation : public NA62Analysis::Analyzer {

public:
  T0Evaluation(NA62Analysis::Core::BaseAnalysis *ba, std::string DetectorName);

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
  void EvaluateGlobalOffset();
  void GenerateT0TextFile();
  void GeneratePDFReport();

private:
  void Publish();

  int      fEvaluateGlobalT0, fEvaluateT0s;
  int      fBurstCounter, fNChannels, fNChannelsActive, ActiveChannelMap[20000];
  double   fBinWidth, fGlobalT0;
  bool     fIsActive[20000];
  TH2D     *fH2, *fH2_Partial, *fH2_Integrated;
  TH1D     *fHRawTime, *fHTime[20000], *fHT0VsBurst[20000];
  TF1      *fFChannelFit[20000], *fFChannelStability[20000];
  int      fChannelID[20000];
  double   fT0[20000], fDeltaT0[20000];
  bool     fUseChannelMap;

protected:

  TString fDetectorName;       ///< Name of the detector
  TString fDirName;            ///< Name of directory in the input file
  TString fTH2Name;            ///< Name of the input histogram for T0 evaluation
  TString fRawTimeHistoName;   ///< Name of the input histogram for global offset evaluation
  TString fConfFileName;       ///< Configuration file name (optional, of one wants channel map in the printout)
  TString fOutTextFileName;    ///< Name of the output file with the computed T0 constants
  TString fOutPDFFileName;     ///< Name of the output PDF report file (optional, if report is required)
  int     fNFilesToAccumulate; ///< Unit of time for stability checks
  double  fFittingRange;       ///< Half-width of the fitting T0 range
  double  fHistoTimeLimit;     ///< Half-size of the X axis span for the PDF report.
};

#endif
