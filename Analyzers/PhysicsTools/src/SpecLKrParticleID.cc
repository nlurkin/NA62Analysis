// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-09-15
//
// ---------------------------------------------------------------

/// \class SpecLKrParticleID
/// \Brief
/// Geometrical association of LKr candidates to Spectrometer candidates.
/// \EndBrief
/// \Detailed
/// For each track, find the LKr candidate (cluster) geometrically associated
/// to the track, and compute E/p. ClusterID=-1 means no associated cluster is found. Example of use:
/// \code
/// OutputState state;
/// std::vector<SpecLKrAssociation> SpecLKR =
///   *(std::vector<SpecLKrAssociation>*)GetOutput("SpecLKrParticleID.Output", state);
/// for (UInt_t i=0; i<SpecLKR.size(); i++) {
///   SpecLKrAssociation sl = SpecLKR[i];
///   sl.Print();
/// }
/// \endcode
/// \author Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)
/// \EndDetailed

#include <stdlib.h>
#include <iostream>
#include "SpecLKrParticleID.hh"
#include "Event.hh"
#include "Persistency.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

SpecLKrParticleID::SpecLKrParticleID(Core::BaseAnalysis *ba) : Analyzer(ba, "SpecLKrParticleID") {
  fMaxTrackClusterDistance = 50.0;
  RequestTree("LKr",          new TRecoLKrEvent,          "Reco");
  RequestTree("Spectrometer", new TRecoSpectrometerEvent, "Reco");
}

void SpecLKrParticleID::InitOutput() {
  RegisterOutput("Output", &fContainer);
}

void SpecLKrParticleID::Process(int iEvent) {

  TRecoLKrEvent* LKRevent = (TRecoLKrEvent*)GetEvent("LKr");
  TRecoSpectrometerEvent* STRAWevent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  SetOutputState("Output", kOValid);

  fContainer.clear();
  for (Int_t iTrack=0; iTrack<STRAWevent->GetNCandidates(); iTrack++) {
    TRecoSpectrometerCandidate* Scand = (TRecoSpectrometerCandidate*)STRAWevent->GetCandidate(iTrack);
    Double_t Momentum = Scand->GetMomentum();
    Double_t Energy   = 0.0;
    Double_t xt       = Scand->xAtAfterMagnet(240406);
    Double_t yt       = Scand->yAtAfterMagnet(240406);
    Double_t Rmin     = 99999;
    Int_t    ClosestClusterIndex = -1;
    for (Int_t iClus=0; iClus<LKRevent->GetNCandidates(); iClus++) {
      TRecoLKrCandidate* Lcand = (TRecoLKrCandidate*)LKRevent->GetCandidate(iClus);
      Double_t xc = Lcand->GetClusterX()*10; // cm --> mm, to overcome bug NARKD-129
      Double_t yc = Lcand->GetClusterY()*10; // cm --> mm
      Double_t R  = sqrt((xt-xc)*(xt-xc)+(yt-yc)*(yt-yc));
      if (R<Rmin && R<fMaxTrackClusterDistance) {
	Rmin = R;
	ClosestClusterIndex = iClus;
	Energy = Lcand->GetClusterEnergy()*1e3; // GeV --> MeV, to overcome bug NARKD-129
      }
    }

    SpecLKrAssociation Asso(iTrack);
    Asso.SetClusterID(ClosestClusterIndex);
    Asso.SetClusterEnergy(Energy);
    Asso.SetTrackMomentum(Momentum);
    Asso.SetEoP(Energy/Momentum);
    fContainer.push_back(Asso);
  }
}
