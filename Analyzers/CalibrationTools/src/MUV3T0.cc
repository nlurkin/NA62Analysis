#include "MUV3T0.hh"
#include "TEllipse.h"

MUV3T0::MUV3T0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "MUV3") {

  // Optional parameters
  fMinIntegral         = 100;  // minimal number of entries (excluding underflows, overflows) for fit attempt
  fFittingRange        = 0.9;  // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width 
  fNFilesToAccumulate  = 20;   // for the T0 stability plots
  fHistoTimeLimit      = 25.0; // time half-span of plotted histograms [ns]
  fSignalPeakWidth     = 3.0;  // exclusion region half-width when looking for anomalous shape [ns]
  fIssueWarnings       = true; // check if the spectrum shape is OK?
  fPlotTimeDependences = true; // check the time stability of the T0 constants?
}

void MUV3T0::RequestUserHistograms() {

  // Correlations of (T1,T2) in channels
  fTileT1T2Found = true;
  for (int iTile=0; iTile<152; iTile++) {
    TString Name = Form("Tile %03d T1,T2", iTile);
    fHTileT1T2[iTile] = (TH1D*)RequestHistogram("MUV3Monitor/MUV3Tiles/Time2D", Name, true);
    if (!fHTileT1T2[iTile]) fTileT1T2Found = false;
  }

  // Channel profile and number of OR, AND in tiles
  fHChannelProfile = (TH1D*)RequestHistogram("MUV3Monitor", "ChannelProfile", true);
  fHTileOR         = (TH1D*)RequestHistogram("MUV3Monitor", "TileOR",         true);
  fHTileAND        = (TH1D*)RequestHistogram("MUV3Monitor", "TileAND",        true);
}

void MUV3T0::GenerateUserPDFReport() {
  if (!fTileT1T2Found) {
    cout << fAnalyzerName << ": Tile T1,T2 histograms not found" << endl;
    return;
  }

  gStyle->SetOptStat(0);
  TText *Text = new TText();
  Text->SetTextAlign(kHAlignCenter+kVAlignCenter);
  Text->SetTextSize(0.07);
  Text->SetTextColor(kBlack);

  /////////////////////////////////////////////////////////////
  // Correlations of (T1,T2) with respect to reference in tiles

  int Npages = 10;
  for (int ipage=0; ipage<Npages; ipage++) {
    for (int i=0; i<16; i++) {
      fCanvas->GetPad(i+1)->Clear();
      fCanvas->cd(i+1);
      int iTile = ipage*16 + i;
      if (iTile>=152) continue;

      // Find out the RO channel IDs for this tile
      int roch1 = -999, roch2 = -999;
      for (int ich=0; ich<fNChannels; ich++) {
	if (fChannelID[ich]==iTile)     roch1 = ich;
	if (fChannelID[ich]==iTile+200) roch2 = ich;
      }
      TString HistoName = Form("Tile %03d times [ns]", iTile);
      fHTileT1T2[iTile]->SetTitle(HistoName);
      fHTileT1T2[iTile]->GetXaxis()->SetTitle("");
      fHTileT1T2[iTile]->GetYaxis()->SetTitle("");
      fHTileT1T2[iTile]->GetXaxis()->SetLabelSize(0.07);
      fHTileT1T2[iTile]->GetYaxis()->SetLabelSize(0.07);
      fHTileT1T2[iTile]->Draw("col");

      bool not_existing     = (iTile==65 || iTile==66 || iTile==77 || iTile==78);
      bool not_instrumented = (roch1<0 || roch2<0);

      if (not_existing) fText->DrawText(0, 0, "DOES NOT EXIST");
      else if (not_instrumented) fText->DrawText(0, 0, "N/A");
      
      if (!not_existing && !not_instrumented) {
	TString XAxisName = Form("Geom %03d, RO %03d", iTile,     roch1);
	TString YAxisName = Form("Geom %03d, RO %03d", iTile+200, roch2);
	Text->DrawText(10, -17, XAxisName);
	Text->SetTextAngle(90);
	Text->DrawText(-18, 5, YAxisName);
	Text->SetTextAngle(0);
      }
    }
    fCanvas->Print(fOutPDFFileName, "pdf");
  }

  //////////////////////////////////////////////////
  // Channel profile and numbers of OR, AND in tiles

  if (fHChannelProfile && fHTileOR && fHTileAND) {

    TCanvas *Canvas = new TCanvas("Canvas1");
    Canvas->Divide(1,3);
    for (int i=1; i<=3; i++) {
      Canvas->GetPad(i)->SetLeftMargin(0.05);
      Canvas->GetPad(i)->SetRightMargin(0.01);
      Canvas->GetPad(i)->SetTopMargin(0.06);
      Canvas->GetPad(i)->SetBottomMargin(0.10);
    }

    Canvas->cd(1);
    fHChannelProfile->SetTitle("MUV3 channel profile");
    fHChannelProfile->GetXaxis()->SetTitleSize(0.055);
    fHChannelProfile->GetXaxis()->SetLabelSize(0.055);
    fHChannelProfile->GetYaxis()->SetTitleSize(0.055);
    fHChannelProfile->GetYaxis()->SetLabelSize(0.055);
    fHChannelProfile->SetStats(0);
    fHChannelProfile->SetLineWidth(1);
    fHChannelProfile->SetLineColor(kBlue);
    fHChannelProfile->SetFillColor(kYellow);
    fHChannelProfile->Draw();

    Canvas->cd(2);
    fHTileOR->SetTitle("MUV3: OR, AND of signals in tiles");
    fHTileOR->GetXaxis()->SetTitleSize(0.055);
    fHTileOR->GetXaxis()->SetLabelSize(0.055);
    fHTileOR->GetYaxis()->SetTitleSize(0.055);
    fHTileOR->GetYaxis()->SetLabelSize(0.055);
    fHTileOR->SetStats(0);
    fHTileOR->SetLineWidth(1);
    fHTileOR->SetLineColor(kBlue);
    fHTileAND->SetStats(0);
    fHTileAND->SetLineWidth(1);
    fHTileAND->SetLineColor(kRed);
    fHTileOR->Draw("hist");
    fHTileAND->Draw("hist same");

    TLegend* ANDORLegend = new TLegend(0.20,0.75,0.45,0.88);
    ANDORLegend->SetFillColor(kWhite);
    ANDORLegend->SetTextSize(0.05);
    ANDORLegend->AddEntry(fHTileOR,  "OR", "l");
    ANDORLegend->AddEntry(fHTileAND, "AND", "l");
    ANDORLegend->Draw();

    Canvas->cd(3);
    TH1D *Ratio = new TH1D(*fHTileAND);
    fHTileAND->Sumw2();
    fHTileOR->Sumw2();
    Ratio->Divide(fHTileAND, fHTileOR, 1, 1, "B");

    Ratio->SetTitle("MUV3: AND/OR ratio in tiles");
    Ratio->GetXaxis()->SetTitleSize(0.055);
    Ratio->GetXaxis()->SetLabelSize(0.055);
    Ratio->GetYaxis()->SetTitleSize(0.055);
    Ratio->GetYaxis()->SetLabelSize(0.055);
    Ratio->SetStats(0);
    Ratio->SetLineWidth(1);
    Ratio->SetLineColor(kGreen+2);
    Ratio->SetMarkerColor(kGreen+2);
    Ratio->SetMinimum(0.0);
    Ratio->SetMaximum(1.0);
    Ratio->Draw("e");

    for (int i=1; i<=Ratio->GetNbinsX(); i++) {
      if (Ratio->GetBinContent(i)<0.05) {
	TEllipse *arc = new TEllipse();
	arc->SetLineColor(kRed);
	arc->SetFillStyle(0);
	arc->DrawEllipse(Ratio->GetBinCenter(i), Ratio->GetBinContent(i), 1.5, 0.05, 0, 360, 0);
      }
    }
    Canvas->Print(fOutPDFFileName, "pdf");
  }
}
