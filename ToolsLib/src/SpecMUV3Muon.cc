// ---------------------------------------------------------------
// History:
//
// Created by Chris Parkinson (cjp@hep.ph.bham.ac.uk) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#include <iostream>
#include "SpecMUV3Muon.hh"

using namespace std;

/// \class SpecMUV3Muon
/// \Brief
/// Container of records of MUV3 candidates associated to a Spectrometer track
/// \EndBrief

SpecMUV3Muon::SpecMUV3Muon() :
  fTrackID(-1), fTrackTime(0), fTrackMomentum(0), fSearchRadius(-1), fTrackX(0), fTrackY(0) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Momentum, Double_t Radius, Double_t TrackX, Double_t TrackY) :
  fTrackID(TrackID), fTrackTime(Time), fTrackMomentum(Momentum),
  fSearchRadius(Radius), fTrackX(TrackX), fTrackY(TrackY) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Momentum, Double_t Radius, TVector2 TrackXY) :
  fTrackID(TrackID), fTrackTime(Time), fTrackMomentum(Momentum),
  fSearchRadius(Radius), fTrackX(TrackXY.X()), fTrackY(TrackXY.Y()) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Momentum, Double_t Radius, Double_t TrackX, Double_t TrackY,
 std::vector<SpecMUV3AssociationRecord> Rec) :
  fTrackID(TrackID), fTrackTime(Time), fTrackMomentum(Momentum), fSearchRadius(Radius),
  fTrackX(TrackX), fTrackY(TrackY), fAssociationRecordContainer(Rec) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Momentum, Double_t Radius, TVector2 TrackXY,
 std::vector<SpecMUV3AssociationRecord> Rec) :
  fTrackID(TrackID), fTrackTime(Time), fTrackMomentum(Momentum), fSearchRadius(Radius),
  fTrackX(TrackXY.X()), fTrackY(TrackXY.Y()), fAssociationRecordContainer(Rec) {}

Double_t SpecMUV3Muon::GetMinimumTrackTileDistance() {
  if (!fAssociationRecordContainer.size()) return 99999;
  Double_t Rmin = 99999;
  for (UInt_t i=0; i<fAssociationRecordContainer.size(); i++) {
    if (fAssociationRecordContainer[i].GetTrackTileDistance() < Rmin) {
      Rmin = fAssociationRecordContainer[i].GetTrackTileDistance();
    }
  }
  return Rmin;
}

Double_t SpecMUV3Muon::GetMinimumTrackCandidateDistance() {
  if (!fAssociationRecordContainer.size()) return 99999;
  Double_t Rmin = 99999;
  for (UInt_t i=0; i<fAssociationRecordContainer.size(); i++) {
    if (fAssociationRecordContainer[i].GetTrackCandidateDistance() < Rmin) {
      Rmin = fAssociationRecordContainer[i].GetTrackCandidateDistance();
    }
  }
  return Rmin;
}

void SpecMUV3Muon::SetTrackXY(Double_t TrackX, Double_t TrackY) {
  fTrackX = TrackX;
  fTrackY = TrackY;
}

void SpecMUV3Muon::SetTrackXY(TVector2 TrackXY) {
  fTrackX = TrackXY.X();
  fTrackY = TrackXY.Y();
}

void SpecMUV3Muon::RemoveAssociationRecord(Int_t iRec) {
  if (iRec<0) {
    cout << "SpecMUV3Muon: Tried to remove a Muon using a negative index." << endl ;
  }
  else {
    fAssociationRecordContainer.erase(fAssociationRecordContainer.begin()+iRec);
  }
}

void SpecMUV3Muon::Clear() {
  fTrackID = -1;
  fSearchRadius = -1.0;
  fTrackTime = fTrackX = fTrackY = 0.0;
  fAssociationRecordContainer.clear();
}

void SpecMUV3Muon::Print() {
  cout << "SpecMUV3Muon: TrackID: " << fTrackID
       << " Time: " << fTrackTime
       << " Mom: " << fTrackMomentum
       << " SearchR: " << fSearchRadius
       << " TrackXY: " << fTrackX << " " << fTrackY
       << " MuRecords: " << fAssociationRecordContainer.size() << endl;
}
