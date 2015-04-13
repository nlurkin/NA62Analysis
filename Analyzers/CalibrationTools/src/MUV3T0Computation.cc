#include "MUV3T0Computation.hh"

MUV3T0Computation::MUV3T0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "MUV3T0Computation";
  fDirName            = "MUV3Monitor";
  fTH2Name            = "LeadingTimeWrtCedarVsReadoutChannelNoT0";
  fOutTextFileName    = "./MUV3-T0.dat";

  // Optional parameters
  fConfFileName       = "./MUV3.conf";
  fOutPDFFileName     = "./MUV3-T0.pdf";
  fFittingRange       = 0.9;
  fNFilesToAccumulate = 100;
  fHistoTimeLimit     = 20.0;
}
