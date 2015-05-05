#include "MUV3TilesT0.hh"

MUV3TilesT0::MUV3TilesT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "MUV3Tiles") {

  // Non-standard histogram names
  fDetectorName = "MUV3Tiles";
  fDirName      = "MUV3Monitor";
  fTH2Name      = "CandidateTimeWrtReferenceNoTileT0VsTile";

  fRawTimeHistoName = ""; // global offset evaluation not required
  fConfFileName     = ""; // channel map not required

  // Optional parameters
  fFittingRange       = 0.9;
  fNFilesToAccumulate = 100;
  fHistoTimeLimit     = 5.0;
}
