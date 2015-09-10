#include "MUV1T0.hh"

MUV1T0::MUV1T0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "MUV1") {
    
    // Optimal parameters
    fMinIntegral         = 1000;   // minimal number of entries (excluding underflows, overflows) for fit attempt
    fMinContentMaxBin    = 200.0;  // minimal content of most populated bin for fit attempt
    fFittingRange        = 6.;   // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width
    fNFilesToAccumulate  = 200;    // for the T0 stability plots
    fHistoTimeLimit      = 30.0;  // time half-span of plotted histograms [ns]
    fSignalPeakWidth     = 5.0;   // exclusion region half-width for the spectrum shape check
    fMaxResol            = 10.0;   // max time resolution to consider the fit successful
    fMaxDeltaT0          = 0.5;   // max precision of T0 to consider the fit successful
    fMaxDeltaResol       = 5.0;   // max precision on resolution to consider the fit successful
    fIssueWarnings       = true; // check if the spectrum shape is OK?
    fPlotChannelTimes    = true;  // plot times in each channel?
    fPlotTimeDependences = true;  // check and plot the time stability of the T0 constants?
        
}


