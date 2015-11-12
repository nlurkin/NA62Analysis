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
  fMinContentMaxBin    = 30.0;  // minimal content of most populated bin for fit attempt
  fFittingRange        = 50.0;  // fitting range [ns]
  fNFilesToAccumulate  = 20;    // for the T0 stability plots
  fHistoTimeLimit      = 120.0; // time half-span of plotted histograms [ns]
  fSignalPeakWidth     = 999;   // exclusion region half-width for the spectrum shape check
  fIssueWarnings       = false; // check if the spectrum shape is OK?
  fPlotChannelTimes    = false; // do not plot channel times (they are too many)
  fPlotTimeDependences = false; // do not plot time stability of T0s im channels (they are too many)

  // Create the map if required
  BuildConfFile();
}

////////////////////////////////////////////////////////////////
// If the configuration file does not exist, build it on the fly

void SpectrometerT0::BuildConfFile() {

  ifstream fs;
  fs.open(fConfFileName.Data(),ios::in);
  if (fs.is_open()) {
    fs.close();
    return;
  }

  cout << fAnalyzerName<< ": no configuration file, building the mapping table" << endl;
  ofstream fout;
  fout.open(fConfFileName.Data(),ios::out);
  fout.setf(ios::fixed);
  fout.setf(ios::right);
  fout.precision(0);
  fout << "## ReadOut (RO) index -> Geometrical position index" << endl;
  fout << "## RO index : ChRemap_(ichamber*8*16 + isrb*16 + icover)= 16 geometrical positions in the order of RO channels " << endl;
  fout << "## Geometrical position : (16*ichamber + 4*iview + 2*ihalf + ilayer)*122 + istraw" << endl;
  fout << endl;

  int icount = 0;
  for (int ic=0; ic<4; ic++) {
    for (int isrb=0; isrb<8; isrb++) {
      for (int icover=0; icover<16; icover++) {
	fout <<"ChRemap_" << icount << "=";
	if (icount <  10) fout << " ";
	if (icount < 100) fout << " ";
	icount++;
	for (int ichannel=0; ichannel<16; ichannel++) {
	  StrawPlace splace(ic*8+isrb, icover, ichannel);
	  fout.width(6);
	  if (splace.ichamber < 0) {
	    fout << -1;
	  }
	  else if (splace.NoStraw) {
	      fout << -2;
	    }
	  else {
	    fout << (16*splace.ichamber + 4*splace.iview + 2*splace.ihalf + splace.ilayer)*122 + splace.istraw;
	  }
	}
	fout << endl;
      }
    }
  }
  fout.close();
  return;
}

////////////////////////////
// A custom fitting function

Double_t SpecT0FitFunction(Double_t *v, Double_t *par) {
  Double_t fitval;
  Double_t sigma = par[2]+(v[0] - par[1])*par[3];
  if (sigma > 0.0) {
    Double_t arg = (v[0] - par[1])/sigma;
    fitval = par[0]*TMath::Exp(-0.5*arg*arg);
  }
  else {
    fitval = 0.0;
  }
  return fitval + par[4];
}

//////////////////////////////////////////////////////
// Non-standard time response: a custom fitting method

bool SpectrometerT0::FitChannel(int ich, double c0, double cmin, double cmax, double maxc) {

  fFChannelFit[ich] = new TF1("fitf", SpecT0FitFunction, cmin, cmax, 5);
  fFChannelFit[ich]->SetParameters(maxc,c0,12.0,0.43,0.0);
  fFChannelFit[ich]->SetParLimits(0, maxc*0.5, maxc*2.0);
  fFChannelFit[ich]->SetParLimits(1, c0-20.0, c0+20.0);
  fFChannelFit[ich]->SetParLimits(2, 0.0, 50.0);
  fFChannelFit[ich]->SetParLimits(3, 0.0, 10.0);
  fFChannelFit[ich]->SetParLimits(4, 0.0, maxc);

  fHTime[ich]->Fit(fFChannelFit[ich], "RB0Q");

  // A factor defining the level when distribution become "visible" 
  double LevelFactor = sqrt(2.0*log(1000.0)); // for 1/1000 level of the peak value

  double PeakPosition = fFChannelFit[ich]->GetParameter(1);
  double s0 = fFChannelFit[ich]->GetParameter(2);
  double ss = fFChannelFit[ich]->GetParameter(3);
  double dt = fabs(LevelFactor*s0/(1.0+LevelFactor*ss));
  double dt0 = fabs(s0/ss);
  if (dt>dt0) dt=dt0; // just in case, it should never happen...

  double T0         = PeakPosition - dt;
  double Resol      = s0;                                 // first term of resolution
  double DeltaT0    = fFChannelFit[ich]->GetParError(1);  // approximately
  double DeltaResol = fFChannelFit[ich]->GetParError(2);

  // Check if the fit is successful
  if (Resol>20.0 || DeltaT0>5.0 || DeltaResol>10.0) return false;

  fT0[ich]         = T0;
  fDeltaT0[ich]    = DeltaT0;
  fResol[ich]      = Resol;
  fDeltaResol[ich] = DeltaResol;
  return true;
}
