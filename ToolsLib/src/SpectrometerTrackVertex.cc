// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-08-25
//
// ---------------------------------------------------------------

/// \class SpectrometerTrackVertex
/// \Brief
/// A vertex formed by spectrometer tracks
/// \EndBrief
/// \Detailed
/// The vertices are built by SpectrometerVertexBuilder, which uses the VertexLSF algorithm.
/// Vertices built can contain different numbers of tracks, as specified in SpectrometerVertexBuilder.
/// \author Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)
/// \EndDetailed

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
  fThreeMomenta0.clear();
}

void SpectrometerTrackVertex::AddTrack(Int_t index, TVector3 Mom, TVector3 Mom0) {
  fTrackIndices.push_back(index);
  fThreeMomenta.push_back(Mom);
  fThreeMomenta0.push_back(Mom0);
  fNTracks = fTrackIndices.size();
}

Int_t SpectrometerTrackVertex::GetTrackIndex(Int_t i) {
  return (i<fNTracks) ? fTrackIndices[i] : -999;
}

TVector3 SpectrometerTrackVertex::GetTrackThreeMomentum(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i] : TVector3(0.0,0.0,0.0);
}

TVector3 SpectrometerTrackVertex::GetTrackThreeMomentum0(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta0[i] : TVector3(0.0,0.0,0.0);
}

Double_t SpectrometerTrackVertex::GetTrackMomentum(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i].Mag() : 0.0;
}

Double_t SpectrometerTrackVertex::GetTrackMomentum0(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta0[i].Mag() : 0.0;
}

Double_t SpectrometerTrackVertex::GetTrackSlopeX(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i].X()/fThreeMomenta[i].Z() : 0.0;
}

Double_t SpectrometerTrackVertex::GetTrackSlopeY(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta[i].Y()/fThreeMomenta[i].Z() : 0.0;
}

Double_t SpectrometerTrackVertex::GetTrackSlopeX0(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta0[i].X()/fThreeMomenta0[i].Z() : 0.0;
}

Double_t SpectrometerTrackVertex::GetTrackSlopeY0(Int_t i) {
  return (i<fNTracks) ? fThreeMomenta0[i].Y()/fThreeMomenta0[i].Z() : 0.0;
}

void SpectrometerTrackVertex::Print() {
  cout <<
    "Vertex: Ntrk= " << fNTracks <<
    " Q= "           << fCharge  <<
    " Z= "           << fPosition.Z()<<
    " P= "           << fTotalThreeMomentum.Mag()<<
    " Chi2= "        << fChi2 <<
    " Tracks=";
  for (Int_t i=0; i<fNTracks; i++) cout << " " << fTrackIndices[i];
  cout << endl;
}
