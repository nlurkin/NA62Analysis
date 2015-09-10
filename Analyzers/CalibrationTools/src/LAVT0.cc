#include "LAVT0.hh"

LAVT0::LAVT0(Core::BaseAnalysis *ba) : T0Evaluation(ba, "LAV") {
  for (Int_t i=0; i<2; i++) fDeltaLAV[i] = NULL;
  fSlewingFound = true;
  fFunctions = new LAVFitFunctions();
  fNFilesToAccumulate  = 20;    // for the T0 stability plots
  fSignalPeakWidth     = 3.0;   // exclusion region half-width for the spectrum shape check
  fFittingRange        = 2.5;   // fitting range = [-3ns:+3ns], i.e. 30 bins of 0.2ns width
  fMaxResol            = 10.0;  // very loose fit quality checks
  fMaxDeltaT0          = 1.5;
  fMaxDeltaResol       = 999;
  fPlotTimeDependences = false; // check the time stability of the T0 constants?
}

void LAVT0::GenerateUserPDFReport(){
  if (!fSlewingFound) {
    cout << fAnalyzerName << ": cannot find one of the input histograms " << fDeltaLAV[0] << " " << fDeltaLAV[1] << endl;
    return;
  }

  fCanvas->Clear();
  fCanvas->Divide(1,2);
  evaluateGlobalTZeroFit(0,1.5,5,1,1); // low threshold
  fCanvas->Update();
  evaluateGlobalTZeroFit(1,2.,6,2,1); // high threshold
  fCanvas->Update();
  fCanvas->Print(fOutPDFFileName, "pdf");
}

void LAVT0::RequestUserHistograms(){
  TString referenceName[2];
  referenceName[0] = "RecoHitdTimeLowWrtTOTLow";
  referenceName[1] = "RecoHitdTimeHighWrtdTLead";
  for (Int_t i=0; i<2; i++) {
    fDeltaLAV[i] = (TH2D*)RequestHistogram(fDirName, referenceName[i], true);
    if (!fDeltaLAV[i]) fSlewingFound = false;
  }
}

void LAVT0::evaluateGlobalTZeroFit(Int_t referenceTime = 1, Double_t sigmaTime = 2., Int_t nSigma = 6, Int_t rebinX = 2, Int_t rebinY = 2){

  TString referenceName;
  Double_t xRangeMin;
  Double_t xRangeMax;
  Double_t yRangeMin;
  Double_t yRangeMax;
  if (referenceTime == 0) {
    referenceName = "RecoHitdTimeLowWrtTOTLow";//"dTimeLowtotLowLow";//"dTimeLow";
    xRangeMin = 0.;
    xRangeMax = 100.;
    yRangeMin = -15.;
    yRangeMax =  15.;
  }
  else if (referenceTime == 1) {
    referenceName = "RecoHitdTimeHighWrtdTLead";
    xRangeMin = 0.;
    xRangeMax = 10.;
    yRangeMin = -15.;
    yRangeMax =  15.;
  }
  else {
    cout << fAnalyzerName << "wrong reference time in input: 0/1 for LowFromToT/HighFromSlope" << endl;
    return;
  }

  TGraphErrors* grafo = NULL;

  Double_t* Constant = new Double_t[1000];
  Double_t* ConstantErr = new Double_t[1000];
  Double_t* Coeff1  = new Double_t[1000];
  Double_t* Coeff1Err = new Double_t[1000];
  Double_t* Coeff2 = new Double_t[1000]; 
  Double_t* Coeff2Err  = new Double_t[1000];
  Double_t* Coeff3 = new Double_t[1000];
  Double_t* Coeff3Err = new Double_t[1000];
  Double_t* ChiSquare = new Double_t[1000];
  
  grafo = correctSlewing(referenceTime, &fDeltaLAV[referenceTime], 100, sigmaTime, nSigma, rebinX, rebinY,
  	   Constant, ConstantErr,
	   	     		   Coeff1, Coeff1Err,
				   	   	       Coeff2, Coeff2Err,
						       	       		   Coeff3, Coeff3Err,
									   	   	       ChiSquare);
  fCanvas->cd(1+referenceTime);
  fDeltaLAV[referenceTime]->GetXaxis()->SetRangeUser(xRangeMin,xRangeMax);
  fDeltaLAV[referenceTime]->GetYaxis()->SetRangeUser(yRangeMin,yRangeMax);
  fDeltaLAV[referenceTime]->Draw("zcol");

  cout << fAnalyzerName << ": plot of slewing correction " << grafo;
  if (grafo) {cout << " with " << grafo->GetN() << " points " << endl;}
  else { cout << " with 0 points -- plot will not be made " << endl;}

  if (grafo) {
    grafo->SetMarkerColor(2);
    grafo->SetMarkerSize(0.4);
    grafo->Draw("Psame");
  }
}

TGraphErrors* LAVT0::correctSlewing(Int_t channel, TH2D** DeltaLAV, Int_t eventLimit, Double_t sigmaTime, Int_t nSigma, Int_t rebinX, Int_t rebinY,
				    Double_t*  constant, Double_t*  constantErr,
				    Double_t*  coeff1, Double_t*  coeff1Err, 
				    Double_t*  coeff2, Double_t*  coeff2Err, 
				    Double_t*  coeff3, Double_t*  coeff3Err, 
				    Double_t*  chi){
  // channel 0 --> low  threshold
  // channel 1 --> high threshold
  // use "dTimeLow" to correction TstartFromTOT vs TOT 
  // use "dTimeHighdtLead" for correction to TStartFromSlope from dTLead

  TH2D* dtlav = *DeltaLAV;

  TH2D* newo = (TH2D*) dtlav->Rebin2D(rebinX,rebinY);
  if (newo->GetEntries() < eventLimit) return NULL;

  Double_t xMin = newo->GetXaxis()->GetXmin();
  Int_t nBins = newo->GetXaxis()->GetNbins();

  Int_t NChannels = nBins;
  Double_t xWid = (newo->GetXaxis()->GetXmax()-xMin)/NChannels;

  Int_t n_points = 0;
  Double_t* Tav = new Double_t[NChannels];
  Double_t* eTav = new Double_t[NChannels];
  Double_t* Xt = new Double_t[NChannels];
  Double_t* eXt = new Double_t[NChannels];
  Double_t* St = new Double_t[NChannels];
  Double_t* eSt = new Double_t[NChannels];
  Double_t* chiS = new Double_t[NChannels];

  TF1* func = NULL;

  for (Int_t i=0; i<NChannels; i++) {
    TH1D* proj = newo->ProjectionY(Form("LAVCh%d",i),i+1,i+1);

    Double_t maxPopulationCh = proj->GetMaximum();
    Int_t maxBinCh = proj->GetMaximumBin();
    Double_t maxTimeCh = proj->GetBinCenter(maxBinCh);
    Double_t windowTMin = maxTimeCh - nSigma*sigmaTime;
    Double_t windowTMax = maxTimeCh + nSigma*sigmaTime;
    Int_t nLow = (windowTMin - xMin)/xWid-0.5;
    if (nLow < 0) nLow = 0;
    Int_t nHigh= (windowTMax - xMin)/xWid+0.5;
    if (nHigh > nBins) nHigh = nBins;
    windowTMin = xMin + nLow*xWid;
    windowTMax = xMin + nHigh*xWid;

    //    cout << " maxPopulation " << maxPopulationCh << " range (" << windowTMin << "," << windowTMax << ") for channel " << channel << endl;
    if (maxPopulationCh > 10) {
      if (func) delete func;
      //      func = new TF1("fitFunction",fitf,windowTMin,windowTMax,3);
      func = new TF1("fitFunction",fFunctions,&LAVFitFunctions::EvaluateFitf,windowTMin,windowTMax,3,"LAVFitFunctions","EvaluateFitf");

      func->SetParameters(maxPopulationCh,maxTimeCh,sigmaTime);
      func->SetParNames("Constant","Mean_value","Sigma");

      Double_t fitTMin = maxTimeCh - sigmaTime*(nSigma-2);
      Double_t fitTMax = maxTimeCh + sigmaTime*(nSigma-2);

      TCanvas* canvaTmp = new TCanvas();
      proj->Draw("");
      proj->Fit("fitFunction","L","same",fitTMin,fitTMax);

      Tav[n_points] = xMin + xWid*(i+0.5);
      eTav[n_points] = xWid*0.5;
      Xt[n_points] = func->GetParameter(1);
      eXt[n_points] = func->GetParError(1);
      St[n_points] = TMath::Abs(func->GetParameter(2));
      eSt[n_points] = func->GetParError(2);
      chiS[n_points] = func->GetChisquare()/func->GetNDF();
      delete canvaTmp;

      n_points++;
    }
    else if (maxPopulationCh > 5) {
      //      proj->GetXaxis()->SetRangeUser(windowTMin,windowTMax); 
      Tav[n_points] = xMin + xWid*(i+0.5);
      eTav[n_points] = xWid*0.5;
      Xt[n_points] = proj->GetMean();
      eXt[n_points] = proj->GetMeanError();
      St[n_points] = TMath::Abs(proj->GetRMS());
      eSt[n_points] = proj->GetRMSError();
      chiS[n_points] = -1.;
      n_points++;
    }
  }
//  cout << "Fit " << n_points << " points " << endl;
//  for (int i=0; i<n_points; i++) {
//    cout << " x = " << Tav[i] << " dx " << eTav[i] << " y = " << Xt[i] << " dy = " << eXt[i] << " sigma = " << St[i] << " es = " << eSt[i] << " chi = " << chi[i] << endl;
//  }

  if (n_points == 0) return NULL;

  TGraphErrors *gr_Mean = new TGraphErrors(n_points, Tav, Xt, eTav, eXt);
  //  TGraphErrors *gr_Sigma = new TGraphErrors(n_points, Tav, St,  eTav, eSt);
  //  TGraphErrors *gr_Chi = new TGraphErrors(n_points, Tav, chi, eTav, 0);
  //  gPad->SetGrid();
  gr_Mean->SetMarkerColor(2);
  gr_Mean->SetMarkerStyle(21);
  Double_t xlim1 = newo->GetXaxis()->GetXmin();
  Double_t xlim2 = newo->GetXaxis()->GetXmax();
  Double_t ylim1 = newo->GetYaxis()->GetXmin();
  Double_t ylim2 = newo->GetYaxis()->GetXmax();

  //  cout << " x limits " << xlim1 << " " << xlim2 << " y limits " << ylim1 << " " << ylim2 << " " << ymax << " " << xOfYmax << endl;

  gr_Mean->GetXaxis()->SetRangeUser(xlim1,xlim2);  
  gr_Mean->GetYaxis()->SetRangeUser(ylim1,ylim2);
  if (n_points > 5) {

    //    TF1* func2 = new TF1("fitFunction2",fitfTimeCorr,Tav[0]-0.5*xWid,Tav[n_points-1]+0.5*xWid,3);
    //    func2->SetParameters(-3.2,2.7,-0.04);
    //    func2->SetParNames("Constant","Slope","Curvature");

    //    TF1* func2 = new TF1("fitFunction2",fitfTimeCorr,Tav[0]-0.5*xWid,Tav[n_points-1]+0.5*xWid,6);
    TF1* func2 = new TF1("fitFunction2",fFunctions,&LAVFitFunctions::EvaluateResidualSlewing,Tav[0]-0.5*xWid,Tav[n_points-1]+0.5*xWid,6,"LAVFitFunctions","EvaluateResidualSlewing"); 

    Int_t nptsLimit = 0;
    Int_t nptsMin = 0;
    if (channel == 0) { // low threshold
      //      func2->SetParNames("xBreak","Const","Lin","Tau","Quart","Channel");
      //      func2->FixParameter(0,xOfYmax);
      //      func2->FixParameter(1,ymax);
      //      func2->SetParameters(xOfYmax,ymax,0.5,2.,-1,(double) channel);

      func2->SetParameters(5.,-1,2,3,0,(double) channel);
      func2->SetParNames("Const","Lin","Quad","Cub","Quart","Channel");
      func2->FixParameter(4,0.);
      func2->FixParameter(5,channel);
      nptsLimit = (45. - xMin)/xWid;
      nptsMin = (4.-xMin)/xWid;
    }
    else { // high threshold
      func2->SetParNames("Const","Lin","Quad","Cub","Quart","Channel");
      func2->SetParameters(1.,0.,-0.5,0.,-1,1.);
      func2->FixParameter(5,1.);
      nptsLimit = n_points-1;
    }

    gr_Mean->Fit("fitFunction2","","0",Tav[nptsMin],Tav[nptsLimit]+1);
    gr_Mean->Fit("fitFunction2","E","0",Tav[nptsMin],Tav[nptsLimit]+1);
    *constant    = func2->GetParameter(1);
    *constantErr = func2->GetParError(1);
    *coeff1      = func2->GetParameter(2);
    *coeff1Err   = func2->GetParError(2);
    *coeff2      = func2->GetParameter(3);
    *coeff2Err   = func2->GetParError(3);
    *chi         = func2->GetChisquare();
    //    canva->Update();
  }
  delete[] Tav  ;
  delete[] eTav ;
  delete[] Xt   ;
  delete[] eXt  ;
  delete[] St   ;
  delete[] eSt  ;
  delete[] chiS ;

  return gr_Mean;
  //  gr_Mean->Draw("AP");

  //  DeltaLAV->Draw("box");
  //
  //  canva->Update();

  //  graphi->cd(2);
  //  gPad->SetGrid();
  //  gr_Sigma->SetMarkerColor(3);
  //  gr_Sigma->SetMarkerStyle(21);
  //  gr_Sigma->GetYaxis()->SetRangeUser(-20., 20.);
  //  gr_Sigma->Draw("AP");
  //  graphi->Update();
  //  graphi->cd(3);
  //  gPad->SetGrid();
  //  gr_Chi->SetMarkerColor(2);
  //  gr_Chi->SetMarkerStyle(21);
  //  gr_Chi->GetYaxis()->SetRangeUser(-20., 20.);
  //  gr_Chi->Draw("AP");
  //  graphi->Update();
}


Double_t LAVFitFunctions::EvaluateFitf(Double_t *v, Double_t *par){
  Double_t arg = 0;
  if (par[2] != 0) arg = (v[0] - par[1])/par[2];
  Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg);
  return fitval;
}

Double_t LAVFitFunctions::EvaluateResidualSlewing(Double_t *v, Double_t *par){
  if (par[5] == 1) { // high threshold fit
    Double_t tRange = 4.;
    Double_t x = v[0]/tRange-1.;
    Double_t fitval = par[0] + par[1]*x + par[2]*x*x + par[3]*x*x*x + par[4]*x*x*x*x;
    return fitval;
  }
  else { // low threshold
    Double_t tRange = 20.;
    Double_t x = v[0]/tRange-1.;
    Double_t fitval = par[0] + par[1]*x + par[2]*x*x + par[3]*x*x*x + par[4]*x*x*x*x;
    return fitval;
    //    Double_t x = v[0]-par[0];
    //    Double_t fitval;
    //    if (x < 0) {
    //      fitval = par[1] + par[2]*x;
    //    }
    //    else {
    //      fitval = par[1]*TMath::Exp(-x/TMath::Abs(par[3]));
    //    }
    //    return fitval;
  }
}
