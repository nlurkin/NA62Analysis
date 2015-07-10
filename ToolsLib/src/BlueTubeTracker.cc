// ------------------------------------------------------------------
// History:
//
// Created by Anne Chappuis (anne.chappuis9@gmail.com) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)    2015-06-18
//
// ------------------------------------------------------------------

/// \class BlueTubeTracker
/// \Brief
/// Tracker of charged particles in the blue tube magnetic field
/// \EndBrief
///
/// \Detailed
/// The parameterization of the blue tube field and the principle of the algorithm 
/// are described in the note NA62-15-06. Input/output units: [mm] and [MeV/c].
/// An example of how to use the tracker is given below.
/// \code
/// #include "BlueTubeTracker.hh"
/// ...
/// BlueTubeTracker *tracker = new BlueTubeTracker();
/// tracker->SetCharge(+1);
/// tracker->SetInitialPosition(+1.60, +0.25, 180000); // Unit: mm
/// tracker->SetInitialMomentum(+0.33, +0.10,  10000); // Unit: MeV
/// tracker->SetZFinal(104000); // mm
/// tracker->TrackParticle();
/// TVector3 NewPosition = tracker->GetFinalPosition(); // mm
/// TVector3 NewMomentum = tracker->GetFinalMomentum(); // MeV
/// \endcode
/// \EndDetailed

#include "BlueTubeTracker.hh"

BlueTubeTracker::BlueTubeTracker() {
  fZmin = 100000; // The last tracking step may extend beyond the end of field map (z0=104458mm)
  fZmax = 183311; // This is the end of the map region and just in front of STRAW1
  fMap = new BlueTubeMagneticFieldMap(fZmin, fZmax);
  fNPlanes = fMap->GetNPlanes();
  for (Int_t i=0; i<fNPlanes; i++) {
    fZ[i] = fMap->GetZ(i);
    for (Int_t j=0; j<6; j++) fBx_param_integral[j][i] = fMap->GetBxParamIntegral(j,i);
    for (Int_t j=0; j<4; j++) fBy_param_integral[j][i] = fMap->GetByParamIntegral(j,i);
  }
  fSpeedOfLight = 299792458.0; // [m/s]

  fInitialPosition = TVector3(0, 0, -999);
  fInitialMomentum = TVector3(0, 0, -999);
  fFinalPosition   = TVector3(0, 0, 0);
  fFinalMomentum   = TVector3(0, 0, 0);
  fZFinal = -999;
  fCharge = 0;
}

BlueTubeTracker::~BlueTubeTracker() {
  delete fMap;
}

void BlueTubeTracker::TrackOneStep (Int_t ipl1, Int_t ipl2) {

  // Step length, including special cases of the first and last steps
  Double_t StandardStepSize = fZ[ipl2] - fZ[ipl1];
  Double_t StepLength       = StandardStepSize;
  if (fZ[ipl1]<fZFinal) StepLength = fZ[ipl2] - fZFinal;
  if (ipl1==0 && fZFinal<fZ[0]) StepLength = fZ[1] - fZFinal;
  if (fZ[ipl2]>fInitialPosition[2]) StepLength = fInitialPosition[2] - fZ[ipl1];
  
  // Unit conversion for field integral computation: [mm] --> [cm],
  // required for compatibility with the BlueFieldMagneticFieldMap class.
  double x = 0.1 * fPosIn.X();
  double y = 0.1 * fPosIn.Y();

  // Build the field integrals [mkT*mm] from pre-computed quantities.
  Double_t FieldIntX =
    fBx_param_integral[0][ipl1] +
    fBx_param_integral[1][ipl1]*x +
    fBx_param_integral[2][ipl1]*y +
    fBx_param_integral[3][ipl1]*x*x +
    fBx_param_integral[4][ipl1]*y*y +
    fBx_param_integral[5][ipl1]*x*y;
  Double_t FieldIntY =
    fBy_param_integral[0][ipl1] +
    fBy_param_integral[1][ipl1]*x +
    fBy_param_integral[2][ipl1]*y +
    fBy_param_integral[3][ipl1]*x*y;

  // Account for the non-standard first and last steps
  FieldIntX *= (StepLength / StandardStepSize);
  FieldIntY *= (StepLength / StandardStepSize);

  // Make the tracking step: we use the approximation Px,Py<<Pz, and neglect the "transverse"
  // field gradients dB/dx, dB/dy over the length of the tracking step.
  // Note that the signs correspond to tracking backwards.
  fMomOut.SetX(fMomIn.X() + fCharge * FieldIntY * 1e-15 * fSpeedOfLight);
  fMomOut.SetY(fMomIn.Y() - fCharge * FieldIntX * 1e-15 * fSpeedOfLight);
  fMomOut.SetZ(sqrt(fMomIn.Mag2() - fMomOut.Perp2()));
  fPosOut.SetX(fPosIn.X() - 0.5 * StepLength * (fMomIn.X()/fMomIn.Z() + fMomOut.X()/fMomOut.Z()));
  fPosOut.SetY(fPosIn.Y() - 0.5 * StepLength * (fMomIn.Y()/fMomIn.Z() + fMomOut.Y()/fMomOut.Z()));
  fPosOut.SetZ(fZ[ipl1]);

  // Special case: the last step
  if (fZ[ipl1]<fZFinal) fPosOut.SetZ(fZFinal);
  if (ipl1==0 && fZFinal<fZ[0]) fPosOut.SetZ(fZFinal);
}

void BlueTubeTracker::TrackParticle() {

  // Sanity checks
  if (fInitialPosition.Z()<fZmin || fInitialPosition.Z()>fZmax) {
    cout << "BlueTubeTracker error: initial position must be ["<<fZmin<<"; "<<fZmax<<"] mm" << endl;
    return;
  }
  if (fZFinal<fZmin || fZFinal>fZmax) {
    cout << "BlueTubeTracker error: final position must be ["<<fZmin<<"; "<<fZmax<<"] mm" << endl;
    return;
  }
  if (fZFinal>=fInitialPosition.Z()) {
    cout << "BlueTubeTracker error: tracking can be performed backwards only" << endl;
    return;
  }
  if (fInitialMomentum.Z()<=0.0) {
    cout << "BlueTubeTracker error: a particle should have positive Pz" << endl;
    return;
  }
  if (fabs(fCharge)!=1) {
    cout << "BlueTubeTracker error: charge |q|=1 is expected" << endl;
    return;
  }
  
  // Tracking from plane 2 backwards to plane 1
  Int_t iPlane1 = fNPlanes-1;
  while (fZ[iPlane1]>fZFinal && iPlane1>0) iPlane1--;
  Int_t iPlane2 = 0;
  while (fZ[iPlane2]<fInitialPosition[2]) iPlane2++;  
  if (iPlane2-iPlane1<2) {
    cout << "BlueTubeTracker error: initial and final position are too close" << endl;
    return;
  }
  
  fPosIn = fInitialPosition;
  fMomIn = fInitialMomentum;
  for (Int_t ipl2=iPlane2; ipl2>iPlane1; ipl2--) {
    TrackOneStep(ipl2-1, ipl2);
    fPosIn = fPosOut;
    fMomIn = fMomOut;
  }
  fFinalPosition = fPosOut;
  fFinalMomentum = fMomOut;
  return;
}
