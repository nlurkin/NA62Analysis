// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

/// \class SpecMUV3ParticleID
/// \Brief
/// Geometrical association of MUV3 candidates to Spectrometer candidates
/// \EndBrief
/// \Detailed
/// Builds an array of SpecMUV3Association objects.
/// The number of these objects is equal to the number of Spectrometer tracks.
/// Each of them contains the track ID and basic track information, as well as
/// a collection of SpecMUV3AssociationRecords.
/// Each SpecMUV3AssociationRecord contains the ID and basic information about a
/// MUV3 candidate associated (i.e. found to be geometrically compatible) with the track.
/// The geometrical compatibility is determined by extrapolating the track to the MUV3
/// front plane, and checking if the MUV3 candidate tile overlaps with a "search circle"
/// around the track impact point. The "search radius" is inversely proportional to
/// the track momentum.
/// No SpecMUV3AssociationRecords in a SpecMUV3Association structure means that
/// no MUV3 candidates are associated with the track. Example of use:
/// \code
///  OutputState state;
///  std::vector<SpecMUV3Association> SpecMUV3 =
///    *(std::vector<SpecMUV3Association>*)GetOutput("SpecMUV3ParticleID.Output", state);
///  for (UInt_t itrack=0; itrack<SpecMUV3.size(); itrack++) {
///    SpecMUV3Association sm = SpecMUV3[itrack];
///    sm.Print();
///    for (UInt_t i=0; i<Match.GetNAssociationRecords(); i++) {
///      SpecMUV3AssociationRecord Record = Match.GetAssociationRecord(i);
///      Record.Print();
///    }
///  }
/// \endcode
/// \author Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)
/// \EndDetailed

#include <stdlib.h>
#include <iostream>
#include "SpecMUV3ParticleID.hh"
#include "Event.hh"
#include "Persistency.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

SpecMUV3ParticleID::SpecMUV3ParticleID(Core::BaseAnalysis *ba) : Analyzer(ba, "SpecMUV3ParticleID") {
  fScaleFactor = 4.0; // defines the track-MUV3 candidate association distance
  RequestTree("MUV3",         new TRecoMUV3Event,         "Reco");
  RequestTree("Spectrometer", new TRecoSpectrometerEvent, "Reco");
}

void SpecMUV3ParticleID::InitOutput() {
  RegisterOutput("Output", &fContainer);
}

void SpecMUV3ParticleID::Process(int iEvent) {

  TRecoMUV3Event* MUV3event = (TRecoMUV3Event*)GetEvent("MUV3");
  TRecoSpectrometerEvent* STRAWevent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  SetOutputState("Output", kOValid);

  fContainer.clear();
  for (Int_t iTrack=0; iTrack<STRAWevent->GetNCandidates(); iTrack++) {
    TRecoSpectrometerCandidate* Scand = (TRecoSpectrometerCandidate*)STRAWevent->GetCandidate(iTrack);
    Double_t xt   = Scand->xAtAfterMagnet(246800);
    Double_t yt   = Scand->yAtAfterMagnet(246800);
    Double_t Time = Scand->GetTime();

    ////////////////////////////////////////////////////////////////////////////////
    // Estimated effect of scattering and STRAW resolution: sigma(xP,yP)=530 mm*GeV.

    Double_t Momentum      = (Scand->GetMomentum()>0) ? Scand->GetMomentum() : 10000.0;
    Double_t SearchRadius  = fScaleFactor * 530000 / Momentum;
    Bool_t InnerAcceptance = (sqrt(xt*xt + yt*yt)>103.0);
    Bool_t OuterAcceptance = (fabs(xt)<1320.0 && fabs(yt)<1320.0);
    Bool_t InAcceptance    = InnerAcceptance && OuterAcceptance;

    Double_t DistanceToOuterEdge = 0.0;
    if (InAcceptance) {
      DistanceToOuterEdge = TMath::Min(1320-fabs(xt), 1320-fabs(yt));
    }
    else {
      if      (fabs(xt)>1320 && fabs(yt)<1320) DistanceToOuterEdge = 1320-fabs(xt);
      else if (fabs(yt)>1320 && fabs(xt)<1320) DistanceToOuterEdge = 1320-fabs(yt);
      else    DistanceToOuterEdge = sqrt((xt-1320)*(xt-1320)+(yt-1320)*(yt-1320));
    }

    SpecMUV3Association Asso(iTrack);
    Asso.SetTrackTime(Time);
    Asso.SetTrackXY(xt, yt);
    Asso.SetSearchRadius(SearchRadius);
    Asso.SetTrackMomentum(Scand->GetMomentum());
    Asso.SetInAcceptance(InAcceptance);
    Asso.SetDistanceToEdge(DistanceToOuterEdge);

    for (Int_t iMu=0; iMu<MUV3event->GetNCandidates(); iMu++) {
      TRecoMUV3Candidate* Mcand = (TRecoMUV3Candidate*)MUV3event->GetCandidate(iMu);
      Int_t    iTile = Mcand->GetTileID();
      Double_t xm    = Mcand->GetX();
      Double_t ym    = Mcand->GetY();

      ///////////////////////////////////////////////////
      // Find corners of the tile; check for direct match

      Double_t length = 220.0;
      if (iTile>=144) length *= (2.0/3.0);
      Double_t xm1 = xm - 0.5*length;
      Double_t xm2 = xm + 0.5*length;
      Double_t ym1 = ym - 0.5*length;
      Double_t ym2 = ym + 0.5*length;

      Double_t TrackTileDistance = 0.0;
      Bool_t   DirectMatch       = (xt>xm1 && xt<xm2 && yt>ym1 && yt<ym2);

      if (!DirectMatch) { // extrapolated track misses the tile: compute the distance
	if      (yt>ym1 && yt<ym2) TrackTileDistance = TMath::Min(fabs(xt-xm1), fabs(xt-xm2));
	else if (xt>xm1 && xt<xm2) TrackTileDistance = TMath::Min(fabs(yt-ym1), fabs(yt-ym2));
	else if (xt>xm2 && yt>ym2) TrackTileDistance = sqrt((xt-xm2)*(xt-xm2)+(yt-ym2)*(yt-ym2));
	else if (xt>xm2 && yt<ym1) TrackTileDistance = sqrt((xt-xm2)*(xt-xm2)+(yt-ym1)*(yt-ym1));
	else if (xt<xm1 && yt<ym1) TrackTileDistance = sqrt((xt-xm1)*(xt-xm1)+(yt-ym1)*(yt-ym1));
	else if (xt<xm1 && yt>ym2) TrackTileDistance = sqrt((xt-xm1)*(xt-xm1)+(yt-ym2)*(yt-ym2));
      }

      if (TrackTileDistance<SearchRadius) {
        Double_t TrackCandidateDistance = sqrt((xt-xm)*(xt-xm)+(yt-ym)*(yt-ym));
	Double_t Time = Mcand->GetTime();
	SpecMUV3AssociationRecord
	  Record(iMu, iTile, Time, TrackTileDistance, TrackCandidateDistance, xm, ym);
	Asso.AddAssociationRecord(Record);
      }
    }
    fContainer.push_back(Asso);
  }
}
