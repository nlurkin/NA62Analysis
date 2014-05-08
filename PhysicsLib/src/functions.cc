#include <iostream>
#include <TVector3.h>
#include <signal.h>
#include <sstream>

#include "functions.hh"
using namespace std;
using namespace NA62Constants;

/// \file functions.cc
/// DISCLAIMER: This file needs reviewing. Most of the functions in this file are here by legacy
/// from early development stages of the framework. None of them should be considered validated
/// at this point.

TVector3 propagate(TVector3 pos, TVector3 p, double z){
	/// \MemberDescr
	/// \param pos : position
	/// \param p : momentum
	/// \param z : projection position
	///
	/// Propagate the particle to z position
	/// \EndMemberDescr

	TVector3 result;
	result = pos + p*((z-pos.Z())/p.Z());
	return result;
}

TString printVector2( TVector2 v )
{
	/// \MemberDescr
	/// \param v : Vector to format
	///
	/// Output correct format for printing TVector2 coordinates
	/// \EndMemberDescr

	ostringstream ss;
    ss << "( " << v.X() << ", " << v.Y() << " )";
    return TString( ss.str().c_str() );
}

TString printVector3( TVector3 v )
{
	/// \MemberDescr
	/// \param v : Vector to format
	///
	/// Output correct format for printing TVector3 coordinates
	/// \EndMemberDescr

	ostringstream ss;
    ss << "( " << v.X() << ", " << v.Y() << ", " << v.Z() << " )";
    return TString( ss.str().c_str() );
}

TString printVector4( TLorentzVector v )
{
	/// \MemberDescr
	/// \param v : Vector to format
	///
	/// Output correct format for printing TLorentzVector coordinates
	/// \EndMemberDescr

	ostringstream ss;
    ss << "( " << v.T() << ", " << v.X() << ", " << v.Y() << ", " << v.Z() << " )";
    return TString( ss.str().c_str() );
}

bool IsInDetector(double propagation_pos, double prod_pos, double min_pos, double des_pos, double x_front, double y_front, double x_back, double y_back, double crnrX[], double crnrY[], int crnr_nb, double center_radius){
	/// \MemberDescr
	/// \param propagation_pos : position of the detector min z
	/// \param prod_pos : initial position of the particle
	/// \param min_pos : minimum prod_pos to be reached for the  particle to be accepted (usually max z of detector - particle has to be produced before the detector)
	/// \param des_pos : minimum decay z position to be reached for the particle to be accepted (usually detector min z or max z(if it has to cross the whole detector to be detected))
	/// \param x_front : particle x coordinate at detector front plane
	/// \param y_front : particle y coordinate at detector front plane
	/// \param x_back : particle x coordinate at detector back plane
	/// \param y_back : particle y coordinate at detector back plane
	/// \param crnrX[] : x coordinates of detector corners
	/// \param crnrY[] : y coordinates of detector corners
	/// \param crnr_nb : number of detector corners
	/// \param center_radius : radius around (x=0,y=0) corresponding to beam hole (no sensitive part). 0 if no beam hole.
	///
	/// Say if the particle is in the detector volume. For polygonal detectors.\n
	/// WARNING : Approximations are always made with this function. Use the DetectorAcceptance class instead.
	/// \EndMemberDescr

	int i, j=crnr_nb-1;
	bool oddNodes_front = false;
	bool oddNodes_back = false;
	bool outside = false;

	for (i=0; i<crnr_nb; i++) {
		if ((crnrY[i]<y_front && crnrY[j]>=y_front) || (crnrY[j]<y_front && crnrY[i]>=y_front)) {
			if (crnrX[i]+(y_front-crnrY[i])/(crnrY[j]-crnrY[i])*(crnrX[j]-crnrX[i])<x_front) {
				oddNodes_front=!oddNodes_front;
			}
		}
		j=i;
	}

	for (i=0; i<crnr_nb; i++) {
		if ((crnrY[i]<y_back && crnrY[j]>=y_back) || (crnrY[j]<y_back && crnrY[i]>=y_back)) {
			if (crnrX[i]+(y_back-crnrY[i])/(crnrY[j]-crnrY[i])*(crnrX[j]-crnrX[i])<x_back) {
				oddNodes_back=!oddNodes_back;
			}
		}
		j=i;
	}

	if((pow(x_front,2)+pow(y_front,2)<pow(center_radius,2)) && (pow(x_back,2)+pow(y_back,2)<pow(center_radius,2))) outside = true;

	if(des_pos < min_pos) outside = true;
	if(prod_pos > propagation_pos) outside = true;

	return oddNodes_front && oddNodes_back && !outside;
}

bool IsInDetector(double propagation_pos, double prod_pos, double min_pos, double des_pos, double x_front, double y_front, double x_back, double y_back, double inner_radius, double outer_radius, double centerx, double centery){
	/// \MemberDescr
	/// \param propagation_pos : position of the detector min z
	/// \param prod_pos : initial position of the particle
	/// \param min_pos : minimum prod_pos to be reached for the  particle to be accepted (usually max z of detector - particle has to be produced before the detector)
	/// \param des_pos : minimum decay z position to be reached for the particle to be accepted (usually detector min z or max z(if it has to cross the whole detector to be detected))
	/// \param x_front : particle x coordinate at detector front plane
	/// \param y_front : particle y coordinate at detector front plane
	/// \param x_back : particle x coordinate at detector back plane
	/// \param y_back : particle y coordinate at detector back plane
	/// \param inner_radius : inner radius of cylinder
	/// \param outer_radius : outer radius od cylinder
	/// \param centerx : x coordinate of cylinder center
	/// \param centery : y coordinate of cylinder center
	///
	/// Say if the particle is in the detector volume. For cylindrical detectors<br>
	/// WARNING : Approximations are always made with this function. Use the DetectorAcceptance class instead.
	/// \EndMemberDescr

	double sq_radius_front = (pow(x_front-centerx,2) + pow(y_front-centery, 2));
	double sq_radius_back = (pow(x_back-centerx,2) + pow(y_back-centery, 2));

	bool outside = false;

	if(((sq_radius_front < pow(outer_radius,2)) && (sq_radius_front > pow(inner_radius,2))) || ((sq_radius_back < pow(outer_radius,2)) && (sq_radius_back > pow(inner_radius,2)))) outside = false;
	else outside = true;

	if(des_pos < min_pos) outside = true;
	if(prod_pos > propagation_pos) outside = true;

	return !outside;
}

TVector3 spec_propagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ,
                      const Double_t fEndZ)
{
	/// \MemberDescr
	/// \param StartPosition : Initial position of the particle
	/// \param StartMomentum : InitialMomentum of the particle
	/// \param fQ : Charge of the particle
	/// \param fEndZ : End z coordinate of the track
	///
	/// ---------------- spec_propagate ------------------------------------------\n
	/// Analytic propagation of a charged particle. It takes into account the\n
	/// presence of the bending magnet of the Spectrometer if fZEnd is greater\n
	/// than the Z coordinate of the center of the spectrometer.\n
	/// \EndMemberDescr

	TVector3 EndPosition, EndMomentum;

    const Double_t fMagnetZLength = 1300;
    const Double_t fMagnetZPosition = 0.5*(196350+197650);      // magnet center position
    const Double_t fMagnetFieldStrength = 0.6928*1000;
    Double_t fEC = TMath::C() * 1.e-9 * 1.e-4 * 1.e-3;

    Double_t fStartX = StartPosition.X();
    Double_t fStartY = StartPosition.Y();
    Double_t fStartZ = StartPosition.Z();

    if ( StartMomentum.Z() == 0)
    {
        cerr << "[spec_propagate] Error : Perpendicular momentum." << endl;
        return -1;
    }

    Double_t fThetaX = StartMomentum.X() / StartMomentum.Z();
    Double_t fThetaY = StartMomentum.Y() / StartMomentum.Z();

    Double_t dMag = fMagnetZLength;
    Double_t zMag = fMagnetZPosition - dMag / 2.0 ;

    // fZEnd before magnet
    if (fEndZ <= zMag)
    {
        EndPosition.SetX( fStartX + fThetaX * (fEndZ - fStartZ) );
        EndPosition.SetY( fStartY + fThetaY * (fEndZ - fStartZ) );
        EndPosition.SetZ( fEndZ );
        return EndPosition;
    }

    // fZEnd after MNP33
    EndPosition.SetX( fStartX + fThetaX * (zMag - fStartZ) );
    EndPosition.SetY( fStartY + fThetaY * (zMag - fStartZ) );
    EndPosition.SetZ( zMag );

    TVector3 B, fP;
    B.SetXYZ( 0.0, fMagnetFieldStrength * 10000.0, 0.0 );
    fP = StartMomentum;

    Int_t qb = B.Y()>0 ? 1 : -1;

    Double_t rho = (fP.Cross(B)).Mag() / (fQ * fEC * B.Mag2() );
    Double_t delta = dMag / rho;
    Double_t sint = sin( atan(fThetaX) );
    Double_t cost = cos( atan(fThetaX) );
    Double_t dx = qb * rho * ( -cost + sqrt( 1 - (delta - qb*sint)*(delta - qb*sint) ) );
    EndPosition.SetX( EndPosition.X() + dx );
    EndPosition.SetY( EndPosition.Y() + fThetaY*dMag );
    EndPosition.SetZ( EndPosition.Z() + dMag );
    TVector3 TempPosition = EndPosition;
    Double_t fThetaXAfter = -qb * (delta - qb*sint) / sqrt( 1.0 - (delta - qb*sint)*(delta - qb*sint) );
    EndPosition.SetX( EndPosition.X() + fThetaXAfter * (fEndZ - EndPosition.Z()) );
    EndPosition.SetY( EndPosition.Y() + fThetaY * (fEndZ - EndPosition.Z()) );
    EndPosition.SetZ( fEndZ );

    EndMomentum = (EndPosition - TempPosition).Unit();

    return EndPosition;
}

double distance3D(TVector3 p1, TVector3 p2){
	/// \MemberDescr
	/// \param p1 : point 1
	/// \param p2 : point 2
	///
	/// Return the distance between two points in 3D space
	/// \EndMemberDescr

	return sqrt(pow(p1.X()-p2.X(), 2) + pow(p1.Y()-p2.Y(), 2) + pow(p1.Z()-p2.Z(), 2));
}

double distance2D(TVector3 p1, TVector3 p2, TString plane){
	/// \MemberDescr
	/// \param p1 : point 1
	/// \param p2 : point 2
	/// \param plane : plane on which the distance is computed
	///
	/// Return the distance between two points in 2D space
	/// \EndMemberDescr

	plane.ToUpper();
	if( (plane.CompareTo("XY")==0) || (plane.CompareTo("YX")==0) ) return sqrt(pow(p1.X()-p2.X(), 2) + pow(p1.Y()-p2.Y(), 2));
	else if( (plane.CompareTo("XZ")==0) || (plane.CompareTo("ZX")==0) ) return sqrt(pow(p1.X()-p2.X(), 2) + pow(p1.Z()-p2.Z(), 2));
	else if( (plane.CompareTo("YZ")==0) || (plane.CompareTo("ZY")==0) ) return sqrt(pow(p1.Y()-p2.Y(), 2) + pow(p1.Z()-p2.Z(), 2));
	else return -1;
}
