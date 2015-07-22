// ------------------------------------------------------------------
// History:
//
// Created by Anne Chappuis (anne.chappuis9@gmail.com) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)    2015-06-18
//
// ------------------------------------------------------------------

#ifndef BLUETUBETRACKER_HH
#define BLUETUBETRACKER_HH

#include "TMath.h"
#include "BlueTubeMagneticFieldMap.hh"

class BlueTubeTracker {

public:
  BlueTubeTracker();
  ~BlueTubeTracker();

  void TrackParticle();

  void     SetInitialPosition(TVector3 value) { fInitialPosition = value; }
  void     SetInitialPosition(Double_t x, Double_t y, Double_t z)
  { fInitialPosition = TVector3(x,y,z); }
  void     SetInitialMomentum(TVector3 value) { fInitialMomentum = value; }
  void     SetInitialMomentum(Double_t x, Double_t y, Double_t z)
  { fInitialMomentum = TVector3(x,y,z); }
  void     SetZFinal         (Double_t value) { fZFinal = value;          }
  void     SetCharge         (Int_t value)    { fCharge = value;          }

  TVector3 GetInitialPosition()               { return fInitialPosition;  }
  TVector3 GetInitialMomentum()               { return fInitialMomentum;  }
  TVector3 GetFinalPosition()                 { return fFinalPosition;    }
  TVector3 GetFinalPositionNonCorrected();
  TVector3 GetFinalMomentum()                 { return fFinalMomentum;    }

private:

  void TrackOneStep(Int_t, Int_t);

  TVector3 fInitialPosition, fInitialMomentum;
  TVector3 fFinalPosition, fFinalMomentum;
  Double_t fZFinal;
  Int_t    fCharge;

  TVector3 fPosIn, fPosOut, fMomIn, fMomOut;

  BlueTubeMagneticFieldMap *fMap;
  Int_t    fNPlanes;
  Double_t fZmin, fZmax;
  Double_t fZ[40];
  Double_t fBx_param_integral[6][40], fBy_param_integral[4][40], fBz_param0_integral[40];
  Double_t fSpeedOfLight;
};

#endif
