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

	fMagnetEffect = false;
	fGeoManager = TGeoManager::Import(GeometryFile);
	fCanvas = 0;
	fVerbosity = AnalysisFW::kNo;

	buildDetectorsDictionaries();

	for (int i = 0; i < 15; i++){
		fDetPath[i] = false;
		fVolumePos[i] = TVector3(0.,0.,0.);
		fDetLength[i] = 0.;
	}
	fTrackNumber=0;

	if(fVerbosity>=AnalysisFW::kNormal){
		cout << "Detector acceptance enum definitions: " << endl << "CEDAR:" << kCEDAR << "\tGTK:" << kGTK << "\tCHANTI:" << kCHANTI << "\tLAV:" << kLAV << "\tSpectrometer:" << kSpectrometer << "\tIRC:" << kIRC << endl;
		cout << "CHOD:" << kCHOD << "\tLKr:" << kLKr << "\tSAC:" << kSAC << "\tMUV0:" << kMUV0 << "\tMUV1:" << kMUV1 << "\tMUV2:" << kMUV2 << "\tMUV3:" << kMUV3 << "\tHAC:" << kHAC << "\t\tVOID:" << kVOID << endl;
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
double DetectorAcceptance::GetDetLength(volume det){
	/// \MemberDescr
	/// \param det : Detector part to check
	///
	/// Return the path length in detector det in mm.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fDetLength[det];
}

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
double DetectorAcceptance::GetGTKLength(GTKVol det){
	/// \MemberDescr
	/// \param det : GTK station to check
	///
	/// Return the path length in GTK station det in mm.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fGTKLength[det];
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
double DetectorAcceptance::GetStrawLength(StrawVol det){
	/// \MemberDescr
	/// \param det : Straw station to check
	///
	/// Return the path length in Straw station det in mm.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fStrawLength[det];
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
double DetectorAcceptance::GetLAVLength(LAVVol det){
	/// \MemberDescr
	/// \param det : LAV station to check
	///
	/// Return the path length in LAV station det in mm.\n
	/// WARNING : need to call FillPath before to be populated
	/// \EndMemberDescr

	return fLAVLength[det];
}

void DetectorAcceptance::CleanDetPath(){
	/// \MemberDescr
	/// Reset the path for a new FillPath
	/// \EndMemberDescr

	for (int i = 0; i < 15; i++){
		fDetPath[i] = false;
		fVolumePos[i].SetXYZ(0.,0.,0.);
		fDetLength[i] = 0.;
	}
	for (int i = 0; i < 4; i++){
		fGTKPath[i] = false;
		fGTKPos[i].SetXYZ(0.,0.,0.);
		fGTKLength[i] = 0.;
	}
	for (int i = 0; i < 5; i++){
		fStrawPath[i] = false;
		fStrawPos[i].SetXYZ(0.,0.,0.);
		fStrawLength[i] = 0.;
	}
	for (int i = 0; i < 13; i++){
		fLAVPath[i] = false;
		fLAVPos[i].SetXYZ(0.,0.,0.);
		fLAVLength[i] = 0.;
	}
	fMagnetEffect = false;
	fPositionAfterMagnet = TVector3();
	fMomentumAfterMagnet = TVector3();
	fPositionCenterMagnet = TVector3();
	fMomentumCenterMagnet = TVector3();
}

void DetectorAcceptance::UpdateTrackingMomentum(double precision, int q, TVector3 currPos, TVector3 startMomentum){
	/// \MemberDescr
	/// \param precision : precision for each step (default: 0 - optimized)
	/// \param q : particle charge
	/// \param currPos : Current position of the particle
	/// \param startMomentum : Initial momentum of the particle
	///
	/// Check if the particle is passing through the magnet.\n
	/// If yes, update the momentum (only when entering the magnet and leaving it).
	/// \EndMemberDescr
	double zStep, endZ;
	TVector3 tempEndPos, tempEndMomentum;
	double magnetStart = 19645.0;
	double magnetEnd = 19775.0;
	double mm = 10.;

	//If particle is neutral, don't bother taking care of the magnetic field
	if(q!= 0){
		//Estimate next step
		if(precision>0) fGeoManager->FindNextBoundary(precision);
		else fGeoManager->FindNextBoundary();
		zStep = fGeoManager->GetStep();
		endZ = currPos.Z() + zStep*fGeoManager->GetCurrentDirection()[2];

		//If magnetic field not yet reached or already passed, don't bother either
		if((endZ>magnetStart) && (currPos.Z()<magnetEnd)){
			//If not yet done, get intermediate and final position and momenta
			if(!fMagnetEffect){
				MagnetEffect(currPos*mm, startMomentum, q, fPositionCenterMagnet, fMomentumCenterMagnet, fPositionAfterMagnet, fMomentumAfterMagnet);
				fPositionCenterMagnet = fPositionCenterMagnet*(1/mm);
				fPositionAfterMagnet = fPositionAfterMagnet*(1/mm);
				fMagnetEffect = true;
			}

			//Change momentum if crossing the magnetStart or the magnetEnd
			if((endZ>magnetStart) && (currPos.Z()<magnetStart)){
				fGeoManager->SetCurrentPoint(fPositionCenterMagnet.X(), fPositionCenterMagnet.Y(), fPositionCenterMagnet.Z());
				fGeoManager->SetCurrentDirection(fMomentumCenterMagnet.X(), fMomentumCenterMagnet.Y(), fMomentumCenterMagnet.Z());
				currPos = fPositionCenterMagnet;
				//Step has been done manually -> re-execute this method in case we  are now going to pass the end of the magnet
				UpdateTrackingMomentum(precision, q, currPos, startMomentum);
			}
			else if((endZ>magnetEnd) && (currPos.Z()<magnetEnd)){
				fGeoManager->SetCurrentPoint(fPositionAfterMagnet.X(), fPositionAfterMagnet.Y(), fPositionAfterMagnet.Z());
				fGeoManager->SetCurrentDirection(fMomentumAfterMagnet.X(), fMomentumAfterMagnet.Y(), fMomentumAfterMagnet.Z());
				currPos = fPositionAfterMagnet;
			}
		}
	}
}

void DetectorAcceptance::FillPath(TVector3 position, TVector3 momentum, double precision, int q){
	/// \MemberDescr
	/// \param position : Initial position of the particle
	/// \param momentum : Momentum of the particle
	/// \param precision : precision for each step (default: 0 - optimized)
	/// \param q : particle charge
	///
	/// Set true for each crossed detector and each station and plane in GTK, LAV, Spectrometer.\n
	/// Set also the corresponding position.
	/// \EndMemberDescr

	double mm = 10.;

	const char *path;
	string str;

	const Double_t* point;
	double stepLength;
	TVector3 currPos = position;
	TVector3 norMom = momentum.Unit();

	volume det;
	bool first = true;

	//Clean for a new path
	CleanDetPath();
	//Define the "track"
	fGeoManager->InitTrack(position.X()/mm, position.Y()/mm, position.Z()/mm, norMom.X(),norMom.Y(),norMom.Z());

	do{
		//Find next crossed volume and get its name
		if(first){
			fGeoManager->FindNode(position.X()/mm, position.Y()/mm, position.Z()/mm);
			first = false;
		}
		else{
			//Check with magnetic field
			UpdateTrackingMomentum(precision, q, currPos, momentum);

			if(precision>0) fGeoManager->FindNextBoundaryAndStep(precision, kFALSE);
			else fGeoManager->FindNextBoundaryAndStep();
		}

		path = fGeoManager->GetPath();
		point = fGeoManager->GetCurrentPoint();
		currPos.SetXYZ(point[0],point[1],point[2]);
		stepLength = fGeoManager->GetStep();
		str = path;

		//Parse volume name to find if it correspond to a sensitive part of a detector
		det = ParseDetector(str);
		//If it's GTK, check stations
		if(det==kGTK){
			GTKVol gtk = ParseGTK(str);
			if(!fGTKPath[gtk]){
				fGTKPath[gtk] = true;
				fGTKPos[gtk].SetXYZ(point[0]*mm,point[1]*mm,point[2]*mm);
			}
			fGTKLength[gtk] += stepLength*mm;
		}
		//If it's Spectrometer, check planes
		if(det==kSpectrometer){
			StrawVol straw = ParseStraw(str);
			if(!fStrawPath[straw]){
				fStrawPath[straw] = true;
				fStrawPos[straw].SetXYZ(point[0]*mm,point[1]*mm,point[2]*mm);
			}
			fStrawLength[straw] += stepLength*mm;
		}
		//If it's LAV, check stations
		if(det==kLAV){
			LAVVol lav = ParseLAV(str);
			if(!fLAVPath[lav]){
				fLAVPath[lav] = true;
				fLAVPos[lav].SetXYZ(point[0]*mm,point[1]*mm,point[2]*mm);
			}
			fLAVLength[lav] += stepLength*mm;
		}
		//Fill path if not already filled
		if(!fDetPath[det]){
			fDetPath[det] = true;
			fVolumePos[det].SetXYZ(point[0]*mm, point[1]*mm, point[2]*mm);
		}
		fDetLength[det] += stepLength*mm;
	}
	while(str.length() > 1);
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

void DetectorAcceptance::CreateTrack(int pdgID, TVector3 position, TVector3 momentum, int charge){
	/// \MemberDescr
	/// \param pdgID : PDG code of the particle
	/// \param position : Initial position of the particle
	/// \param momentum : Momentum of the particle
	/// \param charge : charge of the particle
	///
	/// Create a new track to display.
	/// \EndMemberDescr

	double zmin = position.Z();
	double zmax = 300000;
	double step = 1000;
	int nstep = (zmax-zmin)/step;

	TVector3 prop, currPos, currMomentum;
	TVirtualGeoTrack *track;
	double z;

	double magnetStart = 19645.0;
	double magnetEnd = 19775.0;

	TVector3 positionCenterMagnet, positionAfterMagnet;
	TVector3 momentumCenterMagnet, momentumAfterMagnet;

	fGeoManager->AddTrack(fTrackNumber, pdgID);
	fTrackNumber++;
	track = fGeoManager->GetLastTrack();

	if(charge!=0) MagnetEffect(position, momentum, charge, positionCenterMagnet, momentumCenterMagnet, positionAfterMagnet, momentumAfterMagnet);

	currPos = position;
	currMomentum = momentum;
	z = 0;
	for(int i=0; i<nstep; i++){
		z += step;
		if(charge==0) {
			prop = propagate(currPos, currMomentum, z);
		}
		else{
			//Change momentum if crossing the magnetStart or the magnetEnd
			if((z>magnetStart) && (currPos.Z()<magnetStart)){
				prop = positionCenterMagnet;
				currMomentum = momentumCenterMagnet;
			}
			else if((z>magnetEnd) && (currPos.Z()<magnetEnd)){
				prop = positionAfterMagnet;
				currMomentum = momentumAfterMagnet;
			}
			else{
				prop = propagate(currPos, currMomentum, z);
			}
		}
		track->AddPoint(prop.X()/10., prop.Y()/10., prop.Z()/10., 0);
		currPos = prop;
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

	if (((int) (str.find("/Parameterised_IrregularElectrode_")) > 1)
			&& ((int) (str.find("/lkrvolume_")) > 1))
		return kLKr;
	else if (((int) (str.find("/CHANTI_")) > 1)
			&& ((int) (str.find("/Strip_")) > 1))
		return kCHANTI;
	else if ((int) (str.find("/CedarPMTCathode")) > 1)
		return kCEDAR;
	else if ((int) (str.find("/CHODScintillatorCounter_")) > 1)
		return kCHOD;
	else if ((int) (str.find("/GigaTrackerSensorActiveArea_")) > 1)
		return kGTK;
	else if ((int) (str.find("/IRCScintillatorLayer_")) > 1)
		return kIRC;
	else if (((int) (str.find("/PbGl_")) > 1)
			&& ((int) (str.find("/LAVStation_")) > 1))
		return kLAV;
	else if ((int) (str.find("/StrawGas")) > 1)
		return kSpectrometer;
	else if ((int) (str.find("/SACScintillatorLayer_")) > 1)
		return kSAC;
	else if ((int) (str.find("/MUV0ScintillatorCounter_")) > 1)
		return kMUV0;
	else if (((int) (str.find("/MUV1Fiver_log_")) > 1)
			|| ((int) (str.find("/MUV1PMTAir_")) > 1)
			|| ((int) (str.find("/MUV1Scintillator_Scint_")) > 1))
		return kMUV1;
	else if ((int) (str.find("/MUV2Counter_")) > 1)
		return kMUV2;
	else if (((int) (str.find("/Scintillator_")) > 1)
			&& ((int) (str.find("/MUV3Module_")) > 1))
		return kMUV3;
	else if (((int) (str.find("/HACScintillatorLayer_")) > 1))
		return kHAC;
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

bool DetectorAcceptance::MagnetEffect(const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, TVector3 &middlePosition, TVector3 &middleMomentum, TVector3 &endPosition, TVector3 &endMomentum){
	/// \MemberDescr
	/// \param StartPosition : Initial position of the particle
	/// \param StartMomentum : InitialMomentum of the particle
	/// \param fQ : Charge of the particle
	/// \param middlePosition : Position of the particle at Z of the beginning of the magnet
	/// \param middleMomentum : Momentum of the particle at Z of the beginning of the magnet
	/// \param endPosition : Final Position of the particle
	/// \param endMomentum : Final momentum of the particle
	///
	/// Compute the positions and momenta of particle at the start and at the end of the spectrometer magnet.
	/// \EndMemberDescr

	double endZ = 197850.;

	//Propagate to the start of the magnet
	if(!MagPropagateBefore(StartPosition, StartMomentum, endZ, middlePosition)) return false;
	//Continue until after the magnet (19850 for the method to get the correct endMomentum)
	MagPropagateMagnet(middlePosition, StartMomentum, fQ, endZ, endPosition, endMomentum);
	//Move the endPosition back to magnetEnd
	endPosition = propagate(endPosition, endMomentum, 197750.);

	middleMomentum = (endPosition - middlePosition).Unit();
	return true;
}

void DetectorAcceptance::MagPropagateMagnet(const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition, TVector3& EndMomentum){
	/// \MemberDescr
	/// \param StartPosition : Position of the particle at Z of the beginning of the magnet
	/// \param StartMomentum : InitialMomentum of the particle
	/// \param fQ : Charge of the particle
	/// \param fEndZ : End z coordinate of the track
	/// \param EndPosition : Final position of the particle
	/// \param EndMomentum : Final momentum of the particle
	///
	/// Analytic propagation of a charged particle in and after the magnet\n
	/// \EndMemberDescr

	const Double_t fMagnetFieldStrength = -0.6928;
	Double_t fEC = TMath::C() * 1.e-9 * 1.e-4 * 1.e-3;

	TVector3 TempPosition;
	TVector3 B, fP;
	Int_t qb;
	Double_t rho, delta, sint, cost, dx, fThetaXAfter;

	Double_t fThetaX = StartMomentum.X() / StartMomentum.Z();
	Double_t fThetaY = StartMomentum.Y() / StartMomentum.Z();

	Double_t dMag = 1300;

	B.SetXYZ( 0.0, fMagnetFieldStrength * 10000.0, 0.0 );
	fP = StartMomentum;

	qb = B.Y()>0 ? 1 : -1;

	rho = (fP.Cross(B)).Mag() / (fQ * fEC * B.Mag2() );
	delta = dMag / rho;
	sint = sin( atan(fThetaX) );
	cost = cos( atan(fThetaX) );
	dx = qb * rho * ( -cost + sqrt( 1 - (delta - qb*sint)*(delta - qb*sint) ) );

	TempPosition.SetX( StartPosition.X() + dx );
	TempPosition.SetY( StartPosition.Y() + fThetaY*dMag );
	TempPosition.SetZ( StartPosition.Z() + dMag );
	fThetaXAfter = -qb * (delta - qb*sint) / sqrt( 1.0 - (delta - qb*sint)*(delta - qb*sint) );
	EndPosition.SetX( TempPosition.X() + fThetaXAfter * (fEndZ - TempPosition.Z()) );
	EndPosition.SetY( TempPosition.Y() + fThetaY * (fEndZ - TempPosition.Z()) );
	EndPosition.SetZ( fEndZ );

	EndMomentum = (EndPosition - TempPosition).Unit();
}

bool DetectorAcceptance::MagPropagateBefore(const TVector3 StartPosition, const TVector3 StartMomentum, const Double_t fEndZ, TVector3& EndPosition){
	/// \MemberDescr
	/// \param StartPosition : Initial position of the particle
	/// \param StartMomentum : InitialMomentum of the particle
	/// \param fEndZ : End z coordinate of the track
	/// \param EndPosition : Position of the particle at Z of the beginning start of the magnet
	///
	/// Analytic propagation of a charged particle to the beginning of the spectrometer\n
	/// \EndMemberDescr

	Double_t fThetaX, fThetaY, dMag, zMag;
	Double_t fStartX, fStartY, fStartZ;

	const Double_t fMagnetZLength = 1300;
	const Double_t fMagnetZPosition = 0.5*(196450+197750);      // magnet center position

	if ( StartMomentum.Z() == 0){
		cerr << "[MagPropagate] Error : Perpendicular momentum." << endl;
		return false;
	}

	fStartX = StartPosition.X();
	fStartY = StartPosition.Y();
	fStartZ = StartPosition.Z();

	fThetaX = StartMomentum.X() / StartMomentum.Z();
	fThetaY = StartMomentum.Y() / StartMomentum.Z();

	dMag = fMagnetZLength;
	zMag = fMagnetZPosition - dMag / 2.0 ;

	// fZEnd before magnet
	if (fEndZ <= zMag){
		//Propagate to fEndZ
		EndPosition.SetX( fStartX + fThetaX * (fEndZ - fStartZ) );
		EndPosition.SetY( fStartY + fThetaY * (fEndZ - fStartZ) );
		EndPosition.SetZ( fEndZ );
		return true;
	}

	// fZEnd after MNP33
	// Propagate to the beginning of MNP33
	EndPosition.SetX( fStartX + fThetaX * (zMag - fStartZ) );
	EndPosition.SetY( fStartY + fThetaY * (zMag - fStartZ) );
	EndPosition.SetZ( zMag );

	return true;
}

bool DetectorAcceptance::MagPropagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition, TVector3& EndMomentum ){
	/// \MemberDescr
	/// \param StartPosition : Initial position of the particle
	/// \param StartMomentum : InitialMomentum of the particle
	/// \param fQ : Charge of the particle
	/// \param fEndZ : End z coordinate of the track
	/// \param EndPosition : Final Position of the particle
	/// \param EndMomentum : Final momentum of the particle
	///
	/// Analytic propagation of a charged particle. It takes into account the\n
	/// presence of the bending magnet of the Spectrometer.\n
	/// \EndMemberDescr

	const double fMagnetEndPosition = 197750;
	TVector3 MiddlePosition;

	if(fEndZ<fMagnetEndPosition){
		cerr << "[MagPropagate] Error : EndPos must be after magnet: " << fEndZ << endl;
		return false;
	}
	if(!MagPropagateBefore(StartPosition, StartMomentum, fEndZ, MiddlePosition)) return false;

	MagPropagateMagnet(MiddlePosition, StartMomentum, fQ, fEndZ, EndPosition, EndMomentum);

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
