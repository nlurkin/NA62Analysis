// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

/// \class SpectrometerVertexBuilder
/// \Brief
/// Builds all possible 3-track vertixes and saves those with good chi2.
/// \EndBrief
/// \Detailed
/// Example of use (a loop over all 3-track vertices):
/// \code
///  OutputState state;
///  std::vector<SpectrometerTrackVertex> Vertices =
///    *(std::vector<SpectrometerTrackVertex>*)GetOutput("SpectrometerVertexBuilder.Output", state);
///  for (UInt_t i=0; i<Vertices.size(); i++) {
///    SpectrometerTrackVertex Vtx = Vertices[i];
///    // ...
///    }
///  }
/// \endcode
/// \author Evgueni Goudzovski (eg@hep.ph.bham.ac.uk)
/// \EndDetailed

#include <stdlib.h>
#include <iostream>
#include "SpectrometerVertexBuilder.hh"
#include "Event.hh"
#include "Persistency.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

SpectrometerVertexBuilder::SpectrometerVertexBuilder(Core::BaseAnalysis *ba) :
  Analyzer(ba, "SpectrometerVertexBuilder") {
  RequestTree("Spectrometer", new TRecoSpectrometerEvent, "Reco");
  fMaxNTracks   = 50;
  fMaxNVertices = 50;
}

void SpectrometerVertexBuilder::InitOutput() {
  RegisterOutput("Output", &fContainer);
}

void SpectrometerVertexBuilder::Process(int iEvent) {

  TRecoSpectrometerEvent* STRAWevent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  SetOutputState("Output", kOValid);
  fContainer.clear();

  TMatrixD cov0(5,5), C(5,5), par(5,1);
  C(0,0) = 1.0;
  C(1,1) = 1.0;
  C(2,2) = 1.0;
  C(3,3) = 1.0;

  if ((UInt_t)STRAWevent->GetNCandidates()>fMaxNTracks) return; // give up

  // check all combinations of three tracks
  Int_t itr[3];
  for (itr[0]=0; itr[0]<STRAWevent->GetNCandidates(); itr[0]++) {
    for (itr[1]=itr[0]+1; itr[1]<STRAWevent->GetNCandidates(); itr[1]++) {
      for (itr[2]=itr[1]+1; itr[2]<STRAWevent->GetNCandidates(); itr[2]++) {

	// Build a vertex
	fVertexLSF.Reset();
	Int_t Charge = 0;
	for (Int_t iTrack=0; iTrack<STRAWevent->GetNCandidates(); iTrack++) {
	  if (iTrack!=itr[0] && iTrack!=itr[1] && iTrack!=itr[2]) continue;

	  TRecoSpectrometerCandidate *cand =
	    (TRecoSpectrometerCandidate*)STRAWevent->GetCandidate(iTrack);
	  Charge += cand->GetCharge();

	  // Transform the covariance matrix from the spectrometer fit
	  // because it is for sigma (1/P) instead of sigma (P)
	  for (Int_t m=0; m<5; m++) {
	    for (Int_t n=0; n<5; n++) {
	      cov0(m,n) = cand->GetCovariance(m,n);
	    }
	  }
	  C(4,4) = -cand->GetMomentum()*cand->GetMomentum();
	  TMatrixD cov1(C, TMatrixD::kTransposeMult, cov0);
	  TMatrixD cov2(cov1, TMatrixD::kMult, C);

	  // Track parameters from the spectrometer at the standard Z-position (z=180m) 
	  par(0,0) = cand->GetSlopeXBeforeMagnet();
	  par(1,0) = cand->GetSlopeYBeforeMagnet();
	  par(2,0) = cand->GetPositionBeforeMagnet().X();
	  par(3,0) = cand->GetPositionBeforeMagnet().Y();
	  par(4,0) = cand->GetMomentum();
	  fVertexLSF.AddTrack(cov2, par);
	}

	fVertexLSF.FitStraightTracksNoBlueField();
	if (fVertexLSF.GetChi2()>100) continue;

	// Save the vertex into a common structure
	SpectrometerTrackVertex Vertex;
	Vertex.SetNTracks (fVertexLSF.GetNTracks());
	Vertex.SetCharge  (Charge);
	Vertex.SetPosition(fVertexLSF.GetVertexPosition());
	Vertex.SetChi2    (fVertexLSF.GetChi2());

	TVector3 TotalThreeMomentum = TVector3(0.0, 0.0, 0.0);
	for (int iTrack=0; iTrack<fVertexLSF.GetNTracks(); iTrack++) {
	  Vertex.AddTrack(itr[iTrack], fVertexLSF.GetTrackThreeMomentum(iTrack));
	  TotalThreeMomentum += fVertexLSF.GetTrackThreeMomentum(iTrack);
	}
	Vertex.SetTotalThreeMomentum(TotalThreeMomentum);
	fContainer.push_back(Vertex);
	if (fContainer.size()>fMaxNVertices) return; // too many vertices, give up
      }
    }
  }
}
