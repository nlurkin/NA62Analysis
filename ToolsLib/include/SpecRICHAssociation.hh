// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-27
//
// ---------------------------------------------------------------

#ifndef INCLUDE_SPECRICHASSOCIATION_HH
#define INCLUDE_SPECRICHASSOCIATION_HH 1

#include "TMath.h"

class SpecRICHAssociation {

public:

  SpecRICHAssociation();
  SpecRICHAssociation(Int_t, Double_t, Double_t, Double_t, Double_t);
  ~SpecRICHAssociation() {}

  Int_t    GetTrackID()             { return fTrackID;             }
  Int_t    GetRingID()              { return fRingID;              }
  Double_t GetTrackTime()           { return fTrackTime;           }
  Double_t GetTrackMomentum()       { return fTrackMomentum;       }
  Double_t GetExpectedXRingCentre() { return fExpectedXRingCentre; }
  Double_t GetExpectedYRingCentre() { return fExpectedYRingCentre; }
  Double_t GetRadiusMargin()        { return fRadiusMargin;        }

  Double_t GetRingRadiusEl()     { return fRingRadiusEl;     }
  Double_t GetRingRadiusMu()     { return fRingRadiusMu;     }
  Double_t GetRingRadiusPi()     { return fRingRadiusPi;     }
  Double_t GetRingAcceptanceEl() { return fRingAcceptanceEl; }
  Double_t GetRingAcceptanceMu() { return fRingAcceptanceMu; }
  Double_t GetRingAcceptancePi() { return fRingAcceptancePi; }

  Double_t GetMaxMeanNHits()     { return fMaxMeanNHits;     }
  Double_t GetNExpectedHitsEl()  { return fNExpectedHitsEl;  }
  Double_t GetNExpectedHitsMu()  { return fNExpectedHitsMu;  }
  Double_t GetNExpectedHitsPi()  { return fNExpectedHitsPi;  }
  Int_t    GetNObservedHitsEl()  { return fNObservedHitsEl;  }
  Int_t    GetNObservedHitsMu()  { return fNObservedHitsMu;  }
  Int_t    GetNObservedHitsPi()  { return fNObservedHitsPi;  }

  Double_t GetPvalueEl()         { return fPvalueEl;         }
  Double_t GetPvalueMu()         { return fPvalueMu;         }
  Double_t GetPvaluePi()         { return fPvaluePi;         }

  Double_t GetMinDistanceTrackRing()  { return fMinDistanceTrackRing;  }
  Double_t GetMinXDistanceTrackRing() { return fMinXDistanceTrackRing; }
  Double_t GetMinYDistanceTrackRing() { return fMinYDistanceTrackRing; }

  Double_t GetRingRadius()       { return fRingRadius;       }
  Double_t GetRingChi2()         { return fRingChi2;         }
  Double_t GetRingRadiusWrtEl()  { return fRingRadiusWrtEl;  }
  Double_t GetRingRadiusWrtMu()  { return fRingRadiusWrtMu;  }
  Double_t GetRingRadiusWrtPi()  { return fRingRadiusWrtPi;  }

  Bool_t   GetElectron1()        { return fElectron1;        }
  Bool_t   GetMuon1()            { return fMuon1;            }
  Bool_t   GetPion1()            { return fPion1;            }
  Bool_t   GetElectron2()        { return fElectron2;        }
  Bool_t   GetMuon2()            { return fMuon2;            }
  Bool_t   GetPion2()            { return fPion2;            }

  void SetTrackID(Int_t val)                { fTrackID = val;             }
  void SetRingID(Int_t val)                 { fRingID = val;              }
  void SetTrackMomentum(Int_t val)          { fTrackMomentum = val;       }
  void SetTrackTime(Int_t val)              { fTrackTime = val;           }
  void SetExpectedXRingCentre(Double_t val) { fExpectedXRingCentre = val; }
  void SetExpectedYRingCentre(Double_t val) { fExpectedYRingCentre = val; }
  void SetRadiusMargin(Double_t val)        { fRadiusMargin = val;        }

  void SetRingRadiusEl    (Double_t val) { fRingRadiusEl = val;     }
  void SetRingRadiusMu    (Double_t val) { fRingRadiusMu = val;     }
  void SetRingRadiusPi    (Double_t val) { fRingRadiusPi = val;     }
  void SetRingAcceptanceEl(Double_t val) { fRingAcceptanceEl = val; }
  void SetRingAcceptanceMu(Double_t val) { fRingAcceptanceMu = val; }
  void SetRingAcceptancePi(Double_t val) { fRingAcceptancePi = val; }

  void SetMaxMeanNHits   (Double_t val)  { fMaxMeanNHits    = val;  }
  void SetNExpectedHitsEl(Double_t val)  { fNExpectedHitsEl = val;  }
  void SetNExpectedHitsMu(Double_t val)  { fNExpectedHitsMu = val;  }
  void SetNExpectedHitsPi(Double_t val)  { fNExpectedHitsPi = val;  }
  void SetNObservedHitsEl(Int_t val)     { fNObservedHitsEl = val;  }
  void SetNObservedHitsMu(Int_t val)     { fNObservedHitsMu = val;  }
  void SetNObservedHitsPi(Int_t val)     { fNObservedHitsPi = val;  }

  void SetMinDistanceTrackRing (Double_t val) { fMinDistanceTrackRing = val;  }
  void SetMinXDistanceTrackRing(Double_t val) { fMinXDistanceTrackRing = val; }
  void SetMinYDistanceTrackRing(Double_t val) { fMinYDistanceTrackRing = val; }
  void SetRingRadius     (Double_t val)       { fRingRadius      = val;       }
  void SetRingChi2       (Double_t val)       { fRingChi2        = val;       }
  void SetRingRadiusWrtEl(Double_t val)       { fRingRadiusWrtEl = val;       }
  void SetRingRadiusWrtMu(Double_t val)       { fRingRadiusWrtMu = val;       }
  void SetRingRadiusWrtPi(Double_t val)       { fRingRadiusWrtPi = val;       } 

  void SetPvalueEl(Double_t val)         { fPvalueEl = val;         }
  void SetPvalueMu(Double_t val)         { fPvalueMu = val;         }
  void SetPvaluePi(Double_t val)         { fPvaluePi = val;         }

  void SetElectron1(Bool_t   val)        { fElectron1 = val;        }
  void SetMuon1    (Bool_t   val)        { fMuon1 = val;            }
  void SetPion1    (Bool_t   val)        { fPion1 = val;            }
  void SetElectron2(Bool_t   val)        { fElectron2 = val;        }
  void SetMuon2    (Bool_t   val)        { fMuon2 = val;            }
  void SetPion2    (Bool_t   val)        { fPion2 = val;            }

  void Clear();
  void Print();

private:

  Int_t    fTrackID;             ///< ID of the spectrometer track
  Int_t    fRingID;              ///< ID of the associated RICH ring
  Double_t fTrackTime;           ///< Track time [ns]
  Double_t fTrackMomentum;       ///< Track momentum [MeV]
  Double_t fExpectedXRingCentre; ///< Expected X coordinate of RICH ring centre
  Double_t fExpectedYRingCentre; ///< Expected Y coordinate of RICH ring centre
  Double_t fRadiusMargin;

  Double_t fRingRadiusEl;
  Double_t fRingRadiusMu;
  Double_t fRingRadiusPi;
  Double_t fRingAcceptanceEl;
  Double_t fRingAcceptanceMu;
  Double_t fRingAcceptancePi;

  Double_t fMaxMeanNHits;
  Double_t fNExpectedHitsEl;
  Double_t fNExpectedHitsMu;
  Double_t fNExpectedHitsPi;
  Int_t    fNObservedHitsEl;
  Int_t    fNObservedHitsMu;
  Int_t    fNObservedHitsPi;

  Double_t fPvalueEl;
  Double_t fPvalueMu;
  Double_t fPvaluePi;

  Double_t fMinDistanceTrackRing;
  Double_t fMinXDistanceTrackRing;
  Double_t fMinYDistanceTrackRing;

  Double_t fRingRadius;
  Double_t fRingChi2;
  Double_t fRingRadiusWrtEl;
  Double_t fRingRadiusWrtMu;
  Double_t fRingRadiusWrtPi;

  Bool_t   fElectron1;
  Bool_t   fMuon1;
  Bool_t   fPion1;
  Bool_t   fElectron2;
  Bool_t   fMuon2;
  Bool_t   fPion2;
};

#endif
