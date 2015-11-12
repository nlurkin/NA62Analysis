// ------------------------------------------------------------------
// History:
//
// Created by Anne Chappuis (anne.chappuis9@gmail.com) &
//            Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)    2015-06-18
//
// Forward tracking functionality added: E Goudzovski      2015-10-10
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
/// The Z range covered is 100 m < Z < 183.311 m.
/// An example of how to use the tracker is given below.
/// \code
/// #include "BlueTubeTracker.hh"
/// ...
/// BlueTubeTracker *tracker = new BlueTubeTracker();
/// tracker->SetCharge(+1);
/// tracker->SetInitialPosition(+1.60, +0.25, 180000);  // Unit: mm
/// tracker->SetInitialMomentum(+0.33, +0.10,  10000);  // Unit: MeV/c
/// tracker->SetZFinal(104000);                         // Unit: mm
/// tracker->TrackParticle();
/// TVector3 NewPosition = tracker->GetFinalPosition(); // Unit: mm
/// TVector3 NewMomentum = tracker->GetFinalMomentum(); // Unit: MeV/c
/// \endcode
/// \author Anne Chappuis (anne.chappuis9@gmail.com), Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)
/// \EndDetailed

#include "BlueTubeTracker.hh"

using namespace std;

BlueTubeTracker::BlueTubeTracker() {

  ////////////////////////////////////////////////////////////////////////////////////
  // The forty Z planes in BlueTubeMagneticFieldMap span
  // from z=104458 mm to z=181175 mm.
  // Outside this range, constant magnetic field is assumed.
  // The valid range of initial and final Z coordinates extend beyond the above limits.

  fZmin = 100000;
  fZmax = 183311; // start of Spectrometer responsibility region 0 in NA62MC

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

/////////////////////////////////////////////////////////////
// Compute the final position assuming straight line.
// Useful for debugging and quantifying the blue tube effect.

TVector3 BlueTubeTracker::GetFinalPositionNonCorrected() {
  return fInitialPosition +
    (fZFinal-fInitialPosition.Z())/fInitialMomentum.Z() * fInitialMomentum;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Track by one step, either forward or backward, depending on fTrackingBackward setting.
// Note: ipl2 - ipl1 = 1.

void BlueTubeTracker::TrackOneStep (Int_t ipl1, Int_t ipl2) {

  // Step length
  Double_t StandardStepSize = fZ[ipl2] - fZ[ipl1];
  Double_t StepLength       = StandardStepSize;

  // Step length: special cases of irregular first and last steps
  if (fTrackingBackward) {
    if (fZ[ipl1]<fZFinal)
      StepLength = fZ[ipl2] - fZFinal;             // short last step
    else if (ipl1==0 && fZFinal<fZ[0])
      StepLength = fZ[1] - fZFinal;                // long last step
    else if (fZ[ipl2]>fInitialPosition[2])
      StepLength = fInitialPosition[2] - fZ[ipl1]; // short first step
    else if (ipl2==fNPlanes-1 && fInitialPosition[2]>fZ[ipl2])
      StepLength = fInitialPosition[2] - fZ[ipl1]; // long first step
  }
  else {
    if (fZ[ipl2]>fZFinal)
      StepLength = fZFinal - fZ[ipl1];             // short last step
    else if (ipl2==fNPlanes-1 && fZFinal>fZ[ipl2])
      StepLength = fZFinal - fZ[ipl1];             // long last step
    else if (fZ[ipl1]<fInitialPosition[2])
      StepLength = fZ[ipl2] - fInitialPosition[2]; // short first step
    else if (ipl1==0 && fInitialPosition[2]<fZ[0])
      StepLength = fZ[ipl2] - fInitialPosition[2]; // long first step
  }

  // Unit conversion for field integral computation: [mm] --> [cm],
  // required for compatibility with the BlueFieldMagneticFieldMap class.
  Double_t x = 0.1 * fPosIn.X();
  Double_t y = 0.1 * fPosIn.Y();

  // Evaluate the field integrals over the step from pre-computed quantities [mkT*mm]
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

  // Make the tracking step: we use the approximation Px,Py<<Pz and neglect the "transverse"
  // field gradients dB/dx, dB/dy over the length of the tracking step.
  fMomOut.SetX(fMomIn.X() + fSign * fCharge * FieldIntY * 1e-15 * fSpeedOfLight);
  fMomOut.SetY(fMomIn.Y() - fSign * fCharge * FieldIntX * 1e-15 * fSpeedOfLight);
  fMomOut.SetZ(sqrt(fMomIn.Mag2() - fMomOut.Perp2()));
  fPosOut.SetX(fPosIn.X() - 0.5 * fSign * StepLength *
	       (fMomIn.X()/fMomIn.Z() + fMomOut.X()/fMomOut.Z()));
  fPosOut.SetY(fPosIn.Y() - 0.5 * fSign * StepLength *
	       (fMomIn.Y()/fMomIn.Z() + fMomOut.Y()/fMomOut.Z()));

  // Set the output Z position, including the special case of the last step
  if (fTrackingBackward) {
    fPosOut.SetZ(fZ[ipl1]);
    if (fZ[ipl1]<fZFinal) fPosOut.SetZ(fZFinal); // short step
    if (ipl1==0 && fZFinal<fZ[ipl1]) fPosOut.SetZ(fZFinal); // long step beyond boundary
  }
  else {
    fPosOut.SetZ(fZ[ipl2]);
    if (fZ[ipl2]>fZFinal) fPosOut.SetZ(fZFinal); // short step
    if (ipl2==fNPlanes-1 && fZFinal>fZ[ipl2]) fPosOut.SetZ(fZFinal); // long step beyond boundary
  }
}

////////////////////////////////////////////////////////
// Main method to track a particle, to be called by user

void BlueTubeTracker::TrackParticle() {

  ////////////////
  // Sanity checks

  if (fInitialPosition.Z()<fZmin || fInitialPosition.Z()>fZmax) {
    cout << "BlueTubeTracker error: initial position must be ["<<fZmin<<"; "<<fZmax<<"] mm" << endl;
    return;
  }
  if (fZFinal<fZmin || fZFinal>fZmax) {
    cout << "BlueTubeTracker error: final position must be ["<<fZmin<<"; "<<fZmax<<"] mm" << endl;
    return;
  }
  if (fInitialMomentum.Z()<=0.0) {
    cout << "BlueTubeTracker error: particle should have positive Pz" << endl;
    return;
  }
  if (fabs(fCharge)!=1) {
    cout << "BlueTubeTracker error: charge |q|=1 is expected" << endl;
    return;
  }

  //////////////////////////////////////////
  // Backward or forward tracking requested?

  fTrackingBackward = (fInitialPosition.Z()>fZFinal);
  fSign = fTrackingBackward ? +1 : -1;

  ///////////////////////////////////////////////////////////////////////////////
  // Find the two Z planes such that all tracking steps are between these planes.
  // Note the exception: the initial/final position can be
  // beyond the boudary planes, in which case long first/last steps are made.

  Double_t Zlower = (fTrackingBackward) ? fZFinal : fInitialPosition[2];
  Double_t Zupper = (fTrackingBackward) ? fInitialPosition[2] : fZFinal;
  Int_t iPlane1 = fNPlanes-1;
  while (fZ[iPlane1]>Zlower && iPlane1>0) iPlane1--;
  Int_t iPlane2 = 0;
  while (fZ[iPlane2]<Zupper && iPlane2<fNPlanes-1) iPlane2++;
  if (iPlane2-iPlane1<2) {
    cout << "[BlueTubeTracker] Z interval too small, using straight line propagation" << endl;
    fFinalPosition = GetFinalPositionNonCorrected();
    fFinalMomentum = fInitialMomentum;
    return;
  }

  fPosIn = fInitialPosition;
  fMomIn = fInitialMomentum;

  // Tracking backward from plane 2 to plane 1
  if (fTrackingBackward) {
    for (Int_t ipl2=iPlane2; ipl2>iPlane1; ipl2--) {
      TrackOneStep (ipl2-1, ipl2);
      fPosIn = fPosOut;
      fMomIn = fMomOut;
    }
  }

  // Tracking forward from plane 1 to plane 2
  else {
    for (Int_t ipl1=iPlane1; ipl1<iPlane2; ipl1++) {
      TrackOneStep (ipl1, ipl1+1);
      fPosIn = fPosOut;
      fMomIn = fMomOut;
    }
  }

  fFinalPosition = fPosOut;
  fFinalMomentum = fMomOut;
  return;
}
