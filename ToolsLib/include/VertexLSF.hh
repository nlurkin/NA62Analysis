#ifndef VertexLSF_H
#define VertexLSF_H

#include "TMatrixD.h"
#include "TVector3.h"
#include "TClonesArray.h"

class VertexLSF {

public:

           VertexLSF(Int_t ntrk=10);
           ~VertexLSF();
  void     Reset();
  void     AddTrack(TMatrixD cov, TMatrixD par);
  void     SetNtrks(Int_t ntrk) {fNtrks = ntrk;  }
  void     SetZref(Double_t zref)  {fZref = zref;  }
  void     SetVertex(TVector3 val) { fVertex = val; }
  void     UpdateABMatrix();
  void     UpdateTrackParams();
  void     UpdateCovC0();
  Bool_t   GetTrackMomentum(TVector3 &track, Int_t i);
  TVector3 GetVertex()             { return fVertex; }
  TMatrixD GetCovC0i(Int_t i);
  TMatrixD GetCovCij(Int_t i, Int_t j);
  TMatrixD GetCovC0iConstr(Int_t i);
  TMatrixD GetCovCijConstr(Int_t i, Int_t j);
  Double_t GetChi2() {return fChi2;}
  void     Fit();
  Double_t Chi2();
  Bool_t   FitStraightTracksNoBlueField(Int_t iter = 3);
  void     Apply3MomConstraints(Double_t px = 0., Double_t py = 0., Double_t pz = 75000., Double_t dpx = 1.2, Double_t dpy = 1.2, Double_t dpz = 150. );
 
private:


  Double_t fChi2;  ///< \f$\chi^{2}\f$ 
  Int_t fNtrks;    ///< Number of selected tracks for the vertex fit
  Double_t fZref;  ///< Z position of the track parametrisation reference plane
  TVector3 fVertex;///< Vertex coordinates
  TClonesArray fV; ///< TClonesArray of covariance matrices of spectrometer tracks
  TClonesArray fT; ///< Tracks parameters from spectrometer track fit (dX/dZ, dY/dZ, X, Y, P)
  TClonesArray fT0;///< Track parameters calculated with updated vertex coordinates (X, Y, Z) and track (dX/dZ, dY/dZ, P)
  TClonesArray fdT;///< T - T0
  TClonesArray fA; ///< Transformation matrix between spectrometer track parameters and vertex coordinates 
  TMatrixD fB;     ///< Transformation matrix between spectrometer track parameters and track momentum
  TClonesArray fM; ///< Tracks momentum components (dX/dZ, dY/dZ, P)
  TClonesArray fW; ///< Weight matrix

  // operational matrices
  TClonesArray fF; ///< AT*W*A
  TClonesArray fD; ///< AT*W*B
  TClonesArray fE; ///< BT*W*B
  TClonesArray fEInv;
  TClonesArray fK;
  TClonesArray fK4;
  TMatrixD fC0;
  TMatrixD fD0;
  TMatrixD fCovK;
  TMatrixD fCovK4;
  TMatrixD fR;
  TMatrixD fH;

};

#endif
