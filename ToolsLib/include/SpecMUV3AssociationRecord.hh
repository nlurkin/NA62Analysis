// ---------------------------------------------------------------
// History:
//
// Created by Chris Parkinson (cjp@hep.ph.bham.ac.uk) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#ifndef INCLUDE_SPECMUV3ASSOCIATIONRECORD_HH 
#define INCLUDE_SPECMUV3ASSOCIATIONRECORD_HH 1

#include "TVector2.h"

class SpecMUV3AssociationRecord {
  
public:

  SpecMUV3AssociationRecord();
  SpecMUV3AssociationRecord (Int_t, Int_t, Double_t, Double_t, Double_t, Double_t, Double_t);
  SpecMUV3AssociationRecord (Int_t, Int_t, Double_t, Double_t, Double_t, TVector2);

  virtual ~SpecMUV3AssociationRecord() {}

  Int_t    GetMuonID()                 { return fMuonID;                  }
  Int_t    GetTileID()                 { return fTileID;                  }
  Double_t GetMuonTime()               { return fMuonTime;                }
  Double_t GetTrackTileDistance()      { return fTrackTileDistance;       }
  Double_t GetTrackCandidateDistance() { return fTrackCandidateDistance;  }
  Bool_t   GetDirectMatch()            { return (fTrackTileDistance<0);   }
  Double_t GetMuonX()                  { return fMuonX;                   }
  Double_t GetMuonY()                  { return fMuonY;                   }
  TVector2 GetMuonXY()                 { return TVector2(fMuonX, fMuonY); }

  void SetMuonID     (Int_t val)                { fMuonID = val;                      }
  void SetTileID     (Int_t val)                { fTileID = val;                      }
  void SetMuonTime   (Double_t val)             { fMuonTime = val;                    }
  void SetTrackTileDistance (Double_t val)      { fTrackTileDistance = val;           }
  void SetTrackCandidateDistance (Double_t val) { fTrackCandidateDistance = val;      }
  void SetMuonX      (Double_t val)             { fMuonX = val;                       }
  void SetMuonY      (Double_t val)             { fMuonY = val;                       }
  void SetMuonXY     (Double_t x, Double_t y)   { fMuonX = x; fMuonY = y;             }
  void SetMuonXY     (TVector2 val)             { fMuonX = val.X(); fMuonY = val.Y(); }

  void Clear();
  void Print();

private:

  Int_t    fMuonID;      ///< ID of the associated MUV3 candidate
  Int_t    fTileID;      ///< MUV3 candidate tile ID
  Double_t fMuonTime;    ///< MUV3 candidate time
  Double_t fTrackTileDistance; ///< Distance between extrapolated track and tile boundary (-1 if inside tile)
  Double_t fTrackCandidateDistance; ///< Distance between extrapolated track and centre of MUV3 tile
  Double_t fMuonX;       ///< Centre of MUV3 tile: X coordinate
  Double_t fMuonY;       ///< Centre of MUV3 tile: Y coordinate
};

#endif
