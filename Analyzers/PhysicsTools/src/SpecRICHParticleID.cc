// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-27
//
// ---------------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include "SpecRICHParticleID.hh"
#include "Event.hh"
#include "Persistency.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

/// \class SpecRICHParticleID
/// \Brief
/// Geometrical association of RICH candidates and hits to Spectrometer candidates
/// \EndBrief
/// \Detailed
/// Two particle identification algorithms are available:
/// 1) straw track association to reconstructed RICH rings;
/// 2) straw track association to RICH reconstructed hits.
/// Each algorithm outputs the verdict: whether the track is compatible with
/// being an electron, a muon and a pion. The verdicts are not exclusive.
/// The output is a SpecRICHAssociation structure.
/// An example of use is available in the SpecRICHEventMonitor class.
/// Below is the most basic example.
/// \code
///  OutputState state;
///  std::vector<SpecRICHAssociation> SpecRICH =
///    *(std::vector<SpecRICHAssociation>*)GetOutput("SpecRICHParticleID.Output", state);
///  for (UInt_t itrack=0; itrack<SpecRICH.size(); itrack++) {
///    SpecRICHAssociation sr = SpecRICH[itrack];
///    Double_t xc = sr.GetExpectedXRingCentre();
///    Double_t yc = sr.GetExpectedYRingCentre();
///    // ...
///  }
/// \endcode
/// \EndDetailed

SpecRICHParticleID::SpecRICHParticleID(Core::BaseAnalysis *ba) : Analyzer(ba, "SpecRICHParticleID") {
  RequestTree("RICH",         new TRecoRICHEvent,         "Reco");
  RequestTree("Spectrometer", new TRecoSpectrometerEvent, "Reco");
  
  // RICH parameters
  fFocalLength    = 17020;
  fRefIndex       = 1.000062;
  fBetaThreshold  = 1.0/fRefIndex;
  fThetaCMax      = acos(1.0/fRefIndex);
  fMomThresholdEl = MEL / sqrt(fRefIndex*fRefIndex-1);
  fMomThresholdMu = MMU / sqrt(fRefIndex*fRefIndex-1);
  fMomThresholdPi = MPI / sqrt(fRefIndex*fRefIndex-1);

  // Secrah parameters
  fMaxMeanNHits = 14; // the expected number of hits for a ring in acceptance with beta=1
  fRadiusMargin =  5;

  fPoisson = new TF1("PoissonFunction", "TMath::Poisson(x, [0])", 0, 100);
}

void SpecRICHParticleID::InitOutput() {
  RegisterOutput("Output", &fContainer);
}

void SpecRICHParticleID::Process(int iEvent) {

  TRecoSpectrometerEvent* STRAWevent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  TRecoRICHEvent*         RICHevent  = (TRecoRICHEvent*)GetEvent("RICH");
  SetOutputState("Output", kOValid);

  Int_t NRings    = RICHevent->GetNRingCandidates();
  Int_t nRichHits = RICHevent->GetNHits();
  Int_t nGoodRichHits = 0;
  for (Int_t i=0; i<nRichHits; i++) {
    TRecoRICHHit* hit = (TRecoRICHHit*)RICHevent->GetHit(i);
    if (hit->GetROChannelID()>=0 && hit->GetROChannelID()<2048) nGoodRichHits++;
  }
  
  fContainer.clear();
  for (Int_t iTrack=0; iTrack<STRAWevent->GetNCandidates(); iTrack++) {
    TRecoSpectrometerCandidate* Scand = (TRecoSpectrometerCandidate*)STRAWevent->GetCandidate(iTrack);
    Double_t xc   = fFocalLength * Scand->GetSlopeXAfterMagnet();
    Double_t yc   = fFocalLength * Scand->GetSlopeYAfterMagnet();
    Double_t Time = Scand->GetTime();
    SpecRICHAssociation Asso(iTrack, Time, Scand->GetMomentum(), xc, yc);
    
    /////////////////////////////////////////////
    // Evaluate the expected Cherenkov ring radii

    Double_t ThetaCEl=0.0, ThetaCMu=0.0, ThetaCPi=0.0;
    Double_t RadiusEl=0.0, RadiusMu=0.0, RadiusPi=0.0;

    if (Scand->GetMomentum() > fMomThresholdEl) {
      Double_t Beta = Scand->GetMomentum() / sqrt(Scand->GetMomentum()*Scand->GetMomentum() + MEL*MEL);
      ThetaCEl = acos(1.0 / fRefIndex / Beta);
      RadiusEl = fFocalLength * ThetaCEl;
    }
    if (Scand->GetMomentum() > fMomThresholdMu) {
      Double_t Beta = Scand->GetMomentum() / sqrt(Scand->GetMomentum()*Scand->GetMomentum() + MMU*MMU);
      ThetaCMu = acos(1.0 / fRefIndex / Beta);
      RadiusMu = fFocalLength * ThetaCMu;
    }
    if (Scand->GetMomentum() > fMomThresholdPi) {
      Double_t Beta = Scand->GetMomentum() / sqrt(Scand->GetMomentum()*Scand->GetMomentum() + MPI*MPI);
      ThetaCPi = acos(1.0 / fRefIndex / Beta);
      RadiusPi = fFocalLength * ThetaCPi;
    }

    Asso.SetRingRadiusEl(RadiusEl);
    Asso.SetRingRadiusMu(RadiusMu);
    Asso.SetRingRadiusPi(RadiusPi);

    ////////////////////////////////////////////
    // Evaluate the parts of rings in acceptance

    Double_t AccEl=0, AccMu=0, AccPi=0;
    for (Int_t j=0; j<100; j++) {
      Double_t Phi = 0.02*j*TMath::Pi();
      if (RadiusEl>0) {
	Double_t x = xc + RadiusEl*cos(Phi);
	Double_t y = yc + RadiusEl*sin(Phi);
	if ((x+150)*(x+150)/350.0/350.0 + y*y/280.0/280.0 < 1.0) AccEl++;
      }
      if (RadiusMu>0) {
	Double_t x = xc + RadiusMu*cos(Phi);
	Double_t y = yc + RadiusMu*sin(Phi);
	if ((x+150)*(x+150)/350.0/350.0 + y*y/280.0/280.0 < 1.0) AccMu++;
      }
      if (RadiusPi>0) {
	Double_t x = xc + RadiusPi*cos(Phi);
	Double_t y = yc + RadiusPi*sin(Phi);
	if ((x+150)*(x+150)/350.0/350.0 + y*y/280.0/280.0 < 1.0) AccPi++;
      }
    }
    AccEl *= 0.01;
    AccMu *= 0.01;
    AccPi *= 0.01;

    Asso.SetRingAcceptanceEl(AccEl);
    Asso.SetRingAcceptanceMu(AccMu);
    Asso.SetRingAcceptancePi(AccPi);

    ///////////////////////////
    // Numbers of expected hits

    Double_t NExpectedHitsEl = fMaxMeanNHits * AccEl * pow(sin(ThetaCEl)/sin(fThetaCMax), 2);
    Double_t NExpectedHitsMu = fMaxMeanNHits * AccMu * pow(sin(ThetaCMu)/sin(fThetaCMax), 2);
    Double_t NExpectedHitsPi = fMaxMeanNHits * AccPi * pow(sin(ThetaCPi)/sin(fThetaCMax), 2);

    Asso.SetMaxMeanNHits   (fMaxMeanNHits);
    Asso.SetNExpectedHitsEl(NExpectedHitsEl);
    Asso.SetNExpectedHitsMu(NExpectedHitsMu);
    Asso.SetNExpectedHitsPi(NExpectedHitsPi);

    //////////////////////////////////////////////
    // Numbers of hits observed in each hypothesis

    Int_t NObservedHitsEl=0, NObservedHitsMu=0, NObservedHitsPi=0;
    for (Int_t i=0; i<nRichHits; i++) {
      TRecoRICHHit* hit = (TRecoRICHHit*)RICHevent->GetHit(i);
      if (hit->GetROChannelID()>=0 && hit->GetROChannelID()<2048) {
	Double_t Xhit = hit->GetFitPosition().X();
	Double_t Yhit = hit->GetFitPosition().Y();
	Double_t R    = sqrt((Xhit-xc)*(Xhit-xc) + (Yhit-yc)*(Yhit-yc));
	if (RadiusEl>0 && fabs(R-RadiusEl)<fRadiusMargin) NObservedHitsEl++;
	if (RadiusMu>0 && fabs(R-RadiusMu)<fRadiusMargin) NObservedHitsMu++;
	if (RadiusPi>0 && fabs(R-RadiusPi)<fRadiusMargin) NObservedHitsPi++;
      }
    }

    Asso.SetRadiusMargin   (fRadiusMargin);
    Asso.SetNObservedHitsEl(NObservedHitsEl);
    Asso.SetNObservedHitsMu(NObservedHitsMu);
    Asso.SetNObservedHitsPi(NObservedHitsPi);

    ///////////////////////////////////////////////////////////
    // Match track to rings produced by the RICH reconstruction

    Bool_t IsElectron1 = false, IsMuon1 = false, IsPion1 = false;

    // Find the closest ring to the track
    Double_t MinDistanceTrackRing  = 9999;
    Double_t MinXDistanceTrackRing = 9999;
    Double_t MinYDistanceTrackRing = 9999;
    Int_t    MatchingRingIndex    = -999;
    for (Int_t iring=0; iring<NRings; iring++) {
      TRecoRICHCandidate* rc = (TRecoRICHCandidate*)RICHevent->GetRingCandidate(iring);
      Double_t XRing = rc->GetRingCenter().X();
      Double_t YRing = rc->GetRingCenter().Y();
      Double_t DistanceTrackRing = sqrt((XRing-xc)*(XRing-xc)+(YRing-yc)*(YRing-yc));
      if (DistanceTrackRing < MinDistanceTrackRing) {
	MatchingRingIndex     = iring;
	MinDistanceTrackRing  = DistanceTrackRing;
	MinXDistanceTrackRing = XRing-xc;
	MinYDistanceTrackRing = YRing-yc;
      }
    }

    Asso.SetMinDistanceTrackRing (MinDistanceTrackRing);
    Asso.SetMinXDistanceTrackRing(MinXDistanceTrackRing);
    Asso.SetMinYDistanceTrackRing(MinYDistanceTrackRing);

    if (MinDistanceTrackRing<15.0) {
      TRecoRICHCandidate* rc = (TRecoRICHCandidate*)RICHevent->GetRingCandidate(MatchingRingIndex);
      Double_t RRing = rc->GetRingRadius();
      Double_t Chi2  = rc->GetRingChi2();
      Asso.SetRingID    (MatchingRingIndex);
      Asso.SetRingRadius(RRing);
      Asso.SetRingChi2  (Chi2);
      if (RadiusEl>0) Asso.SetRingRadiusWrtEl(RRing-RadiusEl);
      if (RadiusMu>0) Asso.SetRingRadiusWrtMu(RRing-RadiusMu);
      if (RadiusPi>0) Asso.SetRingRadiusWrtPi(RRing-RadiusPi);
      if (RadiusEl>0 && fabs(RRing-RadiusEl)<6.0) IsElectron1 = true;
      if (RadiusMu>0 && fabs(RRing-RadiusMu)<6.0) IsMuon1     = true;
      if (RadiusPi>0 && fabs(RRing-RadiusPi)<6.0) IsPion1     = true;
    }

    Asso.SetElectron1(IsElectron1);
    Asso.SetMuon1(IsMuon1);
    Asso.SetPion1(IsPion1);

    ////////////////////////////////////////////////////
    // Compare the numbers of observed and expected hits

    Double_t Integral_low, Integral_high;
    Double_t PvalueEl = 1.0, PvalueMu = 1.0, PvaluePi = 1.0;

    if (NExpectedHitsEl>0) {
      fPoisson->SetParameter(0, NExpectedHitsEl);
      Integral_low = Integral_high = 0.0;
      for (Int_t i=0; i<100; i++) {
	Double_t value = fPoisson->Eval(i);
	if (i<=NObservedHitsEl) Integral_low  += value;
	if (i>=NObservedHitsEl) Integral_high += value;
      }
      PvalueEl = (Integral_low<Integral_high) ? Integral_low : Integral_high;
    }

    if (NExpectedHitsMu>0) {
      fPoisson->SetParameter(0, NExpectedHitsMu);
      Integral_low = Integral_high = 0.0;
      for (Int_t i=0; i<100; i++) {
	Double_t value = fPoisson->Eval(i);
	if (i<=NObservedHitsMu) Integral_low  += value;
	if (i>=NObservedHitsMu) Integral_high += value;
      }
      PvalueMu = (Integral_low<Integral_high) ? Integral_low : Integral_high;
    }

    if (NExpectedHitsPi>0) {
      fPoisson->SetParameter(0, NExpectedHitsPi);
      Integral_low = Integral_high = 0.0;
      for (Int_t i=0; i<100; i++) {
	Double_t value = fPoisson->Eval(i);
	if (i<=NObservedHitsPi) Integral_low  += value;
	if (i>=NObservedHitsPi) Integral_high += value;
      }
      PvaluePi = (Integral_low<Integral_high) ? Integral_low : Integral_high;
    }

    Asso.SetPvalueEl(PvalueEl);
    Asso.SetPvalueMu(PvalueMu);
    Asso.SetPvaluePi(PvaluePi);
    
    ////////////////////////////////////////////////////////////////////////////
    // Make the decision: is a track compatible with being electron, muon, pion?
    // p-values not used currently, bue can be used

    Bool_t IsElectron2 = (NExpectedHitsEl>=2 && NObservedHitsEl>=2 && PvalueEl>0.00);
    Bool_t IsMuon2     = (NExpectedHitsMu>=2 && NObservedHitsMu>=2 && PvalueMu>0.00);
    Bool_t IsPion2     = (NExpectedHitsPi>=2 && NObservedHitsPi>=2 && PvaluePi>0.00);
   
    Asso.SetElectron2(IsElectron2);
    Asso.SetMuon2(IsMuon2);
    Asso.SetPion2(IsPion2);

    fContainer.push_back(Asso);
  }
}
