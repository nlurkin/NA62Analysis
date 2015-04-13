#include "MUV3TilesT0Computation.hh"

MUV3TilesT0Computation::MUV3TilesT0Computation(Core::BaseAnalysis *ba) : T0Computation(ba) {

  // Mandatory parameters
  fAnalyzerName       = "MUV3TilesT0Computation";
  fDirName            = "MUV3Monitor";
  fTH2Name            = "CandidateTimeWrtCedarNoTileT0VsTile";
  fOutTextFileName    = "./MUV3-T0-tiles.dat";

  // Optional parameters
  fConfFileName       = "";
  fOutPDFFileName     = "./MUV3-T0-tiles.pdf";
  fFittingRange       = 0.9;
  fNFilesToAccumulate = 100;
  fHistoTimeLimit     = 5.0;
}
