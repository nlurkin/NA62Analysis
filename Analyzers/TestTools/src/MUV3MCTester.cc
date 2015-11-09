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
/// A test tool to check MUV3 simulation in NA62MC
/// \EndBrief

MUV3MCTester::MUV3MCTester(Core::BaseAnalysis *ba) : Analyzer(ba, "MUV3MCTester") {
  RequestTree("MUV3", new TMUV3Event);
}

void MUV3MCTester::InitHist() {
  BookHisto(new TH1D("NHits",
		     "Number of MUV3 hits per event;Number of hits;",
		     30, -0.5, 29.5));
  BookHisto(new TH1D("NMuonHits",
		     "Number of MUV3 muon hits per event;Number of hits;",
		     30, -0.5, 29.5));
  BookHisto(new TH1D("ChannelID",
		     "Hit channel ID: normal hits;Geometric channel ID;",
		     360, -0.5, 359.5));
  BookHisto(new TH1D("ChannelIDCherenkov",
		     "Hit channel ID: Cherenkov hits;Geometric channel ID;",
		     360, -0.5, 359.5));
  BookHisto(new TH1D("EnergyDeposit",
		     "Energy deposit in a counter: expect a MIP peak at 10 MeV;Energy deposit [MeV];",
		     100, 0, 50));
  BookHisto(new TH1D("EnergyDepositMuonHit",
		     "Energy deposit from muon hits in a counter;Energy deposit [MeV];",
		     100, 0, 50));
  BookHisto(new TH2D("EnergyDepositVsTime",
		     "Energy deposit vs time;Hit time [ns];Energy deposit [MeV];",
		     50, 810, 860, 100, 0, 100));
  BookHisto(new TH1D("Zhit",
		     "Hit Z position (i.e. the position of the first energy deposit);Hit Z coordinate [m]",
		     75, 246.75, 246.90));
  BookHisto(new TH1D("ZhitMuonHit",
		     "Hit Z position for muon hits;Hit Z coordinate [m]",
		     75, 246.75, 246.90));
  BookHisto(new TH2D("EnergyDepositVsZhit",
		     "Energy deposit vs Z position;Hit Z coordinate [m]",
		     100, 246.75, 246.90, 100, 0, 100));
  BookHisto(new TH2D("XYhit",
		     "Hit XY positions;Hit X coordinate [m];Hit Y coordinate [m]",
		     150, -1.5, 1.5, 150, -1.5, 1.5));
  BookHisto(new TH2D("XYhitMuonHit",
		     "Hit XY positions for muon hits;Hit X coordinate [m];Hit Y coordinate [m]",
		     150, -1.5, 1.5, 150, -1.5, 1.5));
  BookHisto(new TH1D("DistanceToMuonX",
		     "X distance: MUV3 hit to extrapolated true muon;Delta(x) [mm];",
		     100, -250, 250));
  BookHisto(new TH1D("DistanceToMuonY",
		     "Y distance: MUV3 hit to extrapolated true muon;Delta(y) [mm];",
		     100, -250, 250));
  BookHisto(new TH1D("DistanceToMuon",
		     "Distance: MUV3 hit to extrapolated true muon;Distance [mm];",
		     100, 0, 500));
}

void MUV3MCTester::Process(int iEvent) {

  TMUV3Event *MUV3Event = (TMUV3Event*)GetEvent("MUV3");
  int NHits = MUV3Event->GetNHits();
  int NMuonHits = 0;
  for (int ihit=0; ihit<NHits; ihit++) {
    TMUV3Hit *Hit = (TMUV3Hit*)MUV3Event->GetHits()->At(ihit);
    if (Hit->IsMuonHit()) NMuonHits++;
  }

  FillHisto("NHits", NHits);
  FillHisto("NMuonHits", NMuonHits);

  ////////////////////////////////////////
  // Loop over hits, fill a few histograms

  for (int ihit=0; ihit<NHits; ihit++) {
    TMUV3Hit *Hit  = (TMUV3Hit*)MUV3Event->GetHits()->At(ihit);
    int id         = Hit->GetChannelID();
    double x       = Hit->GetPosition().X();
    double y       = Hit->GetPosition().Y();
    double z       = Hit->GetPosition().Z();
    double Energy  = Hit->GetEnergy();
    double Time    = Hit->GetTime();
    bool   MuonHit = Hit->IsMuonHit();

    //bool HitInCentralCounter  = ((id%200)>=144);

    if (Energy>0) { // Scintillator (not PMT window) hits

      FillHisto("ChannelID", id);
      if (id>=200) continue; // hits are duplicated for the PMTs

      FillHisto("EnergyDeposit", Energy);
      if (MuonHit) FillHisto("EnergyDepositMuonHit", Energy);
      FillHisto("EnergyDepositVsTime", Time, Energy);
      FillHisto("EnergyDepositVsZhit", 0.001*z, Energy);
      FillHisto("XYhit", 0.001*x, 0.001*y); // [m]
      FillHisto("Zhit", 0.001*z); // [m]
      if (MuonHit) {
	FillHisto("XYhitMuonHit", 0.001*x, 0.001*y); // [m]
	FillHisto("ZhitMuonHit", 0.001*z); // [m]
      }
    }

    if (Energy<0) { // "Cherenkov" hits directly on the PMT window
      FillHisto("ChannelIDCherenkov", id);
    }
  }

  /////////////////////////////////////////////////////////
  // Look for a single generated muon for muon-hit matching

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

  ////////////////////////////////////
  // Evaluate the muon to hit distance

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
