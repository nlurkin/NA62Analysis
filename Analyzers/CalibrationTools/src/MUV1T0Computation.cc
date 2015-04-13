#include "MUV1T0Computation.hh"

MUV1T0Computation::MUV1T0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "MUV1T0Computation";
  fDirName            = "MUV1Monitor";
  fTH2Name            = "";
  fOutTextFileName    = "./MUV1-T0.dat";

  // Optional parameters
  //fConfFileName       = "";
  //fOutPDFFileName     = "./MUV1-T0.pdf";
  //fFittingRange       = 0.9;
  //fNFilesToAccumulate = 1;
  //fHistoTimeLimit     = 20.0;
}
