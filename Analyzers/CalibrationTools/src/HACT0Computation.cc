#include "HACT0Computation.hh"

HACT0Computation::HACT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "HACT0Computation";
  fDirName            = "HACMonitor";
  fTH2Name            = "";
  fOutTextFileName    = "./HAC-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./HAC-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
