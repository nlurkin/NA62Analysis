// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2015-04-10
// Developed by Dmitry Madigozhin (madigo@mail.cern.ch) 2015-05-31
//
// ---------------------------------------------------------------

#include <iostream>
#include <fstream>
#include "SpectrometerT0.hh"
#include "StrawAddress.hh"
#include "StrawPlace.hh"

using namespace std;

SpectrometerT0::SpectrometerT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "Spectrometer") {

  fMinIntegral         = 100;   // minimal number of entries (excluding underflows, overflows) for fit attempt
  fFittingRange        = 50.0;   // fitting range  

  fNFilesToAccumulate  = 20; // for the T0 stability plots
  fHistoTimeLimit      = 100.0;  // time half-span of plotted histograms [ns]
  fSignalPeakWidth     = 10.0;   // exclusion region half-width for the spectrum shape check
  fIssueWarnings       = false; // check if the spectrum shape is OK?
  fPlotTimeDependences = false;  // check the time stability of the T0 constants?

  // Initialization of the analyzer
  fEvaluateGlobalT0 = false;
  fEvaluateT0s = true; 

  fLevelFactor = sqrt(2.0*log(1000.0));   //  for 1/1000 level of the peak value 
  fMinContentMaxBin = 30.0;               //  minimum in the peak bin

  // Create the map

  fUseChannelMap = true;
  fConfFileName  = "./SpectrometerAnalysis.conf";

  ifstream fs;
  fs.open(fConfFileName.Data(),ios::in);
  if(fs.is_open()){
     fs.close();
     cout << "Configuration file exists yet" << endl;
     return;
  }

  ofstream fout;
  fout.open(fConfFileName.Data(),ios::out);
  fout.setf(ios::fixed);
  fout.setf(ios::right);
  fout.precision(0);
  fout << "## ReadOut (RO) index -> Geometrical position index" << endl;
  fout << "## RO index : ChRemap_(ichamber*8*16 + isrb*16 + icover)= 16 geometrical positions in the order of RO channels " << endl;
  fout << "## Geometrical position : (16*ichamber + 4*iview + 2*ihalf + ilayer)*122 + istraw" << endl;
  fout << endl;

  int icount=0;  

  for(int ic=0;ic<4;ic++){
    for(int isrb=0; isrb<8; isrb++){
      for(int icover=0; icover<16; icover++){
	fout <<"ChRemap_" << icount << "=";
	if(icount < 10)fout << " ";
	if(icount < 100)fout << " ";
	icount++;
	for(int ichannel=0; ichannel<16; ichannel++){
	  StrawPlace splace(ic*8+isrb, icover, ichannel);
	  fout.width(6);
	  if(splace.ichamber < 0){
	    fout << -1;
	  }else{
	    if(splace.NoStraw){
	      fout << -2;
	    }else{
	      fout << (16*splace.ichamber + 4*splace.iview + 2*splace.ihalf + splace.ilayer)*122 + splace.istraw;
	    }
	  }
	}
	fout << endl;
      }
    }
  }
  fout.close();

  return;
}

Double_t fitf(Double_t *v, Double_t *par)
{
  Double_t sigma;
   Double_t fitval;
   Double_t arg = 0;

   sigma = par[2]+(v[0] - par[1])*par[3];

   if (sigma > 0.0){
     arg = (v[0] - par[1])/sigma;
     fitval = par[0]*TMath::Exp(-0.5*arg*arg);
   }else{
     fitval=0.0;
   }

   return fitval + par[4];
}

void SpectrometerT0::EvaluateChannelT0 (int ich, bool IssueWarning) {

  // Check if there are enough entries for the fit to converge (doubled check)
  if (fHTime[ich]->Integral()==0) return;
  fT0[ich] = +999.999;
  if (fHTime[ich]->Integral()<fMinIntegral) return;

  // Fitting interval: around the bin with max content
  Int_t maxbin  = fHTime[ich]->GetMaximumBin();
  Double_t c0   = fHTime[ich]->GetBinCenter(maxbin);
  Double_t maxc = fHTime[ich]->GetBinContent(maxbin);

  if(maxc < fMinContentMaxBin)return;

  Double_t cmin = c0 - fFittingRange;
  Double_t cmax = c0 + fFittingRange;

  fFChannelFit[ich] = new TF1("fitf", fitf, cmin, cmax,5);

  fFChannelFit[ich]->SetParameters(maxc,c0,12.0,0.43,0.0);

  fFChannelFit[ich]->SetParLimits(0,maxc*0.5,maxc*2.0);
  fFChannelFit[ich]->SetParLimits(1,c0-20.0,c0+20.0);
  fFChannelFit[ich]->SetParLimits(2,0.0,50.0);
  fFChannelFit[ich]->SetParLimits(3,0.0,10.0);
  fFChannelFit[ich]->SetParLimits(4,0.0,maxc);

  fHTime[ich]->Fit(fFChannelFit[ich], "RQB");

  double PeakPosition = fFChannelFit[ich]->GetParameter(1);
  double s0 = fFChannelFit[ich]->GetParameter(2);
  double ss = fFChannelFit[ich]->GetParameter(3);

  double dt = fabs(fLevelFactor*s0/(1.0+fLevelFactor*ss));
  double dt0 = fabs(s0/ss);
  if(dt > dt0)dt=dt0; // just for the case, it should never happen...

  double T0         = PeakPosition - dt;
  double Resol      = s0;                                                    // first term of resolution
  double DeltaT0    = fFChannelFit[ich]->GetParError(1);  // approximately
  double DeltaResol = fFChannelFit[ich]->GetParError(2);

  // Check if the fit is successful
  if (Resol>20.0 || DeltaT0>5.0 || DeltaResol>10.0) return;

  fT0[ich]      = T0;
  fDeltaT0[ich] = DeltaT0;

  fHisto.GetHisto("T0")->          SetBinContent(ich+1, T0);
  fHisto.GetHisto("T0")->          SetBinError  (ich+1, DeltaT0);
  fHisto.GetHisto("T0Resolution")->SetBinContent(ich+1, Resol);
  fHisto.GetHisto("T0Resolution")->SetBinError  (ich+1, DeltaResol);

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
  } // if (IssueWarning && maxc>20.0)

  return;
}

