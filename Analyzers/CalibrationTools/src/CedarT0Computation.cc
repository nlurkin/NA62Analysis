#include "CedarT0Computation.hh"

CedarT0Computation::CedarT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "CedarT0Computation";
  fDirName            = "CedarMonitor";
  fTH2Name            = "RecoHitTimeWrtCandidateVsChannel";
  fOutTextFileName    = "./Cedar-T0.dat";

  // Optional parameters
  fConfFileName       = "./Cedar.conf";
  fOutPDFFileName     = "./Cedar-T0.pdf";
  fFittingRange       = 0.9;
  fNFilesToAccumulate = 15;
  fHistoTimeLimit     = 10.0;
}
