// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-07-27
//
// ---------------------------------------------------------------

#include "SpecRICHEventMonitor.hh"
#include "SpecRICHAssociation.hh"
#include "TLegend.h"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

/// \class SpecRICHEventMonitor
/// \Brief
/// Visualization of expected and reconstructed RICH rings
/// \EndBrief
/// \Detailed 
/// Runs on reconstruction output with the RICH and STRAW recostructions activated.
/// Builds a PDF file with one page per eligible event.
/// RICH hit positions, RICH reconstructed rings, MC expected RICH rings,
/// and expected RICH rings for each STRAW track in the electron, muon and pion hypotheses
/// are plotted. Some additional information about the event is printed out.
/// A series monitoring plots are for the whole data sample are also produced.
/// \EndDetailed

SpecRICHEventMonitor::SpecRICHEventMonitor(Core::BaseAnalysis *ba) : Analyzer(ba, "SpecRICHEventMonitor") {

  RequestTree("RICH", new TRecoRICHEvent);
  RequestTree("Spectrometer", new TRecoSpectrometerEvent);

  fOutputPDFName = "./SpecRICHEventMonitor.pdf";
  fMaxGraphCount = 100;
  fNTrackCutLow  = 1;
  fNTrackCutHigh = 3;
  fGraphCount    = 0;

  //////////////////////////////////////////////

  gStyle->SetOptStat(0);
  gErrorIgnoreLevel = 5000; // suppress messages generated for each page printed

  fH2 = new TH2D("h2", "h2", 1, -600, 300, 1, -450, 450);
  fGHits = new TGraph();
  fGHits->SetMarkerStyle(20);
  fGHits->SetMarkerSize(1);

  fCrown = new TCrown();
  fCrown->SetLineWidth(1);
  fCrown->SetFillStyle(1001);

  fArc = new TArc();
  fArc->SetFillStyle(0);
  fArc->SetLineWidth(2);
  
  fAcceptance = new TEllipse(-150,0,350,280);
  fAcceptance->SetFillColor(19);
  fAcceptance->SetLineColor(18);
  fAcceptance->SetLineWidth(2);

  fText = new TLatex();
  fText->SetTextSize(0.022);
  fText->SetTextFont(102);
  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);

  fLine = new TLine();
  fLine->SetLineWidth(2);

  fCanvas = new TCanvas("c", "c", 800, 800);
  fCanvas->SetLeftMargin(0.07);
  fCanvas->SetRightMargin(0.03);
  fCanvas->SetTopMargin(0.05);
  fCanvas->SetBottomMargin(0.05);
}

void SpecRICHEventMonitor::InitHist() {
  BookHisto("hHits", new TH2F
	    ("RICH hit positions", "RICH hit positions", 900, -600, 300, 900, -450, 450));
  BookHisto("hNHitsPerRing", new TH1D
	    ("RICH Nhits/Ring","RICH Nhits/Ring", 30, -0.5, 29.5));
  BookHisto("hNHitsPerGoodRing", new TH1D
	    ("RICH Ngoodhits/Ring","RICH Ngoodhits/Ring", 30, -0.5, 29.5));

  BookHisto("hMinDistanceTrackRing", new TH1D
	    ("RICH ring centre distance from expected position",
	     "RICH ring centre distance from expected position", 60, 0, 30));
  BookHisto("hMinXDistanceTrackRing", new TH1D
	    ("MinXDistanceTrackRing", "MinXDistanceTrackRing", 50, -50, 50));
  BookHisto("hMinYDistanceTrackRing", new TH1D
	    ("MinYDistanceTrackRing", "MinYDistanceTrackRing", 50, -50, 50));

  BookHisto("hRingChi2", new TH1D ("RingChi2", "RingChi2", 50, 0, 2));
  
  BookHisto("hRingRadiusWrtEl", new TH1D
	    ("RingRadiusWrtEl", "RingRadiusWrtEl", 100, -50, 50));
  BookHisto("hRingRadiusWrtMu", new TH1D
	    ("RingRadiusWrtMu", "RingRadiusWrtMu", 100, -50, 50));
  BookHisto("hRingRadiusWrtPi", new TH1D
	    ("RingRadiusWrtPi", "RingRadiusWrtPi", 100, -50, 50));

  BookHisto("hRingRadiusWrtElvsMom", new TH2D
	    ("RingRadiusWrtElvsMom", "RingRadiusWrtElvsMom", 40, 0, 80, 50, -50, 50));
  BookHisto("hRingRadiusWrtMuvsMom", new TH2D
	    ("RingRadiusWrtMuvsMom", "RingRadiusWrtMuvsMom", 40, 0, 80, 50, -50, 50));
  BookHisto("hRingRadiusWrtPivsMom", new TH2D
	    ("RingRadiusWrtPivsMom", "RingRadiusWrtPivsMom", 40, 0, 80, 50, -50, 50));

  BookHisto("hDistanceTrackRingvsRadiusWrtEl", new TH2D
	    ("hDistanceTrackRingvsRadiusWrtEl", "hDistanceTrackRingvsRadiusWrtEl",
	     40, 0, 20, 80, -40, 40));
  BookHisto("hDistanceTrackRingvsRadiusWrtMu", new TH2D
	    ("hDistanceTrackRingvsRadiusWrtMu", "hDistanceTrackRingvsRadiusWrtMu",
	     40, 0, 20, 80, -40, 40));
  BookHisto("hDistanceTrackRingvsRadiusWrtPi", new TH2D
	    ("hDistanceTrackRingvsRadiusWrtPi", "hDistanceTrackRingvsRadiusWrtPi",
	     40, 0, 20, 80, -40, 40));

  BookHisto("hMomentum_IsElectron1", new TH1D
	    ("hMomentum_IsElectron1", "hMomentum_IsElectron1", 80, 0, 80));
  BookHisto("hMomentum_NotElectron1", new TH1D
	    ("hMomentum_NotElectron1", "hMomentum_NotElectron1", 80, 0, 80));
  BookHisto("hMomentum_IsMuon1", new TH1D
	    ("hMomentum_IsMuon1", "hMomentum_IsMuon1", 80, 0, 80));
  BookHisto("hMomentum_NotMuon1", new TH1D
	    ("hMomentum_NotMuon1", "hMomentum_NotMuon1", 80, 0, 80));
  BookHisto("hMomentum_IsPion1", new TH1D
	    ("hMomentum_IsPion1", "hMomentum_IsPion1", 80, 0, 80));
  BookHisto("hMomentum_NotPion1", new TH1D
	    ("hMomentum_NotPion1", "hMomentum_NotPion1", 80, 0, 80));

  BookHisto("hMomentum_IsElectron2", new TH1D
	    ("hMomentum_IsElectron2", "hMomentum_IsElectron2", 80, 0, 80));
  BookHisto("hMomentum_NotElectron2", new TH1D
	    ("hMomentum_NotElectron2", "hMomentum_NotElectron2", 80, 0, 80));
  BookHisto("hMomentum_IsMuon2", new TH1D
	    ("hMomentum_IsMuon2", "hMomentum_IsMuon2", 80, 0, 80));
  BookHisto("hMomentum_NotMuon2", new TH1D
	    ("hMomentum_NotMuon2", "hMomentum_NotMuon2", 80, 0, 80));
  BookHisto("hMomentum_IsPion2", new TH1D
	    ("hMomentum_IsPion2", "hMomentum_IsPion2", 80, 0, 80));
  BookHisto("hMomentum_NotPion2", new TH1D
	    ("hMomentum_NotPion2", "hMomentum_NotPion2", 80, 0, 80));
}

void SpecRICHEventMonitor::Process(Int_t iEvent) {
  
  TRecoRICHEvent* RichEvent = (TRecoRICHEvent*)GetEvent("RICH");
  Int_t NRings    = RichEvent->GetNRingCandidates();
  Int_t nRichHits = RichEvent->GetNHits();
  Int_t nGoodRichHits = 0;
  for (Int_t i=0; i<nRichHits; i++) {
    TRecoRICHHit* hit = (TRecoRICHHit*)RichEvent->GetHit(i);
    if (hit->GetROChannelID()>=0 && hit->GetROChannelID()<2048) { // discard SuperCells
      nGoodRichHits++;
      FillHisto("hHits", hit->GetFitPosition().X(), hit->GetFitPosition().Y());
    }
  }
  for (Int_t iring=0; iring<NRings; iring++) {
    TRecoRICHCandidate* rc = (TRecoRICHCandidate*)RichEvent->GetRingCandidate(iring);
    FillHisto("hNHitsPerRing", rc->GetNHits());
  }
  if (!nGoodRichHits) return;

  TRecoSpectrometerEvent* SpecEvent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");
  Int_t NTracks = SpecEvent->GetNCandidates();
  if (NTracks<fNTrackCutLow || NTracks>fNTrackCutHigh) return;

  fGHits->Set(nGoodRichHits);
  Int_t igoodhit = 0;
  for (Int_t i=0; i<RichEvent->GetNHits(); i++) {
    TRecoRICHHit* hit = (TRecoRICHHit*)RichEvent->GetHit(i);
    if (hit->GetROChannelID()>=0 && hit->GetROChannelID()<2048) { // discard SuperCells
      fGHits->SetPoint(igoodhit, hit->GetFitPosition().X(), hit->GetFitPosition().Y());
      igoodhit++;
    }
  }

  fH2->SetTitle(Form("Plot %d: event %d", fGraphCount, iEvent));
  fH2->Draw();
  fAcceptance->Draw("same");

  OutputState state;
  std::vector<SpecRICHAssociation> SpecRICH =
    *(std::vector<SpecRICHAssociation>*)GetOutput("SpecRICHParticleID.Output", state);

  /////////////////////////////////////
  // Draw expected rings for each track

  for (UInt_t itrack=0; itrack<SpecRICH.size(); itrack++) {
    SpecRICHAssociation sr = SpecRICH[itrack];
    Double_t xc            = sr.GetExpectedXRingCentre();
    Double_t yc            = sr.GetExpectedYRingCentre();
    Double_t RadiusMargin  = sr.GetRadiusMargin();
    Int_t    RingID        = sr.GetRingID();
    Double_t Chi2          = sr.GetRingChi2();

    FillHisto("hMinDistanceTrackRing",  sr.GetMinDistanceTrackRing());
    FillHisto("hMinXDistanceTrackRing", sr.GetMinXDistanceTrackRing());    
    FillHisto("hMinYDistanceTrackRing", sr.GetMinYDistanceTrackRing());

    if (RingID>=0) {
      FillHisto("hRingChi2", Chi2);
    }

    if (sr.GetRingRadiusWrtEl()<999) {
      FillHisto("hRingRadiusWrtEl", sr.GetRingRadiusWrtEl());
      FillHisto("hRingRadiusWrtElvsMom", 1e-3*sr.GetTrackMomentum(), sr.GetRingRadiusWrtEl());
      FillHisto("hDistanceTrackRingvsRadiusWrtEl", sr.GetMinDistanceTrackRing(), sr.GetRingRadiusWrtEl());
    }
    if (sr.GetRingRadiusWrtMu()<999) {
      FillHisto("hRingRadiusWrtMu", sr.GetRingRadiusWrtMu());
      FillHisto("hRingRadiusWrtMuvsMom", 1e-3*sr.GetTrackMomentum(), sr.GetRingRadiusWrtMu());
      FillHisto("hDistanceTrackRingvsRadiusWrtMu", sr.GetMinDistanceTrackRing(), sr.GetRingRadiusWrtMu());
    }
    if (sr.GetRingRadiusWrtPi()<999) {
      FillHisto("hRingRadiusWrtPi", sr.GetRingRadiusWrtPi());
      FillHisto("hRingRadiusWrtPivsMom", 1e-3*sr.GetTrackMomentum(), sr.GetRingRadiusWrtPi());
      FillHisto("hDistanceTrackRingvsRadiusWrtPi", sr.GetMinDistanceTrackRing(), sr.GetRingRadiusWrtPi());
    }

    if (sr.GetRingRadiusEl()>0.0) {
      fCrown->SetLineColor(kGreen+3);
      fCrown->SetFillColor(kGreen-7);
      fCrown->DrawCrown(xc, yc, sr.GetRingRadiusEl()-RadiusMargin, sr.GetRingRadiusEl()+RadiusMargin);
    }
    if (sr.GetRingRadiusMu()>0.0) {
      fCrown->SetLineColor(kGreen+2);
      fCrown->SetFillColor(kGreen-9);
      fCrown->DrawCrown(xc, yc, sr.GetRingRadiusMu()-RadiusMargin, sr.GetRingRadiusMu()+RadiusMargin);
    }
    if (sr.GetRingRadiusPi()>0.0) {
      fCrown->SetLineColor(kGreen);
      fCrown->SetFillColor(kGreen-10);
      fCrown->DrawCrown(xc, yc, sr.GetRingRadiusPi()-RadiusMargin, sr.GetRingRadiusPi()+RadiusMargin);
    }

    if (sr.GetTrackMomentum()>40000.0 && sr.GetRingAcceptanceMu()==1.0) {
      FillHisto("hNHitsPerGoodRing", sr.GetNObservedHitsMu());
    }
    
    fText->SetTextAlign(kHAlignCenter+kVAlignCenter);
    fText->SetTextColor(kGreen+1);
    fText->DrawLatex(xc, yc, Form("%d", itrack));

    //////////////////////////////
    // Print the track information

    TString  IsElectron1 = sr.GetElectron1() ? "Y" : "N";
    TString  IsMuon1     = sr.GetMuon1()     ? "Y" : "N";
    TString  IsPion1     = sr.GetPion1()     ? "Y" : "N";
    TString  IsElectron2 = sr.GetElectron2() ? "Y" : "N";
    TString  IsMuon2     = sr.GetMuon2()     ? "Y" : "N";
    TString  IsPion2     = sr.GetPion2()     ? "Y" : "N";

    TString Report =
      Form("TR%d: P=%4.1f R_{e,#mu,#pi}=%3d/%3d/%3d N_{exp}=%4.1f/%4.1f/%4.1f N_{obs}=%2d/%2d/%2d ID=%s%s%s %s%s%s",
	   itrack, 1e-3*sr.GetTrackMomentum(),
	   (Int_t)sr.GetRingRadiusEl(), (Int_t)sr.GetRingRadiusMu(), (Int_t)sr.GetRingRadiusPi(),
	   sr.GetNExpectedHitsEl(), sr.GetNExpectedHitsMu(), sr.GetNExpectedHitsPi(),
	   sr.GetNObservedHitsEl(), sr.GetNObservedHitsMu(), sr.GetNObservedHitsPi(),
	   IsElectron1.Data(), IsMuon1.Data(), IsPion1.Data(),
	   IsElectron2.Data(), IsMuon2.Data(), IsPion2.Data());

    fText->SetTextColor(kBlack);
    fText->SetTextSize(0.020);
    fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
    fText->DrawLatex(-590, 425-30*itrack, Report.Data());
    fText->SetTextSize(0.022);

    if (sr.GetElectron1()) FillHisto("hMomentum_IsElectron1", 1e-3*sr.GetTrackMomentum());
    else FillHisto("hMomentum_NotElectron1", 1e-3*sr.GetTrackMomentum());
    if (sr.GetMuon1()) FillHisto("hMomentum_IsMuon1", 1e-3*sr.GetTrackMomentum());
    else FillHisto("hMomentum_NotMuon1", 1e-3*sr.GetTrackMomentum());
    if (sr.GetPion1()) FillHisto("hMomentum_IsPion1", 1e-3*sr.GetTrackMomentum());
    else FillHisto("hMomentum_NotPion1", 1e-3*sr.GetTrackMomentum());

    if (sr.GetElectron2()) FillHisto("hMomentum_IsElectron2", 1e-3*sr.GetTrackMomentum());
    else FillHisto("hMomentum_NotElectron2", 1e-3*sr.GetTrackMomentum());
    if (sr.GetMuon2()) FillHisto("hMomentum_IsMuon2", 1e-3*sr.GetTrackMomentum());
    else FillHisto("hMomentum_NotMuon2", 1e-3*sr.GetTrackMomentum());
    if (sr.GetPion2()) FillHisto("hMomentum_IsPion2", 1e-3*sr.GetTrackMomentum());
    else FillHisto("hMomentum_NotPion2", 1e-3*sr.GetTrackMomentum());
  }

  ///////////////////////////////
  // Draw the reconstructed rings
  
  for (Int_t iring=0; iring<NRings; iring++) {

    TRecoRICHCandidate* rc = (TRecoRICHCandidate*)RichEvent->GetRingCandidate(iring);
    Double_t xc = rc->GetRingCenter().X();
    Double_t yc = rc->GetRingCenter().Y();
    Double_t r  = rc->GetRingRadius();
    Double_t Acc=0;
    for (Int_t j=0; j<100; j++) {
      Double_t Phi = 0.02*j*TMath::Pi();
      Double_t x = xc + r*cos(Phi);
      Double_t y = yc + r*sin(Phi);
      if ((x+150)*(x+150)/350.0/350.0 + y*y/280.0/280.0 < 1.0) Acc++;
    }
    Acc *= 0.01;

    fArc->SetLineColor(kBlue);
    fArc->DrawArc(xc, yc, r);

    fText->SetTextColor(kBlue);
    fText->SetTextAlign(kHAlignCenter+kVAlignCenter);
    fText->DrawLatex(xc, yc, Form("%d", iring));

    TString Report = Form("RING %d: R=%3d, N_{hits}=%2d, A=%4.2f",
			  iring, (Int_t)rc->GetRingRadius(), rc->GetNHits(), Acc);
    fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
    fText->DrawLatex(-80, -440-30*(iring-NRings), Report.Data());
  }

  /////////////////////////////////////
  // Draw rings expected from MC tracks

  if (GetWithMC()) {
    Double_t FocalLength = 17020;
    Double_t RefIndex    = 1.000062;
    
    Event *truth = GetMCEvent();
    Int_t Npart = truth->GetNKineParts();
    TClonesArray KinePartsArray = *(truth->GetKineParts());
    KinePart *Particle;
    for (Int_t ipart=0; ipart<Npart; ipart++) {
      Particle = (KinePart*)KinePartsArray[ipart];
      TString partname = Particle->GetParticleName();
      if (!(partname.Contains("+") || partname.Contains("-"))) continue;
      if (!(partname.Contains("e") || partname.Contains("mu") || partname.Contains("pi"))) continue;
      if (Particle->GetProdPos().Z()>220000) continue;
      if (Particle->GetEndPos().Z() <238000) continue;
      TVector3 Mom3 = Particle->GetMomAtCheckPoint(5).Vect(); // RICH mirror entry
      if (Mom3.Z()<1) continue; // RICH mirror acceptance check

      Double_t xc     = FocalLength*Mom3.X()/Mom3.Z();
      Double_t yc     = FocalLength*Mom3.Y()/Mom3.Z();
      Double_t Beta   = Particle->GetMomAtCheckPoint(5).Beta();
      if (Beta*RefIndex < 1) continue; // below Cherenkov threshold
      Double_t Theta  = acos(1.0 / RefIndex / Beta);
      Double_t Radius = FocalLength * Theta;

      fArc->SetLineColor(kRed);
      fArc->DrawArc(xc, yc, Radius);

      TString Label = "e/";
      if (partname.Contains("mu")) Label = "#mu/";
      if (partname.Contains("pi")) Label = "#pi/";
      Label += Form("%3.1fGeV", Mom3.Mag()*1e-3);
      fText->SetTextAlign(kHAlignCenter+kVAlignTop);
      fText->SetTextColor(kRed);
      fText->DrawLatex(xc, yc-1.05*Radius, Label.Data());
    }
  }
  
  // Mark RICH hit positions
  fGHits->Draw("p same");

  ////////////////
  // Draw a legend

  TBox *b = new TBox();
  b->SetLineColor(kGreen+3); b->SetFillColor(kGreen-7);  b->DrawBox(-570, -319, -530, -301);
  b->SetLineColor(kGreen+2); b->SetFillColor(kGreen-9);  b->DrawBox(-570, -344, -530, -326);
  b->SetLineColor(kGreen+0); b->SetFillColor(kGreen-10); b->DrawBox(-570, -369, -530, -351);
  b->SetFillStyle(0);
  b->SetLineWidth(1);
  b->SetLineColor(kGreen+3); b->SetFillColor(kGreen-7);  b->DrawBox(-570, -319, -530, -301);
  b->SetLineColor(kGreen+2); b->SetFillColor(kGreen-9);  b->DrawBox(-570, -344, -530, -326);
  b->SetLineColor(kGreen+0); b->SetFillColor(kGreen-10); b->DrawBox(-570, -369, -530, -351);

  fLine->SetLineColor(kBlue); fLine->DrawLine(-570, -385, -530, -385);
  fLine->SetLineColor(kRed);  fLine->DrawLine(-570, -410, -530, -410);

  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
  fText->SetTextColor(kBlack);
  fText->DrawLatex(-520, -310, "STRAW expected electron");
  fText->DrawLatex(-520, -335, "STRAW expected muon");
  fText->DrawLatex(-520, -360, "STRAW expected pion");
  fText->DrawLatex(-520, -385, "RICH reconstructed ring");
  if (GetWithMC()) fText->DrawLatex(-520, -410, "MC expected ring");

  if (!fGraphCount) fCanvas->SaveAs(fOutputPDFName+"[");
  if (fGraphCount<fMaxGraphCount) fCanvas->SaveAs(fOutputPDFName);
  /*
  if (!SpecRICH[0].GetMuon2() &&
      SpecRICH[0].GetTrackMomentum()>15000 &&
      SpecRICH[0].GetTrackMomentum()<35000) fCanvas->SaveAs(fOutputPDFName);
  */
  fGraphCount++;
}

void SpecRICHEventMonitor::EndOfRunUser() {

  // Plot the positions of all RICH hits
  fCanvas->Clear();
  fHisto.GetTH2("hHits")->Draw();
  fAcceptance->Draw("same");
  fHisto.GetTH2("hHits")->Draw("same");
  fCanvas->SaveAs(fOutputPDFName);

  // Plot the track-ring distance
  fCanvas->Clear();
  fHisto.GetTH1("hMinDistanceTrackRing")->SetLineColor(kBlue);
  fHisto.GetTH1("hMinDistanceTrackRing")->SetFillColor(kYellow);
  fHisto.GetTH1("hMinDistanceTrackRing")->Draw();
  fCanvas->SaveAs(fOutputPDFName);

  // Plot ring radii minus expected radii in the 3 PID hypotheses
  fCanvas->Clear();
  fHisto.GetTH1("hRingRadiusWrtEl")->SetLineColor(kRed);
  fHisto.GetTH1("hRingRadiusWrtMu")->SetLineColor(kBlue);
  fHisto.GetTH1("hRingRadiusWrtPi")->SetLineColor(kGreen+2);
  fHisto.GetTH1("hRingRadiusWrtEl")->SetLineWidth(2);
  fHisto.GetTH1("hRingRadiusWrtMu")->SetLineWidth(2);
  fHisto.GetTH1("hRingRadiusWrtPi")->SetLineWidth(2);
  Double_t YmaxEl = fHisto.GetTH1("hRingRadiusWrtEl")->GetMaximum();
  Double_t YmaxMu = fHisto.GetTH1("hRingRadiusWrtMu")->GetMaximum();
  Double_t YmaxPi = fHisto.GetTH1("hRingRadiusWrtPi")->GetMaximum();
  Double_t Ymax = YmaxEl;
  if (YmaxMu>Ymax) Ymax = YmaxMu;
  if (YmaxPi>Ymax) Ymax = YmaxPi;
  fHisto.GetTH1("hRingRadiusWrtEl")->SetMaximum(1.05*Ymax);
  fHisto.GetTH1("hRingRadiusWrtEl")->SetTitle("Ring radius #font[122]{-} expected radius");
  fHisto.GetTH1("hRingRadiusWrtEl")->Draw();
  fHisto.GetTH1("hRingRadiusWrtMu")->Draw("same");
  fHisto.GetTH1("hRingRadiusWrtPi")->Draw("same");
  TLegend *leg = new TLegend(0.70, 0.80, 0.95, 0.93);
  leg->SetFillColor(0);
  leg->AddEntry(fHisto.GetTH1("hRingRadiusWrtEl"), "e hypothesis", "l");
  leg->AddEntry(fHisto.GetTH1("hRingRadiusWrtMu"), "#mu hypothesis", "l");
  leg->AddEntry(fHisto.GetTH1("hRingRadiusWrtPi"), "#pi hypothesis", "l");
  leg->Draw();
  fCanvas->SaveAs(fOutputPDFName);
  
  // Same, versus momentum
  fCanvas->Clear();
  fHisto.GetTH2("hRingRadiusWrtElvsMom")->SetLineColor(kRed);
  fHisto.GetTH2("hRingRadiusWrtMuvsMom")->SetLineColor(kBlue);
  fHisto.GetTH2("hRingRadiusWrtPivsMom")->SetLineColor(kGreen+2);
  fHisto.GetTH2("hRingRadiusWrtElvsMom")->SetTitle("Ring radius #font[122]{-} expected radius vs momentum");
  fHisto.GetTH2("hRingRadiusWrtElvsMom")->Draw("box");
  fHisto.GetTH2("hRingRadiusWrtMuvsMom")->Draw("box same");
  fHisto.GetTH2("hRingRadiusWrtPivsMom")->Draw("box same");
  leg->Draw();
  fCanvas->SaveAs(fOutputPDFName);

  // Plot momentum spectra of identified and non-identified tracks

  // a) electrons, method 1
  fHisto.GetTH1("hMomentum_IsElectron1") ->SetLineColor(kBlue);
  fHisto.GetTH1("hMomentum_NotElectron1")->SetLineColor(kRed);
  fHisto.GetTH1("hMomentum_IsElectron1") ->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_NotElectron1")->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_IsElectron1") ->Draw();
  fHisto.GetTH1("hMomentum_NotElectron1")->Draw("same");
  Double_t IDeff =
    fHisto.GetTH1("hMomentum_IsElectron1")->Integral(15,34)/
    (fHisto.GetTH1("hMomentum_IsElectron1")->Integral(15,34)+
     fHisto.GetTH1("hMomentum_NotElectron1")->Integral(15,34));
  TString Report = Form("Electron ID probability: %4.2f", IDeff);
  fText->SetTextColor(kBlue);
  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
  fText->DrawLatex(2, fHisto.GetTH1("hMomentum_IsElectron1")->GetMaximum(), Report.Data());
  fCanvas->SaveAs(fOutputPDFName);

  // b) electrons, method 2
  fHisto.GetTH1("hMomentum_IsElectron2") ->SetLineColor(kBlue);
  fHisto.GetTH1("hMomentum_NotElectron2")->SetLineColor(kRed);
  fHisto.GetTH1("hMomentum_IsElectron2") ->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_NotElectron2")->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_IsElectron2") ->Draw();
  fHisto.GetTH1("hMomentum_NotElectron2")->Draw("same");
  IDeff =
    fHisto.GetTH1("hMomentum_IsElectron2")->Integral(15,34)/
    (fHisto.GetTH1("hMomentum_IsElectron2")->Integral(15,34)+
     fHisto.GetTH1("hMomentum_NotElectron2")->Integral(15,34));
  Report = Form("Electron ID probability: %4.2f", IDeff);
  fText->SetTextColor(kBlue);
  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
  fText->DrawLatex(2, fHisto.GetTH1("hMomentum_IsElectron2")->GetMaximum(), Report.Data());
  fCanvas->SaveAs(fOutputPDFName);

  // c) muons, method 1
  fHisto.GetTH1("hMomentum_IsMuon1") ->SetLineColor(kBlue);
  fHisto.GetTH1("hMomentum_NotMuon1")->SetLineColor(kRed);
  fHisto.GetTH1("hMomentum_IsMuon1") ->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_NotMuon1")->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_IsMuon1") ->Draw();
  fHisto.GetTH1("hMomentum_NotMuon1")->Draw("same");
  IDeff =
    fHisto.GetTH1("hMomentum_IsMuon1")->Integral(15,34)/
    (fHisto.GetTH1("hMomentum_IsMuon1")->Integral(15,34)+
     fHisto.GetTH1("hMomentum_NotMuon1")->Integral(15,34));
  Report = Form("Muon ID probability: %4.2f", IDeff);
  fText->SetTextColor(kBlue);
  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
  fText->DrawLatex(2, fHisto.GetTH1("hMomentum_IsMuon1")->GetMaximum(), Report.Data());
  fCanvas->SaveAs(fOutputPDFName);

  // d) muons, method 2
  fHisto.GetTH1("hMomentum_IsMuon2") ->SetLineColor(kBlue);
  fHisto.GetTH1("hMomentum_NotMuon2")->SetLineColor(kRed);
  fHisto.GetTH1("hMomentum_IsMuon2") ->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_NotMuon2")->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_IsMuon2") ->Draw();
  fHisto.GetTH1("hMomentum_NotMuon2")->Draw("same");
  IDeff =
    fHisto.GetTH1("hMomentum_IsMuon2")->Integral(15,34)/
    (fHisto.GetTH1("hMomentum_IsMuon2")->Integral(15,34)+
     fHisto.GetTH1("hMomentum_NotMuon2")->Integral(15,34));
  Report = Form("Muon ID probability: %4.2f", IDeff);
  fText->SetTextColor(kBlue);
  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
  fText->DrawLatex(2, fHisto.GetTH1("hMomentum_IsMuon2")->GetMaximum(), Report.Data());
  fCanvas->SaveAs(fOutputPDFName);

  // e) pions, method 1
  fHisto.GetTH1("hMomentum_IsPion1") ->SetLineColor(kBlue);
  fHisto.GetTH1("hMomentum_NotPion1")->SetLineColor(kRed);
  fHisto.GetTH1("hMomentum_IsPion1") ->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_NotPion1")->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_IsPion1") ->Draw();
  fHisto.GetTH1("hMomentum_NotPion1")->Draw("same");
  IDeff =
    fHisto.GetTH1("hMomentum_IsPion1")->Integral(15,34)/
    (fHisto.GetTH1("hMomentum_IsPion1")->Integral(15,34)+
     fHisto.GetTH1("hMomentum_NotPion1")->Integral(15,34));
  Report = Form("Pion ID probability: %4.2f", IDeff);
  fText->SetTextColor(kBlue);
  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
  fText->DrawLatex(2, fHisto.GetTH1("hMomentum_IsPion1")->GetMaximum(), Report.Data());
  fCanvas->SaveAs(fOutputPDFName);

  // f) pions, method 2
  fHisto.GetTH1("hMomentum_IsPion2") ->SetLineColor(kBlue);
  fHisto.GetTH1("hMomentum_NotPion2")->SetLineColor(kRed);
  fHisto.GetTH1("hMomentum_IsPion2") ->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_NotPion2")->SetLineWidth(2);
  fHisto.GetTH1("hMomentum_IsPion2") ->Draw();
  fHisto.GetTH1("hMomentum_NotPion2")->Draw("same");
  IDeff =
    fHisto.GetTH1("hMomentum_IsPion2")->Integral(15,34)/
    (fHisto.GetTH1("hMomentum_IsPion2")->Integral(15,34)+
     fHisto.GetTH1("hMomentum_NotPion2")->Integral(15,34));
  Report = Form("Pion ID probability: %4.2f", IDeff);
  fText->SetTextColor(kBlue);
  fText->SetTextAlign(kHAlignLeft+kVAlignCenter);
  fText->DrawLatex(2, fHisto.GetTH1("hMomentum_IsPion2")->GetMaximum(), Report.Data());
  fCanvas->SaveAs(fOutputPDFName);
  // fCanvas->SaveAs("eff.gif"); // save a standalone plot

  fCanvas->SaveAs(fOutputPDFName+"]");
  SaveAllPlots();
}
