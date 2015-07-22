// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include "SpecMUV3Muon.hh"
#include "SpecMUV3Association.hh"
#include "Event.hh"
#include "Persistency.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

/// \class SpecMUV3Association
/// \Brief
/// Geometrical association of MUV3 candidates to Spectrometer candidates
/// \EndBrief
/// \Detailed
/// Builds an array of SpecMUV3Muon objects.
/// The number of SpecMUV3Muon objects is equal to the number of Spectrometer tracks.
/// Each object contains the track ID and basic track information, as well as
/// a collection of SpecMUV3AssociationRecords.
/// Each SpecMUV3AssociationRecord contains the ID and basic information of a
/// MUV3 candidate found to be geometrically compatible with the extrapolated track
/// (i.e. the MUV3 tile intersecting the circle around the projected track impact point
/// with a radius equal to the search radius; the latter scales as inverse track momentum).
/// No SpecMUV3AssociationRecords in the SpecMUV3Muon structure means that
/// no MUV3 candidates are associated to the track. Example of use:
/// \code
///  OutputState state;
///  std::vector<SpecMUV3Muon> SpecMUV3Muons =
///    *(std::vector<SpecMUV3Muon>*)GetOutput("SpecMUV3Association.MuonContainer", state);
///
///  for (int i=0; i<SpecMUV3Muons.size(); i++) {
///    SpecMUV3Muon Muon = SpecMUV3Muons[i];
///    Muon.Print();
///    for (int i=0; i<Muon.GetNAssociationRecords(); i++) {
///      SpecMUV3AssociationRecord Record = Muon.GetAssociationRecord(i);
///      Record.Print();
///    }
///  }
/// \endcode
/// \EndDetailed

SpecMUV3Association::SpecMUV3Association(Core::BaseAnalysis *ba) : Analyzer(ba, "SpecMUV3Association") {
  fScaleFactor = 4.0; // defines the track-MUV3 candidate association distance
  RequestTree("MUV3",         new TRecoMUV3Event,         "Reco");
  RequestTree("Spectrometer", new TRecoSpectrometerEvent, "Reco");
}

void SpecMUV3Association::InitOutput() {
  RegisterOutput("MuonContainer", &fMuons);
}

void SpecMUV3Association::Process(int iEvent) {

  TRecoMUV3Event* MUV3event = (TRecoMUV3Event*)GetEvent("MUV3");
  TRecoSpectrometerEvent* STRAWevent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  SetOutputState("MuonContainer", kOValid);

  fMuons.clear();
  for (Int_t iTrack=0; iTrack<STRAWevent->GetNCandidates(); iTrack++) {
    TRecoSpectrometerCandidate* Scand = (TRecoSpectrometerCandidate*)STRAWevent->GetCandidate(iTrack);
    Double_t xt   = Scand->xAtAfterMagnet(246800);
    Double_t yt   = Scand->yAtAfterMagnet(246800);
    Double_t Time = Scand->GetTime();

    ////////////////////////////////////////////////////////////////////////////////
    // Estimated effect of scattering and STRAW resolution: sigma(xP,yP)=530 mm*GeV.

    Double_t Momentum     = (Scand->GetMomentum()>0) ? Scand->GetMomentum() : 10000.0;
    Double_t SearchRadius = fScaleFactor * 530000 / Momentum;

    SpecMUV3Muon Muon(iTrack, Time, Scand->GetMomentum(), SearchRadius, xt, yt);

    for (Int_t iMu=0; iMu<MUV3event->GetNCandidates(); iMu++) {
      TRecoMUV3Candidate* Mcand = (TRecoMUV3Candidate*)MUV3event->GetCandidate(iMu);
      Int_t iTile = Mcand->GetTileID();
      Double_t xm = Mcand->GetX();
      Double_t ym = Mcand->GetY();

      //////////////////////////////////////////////
      // The corners of the tile; check direct match

      Double_t length = 220;
      if (iTile>=144) length *= (2.0/3.0);
      Double_t xm1 = xm - 0.5*length;
      Double_t xm2 = xm + 0.5*length;
      Double_t ym1 = ym - 0.5*length;
      Double_t ym2 = ym + 0.5*length;

      Double_t TrackTileDistance = -1;
      Bool_t DirectMatch = (xt>xm1 && xt<xm2 && yt>ym1 && yt<ym2);

      if (!DirectMatch) {
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
	SpecMUV3AssociationRecord Record(iMu, iTile, Time,
					 TrackTileDistance, TrackCandidateDistance, xm, ym);
	Muon.AddAssociationRecord(Record);
      }
    }
    fMuons.push_back(Muon);
  }
}
