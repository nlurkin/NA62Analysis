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
/// An object containing the records of MUV3 candidates associated to a Spectrometer track
/// \EndBrief

SpecMUV3Muon::SpecMUV3Muon() :
  fTrackID(-1), fTrackTime(0), fSearchRadius(-1), fTrackX(0), fTrackY(0) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Radius, Double_t TrackX, Double_t TrackY) :
  fTrackID(TrackID), fTrackTime(Time), fSearchRadius(Radius), fTrackX(TrackX), fTrackY(TrackY) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Radius, TVector2 TrackXY) :
  fTrackID(TrackID), fTrackTime(Time), fSearchRadius(Radius), fTrackX(TrackXY.X()), fTrackY(TrackXY.Y()) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Radius, Double_t TrackX, Double_t TrackY,
 std::vector<SpecMUV3AssociationRecord> Rec) :
  fTrackID(TrackID), fTrackTime(Time), fSearchRadius(Radius),
  fTrackX(TrackX), fTrackY(TrackY), fAssociationRecordContainer(Rec) {}

SpecMUV3Muon::SpecMUV3Muon
(Int_t TrackID, Double_t Time, Double_t Radius, TVector2 TrackXY, std::vector<SpecMUV3AssociationRecord> Rec) :
  fTrackID(TrackID), fTrackTime(Time), fSearchRadius(Radius),
  fTrackX(TrackXY.X()), fTrackY(TrackXY.Y()), fAssociationRecordContainer(Rec) {}

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
       << " SearchRadius: " << fSearchRadius
       << " TrackXY: " << fTrackX << " " << fTrackY
       << " MuRecords: " << fAssociationRecordContainer.size() << endl;
}
