#include "RICHT0.hh"

RICHT0::RICHT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "RICH") {

  // Optional parameters
  fMinIntegral         = 100;  // minimal number of entries (excluding underflows, overflows) for fit attempt
  fFittingRange        = 0.9;  // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width 
  fNFilesToAccumulate  = 20;   // for the T0 stability plots
  fHistoTimeLimit      = 15.0; // time half-span of plotted histograms [ns]
  fSignalPeakWidth     = 2.0;  // exclusion region half-width when looking for anomalous shape [ns]
  fIssueWarnings       = true; // check if the spectrum shape is OK?
  fPlotChannelTimes    = true; // plot times in each channel?
  fPlotTimeDependences = true; // check and plot the time stability of the T0 constants?
  fTH2Name             = "RecoHitTimeWrtReferenceVsSeqChannelNoT0";  // The RICHReconstruction uses SeqChannelID to apply the corrections  
}
