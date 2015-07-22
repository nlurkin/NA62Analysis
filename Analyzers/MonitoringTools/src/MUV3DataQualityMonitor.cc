// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-06-25
//
// ---------------------------------------------------------------

#include "MUV3DataQualityMonitor.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

/// \class MUV3DataQualityMonitor
/// \Brief
/// Build a PDF report using the standard MUV3 monitoring histograms as input
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
  fHChannelProfile    = (TH1D*)RequestHistogram(fDirName, "ChannelProfile",    true);
  fHChannelProfileEOB = (TH1D*)RequestHistogram(fDirName, "ChannelProfileEOB", true);
  fHTileOR            = (TH1D*)RequestHistogram(fDirName, "TileOR",            true);
  fHTileAND           = (TH1D*)RequestHistogram(fDirName, "TileAND",           true);
  fHChannelProfileVsBurst    = (TH2D*)RequestHistogram(fDirName, "ChannelProfileVsBurst",    true);
  fHChannelProfileVsBurstEOB = (TH2D*)RequestHistogram(fDirName, "ChannelProfileVsBurstEOB", true);

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

  // SL EOB data
  fHTotalPrimitiveCountsEOB = (TH1D*)RequestHistogram(fDirName, "TotalPrimitiveCountsEOB", true);
  fHErrorCountsEOB          = (TH1D*)RequestHistogram(fDirName, "ErrorCountsEOB",          true);
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

  // Build tile asymmetries
  TString Name = "TileAsymmetry";
  fHTileAsymmetry = new TH1D(Name, Name, 152, -0.5, 151.5);
  fHTileAsymmetry->GetXaxis()->SetTitle("MUV3 tile");
  fHTileAsymmetry->GetYaxis()->SetTitle("Asymmetry N(hits): (PM1-PM0)/(PM1+PM0)");

  Name = "TileAsymmetryEOB";
  fHTileAsymmetryEOB = new TH1D(Name, Name, 152, -0.5, 151.5);
  fHTileAsymmetryEOB->GetXaxis()->SetTitle("MUV3 tile");
  fHTileAsymmetryEOB->GetYaxis()->SetTitle("Asymmetry N(hits): (PM1-PM0)/(PM1+PM0)");

  for (int i=1; i<=152; i++) {
    double x    = fHChannelProfile->GetBinContent(i+200);
    double y    = fHChannelProfile->GetBinContent(i);
    if (x+y<1) continue;
    double dx   = sqrt(x);
    double dy   = sqrt(y);
    double f    = (x-y)/(x+y);
    double dfdx = 2.0*y/(x+y)/(x+y);
    double dfdy = 2.0*x/(x+y)/(x+y);
    double df   = sqrt((dfdx*dx)*(dfdx*dx)+(dfdy*dy)*(dfdy*dy));
    fHTileAsymmetry->SetBinContent(i, f);
    fHTileAsymmetry->SetBinError(i, df);
  }

  for (int i=1; i<=152; i++) {
    double x    = fHChannelProfileEOB->GetBinContent(i+200);
    double y    = fHChannelProfileEOB->GetBinContent(i);
    if (x+y<1) continue;
    double dx   = sqrt(x);
    double dy   = sqrt(y);
    double f    = (x-y)/(x+y);
    double dfdx = 2.0*y/(x+y)/(x+y);
    double dfdy = 2.0*x/(x+y)/(x+y);
    double df   = sqrt((dfdx*dx)*(dfdx*dx)+(dfdy*dy)*(dfdy*dy));
    fHTileAsymmetryEOB->SetBinContent(i, f);
    fHTileAsymmetryEOB->SetBinError(i, df);
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
      Canvas->GetPad(i)->SetLeftMargin(0.04);
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
      Canvas->Update();
      Text->SetTextSize(0.07);
      Text->SetTextColor(kBlack);
      Text->SetTextAlign(kHAlignLeft+kVAlignTop);
      Text->DrawText(10, 0.92*gPad->GetUymax(), Form("Bursts processed: %d", NBurstsProcessed));
      Text->DrawText(10, 0.80*gPad->GetUymax(), Form("Events processed: %d", NEventsProcessed));
      Text->DrawText(10, 0.68*gPad->GetUymax(), Form("Digis: %d", (int)fHChannelProfile->Integral()));
    }

    Canvas->cd(2);
    Double_t ymax = fHTileOR->GetMaximum();
    fHTileOR->SetTitle("MUV3: OR, AND of signals in tiles");
    fHTileOR->GetXaxis()->SetTitleSize(0.055);
    fHTileOR->GetXaxis()->SetLabelSize(0.055);
    fHTileOR->GetYaxis()->SetTitleSize(0.055);
    fHTileOR->GetYaxis()->SetLabelSize(0.055);
    fHTileOR->GetYaxis()->SetRangeUser(0, 1.02*ymax);
    fHTileOR->SetStats(0);
    fHTileOR->SetLineWidth(1);
    fHTileOR->SetLineColor(kBlue);
    fHTileAND->SetStats(0);
    fHTileAND->SetLineWidth(1);
    fHTileAND->SetLineColor(kRed);
    fHTileOR->Draw("hist");
    fHTileAND->Draw("hist same");

    // Mark the non-existing tiles
    TBox *b0 = new TBox();
    b0->SetFillColor(kGreen-7);
    b0->SetLineColor(kGreen-7);
    b0->DrawBox(64.6, 0.01*fHTileOR->GetMaximum(), 66.4, 1.01*ymax);
    b0->DrawBox(76.6, 0.01*fHTileOR->GetMaximum(), 78.4, 1.01*ymax);

    TLegend* ANDORLegend = new TLegend(0.20,0.75,0.35,0.88);
    ANDORLegend->SetFillColor(kWhite);
    ANDORLegend->SetTextSize(0.05);
    ANDORLegend->AddEntry(fHTileOR,  "OR", "l");
    ANDORLegend->AddEntry(fHTileAND, "AND", "l");
    ANDORLegend->Draw();

    Canvas->cd(3);
    fHTileAsymmetry->SetTitle("MUV3: Asymmetries in tiles, A = (N1-N0) / (N1+N0). Red=Readout, Blue=EoB.");
    fHTileAsymmetry->GetXaxis()->SetTitleSize(0.055);
    fHTileAsymmetry->GetXaxis()->SetLabelSize(0.055);
    fHTileAsymmetry->GetYaxis()->SetTitleSize(0.055);
    fHTileAsymmetry->GetYaxis()->SetLabelSize(0.055);
    fHTileAsymmetry->SetMinimum(-1.05);
    fHTileAsymmetry->SetMaximum(+1.05);
    fHTileAsymmetry->SetLineColor(kRed);
    fHTileAsymmetry->SetMarkerColor(kRed);
    fHTileAsymmetry->SetMarkerStyle(21);
    fHTileAsymmetry->SetMarkerSize(0.32);
    fHTileAsymmetryEOB->SetLineColor(kBlue);
    fHTileAsymmetryEOB->SetMarkerColor(kBlue);
    fHTileAsymmetryEOB->SetMarkerStyle(20);
    fHTileAsymmetryEOB->SetMarkerSize(0.32);

    fHTileAsymmetry->Draw("p");
    fHTileAsymmetryEOB->Draw("p same");

    // Mark the non-existing tiles
    b0->DrawBox(64.6, -1.03, 66.4, 1.03);
    b0->DrawBox(76.6, -1.03, 78.4, 1.03);

    Text->SetTextSize(0.05);
    Text->SetTextAngle(90);
    Text->SetTextColor(kGreen+2);
    Text->SetTextAlign(kHAlignCenter+kVAlignCenter);
    for (int i=1; i<=152; i++) {
      int iTile = fHTileAsymmetry->GetBinCenter(i);
      double Asym = fHTileAsymmetry->GetBinContent(i);
      if (fabs(Asym)>0.6 &&
          iTile!=65 && iTile!=66 && iTile!=77 && iTile!=78) { // these tiles do not exist
	double y = (Asym>0) ? 0.5 : -0.5;
        Text->DrawText(iTile, y, Form("%d", iTile));
      }
    }
    Canvas->Print(Form(fOutPDFFileName + "("), "pdf"); // open file and print
  }
  Text->SetTextAngle(0);

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
  titles [0] = "N(Digis, low PMT)";
  titles [1] = "N(Digis, high PMT)";
  titles [2] = "N(Digis), High#minusLow and A=(High#minusLow)/(High+Low)";
  titles [3] = "N(Candidates)";
  titles [4] = "void"; // tight candidates are not plotted separately
  titles [5] = "EOB: N(hits, low PMT)";
  titles [6] = "EOB: N(hits, high PMT)";
  titles [7] = "EOB: N(hits), High#minusLow and A=(High#minusLow)/(High+Low)";
  titles [8] = "N(EOB L0FW hits)";

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

      // Display integrals in title
      if (i==0 || i==2 || i==6) {
	titles[i/2] += Form(". Inner: %d, outer: %d",
			    (int)fHList[i+1]->Integral(), (int)fHList[i]->Integral());
      }
      if (i==10 || i==12 || i==16) {
	titles[i/2] += Form(". Inner: %4.2fM, outer: %4.2fM",
			    1e-6*fHList[i+1]->Integral(), 1e-6*fHList[i]->Integral());
      }

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

  //////////////////////////////////////
  // EOB primitive type and error counts

  if (fHTotalPrimitiveCountsEOB && fHErrorCountsEOB) {
    TCanvas *Canvas3 = new TCanvas("Canvas3");
    Canvas3->Divide(2,1);
    Canvas3->SetLeftMargin(0.01);
    Canvas3->SetRightMargin(0.01);
    Canvas3->SetTopMargin(0.01);
    Canvas3->SetBottomMargin(0.01);
    for (int i=1; i<=2; i++) {
      Canvas3->GetPad(i)->SetLeftMargin(0.08);
      Canvas3->GetPad(i)->SetRightMargin(0.01);
      Canvas3->GetPad(i)->SetTopMargin(0.06);
      Canvas3->GetPad(i)->SetBottomMargin(0.05);
    }

    Canvas3->cd(1);
    fHTotalPrimitiveCountsEOB->SetTitle("EOB primitive counts by type [mln]");
    fHTotalPrimitiveCountsEOB->GetXaxis()->SetTitle("Primitive type");
    fHTotalPrimitiveCountsEOB->GetXaxis()->SetLabelSize(0); // suppress labels
    fHTotalPrimitiveCountsEOB->GetXaxis()->SetTitleOffset(0.4);
    fHTotalPrimitiveCountsEOB->SetLineWidth(1);
    fHTotalPrimitiveCountsEOB->SetLineColor(kBlue);
    fHTotalPrimitiveCountsEOB->SetFillColor(kYellow);
    fHTotalPrimitiveCountsEOB->Draw();

    Text->SetTextSize(0.04);
    Text->SetTextAngle(90);
    Text->SetTextAlign(kHAlignCenter+kVAlignCenter);
    Text->SetTextColor(kGreen+2);
    for (int i=0; i<16; i++) {
      Text->DrawText(i, 0.5*fHTotalPrimitiveCountsEOB->GetMaximum(),
		     fHTotalPrimitiveCountsEOB->GetXaxis()->GetBinLabel(i+1));
    }
    TLine *l = new TLine();
    l->SetLineColor(kRed);
    Canvas3->Update();
    l->DrawLine( 9.5, 0,  9.5, gPad->GetUymax());
    l->DrawLine(13.5, 0, 13.5, gPad->GetUymax());
      
    Canvas3->cd(2);
    fHErrorCountsEOB->SetTitle("SL EOB error counts by type");
    fHErrorCountsEOB->GetXaxis()->SetTitle("Error type");
    fHErrorCountsEOB->GetXaxis()->SetLabelSize(0); // suppress labels
    fHErrorCountsEOB->GetXaxis()->SetTitleOffset(0.4);
    fHErrorCountsEOB->SetLineWidth(1);
    fHErrorCountsEOB->SetLineColor(kBlue);
    fHErrorCountsEOB->SetFillColor(kYellow);
    fHErrorCountsEOB->Draw();
    for (int i=0; i<12; i++) {
      Text->DrawText(i, 0.5*fHErrorCountsEOB->GetMaximum(),
		     fHErrorCountsEOB->GetXaxis()->GetBinLabel(i+1));
    }
    Text->SetTextAngle(0);

    Canvas3->Print(fOutPDFFileName, "pdf");
  }

  /////////////////////////////////////////////////
  // Tile asymmetry vs burst ID plots for each tile
  GenerateTileAsymmetryVsBurstIDPlots();

  Canvas->Print(Form(fOutPDFFileName + "]"), "pdf"); // close file
  gErrorIgnoreLevel = -1; // restore the default
}

void MUV3DataQualityMonitor::GenerateTileAsymmetryVsBurstIDPlots() {

  if (!fHChannelProfileVsBurstEOB->Integral()) return;

  Int_t FirstBurst = 0;
  while (!fHChannelProfileVsBurstEOB->Integral(FirstBurst+1, FirstBurst+1)) FirstBurst++;
  Int_t LastBurst = fHChannelProfileVsBurstEOB->GetNbinsX();
  while (!fHChannelProfileVsBurstEOB->Integral(LastBurst+1, LastBurst+1)) LastBurst--;
  
  Int_t Ntiles = 152;
  Int_t Npages = Ntiles/16;
  if (Ntiles%16) Npages++;

  TCanvas* Canvas = new TCanvas("CanvasA");
  Canvas->Divide(4, 4);
  for (Int_t i=1; i<=16; i++) {
    Canvas->GetPad(i)->SetLeftMargin(0.04);
    Canvas->GetPad(i)->SetRightMargin(0.0);
    Canvas->GetPad(i)->SetTopMargin(0.0);
    Canvas->GetPad(i)->SetBottomMargin(0.03);
  }

  vector<TH1D*> hAsym1(Ntiles), hAsym2(Ntiles);

  for (int ipage=0; ipage<Npages; ipage++) {
    for (int ipad=0; ipad<16; ipad++) {
      Canvas->GetPad(ipad+1)->Clear();
      int itile = ipage*16 + ipad;
      if (itile>=Ntiles) continue;
      Canvas->cd(ipad+1);

      // TDC event monitor
      TString Name = Form("Tile %03d asymmetry vs burst ID", itile);
      hAsym1[itile] = new TH1D(Name, Name, 5000, -0.5, 4999.5);

      for (int i=FirstBurst; i<=LastBurst; i++) {
	Double_t x = fHChannelProfileVsBurst->GetBinContent(i+1, itile+201);
	Double_t y = fHChannelProfileVsBurst->GetBinContent(i+1, itile+1);
	if (x+y<1) {
	  hAsym1[itile]->SetBinContent(i+1, 999);
	  hAsym1[itile]->SetBinError(i+1, 0);
	  continue;
	}
	double dx   = sqrt(x);
	double dy   = sqrt(y);
	double f    = (x-y)/(x+y);
	double dfdx = 2.0*y/(x+y)/(x+y);
	double dfdy = 2.0*x/(x+y)/(x+y);
	double df   = sqrt((dfdx*dx)*(dfdx*dx)+(dfdy*dy)*(dfdy*dy));
	if (df==0) df = 1e-3;
	hAsym1[itile]->SetBinContent(i+1, f);
	hAsym1[itile]->SetBinError(i+1, df);
      }

      hAsym1[itile]->GetXaxis()->SetRangeUser(FirstBurst-0.5, LastBurst+0.5);
      hAsym1[itile]->GetXaxis()->SetLabelSize(0.05);
      hAsym1[itile]->GetYaxis()->SetLabelSize(0.05);
      hAsym1[itile]->SetTitle(Name);
      hAsym1[itile]->SetMinimum(-1.1);
      hAsym1[itile]->SetMaximum(1.1);
      hAsym1[itile]->SetMarkerStyle(21);
      hAsym1[itile]->SetMarkerColor(kRed);
      hAsym1[itile]->SetMarkerSize(.15);
      hAsym1[itile]->SetLineColor(kRed);
      hAsym1[itile]->Draw();

      // EOB counters
      Name = Form("Tile %03d asymmetry(EOB) vs burst ID", itile);
      hAsym2[itile] = new TH1D(Name, Name, 5000, -0.5, 4999.5);

      for (int i=FirstBurst; i<=LastBurst; i++) {
	Double_t x = fHChannelProfileVsBurstEOB->GetBinContent(i+1, itile+201);
	Double_t y = fHChannelProfileVsBurstEOB->GetBinContent(i+1, itile+1);
	if (x+y<1) {
	  hAsym2[itile]->SetBinContent(i+1, 999);
	  hAsym2[itile]->SetBinError(i+1, 0);
	  continue;
	}
	double dx   = sqrt(x);
	double dy   = sqrt(y);
	double f    = (x-y)/(x+y);
	double dfdx = 2.0*y/(x+y)/(x+y);
	double dfdy = 2.0*x/(x+y)/(x+y);
	double df   = sqrt((dfdx*dx)*(dfdx*dx)+(dfdy*dy)*(dfdy*dy));
	if (df==0) df = 1e-3;
	hAsym2[itile]->SetBinContent(i+1, f);
	hAsym2[itile]->SetBinError(i+1, df);
      }

      hAsym2[itile]->SetMarkerStyle(20);
      hAsym2[itile]->SetMarkerColor(kBlue);
      hAsym2[itile]->SetMarkerSize(.15);
      hAsym2[itile]->SetLineColor(kBlue);
      hAsym2[itile]->Draw("same");

      if (!ipad) { // a simple legend
	TText *Text = new TText();
	Text->SetTextSize(0.07);
	Text->SetTextColor(kRed);
	Text->SetTextAlign(kHAlignLeft+kVAlignBottom);
	Text->DrawText(FirstBurst+0.03*(LastBurst-FirstBurst), -0.8, "Readout");
	Text->SetTextColor(kBlue);
	Text->DrawText(FirstBurst+0.03*(LastBurst-FirstBurst), -1.0, "EOB");
      }
    }
    Canvas->Print(fOutPDFFileName, "pdf");
  }

  hAsym1.clear();
  hAsym2.clear();
  delete Canvas;
}
