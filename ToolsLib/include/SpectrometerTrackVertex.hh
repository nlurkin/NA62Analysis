// ------------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)    2015-08-25
//
// ------------------------------------------------------------------

#ifndef SPECTROMETERTRACKVERTEX_HH
#define SPECTROMETERTRACKVERTEX_HH 1

#include <TVector3.h>

class SpectrometerTrackVertex {

public:
  SpectrometerTrackVertex();
  virtual ~SpectrometerTrackVertex() {}

  void     Clear();
  void     AddTrack(Int_t index, TVector3 Mom);

  void     SetNTracks (Int_t val)               { fNTracks = val;            }
  void     SetCharge  (Int_t val)               { fCharge   = val;           }
  void     SetPosition(TVector3 val)            { fPosition = val;           }
  void     SetChi2    (Double_t val)            { fChi2     = val;           }
  void     SetTotalThreeMomentum (TVector3 val) { fTotalThreeMomentum = val; }

  Int_t    GetNTracks()              { return fNTracks;                  }
  Int_t    GetCharge()               { return fCharge;                   }
  TVector3 GetPosition()             { return fPosition;                 }
  Double_t GetChi2()                 { return fChi2;                     }
  TVector3 GetTotalThreeMomentum()   { return fTotalThreeMomentum;       }
  Double_t GetTotalMomentum()        { return fTotalThreeMomentum.Mag(); }

  Int_t    GetTrackIndex(Int_t i);
  TVector3 GetTrackThreeMomentum(Int_t i);
  Double_t GetTrackMomentum(Int_t i);
  Double_t GetTrackSlopeX(Int_t i);
  Double_t GetTrackSlopeY(Int_t i);

private:

  Int_t    fNTracks;                  ///< Number of tracks forming the vertex
  Int_t    fCharge;                   ///< Total electrical charge of the tracks
  TVector3 fPosition;                 ///< Vertex position
  Double_t fChi2;                     ///< Vertex fit quality: chi2
  TVector3 fTotalThreeMomentum;       ///< Total three-momentum of all tracks
  std::vector<Int_t>fTrackIndices;    ///< Spectrometer track indices
  std::vector<TVector3>fThreeMomenta; ///< Corrected three-momenta
};

#endif
