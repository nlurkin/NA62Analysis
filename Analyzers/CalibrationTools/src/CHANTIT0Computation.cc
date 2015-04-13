#include "CHANTIT0Computation.hh"

CHANTIT0Computation::CHANTIT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "CHANTIT0Computation";
  fDirName            = "CHANTIMonitor";
  fTH2Name            = "";
  fOutTextFileName    = "./CHANTI-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./CHANTI-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
