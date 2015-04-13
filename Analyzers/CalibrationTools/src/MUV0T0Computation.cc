#include "MUV0T0Computation.hh"

MUV0T0Computation::MUV0T0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "MUV0T0Computation";
  fDirName            = "MUV0Monitor";
  fTH2Name            = "";
  fOutTextFileName    = "./MUV0-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./MUV0-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
