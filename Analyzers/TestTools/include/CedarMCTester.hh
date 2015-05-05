#ifndef CEDARMCTESTER_HH
#define CEDARMCTESTER_HH

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

class CedarMCTester : public NA62Analysis::Analyzer {
public:
  CedarMCTester(NA62Analysis::Core::BaseAnalysis *ba);
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
  void Publish();

  double pi;
  double hc;
  double fKaonRate;
  std::string fDivider;

  double fZCedarOriginLab;
  double fZDiaphragmCentre;
  double fZLightGuideCentre;
  double fLightGuideInnerRadius;
  double fZQuartzWindowStart;
  double fZQuartzWindowLength;
  double fQuartzWindowRadialOffset;
  double fZSphericalMirrorCapCentreNominal;
  double fSphericalMirrorDiameter;
  double fSphericalMirrorCentralAngle;
  double fSphericalMirrorCapRadialOffset;
  double fAngularSeparation;

  TH3D * fhAllPositions;
  TH3D * fhKTAGPositions;

protected:

};


namespace CedarTools
{
  Double_t QE                 (Double_t wavelength, Int_t PMType);
  Double_t QE_R7400U_03       (Double_t wavelength);
  Double_t QE_R9880U_110      (Double_t wavelength);
  Double_t QE_R9880U_210      (Double_t wavelength);
  Double_t QE_EMI_9820_QB_Lau (Double_t wavelength);
  Double_t QE_EMI_9820_QB     (Double_t wavelength);
}

#endif
