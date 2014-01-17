#ifndef DETECTORACCEPTANCE_HH
#define DETECTORACCEPTANCE_HH

#include <TString.h>
#include <iostream>
#include <string>
#include <TGeoManager.h>
#include <TVector3.h>
#include <TCanvas.h>
#include <map>
#include "FWEnums.hh"
using namespace std;

/// \class DetectorAcceptance
/// \Brief 
/// Provide a good description of detector using a gdml file.
/// \EndBrief 
///
/// \Detailed
/// Provide a good description of detector using a gdml file.
/// \EndDetailed

class DetectorAcceptance
{
public:
	enum volume {kCEDAR, kGTK, kCHANTI, kLAV, kSpectrometer, kIRC, kCHOD, kLKr, kSAC, kMUV1, kMUV2, kMUV3, kVOID}; ///< List of volumes of the geometry
	enum GTKVol {kGTK0, kGTK1, kGTK2, kNOGTK}; ///< List of GTK parts
	enum StrawVol {kSTRAW0, kSTRAW1, kSTRAW2, kSTRAW3, kNOSTRAW}; ///< List of straw parts
	enum LAVVol {kLAV0, kLAV1, kLAV2, kLAV3, kLAV4, kLAV5, kLAV6, kLAV7, kLAV8, kLAV9, kLAV10, kLAV11, kNOLAV}; ///< List of LAV stations

	DetectorAcceptance(TString GeometryFile);
	~DetectorAcceptance();

	TGeoManager *GetGeoManager();
	volume CheckDetectorAcceptPoint(Double_t x,Double_t y, Double_t z);
	void FillPath(TVector3 position, TVector3 momentum, double precision=0, int q=0);
	void DrawDetector();
	void DrawTracks();
	void ClearTracks();
	void CreateTrack(int pdgID,TVector3 position, TVector3 momentum, int charge);
	void DrawPoint(double x, double y, double z);
	bool GetDetAcceptance(volume det);
	TVector3 GetDetPosition(volume det);
	bool GetGTKAcceptance(GTKVol det);
	TVector3 GetGTKPosition(GTKVol det);
	bool GetStrawAcceptance(StrawVol det);
	TVector3 GetStrawPosition(StrawVol det);
	bool GetLAVAcceptance(LAVVol det);
	TVector3 GetLAVPosition(LAVVol det);
	void CleanDetPath();

	volume FirstTouchedDetector();

	bool MagPropagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition, TVector3& EndMomentum );

private:
	void MagPropagateMagnet( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition, TVector3& EndMomentum );
	bool MagPropagateBefore( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition);
	bool MagnetEffect(const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, TVector3 &middlePosition, TVector3 &middleMomentum, TVector3 &endPosition, TVector3 &endMomentum);

	void UpdateTrackingMomentum(double precision, int q, TVector3 currPos, TVector3 startMomentum);

	void buildDetectorsDictionaries();

	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity level

	TGeoManager *fGeoManager; ///< TGeoManager

	volume ParseDetector(string str);
	GTKVol ParseGTK(string str);
	StrawVol ParseStraw(string str);
	LAVVol ParseLAV(string str);

	bool fDetPath[13];
	TVector3 fVolumePos[13];
	bool fGTKPath[3];
	TVector3 fGTKPos[3];
	bool fStrawPath[3];
	TVector3 fStrawPos[4];
	bool fLAVPath[13];
	TVector3 fLAVPos[13];
	int fTrackNumber;

	TCanvas *fCanvas;

	map<int, int> fLAVDictionary;
	map<int, int> fGTKDictionary;

	TVector3 fMomentumAfterMagnet, fMomentumCenterMagnet;
	TVector3 fPositionAfterMagnet, fPositionCenterMagnet;
	bool fMagnetEffect;
};

#endif

