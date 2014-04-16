/*
 * HistoHandler.cc
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#include <iostream>
#include <TFile.h>
#include "HistoHandler.hh"
#include "StringTable.hh"
#include "functions.hh"
#include <TGraphQQ.h>
#include <TF1.h>

HistoHandler::HistoHandler():
			fUpdateRate(10)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

HistoHandler::HistoHandler(const HistoHandler& c):
			fHisto(c.fHisto),
			fHisto2(c.fHisto2),
			fGraph(c.fGraph),
			fPoint(c.fPoint),
			fCanvas(c.fCanvas),
			fOutTree(c.fOutTree),
			fHistoOrder(c.fHistoOrder),
			fAutoUpdateList(c.fAutoUpdateList),
			fPlotsDirectory(c.fPlotsDirectory),
			fUpdateRate(c.fUpdateRate)
{
	/// \MemberDescr
	/// Copy constructor
	/// \EndMemberDescr
}

HistoHandler::~HistoHandler() {
	/// \MemberDescr
	/// Destructor. Delete all the histograms and canvases.
	/// \EndMemberDescr

	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator it3;
	vector<TCanvas*>::iterator it4;

	for(it1=fHisto.begin(); it1!=fHisto.end(); it1++){
		delete it1->second;
	}
	for(it2=fHisto2.begin(); it2!=fHisto2.end(); it2++){
		delete it2->second;
	}
	for(it3=fGraph.begin(); it3!=fGraph.end(); it3++){
		delete it3->second;
	}
	for(it4=fCanvas.begin(); it4!=fCanvas.end(); it4++){
		delete (*it4);
	}
}

void HistoHandler::BookHisto(TString name, TH1* const histo, TString analyzerName, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param analyzerName : Name of the analyzer calling the method
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHistoOrder.push_back(name);
	fHisto.insert(pair<TString,TH1*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name, analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
}

void HistoHandler::BookHisto(TString name, TH2* histo, TString analyzerName, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param analyzerName : Name of the analyzer calling the method
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHistoOrder.push_back(name);
	fHisto2.insert(pair<TString,TH2*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name, analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
}

void HistoHandler::BookHisto(TString name, TGraph* histo, TString analyzerName, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param analyzerName : Name of the analyzer calling the method
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHistoOrder.push_back(name);
	fGraph.insert(pair<TString,TGraph*>(name, histo));
	fGraph[name]->SetNameTitle(name, name);
	//fPoint.insert(pair<TString,int>(name,0));
	if(refresh) SetPlotAutoUpdate(name, analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
}

void HistoHandler::BookHistoArray(TString baseName, TH1* histo, int number, TString analyzerName, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
	/// \param analyzerName : Name of the analyzer calling the method
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	string name, title;
	name = histo->GetName();
	title = histo->GetTitle();
	TH1* h;

	histo->SetName(TString(name + "0").Data());
	histo->SetTitle(TString(title + "0").Data());
	fHistoOrder.push_back(baseName+"0");
	fHisto.insert(pair<TString,TH1*>(baseName + "0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH1*)histo->Clone();
		h->SetName(TString(name + (Long_t)i).Data());
		h->SetTitle(TString(title + (Long_t)i).Data());
		fHistoOrder.push_back(baseName + (Long_t)i);
		fHisto.insert(pair<TString,TH1*>(baseName + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + (Long_t)i, directory.Strip(TString::kBoth, '/')));
	}
}

void HistoHandler::BookHistoArray(TString baseName, TH2* histo, int number, TString analyzerName, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
	/// \param analyzerName : Name of the analyzer calling the method
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	string name, title;
	name = histo->GetName();
	title = histo->GetTitle();
	TH2* h;

	histo->SetName(TString(name + "0").Data());
	histo->SetTitle(TString(title + "0").Data());
	fHistoOrder.push_back(baseName + "0");
	fHisto2.insert(pair<TString,TH2*>(baseName + "0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH2*)histo->Clone();
		h->SetName(TString(name + (Long_t)i).Data());
		h->SetTitle(TString(title + (Long_t)i).Data());
		fHistoOrder.push_back(baseName + (Long_t)i);
		fHisto2.insert(pair<TString,TH2*>(baseName + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + (Long_t)i, directory.Strip(TString::kBoth, '/')));
	}
}

void HistoHandler::BookHistoArray(TString baseName, TGraph* histo, int number, TString analyzerName, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
	/// \param analyzerName : Name of the analyzer calling the method
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	string name, title;
	name = histo->GetName();
	title = histo->GetTitle();
	TGraph* h;

	histo->SetName(TString(name + "0").Data());
	histo->SetTitle(TString(title + "0").Data());
	fHistoOrder.push_back(baseName+ "0");
	fGraph.insert(pair<TString,TGraph*>(baseName + "0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TGraph*)histo->Clone();
		h->SetName(TString(name + (Long_t)i).Data());
		h->SetTitle(TString(title + (Long_t)i).Data());
		fHistoOrder.push_back(baseName + (Long_t)i);
		fGraph.insert(pair<TString,TGraph*>(baseName + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + (Long_t)i, directory.Strip(TString::kBoth, '/')));
	}
}

void HistoHandler::FillHisto(TString name, TString x, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	int th1 = fHisto.count(name);

	if(th1>0) fHisto[name]->Fill(x,w);
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

void HistoHandler::FillHisto(TString name, TString x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	int th1 = fHisto.count(name);
	int th2 = fHisto2.count(name);
	int tgraph = fGraph.count(name);

	if(th2>0) fHisto2[name]->Fill(x,y,w);
	else if(th1>0) cerr << name << " is a TH1. Cannot call with (TString,double,double)." << endl;
	else if(tgraph>0) cerr << name << " is a TGraph. Cannot call with (TString,double,double)." << endl;
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

void HistoHandler::FillHisto(TString name, TString x, TString y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	int th1 = fHisto.count(name);
	int th2 = fHisto2.count(name);
	int tgraph = fGraph.count(name);

	if(th2>0) fHisto2[name]->Fill(x,y,w);
	else if(th1>0) cerr << name << " is a TH1. Cannot call with (TString,TString,double)." << endl;
	else if(tgraph>0) cerr << name << " is a TGraph. Cannot call with (TString,TString,double)." << endl;
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

void HistoHandler::FillHisto(TString name, double x){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	int th1 = fHisto.count(name);
	int th2 = fHisto2.count(name);
	int tgraph = fGraph.count(name);

	if(th1>0) fHisto[name]->Fill(x,1);
	else if(th2>0) cerr << name << " is a TH2. Cannot call with (double)." << endl;
	else if(tgraph>0) cerr << name << " is a TGraph. Cannot call with (double)." << endl;
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

void HistoHandler::FillHisto(TString name, double x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	int th1 = fHisto.count(name);
	int th2 = fHisto2.count(name);
	int tgraph = fGraph.count(name);

	if(th2>0) fHisto2[name]->Fill(x,y,w);
	else if(th1>0) cerr << name << " is a TH1. Cannot call with (double,double,double)." << endl;
	else if(tgraph>0) cerr << name << " is a TGraph. Cannot call with (double,double,double)." << endl;
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

void HistoHandler::FillHisto(TString name, double x, double y){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	int th1 = fHisto.count(name);
	int th2 = fHisto2.count(name);
	int tgraph = fGraph.count(name);

	if(th1>0) fHisto2[name]->Fill(x,y);
	else if(th2>0) fHisto2[name]->Fill(x,y,1);
	else if(tgraph>0) {
		fPoint[name]++;
		fGraph[name]->SetPoint(fPoint[name], x, y);
	}
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

//########################################
void HistoHandler::FillHistoArray(TString baseName, int index, TString x, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index, x, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, TString x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index, x, y, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, TString x, TString y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index, x, y, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index, x);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index,x,y,w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x, double y){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index, x, y);
}

void HistoHandler::PrintInitSummary() const{
	/// \MemberDescr
	///
	/// Print a list of booked histograms.
	/// \EndMemberDescr

	map<TString,TH1*>::const_iterator it1;
	map<TString,TH2*>::const_iterator it2;
	map<TString,TGraph*>::const_iterator itGraph;

	StringTable histoTable("List of booked histograms");

	TString sDetAcc;

	//Fill histograms table
	histoTable.AddColumn("th1", "1D Histograms");
	histoTable.AddColumn("auth1", "AU");
	histoTable.AddColumn("th2", "2D Histograms");
	histoTable.AddColumn("auth2", "AU");
	histoTable.AddColumn("gr", "Graphs");
	histoTable.AddColumn("augr", "AU");
	histoTable << sepr;
	for(it1 = fHisto.begin(); it1 != fHisto.end(); it1++){
		histoTable.AddValue(0, it1->first);
		if(fAutoUpdateList.count(it1->first)>0) histoTable.AddValue(1, "x");
		else histoTable.AddValue(1, "");
	}
	for(it2 = fHisto2.begin(); it2 != fHisto2.end(); it2++){
		histoTable.AddValue(2, it2->first);
		if(fAutoUpdateList.count(it1->first)>0) histoTable.AddValue(3, "x");
		else histoTable.AddValue(3, "");
	}
	for(itGraph = fGraph.begin(); itGraph != fGraph.end(); itGraph++){
		histoTable.AddValue(4, itGraph->first);
		if(fAutoUpdateList.count(it1->first)>0) histoTable.AddValue(5, "x");
		else histoTable.AddValue(5, "");
	}

	histoTable.Print("\t");
}

void HistoHandler::ExportAllPlot(map<TString,TTree*> &trees, map<TString,void*> &branches){
	/// \MemberDescr
	/// \param trees : pointer to the list of TTrees
	/// \param branches : point to the list of branches
	///
	/// Export all booked histograms into the output file histograms trees
	/// \EndMemberDescr

	vector<TString>::iterator itOrder;

	for(itOrder=fHistoOrder.begin(); itOrder!=fHistoOrder.end(); itOrder++){
		if(fHisto.count(*itOrder)){
			branches[fHisto[*itOrder]->ClassName()] = fHisto[*itOrder];
			trees[fHisto[*itOrder]->ClassName()]->Fill();
		}
		if(fHisto2.count(*itOrder)){
			branches[fHisto2[*itOrder]->ClassName()] = fHisto2[*itOrder];
			trees[fHisto2[*itOrder]->ClassName()]->Fill();
		}
		if(fGraph.count(*itOrder)){
			branches[fGraph[*itOrder]->ClassName()] = fGraph[*itOrder];
			trees[fGraph[*itOrder]->ClassName()]->Fill();
		}
	}
};

void HistoHandler::DrawAllPlots(TString analyzerName){
	/// \MemberDescr
	/// \param analyzerName : Name of the analyzer calling the method
	///
	/// Draw all booked histograms on the screen
	/// \EndMemberDescr

	vector<TString>::iterator itOrder;

	for(itOrder=fHistoOrder.begin(); itOrder!=fHistoOrder.end(); itOrder++){
		if(fHisto.count(*itOrder)){
			new TCanvas(TString("c_" + analyzerName + "_") + *itOrder);
			fHisto[*itOrder]->Draw();
		}
		if(fHisto2.count(*itOrder)){
			new TCanvas(TString("c_" + analyzerName + "_") + *itOrder);
			fHisto2[*itOrder]->Draw("COLZ");
		}

		if(fGraph.count(*itOrder)){
			new TCanvas(TString("c_" + analyzerName + "_") + *itOrder);
			fGraph[*itOrder]->Draw("A*");
		}
	}
}

void HistoHandler::UpdatePlots(int evtNbr){
	/// \MemberDescr
	/// \param evtNbr : Current event number
	///
	/// Update all plots with refresh
	/// \EndMemberDescr

	vector<TCanvas*>::iterator it;

	if((evtNbr % fUpdateRate) == 0){
		for(it = fCanvas.begin(); it!=fCanvas.end(); it++){
			(*it)->Update();
			(*it)->Draw();
		}
	}
}

void HistoHandler::SaveAllPlots(TString analyzerName){
	/// \MemberDescr
	/// \param analyzerName : Name of the analyzer calling the method
	///
	/// Write all the booked histograms into the output file ordered as the booking order
	/// \EndMemberDescr

	TString name;
	map<TString, TString>::iterator itDirectory;
	vector<TString>::iterator itOrder;

	for(itOrder=fHistoOrder.begin(); itOrder!=fHistoOrder.end(); itOrder++){
		if(fHisto.count(*itOrder)){
			name = *itOrder;
			itDirectory = fPlotsDirectory.find(name);
			if(itDirectory != fPlotsDirectory.end()){
				Mkdir(itDirectory->second, analyzerName);
				gFile->Cd(itDirectory->second);
			}
			fHisto[*itOrder]->Write();
			if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
		}
		if(fHisto2.count(*itOrder)){
			name = *itOrder;
			itDirectory = fPlotsDirectory.find(name);
			if(itDirectory != fPlotsDirectory.end()){
				Mkdir(itDirectory->second, analyzerName);
				gFile->Cd(itDirectory->second);
			}
			fHisto2[*itOrder]->Write();
			if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
		}
		if(fGraph.count(*itOrder)){
			name = *itOrder;
			itDirectory = fPlotsDirectory.find(name);
			if(itDirectory != fPlotsDirectory.end()){
				Mkdir(itDirectory->second, analyzerName);
				gFile->Cd(itDirectory->second);
			}
			fGraph[*itOrder]->Write();
			if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
		}
	}
}

void HistoHandler::SetUpdateInterval(int interval){
	/// \MemberDescr
	/// \param interval : Events interval at which the plots should be updated
	//
	/// Set the update interval for the plots
	/// \EndMemberDescr

	fUpdateRate = interval;
}

int HistoHandler::GetUpdateInterval() const{
	/// \MemberDescr
	/// Return the update interval for the plots
	/// \EndMemberDescr

	return fUpdateRate;
}

void HistoHandler::SetPlotAutoUpdate(TString name, TString analyzerName){
	/// \MemberDescr
	/// \param name : Name of the plot
	/// \param analyzerName : Name of the analyzer calling the method
	///
	/// Define the plot as AutoUpdate. Create the corresponding Canvas and Draw the plot
	/// \EndMemberDescr

	TCanvas *c;
	TString canvasName = TString("c_" + analyzerName + "_") + name;
	if(fHisto.count(name)>0){
		c = new TCanvas(canvasName, canvasName);
		c->Draw();
		fHisto[name]->Draw();
	}
	else if(fHisto2.count(name)>0){
		c = new TCanvas(canvasName, canvasName);
		c->Draw();
		fHisto2[name]->Draw();
	}
	else if(fGraph.count(name)>0){
		c = new TCanvas(canvasName, canvasName);
		c->Draw();
		fGraph[name]->Draw();
	}
	else{
		cerr << "Plot " << name << " does not exist. Unable to set AutoUpdate." << endl;
		return;
	}

	fCanvas.push_back(c);
	fAutoUpdateList.insert(name);
}

double HistoHandler::compareToReferencePlot(const TH1* const hRef, const TH1* const h2, bool KS) {
	/// \MemberDescr
	/// \param hRef : Pointer to the reference plot
	/// \param h2 : Pointer to the plot to compare
	/// \param KS : If true, use Kolmogorov-Smirnov test, else use chi square test
	///
	/// Compare similarity between two 1D histograms, returning the probability of
	///	the tested (h2) histogram following the same distribution as the reference (hRef)
	///	histogram.
	/// \EndMemberDescr

	int nBins = h2->GetNbinsX();
	double *res = new double[nBins];
	TString name = hRef->GetName();
	double probability;

	TH1* hRefCl = (TH1*)hRef->Clone(TString(hRef->GetName()) + "Ref");

	BookHisto(name, hRefCl);
	hRefCl->Sumw2();
	hRefCl->Scale(h2->Integral()/hRefCl->Integral());

	if(KS){
		probability = hRefCl->KolmogorovTest(h2);
	}
	else{
		BookHisto(name + "_res", new TGraph());
		TF1 *f = new TF1("f", "TMath::Gaus(x,0,1)", -10, 10);
		probability = hRefCl->Chi2Test(h2, "UUNORM P", res);

		BookHisto(name + "_QQ", new TGraphQQ(hRefCl->GetNbinsX(), res, f));
		for(int i=0;i<hRefCl->GetNbinsX();i++){
			FillHisto(name+"_res", hRefCl->GetBinCenter(i+1), res[i]);
			//FillHisto(name+"_QQ", hRef->GetBinCenter(i+1), res[i]);
		}
		fGraph[name+"_res"]->GetXaxis()->SetRangeUser(hRefCl->GetBinCenter(0), hRefCl->GetBinCenter(hRefCl->GetNbinsX()));
		fGraph[name+"_res"]->SetTitle(name + " normalised residuals");
		fGraph[name+"_QQ"]->SetTitle(name + " Q-Q plot of normalised residuals");
	}
	delete[] res;
	return probability;
}

TH1* HistoHandler::GetTH1(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TH1 to retrieve
	///
	/// Return the previously booked histogram with the specified name.
	///	If the histogram does not exist or is not of TH1 type, print an error
	///	message and return NULL.
	/// \EndMemberDescr

	if(fHisto.count(name)>0){
		return fHisto[name];
	}
	else{
		cerr << "1D Plot " << name << " does not exist." << endl;
		return NULL;
	}
}

TH2* HistoHandler::GetTH2(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TH2 to retrieve
	///
	/// Return the previously booked histogram with the specified name.
	///	If the histogram does not exist or is not of TH2 type, print an error
	///	message and return NULL.
	/// \EndMemberDescr

	if(fHisto2.count(name)>0){
		return fHisto2[name];
	}
	else{
		cerr << "2D Plot " << name << " does not exist." << endl;
		return NULL;
	}
}

TGraph* HistoHandler::GetTGraph(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TGraph to retrieve
	///
	/// Return the previously booked graph with the specified name.
	///	If the graph does not exist or is not of TGraph type, print an error
	///	message and return NULL.
	/// \EndMemberDescr

	if(fGraph.count(name)>0){
		return fGraph[name];
	}
	else{
		cerr << "Graph " << name << " does not exist." << endl;
		return NULL;
	}
}

void HistoHandler::Mkdir(TString name, TString analyzerName) const{
	/// \MemberDescr
	/// \param name: Name of the directory to create
	/// \param analyzerName : Name of the analyzer calling the method
	///
	/// Check if the directory name already exists in the analyzer subdirectory. If not create it.
	/// \EndMemberDescr

	if(gFile->GetDirectory(analyzerName + "/" + name)==NULL){
		gFile->mkdir(analyzerName + "/" + name);
	}
}
