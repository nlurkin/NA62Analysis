#include "CHODT0.hh"

CHODT0::CHODT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "CHOD") {

  fTH2Name = "RecoHitTimeWrtReferenceVsIntersectionIDNoT0";
  fConfFileName     = ""; // channel map not required 

}
