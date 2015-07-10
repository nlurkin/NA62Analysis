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
  fMuonID(-1), fTileID(-1), fMuonTime(0), fDirectMatch(false), fDistance(-1), fMuonX(0), fMuonY(0) {}

SpecMUV3AssociationRecord::SpecMUV3AssociationRecord
(Int_t MuonID, Int_t TileID, Double_t Time, Bool_t DirectMatch,
 Double_t Distance, Double_t MuonX, Double_t MuonY) :
  fMuonID(MuonID), fTileID(TileID), fMuonTime(Time), fDirectMatch(DirectMatch),
  fDistance(Distance), fMuonX(MuonX), fMuonY(MuonY) {}

SpecMUV3AssociationRecord::SpecMUV3AssociationRecord
(Int_t MuonID, Int_t TileID, Double_t Time, Bool_t DirectMatch, Double_t Distance, TVector2 MuonXY) :
  fMuonID(MuonID), fTileID(TileID), fMuonTime(Time), fDirectMatch(DirectMatch), 
  fDistance(Distance), fMuonX(MuonXY.X()), fMuonY(MuonXY.Y()) {}

void SpecMUV3AssociationRecord::Clear() {
  fMuonID = fTileID = fDistance = -1;
  fDirectMatch = false;
  fMuonTime = fMuonX = fMuonY = 0.0;
}

void SpecMUV3AssociationRecord::Print() {
  cout << "SpecMUV3MuonRecord: MuonID: " << fMuonID
       << " Tile: " << fTileID
       << " Time: " << fMuonTime
       << " DirectMatch: " << fDirectMatch
       << " MuonXY: " << fMuonX << " " << fMuonY
       << " TrackMuDist: "<< fDistance << endl;
}
