#include "RICHT0Computation.hh"

RICHT0Computation::RICHT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "RICHT0Computation";
  fDirName            = "RICHMonitor";
  fTH2Name            = "";
  fOutTextFileName    = "./RICH-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./RICH-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
