// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-27
//
// ---------------------------------------------------------------

#include <iostream>
#include "SpecRICHAssociation.hh"

using namespace std;

/// \class SpecRICHAssociation
/// \Brief
/// Container of RICH association records for each Spectrometer track
/// \EndBrief

SpecRICHAssociation::SpecRICHAssociation() {
  Clear();
}

SpecRICHAssociation::SpecRICHAssociation
(Int_t TrackID, Double_t Time, Double_t Momentum, Double_t XRingCentre, Double_t YRingCentre) {
  Clear();
  fTrackID   = TrackID;
  fTrackTime = Time;
  fTrackMomentum = Momentum;
  fExpectedXRingCentre = XRingCentre;
  fExpectedYRingCentre = YRingCentre;
}

void SpecRICHAssociation::Clear() {
  fTrackID = fRingID = -1;
  fTrackTime = fTrackMomentum = fExpectedXRingCentre = fExpectedYRingCentre = 0.0;
  fRadiusMargin = 0.0;
  fRingRadiusEl = fRingRadiusMu = fRingRadiusPi = 0.0;
  fRingAcceptanceEl = fRingAcceptanceMu = fRingAcceptancePi = 0.0;
  fMaxMeanNHits = 0.0;
  fNExpectedHitsEl = fNExpectedHitsMu = fNExpectedHitsPi = 0.0;
  fNObservedHitsEl = fNObservedHitsMu = fNObservedHitsPi = 0;
  fPvalueEl = fPvalueMu = fPvaluePi = 0.0;
  fMinDistanceTrackRing = fMinXDistanceTrackRing = fMinYDistanceTrackRing = 0.0;
  fRingRadiusWrtEl = fRingRadiusWrtMu = fRingRadiusWrtPi = 999.9;
  fRingRadius = fRingChi2 = 0;
}

void SpecRICHAssociation::Print() {}
