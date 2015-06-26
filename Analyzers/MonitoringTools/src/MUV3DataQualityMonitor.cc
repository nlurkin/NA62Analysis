// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-06-25
//
// ---------------------------------------------------------------

#include "MUV3DataQualityMonitor.hh"
#include "TEllipse.h"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

/// \class MUV3DataQualityMonitor
/// \Brief
/// Builds a PDF report using the standard MUV3 monitoring histograms as input
/// \EndBrief

MUV3DataQualityMonitor::MUV3DataQualityMonitor(Core::BaseAnalysis *ba) : Analyzer(ba) {
  fOutPDFFileName = "./MUV3Monitor.pdf";
  fDirName        = "MUV3Monitor";
}

void MUV3DataQualityMonitor::InitHist() {

  if (!GetIsHisto() || GetIsTree()) {
    cout << "ERROR: MUV3DataQualityMonitor must be run in the --histo mode" << endl;
    exit(0);
  }

  // Channel profile and number of OR, AND in tiles
  fHNEventsProcessedPerBurst = (TH1D*)RequestHistogram("/", "NProcessedEventsInFile", true);
  fHChannelProfile = (TH1D*)RequestHistogram(fDirName, "ChannelProfile", true);
  fHTileOR         = (TH1D*)RequestHistogram(fDirName, "TileOR",         true);
  fHTileAND        = (TH1D*)RequestHistogram(fDirName, "TileAND",        true);

  // 2D count-maps
  fHList.resize(18);

  // Offline: RecoHits
  fHList[0] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2D_PM0",             true);
  fHList[1] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2DInner_PM0",        true);
  fHList[2] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2D_PM1",             true);
  fHList[3] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2DInner_PM1",        true);
  if (fHList[2]) fHList[4] = (TH2D*)fHList[2]->Clone("ChannelProfile2D_PM1-PM0");
  if (fHList[3]) fHList[5] = (TH2D*)fHList[3]->Clone("ChannelProfile2DInner_PM1-PM0");

  // Offline: candidates
  fHList[6] = (TH2D*)RequestHistogram(fDirName, "CandidateProfile2D",               true);
  fHList[7] = (TH2D*)RequestHistogram(fDirName, "CandidateProfile2DInner",          true);
  fHList[8] = (TH2D*)RequestHistogram(fDirName, "TightCandidateProfile2D",          true);
  fHList[9] = (TH2D*)RequestHistogram(fDirName, "TightCandidateProfile2DInner",     true);

  // EOB: channel counts
  fHList[10] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2D_PM0 EOB",         true);
  fHList[11] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2DInner_PM0 EOB",    true);
  fHList[12] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2D_PM1 EOB",         true);
  fHList[13] = (TH2D*)RequestHistogram(fDirName, "ChannelProfile2DInner_PM1 EOB",    true);
  if (fHList[12]) fHList[14] = (TH2D*)fHList[12]->Clone("ChannelProfile2D_PM0-PM1");
  if (fHList[13]) fHList[15] = (TH2D*)fHList[13]->Clone("ChannelProfile2DInner_PM0-PM1");

  // EOB: L0 primitives
  fHList[16] = (TH2D*)RequestHistogram(fDirName, "TightPrimitiveProfile2D EOB",      true);
  fHList[17] = (TH2D*)RequestHistogram(fDirName, "TightPrimitiveProfile2DInner EOB", true);
}

void MUV3DataQualityMonitor::EndOfRunUser() {

  // The numbers of bursts and events processed
  Int_t NBurstsProcessed = 0, NEventsProcessed = 0;
  if (fHNEventsProcessedPerBurst) {
    for (Int_t i=1; i<=fHNEventsProcessedPerBurst->GetNbinsX(); i++) {
      Int_t Nevents = fHNEventsProcessedPerBurst->GetBinContent(i);
      if (Nevents>0) {
	NBurstsProcessed++;
	NEventsProcessed += Nevents;
      }
    }
  }
    
  TCanvas *Canvas = new TCanvas("Canvas1");
  Canvas->Divide(1,3);
  gStyle->SetOptStat(0);
  gErrorIgnoreLevel = 5000; // suppress messages generated for each page printed

  TText *Text = new TText();

  //////////////////////////////////////////////////
  // Channel profile and numbers of OR, AND in tiles

  if (!(fHChannelProfile && fHTileOR && fHTileAND)) {
    cout << fAnalyzerName <<
      ": WARNING: either 'fHChannelProfile', 'fHTileOR', or 'fHTileAND' histogram not found" << endl;
  }
  else {
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

    if (fHNEventsProcessedPerBurst) {
      Text->SetTextSize(0.07);
      Text->SetTextColor(kBlack);
      Text->SetTextAlign(kHAlignLeft+kVAlignTop);
      Text->DrawText(10, fHChannelProfile->GetMaximum(), Form("Bursts processed: %d", NBurstsProcessed));
      Text->DrawText(10, 0.88*fHChannelProfile->GetMaximum(), Form("Events processed: %d", NEventsProcessed));
    }

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

    // Mark the non-existing tiles
    TBox *b = new TBox();
    b->SetFillColor(kGreen-7);
    b->SetLineColor(kGreen-7);
    b->DrawBox(64.5,0,66.5,fHTileOR->GetMaximum());
    b->DrawBox(76.5,0,78.5,fHTileOR->GetMaximum());

    TLegend* ANDORLegend = new TLegend(0.20,0.75,0.35,0.88);
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
 
    Text->SetTextSize(0.05);
    Text->SetTextColor(kRed);
    Text->SetTextAlign(kHAlignCenter+kVAlignCenter);
    int Nbad = 0;
    for (int i=1; i<=Ratio->GetNbinsX(); i++) {
      int iTile = Ratio->GetBinCenter(i);
      if (Ratio->GetBinContent(i)<0.05 &&
          iTile!=65 && iTile!=66 && iTile!=77 && iTile!=78) { // these tiles do not exist
        Nbad++;
        TEllipse *arc = new TEllipse();
        arc->SetLineColor(kRed);
        arc->SetFillStyle(0);
        arc->DrawEllipse(iTile, Ratio->GetBinContent(i), 1.5, 0.05, 0, 360, 0);
        Text->DrawText(iTile, 0.1+0.05*(Nbad%2), Form("%d", iTile));
      }
    }
    Canvas->Print(Form(fOutPDFFileName + "("), "pdf"); // open file and print
  }

  /////////////////////////     
  // 2D count-maps, textual

  bool create2Dcountmaps = true;
  for (unsigned int i=0; i<fHList.size(); i++) {
    if (!fHList[i]) {
      cout << fAnalyzerName << ": WARNING: histo number " << i << " not found." << endl;
      cout << fAnalyzerName << ": WARNING: 2D count maps will not be produced." << endl;
      create2Dcountmaps = false;
    }
  }

  if (!create2Dcountmaps) return;
  
  TCanvas *Canvas2 = new TCanvas("Canvas2");
  Canvas2->SetLeftMargin(0.05);
  Canvas2->SetRightMargin(0.01);
  Canvas2->SetTopMargin(0.07);
  Canvas2->SetBottomMargin(0.05);

  fHList [4]->Add(fHList [2], fHList [0], 1, -1);
  fHList [5]->Add(fHList [3], fHList [1], 1, -1);
  fHList[14]->Add(fHList[12], fHList[10], 1, -1);
  fHList[15]->Add(fHList[13], fHList[11], 1, -1);

  vector <TString> titles(9, TString());
  titles [0] = "N(RecoHits), Low PMT";
  titles [1] = "N(RecoHits), High PMT";
  titles [2] = "N(RecoHits), High#minusLow and A=(High#minusLow)/(High+Low)";
  titles [3] = "N(Candidates)";
  titles [4] = "void"; // tight candidates are not plotted separately
  titles [5] = "EOB: N(hits), Low PMT";
  titles [6] = "EOB: N(hits), High PMT";
  titles [7] = "EOB: N(hits), High#minusLow and A=(High#minusLow)/(High+Low)";
  titles [8] = "SL EOB: N(tight L0 muons)";

  TBox *b = new TBox();
  b->SetFillStyle(0);
  b->SetLineColor(kBlack);
  gStyle->SetPaintTextFormat(".0f");

  for (UInt_t i=0; i<fHList.size(); i++) {
    if (i==8 || i==9) continue; // do not plot tight candidates separately
    fHList[i]->SetMarkerSize(0.8);
    fHList[i]->GetXaxis()->SetTitle("");
    fHList[i]->GetYaxis()->SetTitle("");

    if (i%2==0) { // Outer tiles
      Canvas2->Clear();
      fHList[i]->SetTitle(titles[i/2]);
      fHList[i]->GetXaxis()->SetTickLength(0);
      fHList[i]->GetYaxis()->SetTickLength(0);
      fHList[i]->Draw("TEXT20");

      // Label the outer tiles
      for (int it=1; it<=12; it++) {
	double xc = fHList[0]->GetXaxis()->GetBinCenter(it);
	for (int jt=1; jt<=12; jt++) {
	  double yc = fHList[0]->GetYaxis()->GetBinCenter(jt);
	  int TileID = it-1+(jt-1)*12;
	  if (TileID==65 || TileID==66 || TileID==77 || TileID==78) continue;
	  b->SetLineColor(kBlack);
	  b->DrawBox(xc-0.11, yc-0.11, xc+0.11, yc+0.11);
	  Text->SetTextSize(0.02);
	  Text->SetTextColor(kGreen+2);
	  Text->SetTextAlign(kHAlignLeft+kVAlignTop);
	  Text->DrawText(xc-0.11, yc+0.11, Form("%d", TileID));
	}
      }

      if (i==4 || i==14) { // Print out asymmetries
	for (int it=1; it<=12; it++) {
	  double xc = fHList[i]->GetXaxis()->GetBinCenter(it);
	  for (int jt=1; jt<=12; jt++) {
	    double yc = fHList[i]->GetYaxis()->GetBinCenter(jt);
	    double Nhigh = fHList[i-2]->GetBinContent(fHList[i-2]->GetBin(it,jt));
	    double Nlow  = fHList[i-4]->GetBinContent(fHList[i-4]->GetBin(it,jt));
	    if (!(Nhigh+Nlow)) continue;
	    double Asym = (Nhigh-Nlow) / (Nhigh+Nlow);
	    Color_t color = kBlack;
	    if (Asym<-0.5) color = kBlue;
	    if (Asym>+0.5) color = kRed;
	    Text->SetTextSize(0.018);
	    Text->SetTextColor(color);
	    Text->SetTextAlign(kHAlignRight+kVAlignBottom);
	    Text->DrawText(xc+0.10, yc-0.10, Form("A=%5.3f", Asym));
	  }
	}
      }

      if (i==6) { // Label tiles with loose candidates
	for (int it=1; it<=12; it++) {
	  double xc = fHList[6]->GetXaxis()->GetBinCenter(it);
	  for (int jt=1; jt<=12; jt++) {
	    double yc = fHList[6]->GetYaxis()->GetBinCenter(jt);
	    double Nall   = fHList[6]->GetBinContent(fHList[6]->GetBin(it,jt));
	    double Ntight = fHList[8]->GetBinContent(fHList[8]->GetBin(it,jt));
	    if (Nall>0 && Nall>Ntight) { // loose candidates are generated
	      Text->SetTextSize(0.020);
	      Text->SetTextColor(kRed);
	      Text->SetTextAlign(kHAlignRight+kVAlignBottom);
	      Text->DrawText(xc+0.10, yc-0.10, "LOOSE");
	    }
	  }
	}
      }
    }

    else { // Inner tiles
      fHList[i]->Draw("TEXT20SAME");

      // Label the inner tiles
      for (int it=1; it<=3; it++) {
	double xc = fHList[1]->GetXaxis()->GetBinCenter(it);
	for (int jt=1; jt<=3; jt++) {
	  double yc = fHList[1]->GetYaxis()->GetBinCenter(jt);
	  if (it==2 && jt==2) continue;
	  int TileID = 144+(it-1)+(jt-1)*3;
	  if (TileID>=149) TileID--;
	  b->SetLineColor(kBlack);
	  b->DrawBox(xc-0.22/3.0, yc-0.22/3.0, xc+0.22/3.0, yc+0.22/3.0);
	  Text->SetTextSize(0.018);
	  Text->SetTextColor(kGreen+2);
	  Text->SetTextAlign(kHAlignLeft+kVAlignTop);
	  Text->DrawText(xc-0.22/3.0, yc+0.22/3.0, Form("%d", TileID));
	}
      }

      if (i==5 || i==15) { // Print out asymmetries
	for (int it=1; it<=3; it++) {
	  double xc = fHList[5]->GetXaxis()->GetBinCenter(it);
	  for (int jt=1; jt<=3; jt++) {
	    if (it==2 && jt==2) continue;
	    double yc = fHList[5]->GetYaxis()->GetBinCenter(jt);
	    double Nhigh = fHList[i-2]->GetBinContent(fHList[i-2]->GetBin(it,jt));
	    double Nlow  = fHList[i-4]->GetBinContent(fHList[i-4]->GetBin(it,jt));
	    if (!(Nhigh+Nlow)) continue;
	    double Asym = (Nhigh-Nlow) / (Nhigh+Nlow);
	    Color_t color = kBlack;
	    if (Asym<-0.5) color = kBlue;
	    if (Asym>+0.5) color = kRed;
	    Text->SetTextSize(0.015);
	    Text->SetTextColor(color);
	    Text->SetTextAlign(kHAlignRight+kVAlignBottom);
	    Text->DrawText(xc+0.07, yc-0.07, Form("A=%5.3f", Asym));
	  }
	}
      }

      if (i==7) { // Label tiles with loose candidates
	for (int it=1; it<=3; it++) {
	  double xc = fHList[7]->GetXaxis()->GetBinCenter(it);
	  for (int jt=1; jt<=3; jt++) {
	    if (it==2 && jt==2) continue;
	    double yc = fHList[7]->GetYaxis()->GetBinCenter(jt);
	    double Nall   = fHList[7]->GetBinContent(fHList[7]->GetBin(it,jt));
	    double Ntight = fHList[9]->GetBinContent(fHList[9]->GetBin(it,jt));
	    if (Nall>0 && Nall>Ntight) { // loose candidates are generated
	      Text->SetTextSize(0.017);
	      Text->SetTextColor(kRed);
	      Text->SetTextAlign(kHAlignRight+kVAlignBottom);
	      Text->DrawText(xc+0.07, yc-0.07, "LOOSE");
	    }
	  }
	}
      }
      Canvas2->Print(fOutPDFFileName, "pdf");
    }
  }

  Canvas->Print(Form(fOutPDFFileName + "]"), "pdf"); // close file
  gErrorIgnoreLevel = -1; // restore the default
}
