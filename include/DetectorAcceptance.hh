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
/// Provide a good description of detector using a geometry file.
/// \EndBrief 
///
/// \Detailed
/// Provide a good description of detector using a geometry file.\n
///	WARNING: this class provides "exact" results, there is not tolerance. If the particle\n
///	passes in the sensitive volume of a detector it's considered as detected, wether it's close\n
///	to the edge or not. But as the class gives you the position of the passage point and the \n
///	estimated path length in the sensitive volume, you can apply further cuts from this\n
///	information at your convenience.
/// \EndDetailed

class DetectorAcceptance
{
public:
	/// List of volumes of the geometry
	enum volume {kCEDAR, kGTK, kCHANTI, kLAV, kSpectrometer, kIRC, kCHOD, kLKr, kSAC, kMUV0, kMUV1, kMUV2, kMUV3, kHAC, kVOID};
	/// List of GTK parts
	enum GTKVol {kGTK0, kGTK1, kGTK2, kNOGTK};
	/// List of straw parts
	enum StrawVol {kSTRAW0, kSTRAW1, kSTRAW2, kSTRAW3, kNOSTRAW};
	/// List of LAV stations
	enum LAVVol {kLAV0, kLAV1, kLAV2, kLAV3, kLAV4, kLAV5, kLAV6, kLAV7, kLAV8, kLAV9, kLAV10, kLAV11, kNOLAV};

	DetectorAcceptance(TString GeometryFile);
	~DetectorAcceptance();

	TGeoManager *GetGeoManager();

	//Path Filling
	void FillPath(TVector3 position, TVector3 momentum, double precision=0, int q=0);
	void CleanDetPath();

	//Drawing
	void DrawDetector();
	void DrawTracks();
	void DrawPoint(double x, double y, double z);
	void ClearTracks();
	void CreateTrack(int pdgID,TVector3 position, TVector3 momentum, int charge);

	//Acceptance checking
	volume CheckDetectorAcceptPoint(Double_t x,Double_t y, Double_t z);
	volume FirstTouchedDetector();
	bool GetDetAcceptance(volume det);
	TVector3 GetDetPosition(volume det);
	double GetDetLength(volume det);
	bool GetGTKAcceptance(GTKVol det);
	TVector3 GetGTKPosition(GTKVol det);
	double GetGTKLength(GTKVol det);
	bool GetStrawAcceptance(StrawVol det);
	TVector3 GetStrawPosition(StrawVol det);
	double GetStrawLength(StrawVol det);
	bool GetLAVAcceptance(LAVVol det);
	TVector3 GetLAVPosition(LAVVol det);
	double GetLAVLength(LAVVol det);

	//Magnetic field effect
	bool MagPropagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition, TVector3& EndMomentum );
	bool MagnetEffect(const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, TVector3 &middlePosition, TVector3 &middleMomentum, TVector3 &endPosition, TVector3 &endMomentum);
private:
	//Internal methods for magnetic field effect
	void MagPropagateMagnet( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ, TVector3& EndPosition, TVector3& EndMomentum );
	bool MagPropagateBefore( const TVector3 StartPosition, const TVector3 StartMomentum, const Double_t fEndZ, TVector3& EndPosition);
	void UpdateTrackingMomentum(double precision, int q, TVector3 currPos, TVector3 startMomentum);

	//Build dictionaries for detectors substations
	void buildDetectorsDictionaries();

	//Node name parsing
	volume ParseDetector(string str);
	GTKVol ParseGTK(string str);
	StrawVol ParseStraw(string str);
	LAVVol ParseLAV(string str);

	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity level
	TGeoManager *fGeoManager; ///< TGeoManager

	//Contains the results of FillPath
	bool fDetPath[15];	///< Array of boolean (Cell index ~ enum volume). True if corresponding detector sensitive volume has been touched.
	TVector3 fVolumePos[15];	///< Array of TVector3 (Cell index ~ enum volume). If the corresponding detector sensitive volume has been touched, the position of the first contact is stored.
	double fDetLength[15];	///< Array of double (Cell index ~ enum volume). If the corresponding detector sensitive volume has been touched, the length of the path in the sensitive volume is stored.
	bool fGTKPath[4];	///< Array of boolean (Cell index ~ enum GTKVol). True if corresponding GTK station sensitive volume has been touched.
	TVector3 fGTKPos[4];///< Array of TVector3 (Cell index ~ enum GTKVol). If the corresponding GTK station sensitive volume has been touched, the position of the first contact is stored.
	double fGTKLength[4];	///< Array of double (Cell index ~ enum GTKVol). If the corresponding GTK station sensitive volume has been touched, the length of the path in the sensitive volume is stored.
	bool fStrawPath[5];	///< Array of boolean (Cell index ~ enum StrawVol). True if corresponding straw station sensitive volume has been touched.
	TVector3 fStrawPos[5];	///< Array of TVector3 (Cell index ~ enum StrawVol). If the corresponding straw station sensitive volume has been touched, the position of the first contact is stored.
	double fStrawLength[5];	///< Array of double (Cell index ~ enum StrawVol). If the corresponding straw station sensitive volume has been touched, the length of the path in the sensitive volume is stored.
	bool fLAVPath[13];	///< Array of boolean (Cell index ~ enum LAVVol). True if corresponding LAV station sensitive volume has been touched.
	TVector3 fLAVPos[13];	///< Array of TVector3 (Cell index ~ enum LAVVol). If the corresponding LAV station sensitive volume has been touched, the position of the first contact is stored.
	double fLAVLength[13];	///< Array of double (Cell index ~ enum LAVVol). If the corresponding LAV station sensitive volume has been touched, the length of the path in the sensitive volume is stored.

	//For drawing
	int fTrackNumber;	///< Number of existing tracks
	TCanvas *fCanvas;	///< Canvas for drawing

	//Dictionaries
	map<int, int> fLAVDictionary;	///< Map for correspondence between geometry element index and LAV station index
	map<int, int> fGTKDictionary;	///< Map for correspondence between geometry element index and GTK station index

	//For keeping track of magnet effect
	TVector3 fMomentumAfterMagnet;	///< Particle momentum after MNP33 magnet
	TVector3 fMomentumCenterMagnet;	///< Particle momentum in MNP33 magnet
	TVector3 fPositionAfterMagnet;	///< Particle position after MNP33 magnet
	TVector3 fPositionCenterMagnet;	///< Particle position in MNP33 magnet
	bool fMagnetEffect;	///< Did we already computed the magnet effect?
};

#endif

