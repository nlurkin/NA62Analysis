#include "CedarT0.hh"

CedarT0::CedarT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "Cedar") {

  // Optional parameters
  fMinIntegral         = 100;  // minimal number of entries (excluding underflows, overflows) for fit attempt
  fFittingRange        = 0.7;  // fitting range = [-0.7ns:+0.7ns], i.e. 7 bins of 0.2ns width 
  fNFilesToAccumulate  = 20;   // for the T0 stability plots
  fHistoTimeLimit      = 15.0; // time half-span of plotted histograms [ns]
  fSignalPeakWidth     = 1.0;  // exclusion region half-width when looking for anomalous shape [ns]
  fInitialResol        = 0.3;  // initial value of the time resolution parameter for the fit [ns]
  fIssueWarnings       = true; // check if the spectrum shape is OK?
  fPlotChannelTimes    = true; // plot times in each channel?
  fPlotTimeDependences = false; // check and plot the time stability of the T0 constants?
}
