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
    // The search radius is currently set to 3 sigma.

    Double_t SearchRadius = (Scand->GetMomentum() > 0) ?
      3.0 * 530 * 1000 / Scand->GetMomentum() :
      3.0 * 53000; // assume a 10 GeV track if momentum is not valid

    Double_t SearchRadius2 = pow(SearchRadius, 2);
    SpecMUV3Muon Muon(iTrack, Time, SearchRadius, xt, yt);

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

      Bool_t DirectMatch = (xt>xm1 && xt<xm2 && yt>ym1 && yt<ym2);
      Bool_t Match       = DirectMatch;

      ///////////////////////////////////////////////
      // Do the tile and the search circle intersect?

      if (!Match) {
	// Are the corners of the pad within the search circle?
        Double_t R1sq = (xm1-xt)*(xm1-xt) + (ym1-yt)*(ym1-yt);
	Double_t R2sq = (xm1-xt)*(xm1-xt) + (ym2-yt)*(ym2-yt);
	Double_t R3sq = (xm2-xt)*(xm2-xt) + (ym1-yt)*(ym1-yt);
        Double_t R4sq = (xm2-xt)*(xm2-xt) + (ym2-yt)*(ym2-yt);

        if (R1sq < SearchRadius2 || R2sq < SearchRadius2 ||
            R3sq < SearchRadius2 || R4sq < SearchRadius2) Match = true;
      }

      if (!Match) {
	// Are the extremes of the search circle within the pad?
        Double_t xc1 = xt - SearchRadius;
        Double_t xc2 = xt + SearchRadius;
        Double_t yc1 = yt - SearchRadius;
        Double_t yc2 = yt + SearchRadius;
        if (xc1>xm1 && xc1<xm2 && yt>ym1 && yt<ym2) Match = true;
        if (xc2>xm1 && xc2<xm2 && yt>ym1 && yt<ym2) Match = true;
        if (yc1>ym1 && yc1<ym2 && xt>xm1 && xt<xm2) Match = true;
        if (yc2>ym1 && yc2<ym2 && xt>xm1 && xt<xm2) Match = true;   
      }

      if (Match) {
        Double_t Distance = sqrt((xt-xm)*(xt-xm)+(yt-ym)*(yt-ym));
	Double_t Time = Mcand->GetTime();
	SpecMUV3AssociationRecord Record(iMu, iTile, Time, DirectMatch, Distance, xm, ym);
	Muon.AddAssociationRecord(Record);
      }
    }
    fMuons.push_back(Muon);
  }
}
