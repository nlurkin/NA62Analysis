#include "IRCT0Computation.hh"

IRCT0Computation::IRCT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "IRCT0Computation";
  fDirName            = "IRCMonitor";
  fTH2Name            = "";
  fOutTextFileName    = "./IRC-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./IRC-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
