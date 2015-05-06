// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
//
// ---------------------------------------------------------------

#include "T0Evaluation.hh"

using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;

/// \class T0Evaluation
/// \Brief
/// A generic tool for computation of the T0 constants.
/// \EndBrief
///
/// \Detailed
/// A generic tool for computation of the T0 constants and the global T0 for any subdetector.
/// The input for T0 computations is a 2-dimensional histogram of
/// (Uncorrected RecoHit time wrt reference time) vs (readout channel number).
/// The only correction to be made to the RecoHit time is the subtraction of the global T0.
/// The recommended histogram name is "RecoHitTimeWrtReferenceVsReadoutChannelNoT0".
/// The recommended time bin width is 0.2ns.
/// Gaussian fits are performed to each slice of the histogram to evaluate the T0s.
/// The output is a (potentially NA62Reco-readable) text with the T0 constants and a PDF report with plots.
/// The input for the global T0 computation is a histogram filled with the "raw"
/// RecoHit leading times (not corrected for anything).
/// The recommended histogram name is "LeadingTimeRaw".
/// The recommended number of bins is 5000, the recommended range is (-5000, 5000)ns.
/// Daughter classes for each subdetector are in Analyzers/CalibrationTools.
/// \EndDetailed

T0Evaluation::T0Evaluation(Core::BaseAnalysis *ba, std::string DetectorName) : Analyzer(ba) {

  // Defaults for the user-defined parameters
  fDetectorName       = DetectorName;
  fAnalyzerName       = fDetectorName + "_T0";
  fDirName            = fDetectorName + "Monitor";
  fConfFileName       = "./" + fDetectorName + ".conf";
  fOutTextFileName    = "./" + fDetectorName + "-T0.dat";
  fOutPDFFileName     = "./" + fDetectorName + "-T0.pdf";
  fTH2Name            = "RecoHitTimeWrtReferenceVsReadoutChannelNoT0";
  fRawTimeHistoName   = "LeadingTimeRaw";

  fFittingRange       = 0.9;  // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width
  fNFilesToAccumulate = 10;   // for the T0 stability plots
  fHistoTimeLimit     = 20.0;

  // Initialization of the analyzer
  fEvaluateGlobalT0 = fEvaluateT0s = fUseChannelMap = 1;
  fBurstCounter  = 0;
  fNChannels = fNChannelsActive = 0;
}

void T0Evaluation::InitHist() {

  if (!GetIsHisto() || GetIsTree()) {
    cout << "ERROR: T0Evaluation-based analyzers must be run in the --histo mode" << endl;
    exit(0);
  }

  fHRawTime      = (TH1D*)RequestHistogram(fDirName, fRawTimeHistoName, true);
  fH2            = (TH2D*)RequestHistogram(fDirName, fTH2Name, false); // reset for each input file
  fH2_Integrated = (TH2D*)RequestHistogram(fDirName, fTH2Name, true);  // accumulated

  if (!fHRawTime) {
    fEvaluateGlobalT0 = 0;
    cout << "Warning in "<<fAnalyzerName<<": histogram for global T0 evaluation (" <<
      fDirName << "/" << fRawTimeHistoName << ") not found" << endl;
  }
  if (!fH2) {
    fEvaluateT0s = 0;
    cout << "Warning in "<<fAnalyzerName<<": histogram for T0 evaluation (" << 
      fDirName << "/" << fTH2Name << ") not found" << endl;
  }

  if (fEvaluateT0s) {
    fH2_Partial = new TH2D(*fH2); // for T0 stability vs time
    fH2_Partial->Reset();
    fNChannels = fH2->GetNbinsX();
    fBinWidth  = fH2->GetYaxis()->GetBinWidth(1);
  }

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
  if (fEvaluateT0s) {
    BookHisto (new TH1D("T0",
			"T0;RO channel ID;T0 and its error",
			fNChannels, -0.5, fNChannels-0.5));
    BookHisto (new TH1D("T0Resolution",
			"T0Resolution;RO channel ID;T0 resolution and its error",
			fNChannels, -0.5, fNChannels-0.5));
  }
}

///////////////////////////////////////////////////
// Read the channle map from the configuration file

void T0Evaluation::ParseConfFile() {
  ifstream confFile(fConfFileName);
  if (!fConfFileName.Length() || !confFile) {
    cout << "Warning in "<<fAnalyzerName<<": config file " << 
      fConfFileName << " not found, channel map not available" << endl;
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

////////////////////////////////////
// Build the T0 time stability plots

void T0Evaluation::EndOfBurstUser() {
  if (!fEvaluateT0s) return;
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

void T0Evaluation::EndOfRunUser() {

  // Time stability of the T0s
  for (int ich=0; ich<fNChannels; ich++) {
    if (fIsActive[ich]) {
      fFChannelStability[ich] = new TF1("pol0", "pol0", -0.5, fBurstCounter-0.5);
      if (fHT0VsBurst[ich]->GetEntries()) {
	fHT0VsBurst[ich]->Fit(fFChannelStability[ich], "0Q", "", -0.5, fBurstCounter-0.5);
      }
    }
  }

  // Evaluate the T0s and global offset with the full data sample
  if (fEvaluateGlobalT0) EvaluateGlobalOffset();
  if (fEvaluateT0s) EvaluateT0s(fH2_Integrated);

  // Generate and save the output
  if (fEvaluateT0s) {
    GenerateT0TextFile();
    GeneratePDFReport();
  }

  SaveAllPlots();
  for (int ich=0; ich<fNChannels; ich++) {
    if (fIsActive[ich]) {
      fHTime[ich]->Write();
      fHT0VsBurst[ich]->Write();
    }
  }
}

void T0Evaluation::EvaluateT0s(TH2D *h2) {
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

void T0Evaluation::EvaluateChannelT0 (int ich) {

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

  fHisto.GetHisto("T0")->          SetBinContent(ich+1, T0);
  fHisto.GetHisto("T0")->          SetBinError  (ich+1, DeltaT0);
  fHisto.GetHisto("T0Resolution")->SetBinContent(ich+1, Resol);
  fHisto.GetHisto("T0Resolution")->SetBinError  (ich+1, DeltaResol);
}

/////////////////////////////////////////////////////////////////////
// Global T0 evaluation: discard bins with low content (accidentals),
// then find the mean value of the remaining bins

void T0Evaluation::EvaluateGlobalOffset() {
  TH1D *fHRawTime1 = new TH1D(*fHRawTime);
  double maxcontent = fHRawTime1->GetBinContent(fHRawTime1->GetMaximumBin());
  for (int i=1; i<=fHRawTime1->GetNbinsX(); i++) {
    if (fHRawTime1->GetBinContent(i)<0.5*maxcontent) fHRawTime1->SetBinContent(i,0);
  }
  fGlobalT0 = round(fHRawTime1->GetMean());
  cout << fDetectorName << " global T0 = " << fGlobalT0 << " ns " << endl;
  delete fHRawTime1;
}

/////////////////////
// Build a PDF report

void T0Evaluation::GeneratePDFReport() {

  cout <<"Generating report: "<< fOutPDFFileName << endl;

  gErrorIgnoreLevel = 5000; // suppress messages generated for each page printed

  TCanvas *FrontCanvas = new TCanvas("FrontCanvas");
  FrontCanvas->Divide(1,3);
  for (int i=1; i<=3; i++) {
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

  FrontCanvas->cd(3);
  if (fEvaluateGlobalT0) {
    double maxcontent = fHRawTime->GetBinContent(fHRawTime->GetMaximumBin());

    int maxbin = fHRawTime->GetNbinsX();
    while (fHRawTime->GetBinContent(maxbin)<0.01*maxcontent &&
	   maxbin>1) maxbin--;
    int minbin = 1;
    while (fHRawTime->GetBinContent(minbin)<0.01*maxcontent &&
	   minbin<fHRawTime->GetNbinsX()) minbin++;

    fHRawTime->SetStats(0);
    fHRawTime->SetTitle(Form("Digi Raw Time: global T0 = %d ns", (int)fGlobalT0));
    fHRawTime->GetXaxis()->SetRangeUser
      (fHRawTime->GetBinLowEdge(minbin), fHRawTime->GetBinLowEdge(maxbin+1));
    fHRawTime->SetLineColor(kBlue);
    fHRawTime->SetLineWidth(1);
    fHRawTime->Draw();
    TLine *l = new TLine();
    l->SetLineColor(kGreen+2);
    l->SetLineWidth(1);
    l->DrawLine(fGlobalT0, 0, fGlobalT0, fHRawTime->GetMaximum());
  }

  FrontCanvas->Print(Form(fOutPDFFileName + "("), "pdf");

  ///////////////////////////////////////////
  // Time distribution plots (multiple pages)

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

  ////////////////////////////////////////
  // Time stability plots (multiple pages)

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
  gErrorIgnoreLevel = -1; // restore the default
}

////////////////////////////////
// Build an output file with T0s

void T0Evaluation::GenerateT0TextFile() {
  time_t now = time(0);
  ofstream outfile (fOutTextFileName);
  outfile << "# "<<fDetectorName<<" T0 constants. Format: RO channel; geometric ID; T0 offset (ns)."<<endl;
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
