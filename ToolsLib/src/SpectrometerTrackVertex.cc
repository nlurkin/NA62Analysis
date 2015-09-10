// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-08-25
//
// ---------------------------------------------------------------

/// \class SpectrometerTrackVertex
/// \Brief
/// A vertex built from spectrometer tracks using the VertexLSF algorithm
/// \EndBrief

#include <iostream>
#include "SpectrometerTrackVertex.hh"

using namespace std;

SpectrometerTrackVertex::SpectrometerTrackVertex() {
  Clear();
}

void SpectrometerTrackVertex::Clear() {
  fNTracks = 0;
  fTrackIndices.clear();
  fThreeMomenta.clear();
}

void SpectrometerTrackVertex::AddTrack(Int_t index, TVector3 Mom) {
  fTrackIndices.push_back(index);
  fThreeMomenta.push_back(Mom);
  fNTracks = fTrackIndices.size();
}

Int_t SpectrometerTrackVertex::GetTrackIndex(Int_t i) {
  return (i<fNTracks) ? fTrackIndices[i] : -999;
}

TVector3 SpectrometerTrackVertex::GetTrackThreeMomentum(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i] : TVector3(0.0,0.0,0.0);
}

Double_t SpectrometerTrackVertex::GetTrackMomentum(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i].Mag() : 0.0;
}

Double_t SpectrometerTrackVertex::GetTrackSlopeX(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i].X()/fThreeMomenta[i].Z() : 0.0;
}

Double_t SpectrometerTrackVertex::GetTrackSlopeY(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i].Y()/fThreeMomenta[i].Z() : 0.0;
}
