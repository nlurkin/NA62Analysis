// ---------------------------------------------------------------
// History:
//
// Created by Chris Parkinson (cjp@hep.ph.bham.ac.uk) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#include <iostream>
#include "SpecMUV3Association.hh"

using namespace std;

/// \class SpecMUV3Association
/// \Brief
/// Container of records of MUV3 candidates associated to each Spectrometer track
/// \EndBrief

SpecMUV3Association::SpecMUV3Association() {
  Clear();
}

SpecMUV3Association::SpecMUV3Association (Int_t TrackID) {
  Clear();
  fTrackID = TrackID;
}

Double_t SpecMUV3Association::GetMinimumTrackTileDistance() {
  if (!fAssociationRecordContainer.size()) return 99999;
  Double_t Rmin = 99999;
  for (UInt_t i=0; i<fAssociationRecordContainer.size(); i++) {
    if (fAssociationRecordContainer[i].GetTrackTileDistance() < Rmin) {
      Rmin = fAssociationRecordContainer[i].GetTrackTileDistance();
    }
  }
  return Rmin;
}

Double_t SpecMUV3Association::GetMinimumTrackCandidateDistance() {
  if (!fAssociationRecordContainer.size()) return 99999;
  Double_t Rmin = 99999;
  for (UInt_t i=0; i<fAssociationRecordContainer.size(); i++) {
    if (fAssociationRecordContainer[i].GetTrackCandidateDistance() < Rmin) {
      Rmin = fAssociationRecordContainer[i].GetTrackCandidateDistance();
    }
  }
  return Rmin;
}

void SpecMUV3Association::SetTrackXY(Double_t TrackX, Double_t TrackY) {
  fTrackX = TrackX;
  fTrackY = TrackY;
}

void SpecMUV3Association::SetTrackXY(TVector2 TrackXY) {
  fTrackX = TrackXY.X();
  fTrackY = TrackXY.Y();
}

void SpecMUV3Association::RemoveAssociationRecord(Int_t iRec) {
  if (iRec<0) {
    cout << "SpecMUV3Association: Tried to remove an Association using a negative index." << endl ;
    return;
  }
  fAssociationRecordContainer.erase(fAssociationRecordContainer.begin()+iRec);
}

void SpecMUV3Association::Clear() {
  fTrackID = -1;
  fSearchRadius = -1.0;
  fTrackMomentum = fTrackTime = fTrackX = fTrackY = 0.0;
  fInAcceptance = kFALSE;
  fDistanceToEdge = 0.0;
  fAssociationRecordContainer.clear();
}

void SpecMUV3Association::Print() {
  cout << "SpecMUV3Association: TrackID: " << fTrackID
       << " Time: " << fTrackTime
       << " Mom: " << fTrackMomentum
       << " SearchR: " << fSearchRadius
       << " TrackXY: " << fTrackX << " " << fTrackY
       << " MuRecords: " << fAssociationRecordContainer.size() << endl;
}
