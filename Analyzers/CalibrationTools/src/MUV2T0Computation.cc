#include "MUV2T0Computation.hh"

MUV2T0Computation::MUV2T0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "MUV2T0Computation";
  fDirName            = "MUV2Monitor";
  fTH2Name            = "";
  fOutTextFileName    = "./MUV2-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./MUV2-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
