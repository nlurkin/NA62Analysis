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
/// (Uncorrected RecoHit time wrt reference time) vs (Readout channel number).
/// The only correction to be made to the RecoHit time is the subtraction of the global T0.
/// The recommended histogram name is "RecoHitTimeWrtReferenceVsReadoutChannelNoT0".
/// The recommended time bin width is 0.2ns.
/// Gaussian fits are performed to each slice of the histogram to evaluate the T0s.
/// Basic checks of time distribution shape are made, and warnings are issued if requested by user.
/// The output is a (potentially) NA62Reco-readable text with the T0 constants and a PDF report with plots.
/// The input for the global T0 computation is a histogram filled with the "raw"
/// RecoHit leading times (not corrected for anything).
/// The recommended histogram name is "LeadingTimeRaw".
/// The recommended number of bins is 5000, the recommended range is (-5000, 5000)ns.
/// Daughter classes for each subdetector are in Analyzers/CalibrationTools.
/// \EndDetailed

T0Evaluation::T0Evaluation(Core::BaseAnalysis *ba, std::string DetectorName) : Analyzer(ba) {

  // Defaults for the user-defined parameters
  fDetectorName        = DetectorName;
  fAnalyzerName        = fDetectorName + "T0";
  fDirName             = fDetectorName + "Monitor";
  fConfFileName        = "./" + fDetectorName + ".conf";
  fOutTextFileName     = "./" + fDetectorName + "-T0.dat";
  fOutPDFFileName      = "./" + fDetectorName + "-T0.pdf";
  fTH2Name             = "RecoHitTimeWrtReferenceVsReadoutChannelNoT0";
  fRawTimeHistoName    = "LeadingTimeRaw";

  fMinIntegral         = 100;   // minimal number of entries (excluding underflows, overflows) for fit attempt
  fMinContentMaxBin    = 10.0;  // minimal content of most populated bin for fit attempt
  fFittingRange        = 0.9;   // fitting range = [-0.9ns:+0.9ns], i.e. 9 bins of 0.2ns width
  fNFilesToAccumulate  = 20;    // for the T0 stability plots
  fHistoTimeLimit      = 30.0;  // time half-span of plotted histograms [ns]
  fSignalPeakWidth     = 1.0;   // exclusion region half-width for the spectrum shape check
  fIssueWarnings       = false; // check if the spectrum shape is OK?
  fPlotChannelTimes    = true;  // plot times in each channel?
  fPlotTimeDependences = true;  // check and plot the time stability of the T0 constants?

  // Initialization of the analyzer
  fEvaluateGlobalT0 = fEvaluateT0s = fUseChannelMap = true;
  fNChannels = fNChannelsActive = 0;
}

void T0Evaluation::InitHist() {

  if (!GetIsHisto() || GetIsTree()) {
    cout << "ERROR: T0Evaluation-based analyzers must be run in the --histo mode" << endl;
    exit(0);
  }

  if (fEvaluateGlobalT0) {
    fHRawTime = (TH1D*)RequestHistogram(fDirName, fRawTimeHistoName, true);
    if (!fHRawTime) {
      fEvaluateGlobalT0 = 0;
      cout << "Warning in "<<fAnalyzerName<<": histogram for global T0 evaluation (" <<
	fDirName << "/" << fRawTimeHistoName << ") not found" << endl;
    }
  }

  if (fEvaluateT0s) {
    fH2            = (TH2D*)RequestHistogram(fDirName, fTH2Name, false); // reset for each input file
    fH2_Integrated = (TH2D*)RequestHistogram(fDirName, fTH2Name, true);  // accumulated    
    if (!fH2) {
      fEvaluateT0s = 0;
      cout << "Warning in "<<fAnalyzerName<<": histogram for T0 evaluation (" << 
	fDirName << "/" << fTH2Name << ") not found" << endl;
    }
  }

  RequestUserHistograms();

  if (fEvaluateT0s) {
    fH2_Partial = new TH2D(*fH2); // for T0 stability vs time
    fH2_Partial->Reset();
    fNChannels = fH2->GetNbinsX();
    fBinWidth  = fH2->GetYaxis()->GetBinWidth(1);
  }

  for (int i=0; i<fNChannels; i++) {
    fBurstCounter[i]    = 0;
    fTimeBinsCounter[i] = 0;
    fChannelID[i]       = -99;
    fT0[i]              = -999.999;
    fWarning[i]         = false;
  }

  // Find and read the channel map
  if (fUseChannelMap) ParseConfFile();

  // Check which the RO channels are active
  fNChannelsActive = 0;
  for (int ich=0; ich<fNChannels; ich++) {
    if (!fUseChannelMap) fChannelID[ich] = 0;
    fIsActive[ich] = (fChannelID[ich]>=0);
    if (fIsActive[ich]) {
      ActiveChannelMap[fNChannelsActive++] = ich;
      TString Name = (fUseChannelMap) ? 
	Form("RO %04d GeomID %04d vs time", ich, fChannelID[ich]) :
	Form("RO %04d vs time", ich);
      fHT0VsTime[ich] = new TH1D(Name, Name, 5000, -0.5, 4999.5);
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
// Read the channel map from the configuration file

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
  if (!fPlotTimeDependences) return;
  fH2_Partial->Add(fH2);

  for (int ich=0; ich<fNChannels; ich++) {
    fBurstCounter[ich]++;

    if (fBurstCounter[ich] < fNFilesToAccumulate) continue;
    double Integral = fH2_Partial->Integral(ich+1, ich+1, 1, fH2_Partial->GetYaxis()->GetNbins());
    if (Integral<fMinIntegral) continue;
    EvaluateT0s(fH2_Partial, ich, false);
    if (fabs(fT0[ich])>999.0) continue;

    // A successful T0 fit is obtained: fill time dependence, clean histogram
    fHT0VsTime[ich]->SetBinContent(fTimeBinsCounter[ich]+1, fT0[ich]);
    fHT0VsTime[ich]->SetBinError  (fTimeBinsCounter[ich]+1, fDeltaT0[ich]);
    for (int i=1; i<=fH2_Partial->GetNbinsY(); i++) {
      fH2_Partial->SetBinContent(ich+1, i, 0);
      fH2_Partial->SetBinError  (ich+1, i, 0);
    }
    fBurstCounter[ich] = 0;
    fTimeBinsCounter[ich]++;
  }
}

void T0Evaluation::EndOfRunUser() {

  // Time stability of the T0s
  for (int ich=0; ich<fNChannels; ich++) {
    if (fIsActive[ich]) {
      fFChannelStability[ich] = new TF1("pol0", "pol0", -0.5, fTimeBinsCounter[ich]-0.5);
      if (fHT0VsTime[ich]->GetEntries()) {
	fHT0VsTime[ich]->Fit(fFChannelStability[ich], "0Q", "", -0.5, fTimeBinsCounter[ich]-0.5);
      }
    }
  }

  // Evaluate the T0s and global offset with the full data sample
  if (fEvaluateGlobalT0) EvaluateGlobalOffset();
  if (fEvaluateT0s) EvaluateT0s(fH2_Integrated, -1, fIssueWarnings); // -1: all channels

  // Generate and save the output
  if (fEvaluateT0s) {
    GenerateT0TextFile();
    GeneratePDFReport();
  }

  SaveAllPlots();
  for (int ich=0; ich<fNChannels; ich++) {
    if (fIsActive[ich]) {
      fHTime[ich]->Write();
      fHT0VsTime[ich]->Write();
    }
  }
}

void T0Evaluation::EvaluateT0s(TH2D *h2, int ChannelID, bool IssueWarnings) {
  // Evaluate T0 for all channels or single channel?
  int ch1 = 0;
  int ch2 = fNChannels-1;
  if (ChannelID>=0) ch1 = ch2 = ChannelID;

  for (int ich=ch1; ich<=ch2; ich++) {
    if (fIsActive[ich]) {
      TString Name = (fUseChannelMap) ? 
	Form("RO %04d GeomID %04d", ich, fChannelID[ich]) :
	Form("RO %04d", ich);
      if (fHTime[ich]) delete fHTime[ich];
      fHTime[ich] = h2->ProjectionY(Name, ich+1, ich+1);
      fHTime[ich]->SetTitle(Name);
      EvaluateChannelT0(ich, IssueWarnings);
    }
  }
}

///////////////////////////
// Evaluate T0 in a channel

void T0Evaluation::EvaluateChannelT0 (int ich, bool IssueWarning) {

  // Check if there are enough entries for the fit to converge
  if (fHTime[ich]->Integral()==0) return;
  fT0[ich] = +999.999;
  if (fHTime[ich]->Integral()<fMinIntegral) return;

  // Fitting interval: around the bin with max content
  Int_t maxbin  = fHTime[ich]->GetMaximumBin();
  Double_t c0   = fHTime[ich]->GetBinCenter(maxbin);
  Double_t cmin = c0 - fFittingRange;
  Double_t cmax = c0 + fFittingRange;
  Double_t maxc = fHTime[ich]->GetBinContent(maxbin);
  if (maxc < fMinContentMaxBin) return;

  if (!FitChannel(ich, c0, cmin, cmax, maxc)) return;

  fHisto.GetHisto("T0")->          SetBinContent(ich+1, fT0[ich]);
  fHisto.GetHisto("T0")->          SetBinError  (ich+1, fDeltaT0[ich]);
  fHisto.GetHisto("T0Resolution")->SetBinContent(ich+1, fResol[ich]);
  fHisto.GetHisto("T0Resolution")->SetBinError  (ich+1, fDeltaResol[ich]);

  /////////////////////////////////////////////////
  // Issue a warning in case there is a second peak

  if (IssueWarning && maxc>20.0) {
    int    nsidebandbins = 0;
    double background    = 0.0;
    int    highestbin    = 0;
    double highestcont   = 0.0;
    for (int ibin=1; ibin<=fHTime[ich]->GetNbinsX(); ibin++) {
      double t    = fHTime[ich]->GetBinCenter(ibin);
      double cont = fHTime[ich]->GetBinContent(ibin);
      if (fabs(t-fT0[ich]) < fSignalPeakWidth) continue;
      nsidebandbins++;
      background += cont;
      if (cont > highestcont) {
	highestbin  = ibin;
	highestcont = cont;
      }
    }
    if (nsidebandbins) background /= nsidebandbins;

    if (highestcont-background > 0.05*maxc && highestcont > 10.0*sqrt(background)) {
      fWarning[ich]       = true;
      fSecondPeakPos[ich] = fHTime[ich]->GetBinCenter(highestbin); 
    }
  }
}

//////////////////////////////////////////////////////////////////////////
// The fitting routine in a channel: can be overloaded in daughter classes

bool T0Evaluation::FitChannel(int ich, double c0, double cmin, double cmax, double maxc) {

  fFChannelFit[ich] = new TF1("gaus", "gaus", cmin, cmax);
  fFChannelFit[ich]->SetParameters(maxc, c0, 1.0);
  fHTime[ich]->Fit(fFChannelFit[ich], "R0Q");
  double T0         = fFChannelFit[ich]->GetParameter(1);
  double Resol      = fFChannelFit[ich]->GetParameter(2);
  double DeltaT0    = fFChannelFit[ich]->GetParError(1);
  double DeltaResol = fFChannelFit[ich]->GetParError(2);

  // If the first fit fails, make a second attempt with more degrees of freedom
  if (Resol>2.0 || DeltaT0>0.5 || DeltaResol>0.5) {

    delete fFChannelFit[ich];
    fFChannelFit[ich] = new TF1("GausPol", "gaus(0)+pol0(3)", cmin, cmax);
    fFChannelFit[ich]->SetParameters(maxc, c0, 1.0, 0.0);
    fHTime[ich]->Fit(fFChannelFit[ich], "R0Q");
    T0         = fFChannelFit[ich]->GetParameter(1);
    Resol      = fFChannelFit[ich]->GetParameter(2);
    DeltaT0    = fFChannelFit[ich]->GetParError(1);
    DeltaResol = fFChannelFit[ich]->GetParError(2);

    // Check if the second fit is successful
    if (Resol>2.0 || DeltaT0>0.5 || DeltaResol>0.5) return false;
  }

  fT0[ich]         = T0;
  fDeltaT0[ich]    = DeltaT0;
  fResol[ich]      = Resol;
  fDeltaResol[ich] = DeltaResol;
  return true;
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
  cout << fAnalyzerName << ": global T0 = " << fGlobalT0 << " ns " << endl;
  delete fHRawTime1;
}

/////////////////////
// Build a PDF report

void T0Evaluation::GeneratePDFReport() {

  if (!fGenerateOutputPDF || !fOutPDFFileName.Length()) {
    cout << fAnalyzerName<< ": report generation is not required" << endl;
    return;
  }

  cout << fAnalyzerName << ": generating report "<< fOutPDFFileName << endl;

  gStyle->SetOptStat("ei"); // print the number of entries and integral within drawing limits
  gStyle->SetOptFit();
  gErrorIgnoreLevel = 5000; // suppress messages generated for each page printed

  fFrontCanvas = new TCanvas("FrontCanvas");
  fFrontCanvas->Divide(1,3);
  for (int i=1; i<=3; i++) {
    fFrontCanvas->GetPad(i)->SetLeftMargin(0.05);
    fFrontCanvas->GetPad(i)->SetRightMargin(0.01);
    fFrontCanvas->GetPad(i)->SetTopMargin(0.01);
    fFrontCanvas->GetPad(i)->SetBottomMargin(0.10);
  }

  fCanvas = new TCanvas("Canvas");
  fCanvas->Divide(4,4);
  for (int i=1; i<=16; i++) {
    fCanvas->GetPad(i)->SetLeftMargin(0.07);
    fCanvas->GetPad(i)->SetRightMargin(0.01);
    fCanvas->GetPad(i)->SetTopMargin(0.01);
    fCanvas->GetPad(i)->SetBottomMargin(0.06);
  }

  fText = new TText();
  fText->SetTextAlign(kHAlignCenter+kVAlignCenter);
  fText->SetTextSize(0.15);
  fText->SetTextColor(kGreen+2);

  TLine *l = new TLine();

  ///////////////////////////
  // Summary plots (one page)

  fFrontCanvas->cd(1);
  fHisto.GetHisto("T0")->SetStats(0);
  fHisto.GetHisto("T0")->GetXaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0")->GetYaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0")->GetXaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0")->GetYaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0")->GetYaxis()->SetTitleOffset(0.5);
  fHisto.GetHisto("T0")->GetXaxis()->SetTitle("Readout channel ID");
  fHisto.GetHisto("T0")->GetYaxis()->SetTitle("T0 and its error [ns]");
  fHisto.GetHisto("T0")->SetLineColor(kBlue);
  fHisto.GetHisto("T0")->SetMarkerColor(kBlue);
  fHisto.GetHisto("T0")->Draw();

  fFrontCanvas->cd(2);
  fHisto.GetHisto("T0Resolution")->SetStats(0);
  fHisto.GetHisto("T0Resolution")->GetXaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetTitleSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetXaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetLabelSize(0.05);
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetTitleOffset(0.5);
  fHisto.GetHisto("T0Resolution")->GetXaxis()->SetTitle("Readout channel ID");
  fHisto.GetHisto("T0Resolution")->GetYaxis()->SetTitle("Peak width and its error [ns]");
  fHisto.GetHisto("T0Resolution")->SetLineColor(kBlue);
  fHisto.GetHisto("T0Resolution")->SetMarkerColor(kBlue);
  fHisto.GetHisto("T0Resolution")->Draw();

  fFrontCanvas->cd(3);
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
    fHRawTime->SetMarkerColor(kBlue);
    fHRawTime->SetLineWidth(1);
    fHRawTime->Draw();
    l->SetLineColor(kGreen+2);
    l->SetLineWidth(1);
    l->DrawLine(fGlobalT0, 0, fGlobalT0, fHRawTime->GetMaximum());
  }

  fFrontCanvas->Print(Form(fOutPDFFileName + "("), "pdf"); // open and print the front canvas

  ///////////////////////////////////////////
  // Time distribution plots (multiple pages)

  int Npages = fNChannelsActive/16;
  if (fNChannelsActive%16) Npages++;

  if (fPlotChannelTimes) {

    gStyle->SetOptStat("e"); // print the number of entries
    for (int ipage=0; ipage<Npages; ipage++) {
      for (int i=0; i<16; i++) {
	fCanvas->GetPad(i+1)->Clear();
	int ichActive = ipage*16 + i;
	if (ichActive>=fNChannelsActive) continue;
	int ich = ActiveChannelMap[ichActive];
	fCanvas->cd(i+1);
	Int_t Integral = fHTime[ich]->Integral(); // the integral is later changed by SetRangeUser()
	Int_t maxbin   = fHTime[ich]->GetMaximumBin();
	Double_t c0    = fHTime[ich]->GetBinContent(maxbin);
	if (c0<1) c0 = 1;
	fHTime[ich]->SetMaximum(1.1*c0);
	fHTime[ich]->GetXaxis()->SetRangeUser(-fHistoTimeLimit, +fHistoTimeLimit);
	fHTime[ich]->GetXaxis()->SetLabelSize(0.07);
	fHTime[ich]->GetYaxis()->SetLabelSize(0.055);
	fHTime[ich]->GetXaxis()->SetTitle("");
	fHTime[ich]->SetLineWidth(1);
	fHTime[ich]->Draw();
	
	if (fFChannelFit[ich]) {
	  fFChannelFit[ich]->SetLineWidth(1);
	  fFChannelFit[ich]->Draw("same");
	}

	// Draw line at the position of the second highest peak
	if (fWarning[ich]) {
	  l->SetLineColor(kGreen+2);
	  l->SetLineWidth(1);
	  l->DrawLine(fSecondPeakPos[ich], 0., fSecondPeakPos[ich], fHTime[ich]->GetMaximum());
	  fText->DrawText(0, 0.55*c0, "WARNING");
	}
	else if (Integral==0) {
	  fText->DrawText(0, 0.55*c0, "EMPTY");
	}
	else if (Integral<fMinIntegral) {
	  fText->DrawText(0, 0.55*c0, "FEW ENTRIES");
	}
	else if (fT0[ich]>999.0) {
	  fText->DrawText(0, 0.55*c0, "FIT FAILED");
	}
      }
      fCanvas->Print(fOutPDFFileName, "pdf");
    }
  }
  else {
    cout << fAnalyzerName<< ": channel time plots not requested" << endl;
  }

  ////////////////////////////////////////
  // Time stability plots (multiple pages)

  if (fPlotTimeDependences) {

    for (int ipage=0; ipage<Npages; ipage++) {
      for (int i=0; i<16; i++) {
	fCanvas->GetPad(i+1)->Clear();
	int ichActive = ipage*16 + i;
	if (ichActive>=fNChannelsActive) continue;
	int ich = ActiveChannelMap[ichActive];
	fCanvas->cd(i+1);

	double ymin = 999, ymax = -999;
	for (int i=1; i<=fHT0VsTime[ich]->GetNbinsX(); i++) {
	  double c = fHT0VsTime[ich]->GetBinContent(i);
	  double e = fHT0VsTime[ich]->GetBinError(i);
	  if (c) {
	    if (ymin > c-e) ymin = c-e;
	    if (ymax < c+e) ymax = c+e;
	  }
	}
	if (ymin>ymax) { ymin = -1; ymax = +1; }

	fHT0VsTime[ich]->GetXaxis()->SetRangeUser(-0.5, fTimeBinsCounter[ich]-0.5);
	fHT0VsTime[ich]->SetMinimum(ymin);
	fHT0VsTime[ich]->SetMaximum(ymax);
	fHT0VsTime[ich]->SetLineWidth(1);
	fHT0VsTime[ich]->SetLineColor(kBlue);
	fHT0VsTime[ich]->SetMarkerColor(kBlue);
	fFChannelStability[ich]->SetLineColor(kRed);
	fFChannelStability[ich]->SetLineWidth(1);
	fHT0VsTime[ich]->Draw();
	if (fHT0VsTime[ich]->GetEntries()) fFChannelStability[ich]->Draw("same");
      }
      fCanvas->Print(fOutPDFFileName, "pdf");
    }
  }
  else if (!fPlotTimeDependences) {
    cout << fAnalyzerName<< ": T0 stability plots not requested" << endl;
  }
  else {
    cout << fAnalyzerName<< ": T0 stability plots not produced (too few input files)" << endl;
  }

  GenerateUserPDFReport(); // add subdetector-specific pages

  fCanvas->Print(Form(fOutPDFFileName + "]"), "pdf"); // close file
  delete fFrontCanvas;
  delete fCanvas;
  gErrorIgnoreLevel = -1; // restore the default
}

////////////////////////////////
// Build an output file with T0s

void T0Evaluation::GenerateT0TextFile() {
  time_t now = time(0);
  ofstream outfile (fOutTextFileName);
  outfile << "# "<<fDetectorName<<" T0 constants. Format: RO channel; geometric ID; T0 offset (ns)."<<endl;
  outfile << "# These T0 offsets should be subtracted from the raw times."<<endl;
  outfile << "# Special values: -999.999 for masked channels, +999.999 for failed T0 fits."<<endl;
  outfile << "# An offset T0 is ignored by the reconstruction in case |T0|>999ns."<<endl;
  outfile << "#\n# Generated on "<<asctime(localtime(&now));
  outfile << "#"<<endl;
  for (int i=0; i<fNChannels; i++) {
    if (fIsActive[i]) outfile << Form("%4d %4d %7.3f\n", i, fChannelID[i], fT0[i]);
  }
  outfile.close();
}
