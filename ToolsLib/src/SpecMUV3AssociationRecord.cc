// ---------------------------------------------------------------
// History:
//
// Created by Chris Parkinson (cjp@hep.ph.bham.ac.uk) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#include <iostream>
#include "SpecMUV3AssociationRecord.hh"

/// \class SpecMUV3AssociationRecord
/// \Brief
/// A record of a MUV3 candidate associated to a Spectrometer track
/// \EndBrief

using namespace std;

SpecMUV3AssociationRecord::SpecMUV3AssociationRecord() :
  fMuonID(-1), fTileID(-1), fMuonTime(0),
  fTrackTileDistance(-1), fTrackCandidateDistance(-1), fMuonX(0), fMuonY(0) {}

SpecMUV3AssociationRecord::SpecMUV3AssociationRecord
(Int_t MuonID, Int_t TileID, Double_t Time,
 Double_t TrackTileDistance, Double_t TrackCandidateDistance, Double_t MuonX, Double_t MuonY) :
  fMuonID(MuonID), fTileID(TileID), fMuonTime(Time),
  fTrackTileDistance(TrackTileDistance), fTrackCandidateDistance(TrackCandidateDistance),
  fMuonX(MuonX), fMuonY(MuonY) {}

SpecMUV3AssociationRecord::SpecMUV3AssociationRecord
(Int_t MuonID, Int_t TileID, Double_t Time,
 Double_t TrackTileDistance, Double_t TrackCandidateDistance, TVector2 MuonXY) :
  fMuonID(MuonID), fTileID(TileID), fMuonTime(Time),
  fTrackTileDistance(TrackTileDistance), fTrackCandidateDistance(TrackCandidateDistance),
  fMuonX(MuonXY.X()), fMuonY(MuonXY.Y()) {}

void SpecMUV3AssociationRecord::Clear() {
  fMuonID = fTileID = fTrackTileDistance = fTrackCandidateDistance = -1;
  fMuonTime = fMuonX = fMuonY = 0.0;
}

void SpecMUV3AssociationRecord::Print() {
  cout << "SpecMUV3MuonRecord: MuonID: " << fMuonID
       << " Tile: " << fTileID
       << " Time: " << fMuonTime
       << " MuonXY: " << fMuonX << " " << fMuonY
       << " D(TrTile): "<< fTrackTileDistance
       << " D(TrMu): "<< fTrackCandidateDistance << endl;
}
