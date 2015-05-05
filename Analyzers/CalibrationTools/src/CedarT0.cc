#include "CedarT0.hh"

CedarT0::CedarT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "Cedar") {

  // Mandatory parameters
  // fDetectorName = "Cedar";

  // Optional parameters
  fConfFileName       = "./Cedar.conf";
  fFittingRange       = 0.9;
  fNFilesToAccumulate = 15;
  fHistoTimeLimit     = 10.0;
}
