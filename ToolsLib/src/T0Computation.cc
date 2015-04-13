// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ---------------------------------------------------------------

#include "T0Computation.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

/// \class T0Computation
/// \Brief
/// A generic tool for computation of the T0 constants.
/// \EndBrief
///
/// \Detailed
/// A generic tool for computation of the T0 constants for any subdetector.
/// The input is a 2-dimensional histogram of time vs readout channel number.
/// Gaussian fits are performed to each slice of the histogram to estimate the T0s.
/// The output is a text file with the T0 estimates and a PDF report with plots.
/// Daughter classes for each subdetector implemenation are in Analyzers/CalibrationTools.
/// \EndDetailed

T0Computation::T0Computation(Core::BaseAnalysis *ba) : Analyzer(ba) {
  fUseChannelMap = 1;
  fBurstCounter = 0;
  fNChannels = fNChannelsActive = 0;

  // Defaults for the user-defined parameters
  fAnalyzerName       = "";
  fDirName            = "";
  fTH2Name            = "";
  fConfFileName       = "";
  fOutTextFileName    = "";
  fOutPDFFileName     = "";
  fFittingRange       = 0.9;  // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width
  fNFilesToAccumulate = 1;    // time unit for the T0 stability plots
  fHistoTimeLimit     = 20.0;
}

void T0Computation::InitHist() {

  // Checks on the input parameters
  if (!fAnalyzerName.Length()) {
    cout << "Fatal error: an unnamed T0Computation daughter analyser found" << endl;
    exit(1);
  }
  if (!fDirName.Length()) {
    cout << "Fatal error in "<<fAnalyzerName<<": directory in input file not defined" << endl;
    exit(1);
  }
  if (!fTH2Name.Length()) {
    cout << "Fatal error in "<<fAnalyzerName<<": histogram name in input file not defined" << endl;
    exit(1);
  }
  if (!fOutTextFileName.Length()) {
    cout << "Fatal error in "<<fAnalyzerName<<": output text file name not defined" << endl;
    exit(1);
  }

  fH2            = (TH2D*)RequestHistogram(fDirName, fTH2Name, false);
  fH2_Integrated = (TH2D*)RequestHistogram(fDirName, fTH2Name, true);
  fH2_Partial    = new TH2D(*fH2); // for T0 stability vs time
  fH2_Partial->Reset();
  fNChannels = fH2->GetNbinsX();
  fBinWidth  = fH2->GetYaxis()->GetBinWidth(1);

  for (int i=0; i<fNChannels; i++) {
    fChannelID[i] = -99;
    fT0[i]        = -99.999;
  }

  // Find and read the channel map
  ParseConfFile();

  // Check which the RO channels are active
  fNChannelsActive = 0;
  for (int ich=0; ich<fNChannels; ich++) {
    if (!fUseChannelMap) fChannelID[ich] = 0;
    fIsActive[ich] = (fChannelID[ich]>=0);
    if (fIsActive[ich]) {
      ActiveChannelMap[fNChannelsActive++] = ich;
      TString Name = Form("Channel RO %04d ID %04d vs time", ich, fChannelID[ich]);
      fHT0VsBurst[ich] = new TH1D(Name, Name, 5000, -0.5, 4999.5);
    }
  }

  // Book monitoring histograms to be saved into the output
  BookHisto (new TH1D("T0Summary",
		      "T0Summary;RO channel ID;T0 and T0 resolution",
		      fNChannels, -0.5, fNChannels-0.5));
  BookHisto (new TH1D("T0",
		      "T0;RO channel ID;T0 and its error",
		      fNChannels, -0.5, fNChannels-0.5));
  BookHisto (new TH1D("T0Resolution",
		      "T0Resolution;RO channel ID;T0 resolution and its error",
		      fNChannels, -0.5, fNChannels-0.5));
}

void T0Computation::ParseConfFile() {
  ifstream confFile(fConfFileName);
  if (confFile == NULL) {
    fUseChannelMap = 0;
    return;
  }
  TString Line;
  while (Line.ReadLine(confFile)) {
    if (!Line.BeginsWith("ChRemap_")) continue;
    for (Int_t iCh=0; iCh<fNChannels/16; iCh++) {
      if (Line.BeginsWith(Form("ChRemap_%d=",iCh))) {
	TObjArray *l = Line.Tokenize(" ");
	for (Int_t jCh=0; jCh<16; jCh++) {
	  fChannelID[16*iCh+jCh] = ((TObjString*)(l->At(jCh+1)))->GetString().Atoi();
	}
      }
    }
  }
  confFile.close();
}

void T0Computation::EndOfBurstUser() {

  fH2_Partial->Add(fH2);
  fBurstCounter++;

  if (!(fBurstCounter%fNFilesToAccumulate)) {
    EvaluateT0s(fH2_Partial);
    for (int ich=0; ich<fNChannels; ich++) {
      if (fIsActive[ich] && fabs(fT0[ich])<99) {
	fHT0VsBurst[ich]->SetBinContent(fBurstCounter/fNFilesToAccumulate, fT0[ich]);
	fHT0VsBurst[ich]->SetBinError  (fBurstCounter/fNFilesToAccumulate, fDeltaT0[ich]);
      }
    }
    fH2_Partial->Reset();
  }
}

void T0Computation::EndOfRunUser() {

  // Check T0 stability vs burst
  for (int ich=0; ich<fNChannels; ich++) {
    if (fIsActive[ich]) {
      fFChannelStability[ich] = new TF1("pol0", "pol0", -0.5, fBurstCounter-0.5);
      if (fHT0VsBurst[ich]->GetEntries()) {
	fHT0VsBurst[ich]->Fit(fFChannelStability[ich], "0Q", "", -0.5, fBurstCounter-0.5);
      }
    }
  }

  EvaluateT0s(fH2_Integrated);
  GenerateOutput();
  if (fOutPDFFileName.Length()) GeneratePDFReport();

  // Save plots to output
  SaveAllPlots();
  for (int ich=0; ich<fNChannels; ich++) {
    if (fIsActive[ich]) {
      fHTime[ich]->Write();
      fHT0VsBurst[ich]->Write();
    }
  }
}

void T0Computation::EvaluateT0s(TH2D *h2) {
  for (int ich=0; ich<fNChannels; ich++) {
    if (fIsActive[ich]) {
      TString Name = Form("Channel RO %04d ID %04d", ich, fChannelID[ich]);
      if (fHTime[ich]) delete fHTime[ich];
      fHTime[ich] = h2->ProjectionY(Name, ich+1, ich+1);
      fHTime[ich]->SetTitle(Name);
      EvaluateChannelT0(ich);
    }
  }
}

void T0Computation::EvaluateChannelT0 (int ich) {

  // Check if there are enough entries for the fit to converge
  if (fHTime[ich]->Integral()==0) return;
  fT0[ich] = +99.999;
  if (fHTime[ich]->Integral()<100) return;

  // Fitting interval: around the bin with max content
  Int_t maxbin  = fHTime[ich]->GetMaximumBin();
  Double_t c0   = fHTime[ich]->GetBinCenter(maxbin);
  Double_t cmin = c0 - fFittingRange;
  Double_t cmax = c0 + fFittingRange;
  fFChannelFit[ich] = new TF1("gaus", "gaus", cmin, cmax);
  fFChannelFit[ich]->SetParameter(0, fHTime[ich]->GetBinContent(maxbin));
  fFChannelFit[ich]->SetParameter(1, c0);
  fFChannelFit[ich]->SetParameter(2, 1.0);
  fHTime[ich]->Fit(fFChannelFit[ich], "0Q", "", cmin, cmax);
  double T0         = fFChannelFit[ich]->GetParameter(1);
  double Resol      = fFChannelFit[ich]->GetParameter(2);
  double DeltaT0    = fFChannelFit[ich]->GetParError(1);
  double DeltaResol = fFChannelFit[ich]->GetParError(2);

  // If the first fit fails, second attempt with more degrees of freedom
  if (Resol>2.0 || DeltaT0>0.5 || DeltaResol>0.5) {

    delete fFChannelFit[ich];
    fFChannelFit[ich] = new TF1("GausPol", "gaus(0)+pol0(3)", cmin, cmax);
    fFChannelFit[ich]->SetParameter(0, fHTime[ich]->GetBinContent(maxbin));
    fFChannelFit[ich]->SetParameter(1, c0);
    fFChannelFit[ich]->SetParameter(2, 1.0);
    fFChannelFit[ich]->SetParameter(3, 0.0);
    fHTime[ich]->Fit(fFChannelFit[ich], "0Q", "", cmin, cmax);
    T0         = fFChannelFit[ich]->GetParameter(1);
    Resol      = fFChannelFit[ich]->GetParameter(2);
    DeltaT0    = fFChannelFit[ich]->GetParError(1);
    DeltaResol = fFChannelFit[ich]->GetParError(2);

    // Check if the second fit is successful
    if (Resol>2.0 || DeltaT0>0.5 || DeltaResol>0.5) return;
  }

  fT0[ich]      = T0;
  fDeltaT0[ich] = DeltaT0;

  fHisto.GetHisto("T0Summary")->   SetBinContent(ich+1, T0);
  fHisto.GetHisto("T0Summary")->   SetBinError  (ich+1, Resol);
  fHisto.GetHisto("T0")->          SetBinContent(ich+1, T0);
  fHisto.GetHisto("T0")->          SetBinError  (ich+1, DeltaT0);
  fHisto.GetHisto("T0Resolution")->SetBinContent(ich+1, Resol);
  fHisto.GetHisto("T0Resolution")->SetBinError  (ich+1, DeltaResol);
}

//////////////////////////
// Build a PDF report file

void T0Computation::GeneratePDFReport() {

  TCanvas *FrontCanvas = new TCanvas("FrontCanvas");
  FrontCanvas->Divide(1,2);
  for (int i=1; i<=2; i++) {
    FrontCanvas->GetPad(i)->SetLeftMargin(0.05);
    FrontCanvas->GetPad(i)->SetRightMargin(0.01);
    FrontCanvas->GetPad(i)->SetTopMargin(0.01);
    FrontCanvas->GetPad(i)->SetBottomMargin(0.10);
  }

  TCanvas *Canvas = new TCanvas("Canvas");
  Canvas->Divide(4,4);
  for (int i=1; i<=16; i++) {
    Canvas->GetPad(i)->SetLeftMargin(0.05);
    Canvas->GetPad(i)->SetRightMargin(0.01);
    Canvas->GetPad(i)->SetTopMargin(0.01);
    Canvas->GetPad(i)->SetBottomMargin(0.06);
  }

  TText *Text = new TText();
  Text->SetTextAlign(kHAlignCenter+kVAlignCenter);
  Text->SetTextSize(0.15);
  Text->SetTextColor(kGreen+2);

  ///////////////////////////
  // Summary plots (one page)

  FrontCanvas->cd(1);
  fHisto.GetHisto("T0")->SetStats(0);
  fHisto.GetHisto("T0")->GetXaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0")->GetYaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0")->GetXaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0")->GetYaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0")->GetYaxis()->SetTitleOffset(0.5);
  fHisto.GetHisto("T0")->GetXaxis()->SetTitle("Channel ID");
  fHisto.GetHisto("T0")->GetYaxis()->SetTitle("T0 and its error [ns]");
  fHisto.GetHisto("T0")->Draw();

  FrontCanvas->cd(2);
  fHisto.GetHisto("T0Resolution")->SetStats(0);
  fHisto.GetHisto("T0Resolution")->GetXaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetXaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetTitleOffset(0.5);
  fHisto.GetHisto("T0Resolution")->GetXaxis()->SetTitle("Channel ID");
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetTitle("Peak width and its error [ns]");
  fHisto.GetHisto("T0Resolution")->Draw();

  FrontCanvas->Print(Form(fOutPDFFileName + "("), "pdf");

  //////////////////////////
  // Time distribution plots

  int Npages = fNChannelsActive/16;
  if (fNChannelsActive%16) Npages++;
  for (int ipage=0; ipage<Npages; ipage++) {
    for (int i=0; i<16; i++) {
      Canvas->GetPad(i+1)->Clear();
      int ichActive = ipage*16 + i;
      if (ichActive>=fNChannelsActive) continue;
      int ich = ActiveChannelMap[ichActive];
      Canvas->cd(i+1);
      Int_t maxbin = fHTime[ich]->GetMaximumBin();
      Double_t c0  = fHTime[ich]->GetBinContent(maxbin);
      if (c0<1) c0 = 1;
      fHTime[ich]->SetMaximum(1.1*c0);
      fHTime[ich]->GetXaxis()->SetRangeUser(-fHistoTimeLimit, +fHistoTimeLimit);
      fHTime[ich]->GetXaxis()->SetLabelSize(0.07);
      fHTime[ich]->GetXaxis()->SetTitle("");
      fHTime[ich]->SetLineWidth(1);
      fHTime[ich]->Draw();
      if (fFChannelFit[ich]) {
	fFChannelFit[ich]->SetLineWidth(1);
	fFChannelFit[ich]->Draw("same");
      }
      if (fHTime[ich]->Integral()==0) {
	Text->DrawText(0, 0.55*c0, "EMPTY");
      }
      else if (fHTime[ich]->Integral()<100) {
	Text->DrawText(0, 0.55*c0, "TOO FEW ENTRIES");
      }
      else if (fT0[ich]>99) {
	Text->DrawText(0, 0.55*c0, "FIT FAILED");
      }
    }
    Canvas->Print(fOutPDFFileName, "pdf");
  }

  ///////////////////////
  // Time stability plots

  for (int ipage=0; ipage<Npages; ipage++) {
    for (int i=0; i<16; i++) {
      Canvas->GetPad(i+1)->Clear();
      int ichActive = ipage*16 + i;
      if (ichActive>=fNChannelsActive) continue;
      int ich = ActiveChannelMap[ichActive];
      Canvas->cd(i+1);

      double ymin = 999, ymax = -999;
      for (int i=1; i<=fHT0VsBurst[ich]->GetNbinsX(); i++) {
	double c = fHT0VsBurst[ich]->GetBinContent(i);
	double e = fHT0VsBurst[ich]->GetBinError(i);
	if (c) {
	  if (ymin > c-e) ymin = c-e;
	  if (ymax < c+e) ymax = c+e;
	}
      }
      if (ymin>ymax) { ymin = -1; ymax = +1; }
      fHT0VsBurst[ich]->GetXaxis()->SetRangeUser(-0.5, fBurstCounter/fNFilesToAccumulate-0.5);
      fHT0VsBurst[ich]->SetMinimum(ymin);
      fHT0VsBurst[ich]->SetMaximum(ymax);
      fHT0VsBurst[ich]->SetLineWidth(1);
      fHT0VsBurst[ich]->SetLineColor(kBlue);
      fHT0VsBurst[ich]->SetMarkerColor(kBlue);
      fFChannelStability[ich]->SetLineColor(kRed);
      fFChannelStability[ich]->SetLineWidth(1);
      fHT0VsBurst[ich]->Draw();
      if (fHT0VsBurst[ich]->GetEntries()) fFChannelStability[ich]->Draw("same");
    }
    Canvas->Print(fOutPDFFileName, "pdf");
  }

  Canvas->Print(Form(fOutPDFFileName + "]"), "pdf"); // close file
  delete FrontCanvas;
  delete Canvas;
}

////////////////////////////////
// Build an output file with T0s

void T0Computation::GenerateOutput() {
  time_t now = time(0);
  ofstream outfile (fOutTextFileName);
  outfile << "# "<<fAnalyzerName<<" T0 constants. Format: RO channel; geometric ID; T0 offset (ns)."<<endl;
  outfile << "# These T0 offsets should be subtracted from the raw times."<<endl;
  outfile << "# Special values: -99.999 for masked channels, +99.999 for failed T0 fits."<<endl;
  outfile << "# An offset T0 is ignored by the reconstruction in case |T0|>99ns."<<endl;
  outfile << "#\n# Generated on "<<asctime(localtime(&now));
  outfile << "#"<<endl;
  for (int i=0; i<fNChannels; i++) {
    if (fIsActive[i]) outfile << Form("%4d %4d %7.3f\n", i, fChannelID[i], fT0[i]);
  }
  outfile.close();
}
