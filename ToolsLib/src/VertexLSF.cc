/// \class VertexLSF
/// \Brief 
/// Least-squares fit of vertex parameters.
/// \EndBrief
///
/// \Detailed
/// As input to the vertex fit are used the reconstructed parameters (dx/dz, dy/dz, x, y, P) and \n
/// cov matrix of the tracks at a given reference plane (e.g. first spectrometer station). The output \n
/// is the vertex position and the (improved) momentum of each track (as well as the corresponding \n
/// covariance matrix and chi2). \n 
/// The method is based on P.Billoir, R.Fruhwirth, M.Regler, Nuc.Inst.Meth. A241(1985) 115. \n
/// Tracks are parametrised by a 5D vector xi, where i is the track number. The vertex position is \n
/// given by X and the track momentum vector by qi. The measurement model h(X,qi) expresses the \n 
/// parameters of track i as a function of X and qi. The chi2 of the vertex fit is defined as: \n
///   chi2 = sum(riT*G*ri) where ri = xi - h(X,qi). \n
/// The measurement model is linearised: \n
///   h(X,qi) = h(X0, qi0) + A(X-X0) + B(qi-qi0)  \n
/// The derivatives A and B follow from the choice of track parametrisation which can be seen in \n
/// UpdateTrackParams() and UpdateABMatrix(). \n 
/// The solution of the fit is the set of parameters (X, q1,..,qn) which minimises the chi2. \n
/// The procedure is iterative. After each iteration the track parameters and derivatives matrices \n
/// are updated at the new vertex position. \n
/// A note describing the method is in preparation. \n
/// This method is called by SpectrometerVertexBuilder to build the standard arrays of vertices. \n
/// \todo 1. Include the track propagation through the blue tube field in the method.
/// \author Plamen Petrov (plpetrov@cern.ch)
/// \EndDetailed
#include "VertexLSF.hh"
#include "MultipleLinesCDA.hh"
#include "TMatrixD.h"
#include <cmath>
#include <iostream>

using namespace std;

VertexLSF::VertexLSF(Int_t ntrk) {
    /// \MemberDescr 
    /// Vertex constructor
    /// \param ntrk     The number of tracks in the fit. Must be >= 2 for the fit to work.
    
  fVertex = TVector3(0,0,0);
  fZref  = 180000.;
  fNtrks  = 0;
  fChi2   = 0.;
  fV.SetClass("TMatrixD", ntrk); 
  fT.SetClass("TMatrixD", ntrk);
  fT0.SetClass("TMatrixD", ntrk);
  fdT.SetClass("TMatrixD", ntrk);
  fM.SetClass("TMatrixD", ntrk);
  fW.SetClass("TMatrixD", ntrk);
  fE.SetClass("TMatrixD", ntrk);
  fD.SetClass("TMatrixD", ntrk);
  fEInv.SetClass("TMatrixD", ntrk);
  fF.SetClass("TMatrixD", ntrk);
  fA.SetClass("TMatrixD", ntrk);
  fK.SetClass("TMatrixD", ntrk);
  fK4.SetClass("TMatrixD", ntrk);
  fB.ResizeTo(5,3);
  fD0.ResizeTo(3,3);
  fC0.ResizeTo(3,3);
  fCovK.ResizeTo(3,3);
  fCovK4.ResizeTo(4,4);
  fH.ResizeTo(4,4);
}

VertexLSF::~VertexLSF() {
  if (fNtrks) {
    fV.Clear("C"); 
    fT.Clear("C");
    fT0.Clear("C");
    fdT.Clear("C");
    fM.Clear("C");
    fW.Clear("C");
    fE.Clear("C");
    fD.Clear("C");
    fEInv.Clear("C");
    fF.Clear("C");
    fA.Clear("C");
    fK.Clear("C");
    fK4.Clear("C");
    fNtrks = 0;
    fChi2 = 0.;
    fZref = 180000.;
  }
}

void VertexLSF::Reset() {
    /// \MemberDescr 
    /// Reset all matrices and arrays to reuse the object in the next event.
  if (fNtrks) {
    fVertex = TVector3(0,0,0);
    fV.Clear("C"); 
    fT.Clear("C");
    fT0.Clear("C");
    fdT.Clear("C");
    fM.Clear("C");
    fW.Clear("C");
    fE.Clear("C");
    fD.Clear("C");
    fEInv.Clear("C");
    fF.Clear("C");
    fA.Clear("C");
    fK.Clear("C");
    fK4.Clear("C");
    fB.Clear();
    fD0.Clear();
    fC0.Clear();
    fCovK.Clear();
    fCovK4.Clear();
    fH.Clear();
    fB.ResizeTo(5,3);
    fD0.ResizeTo(3,3);
    fC0.ResizeTo(3,3);
    fCovK.ResizeTo(3,3);
    fCovK4.ResizeTo(4,4);
    fH.ResizeTo(4,4);
    fNtrks = 0;
    fChi2 = 0.;
    fZref = 180000.;
  }
}

void VertexLSF::AddTrack(TMatrixD cov, TMatrixD par) {
    /// \MemberDescr 
    /// Add a track candidate to the fit.
    /// \param par    The track candidate parameters at the reference plane fZref as they are measured by the spectrometer Kalman fit.
    /// \param cov    Covariance matrix of the track parameters.
  new(fV[fNtrks]) TMatrixD(5,5); 
  new(fT[fNtrks]) TMatrixD(5,1); 
  new(fM[fNtrks]) TMatrixD(3,1);
  new(fT0[fNtrks]) TMatrixD(5,1);
  new(fdT[fNtrks]) TMatrixD(5,1);
  new(fA[fNtrks]) TMatrixD(5,3);
  new(fK[fNtrks]) TMatrixD(3,3);
  new(fK4[fNtrks]) TMatrixD(4,3);
  new(fW[fNtrks]) TMatrixD(5,5);
  new(fE[fNtrks]) TMatrixD(3,3);
  new(fF[fNtrks]) TMatrixD(3,3);
  new(fD[fNtrks]) TMatrixD(3,3);
  new(fE[fNtrks]) TMatrixD(3,3);
  new(fEInv[fNtrks]) TMatrixD(3,3);
  TMatrixD &V = *(TMatrixD*)fV[fNtrks];
  V = cov;
  TMatrixD &W = *(TMatrixD*)fW[fNtrks];
  W = cov.Invert();
  TMatrixD &Par = *(TMatrixD*)fT[fNtrks];
  Par = par;
  TMatrixD &M = *(TMatrixD*)fM[fNtrks];
  M(0,0) = par(0,0);
  M(1,0) = par(1,0);
  M(2,0) = par(4,0);
  fNtrks++;
}

void VertexLSF::UpdateABMatrix() {
    /// \MemberDescr 
    /// Update the derivatives matrices A and B.
  for (Int_t i=0; i<fNtrks; i++) {
    TMatrixD &A = *(TMatrixD*)fA[i];
    TMatrixD &M = *(TMatrixD*)fM[i];
    TMatrixD &W = *(TMatrixD*)fW[i];
    TMatrixD &F = *(TMatrixD*)fF[i];
    TMatrixD &D = *(TMatrixD*)fD[i];
    TMatrixD &E = *(TMatrixD*)fE[i];
    TMatrixD &EInv = *(TMatrixD*)fEInv[i];
    A(2,0) = 1.;
    A(2,2) = (-1.)*M(0,0);      
    A(3,1) = 1.;
    A(3,2) = (-1.)*M(1,0);
    fB(0,0) = 1.;
    fB(1,1) = 1.;
    fB(2,0) = fZref - fVertex(2);
    fB(3,1) = fZref - fVertex(2);
    fB(4,2) = 1.;    
    TMatrixD ATW(A, TMatrixD::kTransposeMult,W);
    TMatrixD BTW(fB, TMatrixD::kTransposeMult,W);
    F = ATW*A;
    D = ATW*fB;
    E = BTW*fB;
    EInv = E.Invert();
    fD0 += F;
  }
}

void VertexLSF::UpdateTrackParams() {
    /// \MemberDescr 
    /// Calculate the track parameters with the
    /// updated vertex position and track momentum.
  for (Int_t i=0; i<fNtrks; i++) {
    TMatrixD &M = *(TMatrixD*)fM[i];
    TMatrixD &T0 = *(TMatrixD*)fT0[i];
    TMatrixD &T = *(TMatrixD*)fT[i];
    TMatrixD &dT = *(TMatrixD*)fdT[i];
    T0(0,0) = M(0,0);
    T0(1,0) = M(1,0);
    T0(2,0) = fVertex(0) + (fZref - fVertex(2))*M(0,0);
    T0(3,0) = fVertex(1) + (fZref - fVertex(2))*M(1,0);
    T0(4,0) = M(2,0);
    dT.Minus(T,T0);
  }  
}

void VertexLSF::Fit() {
    /// \MemberDescr 
    /// Run the LS fit of the vertex position.
  TMatrixD SumDEInvDT(3,3);        
  TMatrixD SumZ(3,1);
  TMatrixD I5(5,5);
  for (int i=0;i<5; i++) I5(i,i)=1.;
  for (int i=0; i<fNtrks; i++){
    TMatrixD &W = *(TMatrixD*)fW[i];
    TMatrixD &A = *(TMatrixD*)fA[i];
    TMatrixD &dT = *(TMatrixD*)fdT[i];
    TMatrixD &EInv = *(TMatrixD*)fEInv[i];
    TMatrixD BEInv(fB, TMatrixD::kMult, EInv);     
    TMatrixD BEInvBTW(BEInv, TMatrixD::kMultTranspose, fB);      
    BEInvBTW *= W;
    BEInvBTW *= -1.;
    BEInvBTW += I5; 
    TMatrixD ATW(A, TMatrixD::kTransposeMult,W);
    TMatrixD X(ATW, TMatrixD::kMult, BEInvBTW);
    TMatrixD Z(X, TMatrixD::kMult, dT);      
    SumZ += Z;
  }
  UpdateCovC0(); 
  TMatrixD dXYZ(fC0, TMatrixD::kMult, SumZ);

  for (int i=0; i<fNtrks; i++){
    TMatrixD &W = *(TMatrixD*)fW[i];
    TMatrixD &A = *(TMatrixD*)fA[i];      
    TMatrixD &dT = *(TMatrixD*)fdT[i];
    TMatrixD &M = *(TMatrixD*)fM[i];
    TMatrixD &EInv = *(TMatrixD*)fEInv[i];
    TMatrixD NegAdXYZ(A, TMatrixD::kMult, dXYZ);
    NegAdXYZ *= -1.;
    NegAdXYZ += dT;
    TMatrixD BTW(fB, TMatrixD::kTransposeMult,W);
    TMatrixD EInvBTW(EInv, TMatrixD::kMultTranspose, fB);      
    EInvBTW *= W;
    TMatrixD dM(EInvBTW, TMatrixD::kMult, NegAdXYZ);
    M(0,0) = M(0,0) + dM(0,0);
    M(1,0) = M(1,0) + dM(1,0);
    M(2,0) = M(2,0) + dM(2,0);  
  }
  fVertex[0] = fVertex[0] + dXYZ(0,0);
  fVertex[1] = fVertex[1] + dXYZ(1,0);
  fVertex[2] = fVertex[2] + dXYZ(2,0);
}

Double_t VertexLSF::Chi2() {
    /// \MemberDescr 
    /// Calculates the chi2. 
  Double_t chi2;
  for (int i=0; i<fNtrks; i++){
    TMatrixD &W = *(TMatrixD*)fW[i];
    TMatrixD &dT = *(TMatrixD*)fdT[i];
    TMatrixD dTW(dT, TMatrixD::kTransposeMult, W);
    TMatrixD CHI2(dTW, TMatrixD::kMult, dT);
    chi2 = chi2 + CHI2(0,0);
  }  
  Double_t dChi2 = fChi2 - chi2;
  fChi2 = chi2;
  return dChi2;
}

void VertexLSF::UpdateCovC0(){
    /// \MemberDescr 
    /// Calculates the covariance matrix of vertex coordinates. Must be called for every iteration of the fit.
  TMatrixD SumDEInvDT(3,3);        
  for (int i=0; i<fNtrks; i++){
    TMatrixD &D = *(TMatrixD*)fD[i];
    TMatrixD &EInv = *(TMatrixD*)fEInv[i];
    // C0
    TMatrixD DEInv(D, TMatrixD::kMult, EInv);
    TMatrixD DEInvDT(DEInv, TMatrixD::kMultTranspose, D);      
    SumDEInvDT += DEInvDT;
  }
  TMatrixD C0Inv(fD0,TMatrixD::kMinus,SumDEInvDT);
  fC0 = C0Inv.Invert();
}

TMatrixD VertexLSF::GetCovC0i(Int_t i) {
    /// \MemberDescr 
    /// Calculates the covariance matrix of vertex coordinates and parameters of track i.
  TMatrixD &D = *(TMatrixD*)fD[i];
  TMatrixD &EInv = *(TMatrixD*)fEInv[i];
  // C0i
  TMatrixD C0D(fC0, TMatrixD::kMult, D);
  TMatrixD C0i(C0D, TMatrixD::kMult, EInv);
  C0i *= -1.;
  return C0i;
}

TMatrixD VertexLSF::GetCovCij(Int_t i, Int_t j) {
    /// \MemberDescr 
    /// Calculates the covariance matrix of parameters of tracks i and j.
  TMatrixD &Di = *(TMatrixD*)fD[i];
  TMatrixD &EInvi = *(TMatrixD*)fEInv[i];
  TMatrixD &Dj = *(TMatrixD*)fD[j];
  TMatrixD &EInvj = *(TMatrixD*)fEInv[j];
  TMatrixD C0Dj(fC0, TMatrixD::kMult, Dj);
  TMatrixD C0j(C0Dj, TMatrixD::kMult, EInvj);
  TMatrixD EInviDiT(EInvi, TMatrixD::kMultTranspose, Di); 
  TMatrixD Cij(EInviDiT, TMatrixD::kMult, C0j);
  if (i==j) Cij += EInvj;
  return Cij;
}

void VertexLSF::Apply3MomConstraints(Double_t px, Double_t py, Double_t pz, Double_t dpx, Double_t dpy, Double_t dpz) {
    /// \MemberDescr 
    /// Adding momentum constraints on the fit using the GTK measured kaon track. In principle the method works also using the beam parameters when the GTK track is not available. However the results are poor.
    /// \param px, py, pz       x,y,z-components of the kaon momentum
    /// \param dpx, dpy, dpz    momentum uncertainty 
 
  TMatrixD r(3,1);
  for (int i=0; i<fNtrks; i++){
    TMatrixD &K = *(TMatrixD*)fK[i];
    TMatrixD &M = *(TMatrixD*)fM[i];
    Double_t n = sqrt(1 + M(0,0)*M(0,0) + M(1,0)*M(1,0) );
    K(0,0) = (1+M(1,0)*M(1,0))*M(2,0)/pow(n,3);
    K(0,1) = (-1.)*M(0,0)*M(1,0)*M(2,0)/pow(n,3);
    K(0,2) = M(0,0)/n;
    K(1,0) = (-1.)*M(0,0)*M(1,0)*M(2,0)/pow(n,3);
    K(1,1) = (1+M(0,0)*M(0,0))*M(2,0)/pow(n,3);
    K(1,2) = M(1,0)/n;
    K(2,0) = (-1.)*M(2,0)*M(0,0)/pow(n,3);    
    K(2,1) = (-1.)*M(2,0)*M(1,0)/pow(n,3);    
    K(2,2) = 1/n;
    r(0,0) += M(0,0)*M(2,0)/n;
    r(1,0) += M(1,0)*M(2,0)/n;
    r(2,0) += M(2,0)/n;
  }
  r(0,0) -= px; // from TDR (0.2% on P and 16urad on Qx and Qy)
  r(1,0) -= py;
  r(2,0) -= pz;
  TMatrixD covK(3,3);
  for (int i=0; i<fNtrks; i++){
    TMatrixD &Ki = *(TMatrixD*)fK[i];
    for (int j=0; j<fNtrks; j++){
      TMatrixD &Kj = *(TMatrixD*)fK[j];
      TMatrixD KjT(TMatrixD::kTransposed, Kj);
      TMatrixD Dij = GetCovCij(i,j);
      TMatrixD KiDijKjT(Ki, TMatrixD::kMult, Dij);
      KiDijKjT *= KjT;
      covK += KiDijKjT;
    }
  } 
  fCovK = covK;
  fCovK(0,0) += dpx*dpx;//
  fCovK(1,1) += dpy*dpy;//
  fCovK(2,2) += dpz*dpz;//
  UpdateCovC0(); //
  TMatrixD R(TMatrixD::kInverted, fCovK);
  TMatrixD dXYZ(3,1);

  for (int i=0; i<fNtrks; i++){
    TMatrixD &K = *(TMatrixD*)fK[i];
    TMatrixD &M = *(TMatrixD*)fM[i];
    TMatrixD C0iT(TMatrixD::kTransposed, GetCovC0i(i));
    TMatrixD cov0i = GetCovC0i(i);
    TMatrixD C0iTKT(cov0i, TMatrixD::kMultTranspose, K);
    TMatrixD C0iTKTR(C0iTKT, TMatrixD::kMult, R);
    TMatrixD C0iTKTRr(C0iTKTR, TMatrixD::kMult, r);
    dXYZ += C0iTKTRr;
    TMatrixD dM(3,1);
    for (int j=0; j<fNtrks; j++){   
      TMatrixD &Kj = *(TMatrixD*)fK[j]; 
      TMatrixD Cij(GetCovCij(i,j));
      TMatrixD CijKT(Cij, TMatrixD::kMultTranspose, Kj);
      TMatrixD CijKTR(CijKT, TMatrixD::kMult, R);
      TMatrixD CijKTRr(CijKTR, TMatrixD::kMult, r);
      dM += CijKTRr;
    }
    M -= dM;
  }
  fVertex(0) -= dXYZ(0,0);
  fVertex(1) -= dXYZ(1,0);
  fVertex(2) -= dXYZ(2,0);
}

TMatrixD VertexLSF::GetCovC0iConstr(Int_t i) {
    /// \MemberDescr 
    /// Calculates the covariance matrix of vertex coordinates and parameters of track i after the constrained fit.
  TMatrixD C0i_temp(3,3);  
  TMatrixD HCovK(fH, TMatrixD::kMult, fCovK4);
  TMatrixD HCovKHT(HCovK, TMatrixD::kMultTranspose, fH);
  TMatrixD R(TMatrixD::kInverted, HCovKHT);
  // update covariances for re-iteration
  TMatrixD C0i(GetCovC0i(i));
  for (int k=0; k<fNtrks; k++){
    TMatrixD Cik(GetCovCij(i,k));
    TMatrixD &Kk = *(TMatrixD*)fK4[k]; 
    TMatrixD CikTKT(Cik, TMatrixD::kMultTranspose, Kk);
    TMatrixD CikTKTHT(CikTKT, TMatrixD::kMultTranspose, fH);
    TMatrixD CikTKTR(CikTKTHT, TMatrixD::kMult, R);
    TMatrixD CikTKTRH(CikTKTR, TMatrixD::kMult, fH);
    for(int l=0; l<fNtrks; l++){
      TMatrixD C0l(GetCovC0i(l));
      TMatrixD &Kl = *(TMatrixD*)fK4[l]; 
      TMatrixD CikTKTRHK(CikTKTRH, TMatrixD::kMult, Kl);
      TMatrixD CikTKTRHKC0l(CikTKTRHK, TMatrixD::kMult, C0l);
      C0i_temp += CikTKTRHKC0l;
    }
  }
  return C0i-C0i_temp;
}

TMatrixD VertexLSF::GetCovCijConstr(Int_t i, Int_t j) {
    /// \MemberDescr 
    /// Calculates the covariance matrix of parameters of tracks i and j after the constrained fit.
  TMatrixD Cij_temp(3,3);  
  TMatrixD Cij(GetCovCij(i,j));
  TMatrixD HCovK(fH, TMatrixD::kMult, fCovK4);
  TMatrixD HCovKHT(HCovK, TMatrixD::kMultTranspose, fH);
  TMatrixD R(TMatrixD::kInverted, HCovKHT);  
  for(int k=0; k<fNtrks; k++){
    TMatrixD Cik(GetCovCij(i,k));
    TMatrixD &Kk = *(TMatrixD*)fK4[k]; 
    TMatrixD CikTKT(Cik, TMatrixD::kMultTranspose, Kk);
    TMatrixD CikTKTHT(CikTKT, TMatrixD::kMultTranspose, fH);
    TMatrixD CikTKTR(CikTKTHT, TMatrixD::kMult, R);
    TMatrixD CikTKTRH(CikTKTR, TMatrixD::kMult, fH);
    for(int l=0; l<fNtrks; l++){
      TMatrixD Clj(GetCovCij(l,j));
      TMatrixD &Kl = *(TMatrixD*)fK4[l]; 
      TMatrixD CikTKTRHK(CikTKTRH, TMatrixD::kMult, Kl);
      TMatrixD CikTKTRHKClk(CikTKTRHK, TMatrixD::kMult, Clj);
      Cij_temp += CikTKTRHKClk;
    }
  }
  return Cij - Cij_temp;
}

TVector3 VertexLSF::GetTrackThreeMomentum(Int_t i){
  /// \MemberDescr 
  /// Returns the fitted px, py and pz momentum components of track i.
  TVector3 mom;
  if (i<fNtrks) { 
    TMatrixD &M = *(TMatrixD*)fM[i];
    Double_t n = sqrt(1 + M(0,0)*M(0,0) + M(1,0)*M(1,0) );
    mom.SetX( M(0,0)*M(2,0)/n );
    mom.SetY( M(1,0)*M(2,0)/n );
    mom.SetZ( M(2,0)/n );
  }
  return mom;
}

TVector3 VertexLSF::GetTrackSlopesAndMomentum(Int_t i){
  /// \MemberDescr 
  /// Returns the fitted dx/dz, dy/dz and P of track i.
  TVector3 mom;
  if (i<fNtrks) { 
    TMatrixD &M = *(TMatrixD*)fM[i];
    mom.SetX( M(0,0) );
    mom.SetY( M(1,0) );
    mom.SetZ( M(2,0) );
  }
  return mom;
}

Bool_t VertexLSF::FitStraightTracksNoBlueField(Int_t iter){
    /// \MemberDescr 
    /// The full algorithm for vertex fit for straight tracks.
    /// \param i    number of iterations
  MultipleLinesCDA MultLinesCDA;
  TVector3 point1;
  TVector3 dir;

  if (fNtrks<2) return 0;
  for (Int_t i=0; i<fNtrks; i++){
    TMatrixD &T = *(TMatrixD*)fT[i];
    point1.SetXYZ(T(2,0),T(3,0),fZref);
    dir.SetXYZ(-T(0,0),-T(1,0),-1.);        
    MultLinesCDA.AddLinePoint1Dir(point1, dir);
  }
  MultLinesCDA.ComputeVertex(); // use as seed for the LSF
  fVertex = MultLinesCDA.GetVertex();
  // set starting vertex fit position
  UpdateABMatrix();
  UpdateTrackParams();
  Chi2();
  for (Int_t i=1; i<iter; i++){
    Fit();
    UpdateABMatrix();
    UpdateTrackParams();
    Chi2();
  }
  return 1;
}

