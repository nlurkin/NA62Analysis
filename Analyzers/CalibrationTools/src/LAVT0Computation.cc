#include "LAVT0Computation.hh"

LAVT0Computation::LAVT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "LAVT0Computation";
  fDirName            = "LAVMonitor";
  fTH2Name            = "";
  fOutTextFileName    = "./LAV-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./LAV-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
