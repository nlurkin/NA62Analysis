// ---------------------------------------------------------------
// History:
//
// Created by Chris Parkinson (cjp@hep.ph.bham.ac.uk) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#ifndef INCLUDE_SPECMUV3MUON_HH
#define INCLUDE_SPECMUV3MUON_HH 1

#include "SpecMUV3AssociationRecord.hh"

class SpecMUV3Muon {

public:

  SpecMUV3Muon();
  SpecMUV3Muon(Int_t, Double_t, Double_t, Double_t, Double_t, Double_t);
  SpecMUV3Muon(Int_t, Double_t, Double_t, Double_t, TVector2);
  SpecMUV3Muon(Int_t, Double_t, Double_t, Double_t, Double_t, Double_t, std::vector<SpecMUV3AssociationRecord>);
  SpecMUV3Muon(Int_t, Double_t, Double_t, Double_t, TVector2, std::vector<SpecMUV3AssociationRecord>);

  ~SpecMUV3Muon() {}

  Int_t    GetTrackID()       { return fTrackID;                   }
  Double_t GetTrackTime()     { return fTrackTime;                 }
  Double_t GetTrackMomentum() { return fTrackMomentum;             }
  Double_t GetSearchRadius()  { return fSearchRadius;              }
  Double_t GetTrackX()        { return fTrackX;                    }
  Double_t GetTrackY()        { return fTrackY;                    }
  TVector2 GetTrackXY()       { return TVector2(fTrackX, fTrackY); }
  Double_t GetMinimumTrackTileDistance();
  Double_t GetMinimumTrackCandidateDistance();

  Int_t    GetNAssociationRecords() { return fAssociationRecordContainer.size(); }
  std::vector<SpecMUV3AssociationRecord> GetMuonContainer() { return fAssociationRecordContainer; }
  SpecMUV3AssociationRecord GetAssociationRecord(Int_t i) { return fAssociationRecordContainer[i]; }

  void SetTrackID(Int_t val)         { fTrackID = val;       }
  void SetTrackMomentum(Int_t val)   { fTrackMomentum = val; }
  void SetTrackTime(Int_t val)       { fTrackTime = val;     }
  void SetSearchRadius(Double_t val) { fSearchRadius = val;  }
  void SetTrackX(Double_t val)       { fTrackX = val;        }
  void SetTrackY(Double_t val)       { fTrackY = val;        }
  void SetTrackXY(Double_t, Double_t);
  void SetTrackXY(TVector2);

  void SetAssociationContainer(std::vector<SpecMUV3AssociationRecord> val) { fAssociationRecordContainer = val; }
  void AddAssociationRecord(SpecMUV3AssociationRecord Rec) { fAssociationRecordContainer.push_back(Rec); }
  void RemoveAssociationRecord(Int_t);
  void Clear();
  void Print();

private:

  Int_t    fTrackID;       ///< ID of the spectrometer track
  Double_t fTrackTime;     ///< Track time [ns]
  Double_t fTrackMomentum; ///< Track momentum [MeV]
  Double_t fSearchRadius;  ///< MUV3 candidate tile search radius around the extrapolated track
  Double_t fTrackX;        ///< X coordinate of track extrapolated to MUV3 tile front plane
  Double_t fTrackY;        ///< Y coordinate of track extrapolated to MUV3 tile front plane
  std::vector<SpecMUV3AssociationRecord> fAssociationRecordContainer; ///< Records of matching MUV3 candidates
};

#endif
