#include "CHODT0Computation.hh"

CHODT0Computation::CHODT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "CHODT0Computation";
  fDirName            = "CHODMonitor";
  fTH2Name            = "";
  fOutTextFileName    = "./CHOD-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./CHOD-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
