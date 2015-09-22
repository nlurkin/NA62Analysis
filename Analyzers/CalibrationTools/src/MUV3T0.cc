#include "MUV3T0.hh"

/// \class MUV3T0
/// \Brief
/// Evaluation of the T0 constants for MUV3
/// \EndBrief

MUV3T0::MUV3T0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "MUV3") {

  // Optional parameters
  fMinIntegral         = 100;  // minimal number of entries (excluding underflows, overflows) for fit attempt
  fFittingRange        = 0.9;  // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width 
  fNFilesToAccumulate  = 20;   // for the T0 stability plots
  fHistoTimeLimit      = 25.0; // time half-span of plotted histograms [ns]
  fSignalPeakWidth     = 3.0;  // exclusion region half-width when looking for anomalous shape [ns]
  fInitialResol        = 0.6;  // initial value of the time resolution parameter for the fit [ns]
  fIssueWarnings       = true; // check if the spectrum shape is OK?
  fPlotChannelTimes    = true; // plot times in each channel?
  fPlotTimeDependences = false; // check and plot the time stability of the T0 constants?
}

void MUV3T0::RequestUserHistograms() {

  // Correlations of (T1,T2) in channels
  fAllTileT1T2Found = true;
  for (int iTile=0; iTile<152; iTile++) {
    TString Name = Form("Tile %03d T1,T2", iTile);
    fHTileT1T2[iTile] = (TH1D*)RequestHistogram("MUV3Monitor/MUV3Tiles/Time2D", Name, true);
    if (!fHTileT1T2[iTile]) fAllTileT1T2Found = false;
  }
}

void MUV3T0::GenerateUserPDFReport() {

  /////////////////////////////////////////////////////////////
  // Correlations of (T1,T2) with respect to reference in tiles

  if (!fAllTileT1T2Found) {
    cout << fAnalyzerName << ": WARNING: Tile T1,T2 histograms not found" << endl;
    cout << fAnalyzerName << ": WARNING: Check that 'EnableChannelHistograms=1' in MUV3.conf in NA62Reco" << endl;
    return;
  }

  int Npages = 10;
  TCanvas *Canvas = new TCanvas("Canvas0");
  Canvas->Divide(4,4);
  for (int ipage=0; ipage<Npages; ipage++) {
    for (int i=0; i<16; i++) {
      Canvas->GetPad(i+1)->Clear();
      Canvas->cd(i+1);
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
      bool not_instrumented = (fUseChannelMap && (roch1<0 || roch2<0));

      fText->SetTextAlign(kHAlignCenter+kVAlignCenter);
      fText->SetTextSize(0.15);
      fText->SetTextColor(kGreen+2);
      if (not_existing) fText->DrawText(0, 0, "DOES NOT EXIST");
      else if (not_instrumented) fText->DrawText(0, 0, "N/A");
      if (not_existing || not_instrumented) continue;

      TString XAxisName = Form("Geom %03d, RO %03d", iTile,     roch1);
      TString YAxisName = Form("Geom %03d, RO %03d", iTile+200, roch2);
      fText->SetTextAlign(kHAlignCenter+kVAlignCenter);
      fText->SetTextSize(0.07);
      fText->SetTextColor(kBlack);
      fText->DrawText(10, -17, XAxisName);
      fText->SetTextAngle(90);
      fText->DrawText(-18, 5, YAxisName);
      fText->SetTextAngle(0);
    }
    Canvas->Print(fOutPDFFileName, "pdf");
  }
}
