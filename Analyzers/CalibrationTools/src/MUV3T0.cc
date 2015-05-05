#include "MUV3T0.hh"

MUV3T0::MUV3T0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "MUV3") {
  // Optional parameters
  fFittingRange       = 0.9;
  fNFilesToAccumulate = 100;
  fHistoTimeLimit     = 20.0;
}
