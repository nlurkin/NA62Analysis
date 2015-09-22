// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-09-15
//
// ---------------------------------------------------------------

#ifndef INCLUDE_SPECLKRASSOCIATION_HH
#define INCLUDE_SPECLKRASSOCIATION_HH 1

#include "TMath.h"

class SpecLKrAssociation {

public:

  SpecLKrAssociation();
  SpecLKrAssociation(Int_t);
  ~SpecLKrAssociation() {}

  Int_t    GetTrackID()               { return fTrackID;       }
  Int_t    GetClusterID()             { return fClusterID;     }
  Double_t GetClusterEnergy()         { return fClusterEnergy; }
  Double_t GetTrackMomentum()         { return fTrackMomentum; }
  Double_t GetEoP()                   { return fEoP;           }

  void SetTrackID(Int_t val)          { fTrackID = val;        }
  void SetClusterID(Int_t val)        { fClusterID = val;      }
  void SetClusterEnergy(Double_t val) { fClusterEnergy = val;  }
  void SetTrackMomentum(Double_t val) { fTrackMomentum = val;  }
  void SetEoP(Double_t val)           { fEoP = val;            }

  void Clear();
  void Print();

private:

  Int_t    fTrackID;       ///< ID of the spectrometer track
  Int_t    fClusterID;     ///< ID of the associated LKr cluster
  Double_t fClusterEnergy; ///< Cluster energy [MeV]
  Double_t fTrackMomentum; ///< Track momentum [MeV/c]
  Double_t fEoP;           ///< E/p
};

#endif
