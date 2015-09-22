// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-09-15
//
// ---------------------------------------------------------------

#include <iostream>
#include "SpecLKrAssociation.hh"

using namespace std;

/// \class SpecLKrAssociation
/// \Brief
/// Container of LKr association records for each Spectrometer track
/// \EndBrief

SpecLKrAssociation::SpecLKrAssociation() {
  Clear();
}
  
SpecLKrAssociation::SpecLKrAssociation(Int_t TrackID) {
  Clear();
  fTrackID = TrackID;
}

void SpecLKrAssociation::Clear() {
  fTrackID = fClusterID = -1;
  fClusterEnergy = fTrackMomentum = -1.0;
  fEoP = 0.0;
}

void SpecLKrAssociation::Print() {
  cout << "SpecLKrAssociation: TrackID: " << fTrackID
       << " ClusterID: " << fClusterID
       << " E= " << fClusterEnergy
       << " p= " << fTrackMomentum
       << " E/p= " << fEoP << endl;
}
