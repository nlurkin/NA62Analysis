// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-09
//
// ---------------------------------------------------------------

/// \class SpectrometerVertexBuilder
/// \Brief
/// Builds all possible N-track vertices and saves those with good chi2
/// into a container of SpectrometerTrackVertex objects. The values of N used
/// in a vertex are determined by the boolean variables fBuildNTrackVertices, N=2,3,4,5.
/// If several of these flags are ON, then the output container in general contains
/// vertices with different numbers of tracks.
/// \EndBrief
/// \Detailed
/// Example of use (a loop over all vertices):
/// \code
///  OutputState state;
///  std::vector<SpectrometerTrackVertex> Vertices =
///    *(std::vector<SpectrometerTrackVertex>*)GetOutput("SpectrometerVertexBuilder.Output", state);
///  for (UInt_t i=0; i<Vertices.size(); i++) {
///    SpectrometerTrackVertex vtx = Vertices[i];
///    vtx.Print();
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

  // Which types of vertices to build?
  fBuild2TrackVertices = kFALSE;
  fBuild3TrackVertices = kTRUE;
  fBuild4TrackVertices = kFALSE;
  fBuild5TrackVertices = kFALSE;

  fMaxNTracks   = 50;
  fMaxNVertices = 50;
  fMaxChi2      = 100.0;
}

void SpectrometerVertexBuilder::InitOutput() {
  RegisterOutput("Output", &fContainer);
}

void SpectrometerVertexBuilder::Process(Int_t iEvent) {

  SetOutputState("Output", kOValid);
  fContainer.clear();

  TRecoSpectrometerEvent* STRAWevent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  if (!STRAWevent) return;
  if ((UInt_t)STRAWevent->GetNCandidates()>fMaxNTracks) return; // too many tracks, give up
  if ((UInt_t)STRAWevent->GetNCandidates()<2)           return; // too few tracks
  
  Int_t ind[5];

  // Two-track vertices
  if (fBuild2TrackVertices) {
    for (ind[0]=0; ind[0]<STRAWevent->GetNCandidates(); ind[0]++) {
      for (ind[1]=ind[0]+1; ind[1]<STRAWevent->GetNCandidates(); ind[1]++) {
	BuildVertex(ind, 2);
      }
    }
  }

  // Three-track vertices
  if ((UInt_t)STRAWevent->GetNCandidates()<3) return;
  if (fBuild3TrackVertices) {
    for (ind[0]=0; ind[0]<STRAWevent->GetNCandidates(); ind[0]++) {
      for (ind[1]=ind[0]+1; ind[1]<STRAWevent->GetNCandidates(); ind[1]++) {
	for (ind[2]=ind[1]+1; ind[2]<STRAWevent->GetNCandidates(); ind[2]++) {
	  BuildVertex(ind, 3);
	}
      }
    }
  }

  // Four-track vertices
  if ((UInt_t)STRAWevent->GetNCandidates()<4) return;
  if (fBuild4TrackVertices) {
    for (ind[0]=0; ind[0]<STRAWevent->GetNCandidates(); ind[0]++) {
      for (ind[1]=ind[0]+1; ind[1]<STRAWevent->GetNCandidates(); ind[1]++) {
	for (ind[2]=ind[1]+1; ind[2]<STRAWevent->GetNCandidates(); ind[2]++) {
	  for (ind[3]=ind[2]+1; ind[3]<STRAWevent->GetNCandidates(); ind[3]++) {
	    BuildVertex(ind, 4);
	  }
	}
      }
    }
  }

  // Five-track vertices
  if ((UInt_t)STRAWevent->GetNCandidates()<5) return;
  if (fBuild5TrackVertices) {
    for (ind[0]=0; ind[0]<STRAWevent->GetNCandidates(); ind[0]++) {
      for (ind[1]=ind[0]+1; ind[1]<STRAWevent->GetNCandidates(); ind[1]++) {
	for (ind[2]=ind[1]+1; ind[2]<STRAWevent->GetNCandidates(); ind[2]++) {
	  for (ind[3]=ind[2]+1; ind[3]<STRAWevent->GetNCandidates(); ind[3]++) {
	    for (ind[4]=ind[3]+1; ind[4]<STRAWevent->GetNCandidates(); ind[4]++) {
	      BuildVertex(ind, 5);
	    }
	  }
	}
      }
    }
  }
}

void SpectrometerVertexBuilder::BuildVertex(Int_t ind[], Int_t NTracks) {
  TRecoSpectrometerEvent* STRAWevent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  fVertexLSF.Reset();

  TMatrixD cov0(5,5), C(5,5), par(5,1);
  C(0,0) = 1.0;
  C(1,1) = 1.0;
  C(2,2) = 1.0;
  C(3,3) = 1.0;

  Int_t Charge = 0;
  for (Int_t iTrack=0; iTrack<NTracks; iTrack++) {
    TRecoSpectrometerCandidate *cand =
      (TRecoSpectrometerCandidate*)STRAWevent->GetCandidate(ind[iTrack]);
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
  if (fVertexLSF.GetChi2()>fMaxChi2) return;

  // Save the vertex into a common structure
  SpectrometerTrackVertex Vertex;
  Vertex.SetNTracks (fVertexLSF.GetNTracks());
  Vertex.SetCharge  (Charge);
  Vertex.SetPosition(fVertexLSF.GetVertexPosition());
  Vertex.SetChi2    (fVertexLSF.GetChi2());

  TVector3 TotalThreeMomentum = TVector3(0.0, 0.0, 0.0);
  for (int iTrack=0; iTrack<fVertexLSF.GetNTracks(); iTrack++) {
    Vertex.AddTrack
      (ind[iTrack],
       fVertexLSF.GetTrackThreeMomentum(iTrack),
       ((TRecoSpectrometerCandidate*)STRAWevent->GetCandidate(ind[iTrack]))->GetThreeMomentumBeforeMagnet());
    TotalThreeMomentum += fVertexLSF.GetTrackThreeMomentum(iTrack);
  }
  Vertex.SetTotalThreeMomentum(TotalThreeMomentum);
  if (fContainer.size()<fMaxNVertices) fContainer.push_back(Vertex);
}
