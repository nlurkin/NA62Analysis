// ---------------------------------------------------------------                                          
// History:                                                                                                 
//                                                                                                          
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-30
//                                                                                                          
// ---------------------------------------------------------------  

#include <stdlib.h>
#include <iostream>
#include <TChain.h>
#include "MUV3MCTester.hh"
#include "functions.hh"
#include "Event.hh"
#include "Persistency.hh"

#include "TFile.h"
#include "TMUV3Hit.hh"
#include <numeric>

using namespace NA62Analysis;
using namespace NA62Constants;

/// \class MUV3MCTester
/// \Brief
/// A test tool to check the output of the MUV3 component of NA62MC
/// \EndBrief

MUV3MCTester::MUV3MCTester(Core::BaseAnalysis *ba) : Analyzer(ba, "MUV3MCTester") {
  RequestTree("MUV3", new TMUV3Event);
}

void MUV3MCTester::InitHist() {
  BookHisto(new TH1D("NHits", "Number of MUV3 hits per event;Number of hits;", 30, -0.5, 29.5));
  BookHisto(new TH1D("ChannelID",
		     "Hit channel ID: normal hits;Geometric channel ID;", 360, -0.5, 359.5));
  BookHisto(new TH1D("ChannelIDCherenkov",
		     "Hit channel ID: Cherenkov hits;Geometric channel ID;", 360, -0.5, 359.5));
  BookHisto(new TH1D("EnergyDeposit",
		     "Energy deposit in a counter: expect a MIP peak at 10 MeV;Energy deposit [MeV];",
		     200, 0, 100));
  BookHisto(new TH1D("Zhit",
		     "Hit Z position (i.e. the position of the first energy deposit);Hit Z coordinate [m];",
		     150, 246.7, 247.0));
  BookHisto(new TH2D("XYhit",
		     "Hit XY positions;Hit X coordinate [m];Hit Y coordinate [m]",
		     150, -1.5, 1.5, 150, -1.5, 1.5));
  BookHisto(new TH1D("DistanceToMuonX",
		     "X distance: extrapolated true muon to MUV3 hit;Delta(x) [mm];",
		     100, -250, 250));
  BookHisto(new TH1D("DistanceToMuonY",
		     "Y distance: extrapolated true muon to MUV3 hit;Delta(x) [mm];",
		     100, -250, 250));
  BookHisto(new TH1D("DistanceToMuon",
		     "Distance: extrapolated true muon to MUV3 hit;Distance [mm];",
		     100, 0, 500));
}

void MUV3MCTester::Process(int iEvent) {

  TMUV3Event *MUV3Event = (TMUV3Event*)GetEvent("MUV3");
  int NHits = MUV3Event->GetNHits();
  FillHisto("NHits", NHits);

  ////////////////////////////////////////
  // Loop over hits, fill a few histograms

  for (int ihit=0; ihit<NHits; ihit++) {
    TMUV3Hit *Hit = (TMUV3Hit*)MUV3Event->GetHits()->At(ihit);
    int id   = Hit->GetChannelID();
    double x = Hit->GetPosition().X();
    double y = Hit->GetPosition().Y();
    double z = Hit->GetPosition().Z();
    double Energy = Hit->GetEnergy();

    if (Energy>0) { // normal hits
      FillHisto("ChannelID", id);
      if (id<200) { // normal hits are duplicated, plot only one of the two
	FillHisto("EnergyDeposit", Energy);
	FillHisto("XYhit", 0.001*x, 0.001*y); // [m]
	FillHisto("Zhit", 0.001*z); // [m]
      }
    }
    else { // "Cherenkov" hits directly on the PMT window
      FillHisto("ChannelIDCherenkov", id);
    }
  }

  ////////////////////////////////////////////////////////////////
  // Attempt to find a single generated muon for muon-hit matching

  Event *evt = GetMCEvent();
  int Npart = evt->GetNKineParts();
  int NMuons = 0, imuon = -999;
  for (int ipart=0; ipart<Npart; ipart++) {
    KinePart *Particle = (KinePart*)evt->GetKineParts()->At(ipart);
    int code = Particle->GetPDGcode();
    if (abs(code)==13) { // muon
      NMuons++;
      imuon = ipart;
    }
  }

  /////////////////////////////
  // Match a hit to a true muon

  if (NMuons==1 && NHits==2) {
    KinePart *Muon = (KinePart*)evt->GetKineParts()->At(imuon);
    double xmu = Muon->xAt(246800);
    double ymu = Muon->yAt(246800);

    TMUV3Hit *Hit = (TMUV3Hit*)MUV3Event->GetHits()->At(0);
    double xhit = Hit->GetPosition().X();
    double yhit = Hit->GetPosition().Y();
    double dist = sqrt((xhit-xmu)*(xhit-xmu) + (yhit-ymu)*(yhit-ymu));
    FillHisto("DistanceToMuonX", xhit-xmu);
    FillHisto("DistanceToMuonY", yhit-ymu);
    FillHisto("DistanceToMuon", dist);
  }
}

void MUV3MCTester::EndOfRunUser() {
  SaveAllPlots();
}
