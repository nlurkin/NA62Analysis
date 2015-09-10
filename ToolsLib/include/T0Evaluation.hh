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
#include <TCanvas.h>
#include <TStyle.h>

#include "TH1D.h"
#include "TProfile.h"
#include "TF1.h"
#include "TText.h"
#include "TLine.h"
#include "TLegend.h"

class T0Evaluation : public NA62Analysis::Analyzer {

public:
  T0Evaluation(NA62Analysis::Core::BaseAnalysis *ba, std::string DetectorName);

  // Standard methods
  void InitOutput() { AddParam("generateoutputpdf", &fGenerateOutputPDF, 1); }
  void DefineMCSimple() {}
  void InitHist();
  void StartOfRunUser() {}
  void StartOfBurstUser() {}
  void Process(int iEvent) {}
  void EndOfBurstUser();
  void EndOfRunUser();
  void PostProcess() {}
  void DrawPlot() {}

  // T0Evaluation-specific methods
  void ParseConfFile();
  void EvaluateT0s(TH2D*, int, bool);
  void EvaluateChannelT0(int, bool);
  virtual bool FitChannel(int, double, double, double, double);
  void EvaluateGlobalOffset();
  void GenerateT0TextFile();
  void GeneratePDFReport();
  virtual void RequestUserHistograms() {}
  virtual void GenerateUserPDFReport() {}

private:
  void Publish();

protected:

  bool    fEvaluateGlobalT0, fEvaluateT0s;
  int     fNChannels, fNChannelsActive, ActiveChannelMap[20000];
  int     fBurstCounter[20000], fTimeBinsCounter[20000];
  double  fBinWidth, fGlobalT0;
  bool    fIsActive[20000];
  TH2D    *fH2, *fH2_Partial, *fH2_Integrated;
  TH1D    *fHNEventsProcessedPerBurst, *fHRawTime, *fHTime[20000], *fHT0VsTime[20000];
  TF1     *fFChannelFit[20000], *fFChannelStability[20000];
  int     fChannelID[20000]; ///< Geometric channel ID versus RO channel ID
  double  fT0[20000], fDeltaT0[20000], fResol[20000], fDeltaResol[20000];
  double  fSecondPeakPos[20000];
  bool    fWarning[20000];
  bool    fUseChannelMap, fPlotChannelTimes, fPlotTimeDependences, fIssueWarnings;
  int     fGenerateOutputPDF;

  TString fDetectorName;       ///< Name of the detector
  TString fDirName;            ///< Name of directory in the input file
  TString fTH2Name;            ///< Name of the input histogram for T0 evaluation
  TString fRawTimeHistoName;   ///< Name of the input histogram for global offset evaluation
  TString fConfFileName;       ///< Configuration file name (optional, of one wants channel map in the printout)
  TString fOutTextFileName;    ///< Name of the output file with the computed T0 constants
  TString fOutPDFFileName;     ///< Name of the output PDF report file (optional, if report is required)
  int     fNFilesToAccumulate; ///< Unit of time for stability checks
  double  fMinIntegral;        ///< Minimal number of entries (excl. underflows, overflows) to attempt fit
  double  fMinContentMaxBin;   ///< Minimal content of most populated bin to attempt fit
  double  fFittingRange;       ///< Half-width of the fitting T0 range
  double  fHistoTimeLimit;     ///< Half-size of the X axis span for the PDF report [ns]
  double  fPeakBkgThreshold;   ///< Second highest peak/Bkg threshold
  double  fSignalPeakWidth;    ///< Exclusion region half-width when looking for anomalous shape
  double  fMaxResol;           ///< Maximum time resolution to consider the fit successful
  double  fMaxDeltaT0;         ///< Maximum precision of T0 to consider the fit successful
  double  fMaxDeltaResol;      ///< Maximum precision on resolution to consider the fit successful

  TCanvas *fCanvas, *fFrontCanvas;
  TText   *fText;
};

#endif
