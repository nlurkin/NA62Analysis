#ifndef MUV3MCTESTER_HH
#define MUV3MCTESTER_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include "MCSimple.hh"
#include "DetectorAcceptance.hh"
#include <TCanvas.h>

class TH1I;
class TH2F;
class TH3D;
class TGraph;
class TTree;

class MUV3MCTester : public NA62Analysis::Analyzer {

public:
  MUV3MCTester(NA62Analysis::Core::BaseAnalysis *ba);
  void InitHist();
  void InitOutput() {}
  void DefineMCSimple() {}
  void Process(int i);
  void StartOfBurstUser() {}
  void EndOfBurstUser() {}
  void StartOfRunUser() {}
  void EndOfRunUser();
  void PostProcess() {}
  void DrawPlot() {}

private:
  void Publish() {}

protected:
};

#endif
