#include "MUV3TilesT0.hh"

MUV3TilesT0::MUV3TilesT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "MUV3Tiles") {

  // Non-standard histogram names
  fDetectorName = "MUV3Tiles";
  fDirName      = "MUV3Monitor";
  fTH2Name      = "CandidateTimeWrtReferenceNoTileT0VsTile";
  fConfFileName = ""; // channel map not required

  // Optional parameters
  fMinIntegral         = 100;   // minimal number of entries (excluding underflows, overflows) for fit attempt
  fFittingRange        = 0.9;   // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width 
  fNFilesToAccumulate  = 20;    // for the T0 stability plots
  fHistoTimeLimit      = 5.0;   // time half-span of plotted histograms [ns]
  fIssueWarnings       = false; // check if the spectrum shape is OK?
  fPlotChannelTimes    = true;  // plot times in each channel?
  fPlotTimeDependences = true;  // check and plot the time stability of the T0 constants?
}
