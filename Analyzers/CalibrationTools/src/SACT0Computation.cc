#include "SACT0Computation.hh"

SACT0Computation::SACT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "SACT0Computation";
  fDirName            = "SACMonitor";
  fTH2Name            = "";
  fOutTextFileName    = "./SAC-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./SAC-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
