// ---------------------------------------------------------------
//
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-08-25
//
// ---------------------------------------------------------------

#ifndef SPECTROMETERVERTEXBUILDER_HH
#define SPECTROMETERVERTEXBUILDER_HH

#include "Analyzer.hh"
#include "VertexLSF.hh"
#include "SpectrometerTrackVertex.hh"

class SpectrometerVertexBuilder : public NA62Analysis::Analyzer {

public:
  SpectrometerVertexBuilder(NA62Analysis::Core::BaseAnalysis *ba);
  void InitHist() {}
  void InitOutput();
  void DefineMCSimple() {}
  void Process(int iEvent);
  void StartOfBurstUser() {}
  void EndOfBurstUser() {}
  void StartOfRunUser() {}
  void EndOfRunUser() {}
  void PostProcess() {}
  void DrawPlot() {}

private:
  Bool_t fBuild2TrackVertices; ///< Build two-track vertices?
  Bool_t fBuild3TrackVertices; ///< Build three-track vertices?
  Bool_t fBuild4TrackVertices; ///< Build four-track vertices?
  Bool_t fBuild5TrackVertices; ///< Build five-track vertices?

  UInt_t    fMaxNTracks;   ///< Max number of tracks in event to attempt vertex fit
  UInt_t    fMaxNVertices; ///< Max number of vetrices to be built before giving up
  Double_t  fMaxChi2;      ///< Vertices with chi2 above maximum are discarded
  VertexLSF fVertexLSF;    ///< The least-squared fitter
  vector<SpectrometerTrackVertex> fContainer;

  void BuildVertex(Int_t ind[], Int_t NTracks);
};

#endif
