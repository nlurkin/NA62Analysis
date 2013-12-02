#include "DetectorAcceptance.hh"
#include "Riostream.h"
#include <iostream>
#include <string>
#include <TGeoManager.h>
#include "functions.hh"
#include <TVirtualGeoTrack.h>
using namespace std;

DetectorAcceptance::DetectorAcceptance(TString GeometryFile){
	/// \MemberDescr
	/// \param GeometryFile : path to the geometry file (root)
	///
	/// Constructor. Imports the geometry.
	/// \EndMemberDescr

	//fGeoManager = new TGeoManager();
	fGeoManager = TGeoManager::Import(GeometryFile);
	//fGeoManager->CloseGeometry();
	fCanvas = 0;
	fVerbosity = AnalysisFW::kNo;

	buildDetectorsDictionaries();

	for (int i = 0; i < 13; i++) fDetPath[i] = false;
	fTrackNumber=0;

	if(fVerbosity>=AnalysisFW::kNormal){
		cout << "Detector acceptance enum definitions: " << endl << "CEDAR:" << kCEDAR << "\tGTK:" << kGTK << "\tCHANTI:" << kCHANTI << "\tLAV:" << kLAV << "\tSpectrometer:" << kSpectrometer << "\tIRC:" << kIRC << endl;
		cout << "CHOD:" << kCHOD << "\tLKr:" << kLKr << "\tSAC:" << kSAC << "\tMUV1:" << kMUV1 << "\tMUV2:" << kMUV2 << "\tMUV3:" << kMUV3 << "\t\tVOID:" << kVOID << endl;
	}
}

DetectorAcceptance::~DetectorAcceptance(){
}

bool DetectorAcceptance::GetDetAcceptance(volume det){
	/// \MemberDescr
	/// \param det : Detector part to check
	///
	/// Return true if the Detector det has been crossed by particle\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fDetPath[det];
}
TVector3 DetectorAcceptance::GetDetPosition(volume det){
	/// \MemberDescr
	/// \param det : Detector part to check
	///
	/// Return the position where the particle first crossed the detector det. (0,0,0) if not crossed.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fVolumePos[det];
}

/// \MemberDescr
/// \param det : GTK station to check
///
/// Return true if the GTK station det has been crossed by particle\n
/// WARNING : need to call FillPath before to be populated
/// \EndMemberDescr
bool DetectorAcceptance::GetGTKAcceptance(GTKVol det){
	/// \MemberDescr
	/// \param det : GTK station to check
	///
	/// Return true if the GTK station det has been crossed by particle\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fGTKPath[det];
}
TVector3 DetectorAcceptance::GetGTKPosition(GTKVol det){
	/// \MemberDescr
	/// \param det : GTK station to check
	///
	/// Return the position where the particle first crossed the GTK station det. (0,0,0) if not crossed.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fGTKPos[det];
}

bool DetectorAcceptance::GetStrawAcceptance(StrawVol det){
	/// \MemberDescr
	/// \param det : Straw station to check
	///
	/// Return true if the Spectrometer plane det has been crossed by particle\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fStrawPath[det];
}
TVector3 DetectorAcceptance::GetStrawPosition(StrawVol det){
	/// \MemberDescr
	/// \param det : Straw station to check
	///
	/// Return the position where the particle first crossed the Spectrometer plane det. (0,0,0) if not crossed.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fStrawPos[det];
}

bool DetectorAcceptance::GetLAVAcceptance(LAVVol det){
	/// \MemberDescr
	/// \param det : LAV station to check
	///
	/// Return true if the LAV station det has been crossed by particle\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fLAVPath[det];
}
TVector3 DetectorAcceptance::GetLAVPosition(LAVVol det){
	/// \MemberDescr
	/// \param det : LAV station to check
	///
	/// Return the position where the particle first crossed the LAV station det. (0,0,0) if not crossed.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fLAVPos[det];
}

void DetectorAcceptance::CleanDetPath(){
	/// \MemberDescr
	/// Reset the path for a new FillPath
	/// \EndMemberDescr

	for (int i = 0; i < 13; i++){
		fDetPath[i] = false;
		fVolumePos[i].SetXYZ(0.,0.,0.);
	}
	for (int i = 0; i < 3; i++){
		fGTKPath[i] = false;
		fGTKPos[i].SetXYZ(0.,0.,0.);
	}
	for (int i = 0; i < 4; i++){
		fStrawPath[i] = false;
		fStrawPos[i].SetXYZ(0.,0.,0.);
	}
	for (int i = 0; i < 13; i++){
		fLAVPath[i] = false;
		fLAVPos[i].SetXYZ(0.,0.,0.);
	}
}


void DetectorAcceptance::FillPath(TVector3 position, TVector3 momentum, double precision){
	/// \MemberDescr
	/// \param position : Initial position of the particle
	/// \param momentum : Momentum of the particle
	/// \param precision : precision for each step (default: 0 - optimized)
	///
	/// Set true for each crossed detector and each station and plane in GTK, LAV, Spectrometer.\n
	/// Set also the corresponding position.
	/// \EndMemberDescr

	TVirtualGeoTrack *track;
	double mm = 10.;
	//Clean for a new path
	CleanDetPath();

	//Define the "track"
	TVector3 norMom = momentum.Unit();
	fGeoManager->InitTrack(position.X()/mm, position.Y()/mm, position.Z()/mm, norMom.X(),norMom.Y(),norMom.Z());

	const char *path;
	string str;
	const Double_t* point;
	bool first = true;
	do{
		//Find next crossed volume and get its name

		if(first){
			fGeoManager->FindNode(position.X()/mm, position.Y()/mm, position.Z()/mm);
			first = false;
		}
		else{
			if(precision>0) fGeoManager->FindNextBoundaryAndStep(precision, kFALSE);
			else fGeoManager->FindNextBoundaryAndStep();
		}

		path = fGeoManager->GetPath();
		point = fGeoManager->GetCurrentPoint();
		str = path;

		//Parse volume name to find if it correspond to a sensitive part of a detector
		volume det = ParseDetector(str);
		//If it's GTK, check stations
		if(det==kGTK){
			GTKVol gtk = ParseGTK(str);
			if(!fGTKPath[gtk]){
				fGTKPath[gtk] = true;
				fGTKPos[gtk].SetXYZ(point[0]*mm,point[1]*mm,point[2]*mm);
			}
		}
		//If it's Spectrometer, check planes
		if(det==kSpectrometer){
			StrawVol straw = ParseStraw(str);
			if(!fStrawPath[straw]){
				fStrawPath[straw] = true;
				fStrawPos[straw].SetXYZ(point[0]*mm,point[1]*mm,point[2]*mm);
			}
		}
		//If it's LAV, check stations
		if(det==kLAV){
			LAVVol lav = ParseLAV(str);
			if(lav==kNOLAV){
				//det=kVOID;
			}
			else if(!fLAVPath[lav]){
				fLAVPath[lav] = true;
				fLAVPos[lav].SetXYZ(point[0]*mm,point[1]*mm,point[2]*mm);
			}
		}
		if(det==kLKr){
			if ( (int) (str.find("/lkrvolume_")) < 0)
				det = kVOID;
		}
		//Fill path if not already filled
		if(!fDetPath[det]){
			fDetPath[det] = true;
			fVolumePos[det].SetXYZ(point[0]*mm, point[1]*mm, point[2]*mm);
		}
	}
	while ( str.length() > 1);
}

void DetectorAcceptance::DrawDetector(){
	/// \MemberDescr
	/// Draw canvas with detector geometry
	/// \EndMemberDescr

	if(!fCanvas){
		fCanvas = new TCanvas("c", "DetectorAcceptance");
	}
	else{
		fCanvas->Clear();
	}
	fGeoManager->GetTopVolume()->Draw();
}

void DetectorAcceptance::ClearTracks(){
	/// \MemberDescr
	/// Remove all tracks
	/// \EndMemberDescr

	fGeoManager->ClearTracks();
}

void DetectorAcceptance::DrawTracks(){
	/// \MemberDescr
	/// Draw all registered tracks
	/// \EndMemberDescr

	DrawDetector();
	fGeoManager->DrawTracks();
}

void DetectorAcceptance::CreateTrack(int pdgID, TVector3 pos, TVector3 momentum, int charge){
	/// \MemberDescr
	/// \param pdgID : PDG code of the particle
	/// \param pos : Initial position of the particle
	/// \param momentum : Momentum of the particle
	/// \param charge : charge of the particle
	///
	/// Create a new track to display.
	/// \EndMemberDescr

	double zmin = pos.Z();
	double zmax = 300000;
	double step = 1000;
	int nstep = (zmax-zmin)/step;

	TVector3 prop, EndMom;
	TVirtualGeoTrack *track;
	bool ret = false;
	double z;

	fGeoManager->AddTrack(fTrackNumber, pdgID);
	fTrackNumber++;
	track = fGeoManager->GetLastTrack();

	for(int i=0; i<nstep; i++){
		z = zmin+i*step;
		if(charge==0) {
			prop = propagate(pos, momentum, z);
			ret = true;
		}
		else ret = MagPropagate(pos, momentum, charge, z, prop, EndMom);
		if(ret) track->AddPoint(prop.X()/10., prop.Y()/10., prop.Z()/10., 0);
	}
}

void DetectorAcceptance::DrawPoint(double x, double y, double z){
	/// \MemberDescr
	/// \param x : x coordinate
	/// \param y : y coordinate
	/// \param z : z coordinate
	///
	/// Draw the specified point on the detector
	/// \EndMemberDescr

	double point[3];
	point[0] = x/10.;
	point[1] = y/10.;
	point[2] = z/10.;
	fGeoManager->SetCurrentPoint(point);
	fGeoManager->DrawCurrentPoint();
}

DetectorAcceptance::volume DetectorAcceptance::ParseDetector(string str){
	/// \MemberDescr
	/// \param str : string of the volume
	///
	/// Parse the volume string to know to which detector it correspond.
	/// \EndMemberDescr

	if ( (int) (str.find("/LKr_")) > 1)
		return kLKr;
	else if ((int) (str.find("/CHANTI_")) > 1)
		return kCHANTI;
	else if ((int) (str.find("/Cedar_")) > 1)
		return kCEDAR;
	else if ((int) (str.find("/CHOD_")) > 1)
		return kCHOD;
	else if ((int) (str.find("/GigaTrackerStation_")) > 1)
		return kGTK;
	else if ((int) (str.find("/IRCModule_")) > 1)
		return kIRC;
	//  else if ((int) (str.find("/LAVStation_")) > 1)
	else if ((int) (str.find("/Banana_")) > 1)
		return kLAV;
	else if ((int) (str.find("/Spectrometer-Region")) > 1)
		return kSpectrometer;
	else if ((int) (str.find("/SAC_")) > 1)
		return kSAC;
	else if ((int) (str.find("/MUV1_")) > 1)
		return kMUV1;
	else if ((int) (str.find("/MUV2_")) > 1)
		return kMUV2;
	else if ((int) (str.find("/MUV3_")) > 1)
		return kMUV3;
	else
		return kVOID;
}

DetectorAcceptance::GTKVol DetectorAcceptance::ParseGTK(string str){
	/// \MemberDescr
	/// \param str : string of the volume
	///
	/// Parse detector string to know which GTK station it correspond.
	/// \EndMemberDescr
	int stationID, index;
	int startSearch, endSearch;

	if((startSearch = str.find("/GigaTrackerStation_")) > 1){
		endSearch = str.find("/", startSearch+20);
		stationID = atoi(str.substr(startSearch+20, endSearch-startSearch-20).data());
		index = fGTKDictionary[stationID];

		switch(index){
		case 0:
			return kGTK0;
			break;
		case 1:
			return kGTK1;
			break;
		case 2:
			return kGTK2;
			break;
		default:
			return kNOGTK;
		}
	}
	else
		return kNOGTK;
}

DetectorAcceptance::StrawVol DetectorAcceptance::ParseStraw(string str){
	/// \MemberDescr
	/// \param str : string of the volume
	///
	/// Parse detector string to know which Spectrometer plane it correspond.
	/// \EndMemberDescr

	if((int) (str.find("/Spectrometer-Region0")) > 1)
		return kSTRAW0;
	else if((int) (str.find("/Spectrometer-Region1")) > 1)
		return kSTRAW1;
	else if((int) (str.find("/Spectrometer-Region2")) > 1)
		return kSTRAW2;
	else if((int) (str.find("/Spectrometer-Region3")) > 1)
		return kSTRAW3;
	else
		return kNOSTRAW;
}

DetectorAcceptance::LAVVol DetectorAcceptance::ParseLAV(string str){
	/// \MemberDescr
	/// \param str : string of the volume
	///
	/// Parse detector string to know which LAV station it corresponds
	/// \EndMemberDescr

	int stationID, index;
	int startSearch, endSearch;

	if((startSearch = str.find("/LAVStation_")) > 1){
		endSearch = str.find("/", startSearch+12);
		stationID = atoi(str.substr(startSearch+12, endSearch-startSearch-12).data());
		index = fLAVDictionary[stationID];

		switch(index){
		case 0:
			return kLAV0;
			break;
		case 1:
			return kLAV1;
			break;
		case 2:
			return kLAV2;
			break;
		case 3:
			return kLAV3;
			break;
		case 4:
			return kLAV4;
			break;
		case 5:
			return kLAV5;
			break;
		case 6:
			return kLAV6;
			break;
		case 7:
			return kLAV7;
			break;
		case 8:
			return kLAV8;
			break;
		case 9:
			return kLAV9;
			break;
		case 10:
			return kLAV10;
			break;
		case 11:
			return kLAV11;
			break;
		default:
			return kNOLAV;
		}
	}
	else
		return kNOLAV;
}

DetectorAcceptance::volume DetectorAcceptance::CheckDetectorAcceptPoint(Double_t x,Double_t y, Double_t z)
{
	/// \MemberDescr
	/// \param x : x coordinate
	/// \param y : y coordinate
	/// \param z : z coordinate
	///
	/// Say in which detector volume the given point is
	/// \EndMemberDescr

	fGeoManager->GetTopVolume();
	fGeoManager->FindNode(x/10., y/10., z/10.);
	const char *path = fGeoManager->GetPath();
	string str(path);
	return ParseDetector(str);
}

DetectorAcceptance::volume DetectorAcceptance::FirstTouchedDetector(){
	/// \MemberDescr
	/// Return the first detector crossed.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	for(int i=0; i<13; i++){
		if(fDetPath[i]) return (volume)i;
	}
	return kVOID;
}

bool DetectorAcceptance::MagPropagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition, TVector3& EndMomentum ){
	/// \MemberDescr
	/// \param StartPosition : Initial position of the particle
	/// \param StartMomentum : InitialMomentum of the particle
	/// \param fQ : Charge of the particle
	/// \param fEndZ : End z coordinate of the track
	/// \param EndPosition : Final position of the particle
	/// \param EndMomentum : Final momentum of the particle
	///
	/// ---------------- spec_propagate ------------------------------------------\n
	/// Analytic propagation of a charged particle. It takes into account the\n
	/// presence of the bending magnet of the Spectrometer if fZEnd is greater\n
	/// than the Z coordinate of the center of the spectrometer.\n
	/// \EndMemberDescr

	const Double_t fMagnetZLength = 1300;
	const Double_t fMagnetZPosition = 0.5*(196450+197750);      // magnet center position
	const Double_t fMagnetFieldStrength = -0.6928;
	Double_t fEC = TMath::C() * 1.e-9 * 1.e-4 * 1.e-3;

	Double_t fStartX = StartPosition.X();
	Double_t fStartY = StartPosition.Y();
	Double_t fStartZ = StartPosition.Z();

	if ( StartMomentum.Z() == 0){
		cerr << "[spec_propagate] Error : Perpendicular momentum." << endl;
		return false;
	}

	Double_t fThetaX = StartMomentum.X() / StartMomentum.Z();
	Double_t fThetaY = StartMomentum.Y() / StartMomentum.Z();

	Double_t dMag = fMagnetZLength;
	Double_t zMag = fMagnetZPosition - dMag / 2.0 ;

	// fZEnd before magnet
	if (fEndZ <= zMag){
		EndPosition.SetX( fStartX + fThetaX * (fEndZ - fStartZ) );
		EndPosition.SetY( fStartY + fThetaY * (fEndZ - fStartZ) );
		EndPosition.SetZ( fEndZ );
		return true;
	}

	// fZEnd after MNP33
	EndPosition.SetX( fStartX + fThetaX * (zMag - fStartZ) );
	EndPosition.SetY( fStartY + fThetaY * (zMag - fStartZ) );
	EndPosition.SetZ( zMag );

	TVector3 B, fP;
	B.SetXYZ( 0.0, fMagnetFieldStrength * 10000.0, 0.0 );
	fP = StartMomentum;
	//    fP.SetZ( StartMomentum.Mag() / sqrt( 1 + fThetaX*fThetaX + fThetaY*fThetaY ) );
	//    fP.SetX( fP.Z() * fThetaX );
	//    fP.SetY( fP.Z() * fThetaY );

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

	return true;
}

TGeoManager* DetectorAcceptance::GetGeoManager(){
	/// \MemberDescr
	/// Return the pointer to the TGeoManager instance.
	/// \EndMemberDescr

	return fGeoManager;
}

void DetectorAcceptance::buildDetectorsDictionaries(){
	/// \MemberDescr
	/// Build the dictionary between LAV and GTK station names in the Geometry file (different each time
	/// the Geometry file is generated) and the station index (0-11 for LAV, 0-2 for GTK).
	/// \EndMemberDescr

	TGeoNode *top = (TGeoNode*)fGeoManager->GetListOfNodes()->At(0);
	TGeoNode *daughter, *subStation;
	int indexLAV = 0;
	int indexGTK = 0;
	TString name;

	for(int i=0; i<top->GetNdaughters(); i++){
		daughter = top->GetDaughter(i);
		if(TString(daughter->GetName()).BeginsWith("LAV_RR_")){
			for(int j=0; j<daughter->GetNdaughters(); j++){
				subStation = daughter->GetDaughter(j);
				if(TString(subStation->GetName()).BeginsWith("LAVStation_")){
					name = subStation->GetName();
					fLAVDictionary.insert(pair<int, int>(name.Remove(0, 11).Atoi(), indexLAV));
					indexLAV++;
				}
			}
		}
		else if(TString(daughter->GetName()).BeginsWith("GigaTracker_")){
			for(int j=0; j<daughter->GetNdaughters(); j++){
				subStation = daughter->GetDaughter(j);
				if(TString(subStation->GetName()).BeginsWith("GigaTrackerStation_")){
					name = subStation->GetName();
					fGTKDictionary.insert(pair<int, int>(name.Remove(0, 19).Atoi(), indexGTK));
					indexGTK++;
				}
			}
		}
	}
}
