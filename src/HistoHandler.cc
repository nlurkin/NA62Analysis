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
#include  "functions.hh"

HistoHandler::HistoHandler() {
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr

	fUpdateRate = 10;
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

void HistoHandler::BookHisto(TString name, TH1* histo, TString analyzerName, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param analyzerName : Name of the analyzer calling the method
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

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

	fGraph.insert(pair<TString,TGraph*>(name, histo));
	fGraph[name]->SetNameTitle(name, name);
	fPoint.insert(pair<TString,int>(name,0));
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

	histo->SetName(TString(name + "_0").Data());
	histo->SetTitle(TString(title + "_0").Data());
	fHisto.insert(pair<TString,TH1*>(baseName + "_0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "_0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH1*)histo->Clone();
		h->SetName(TString(name + "_" + (Long_t)i).Data());
		h->SetTitle(TString(title + "_" + (Long_t)i).Data());
		fHisto.insert(pair<TString,TH1*>(baseName + "_" + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + "_" + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_" + (Long_t)i, directory.Strip(TString::kBoth, '/')));
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

	histo->SetName(TString(name + "_0").Data());
	histo->SetTitle(TString(title + "_0").Data());
	fHisto2.insert(pair<TString,TH2*>(baseName + "_0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "_0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH2*)histo->Clone();
		h->SetName(TString(name + "_" + (Long_t)i).Data());
		h->SetTitle(TString(title + "_" + (Long_t)i).Data());
		fHisto2.insert(pair<TString,TH2*>(baseName + "_" + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + "_" + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_" + (Long_t)i, directory.Strip(TString::kBoth, '/')));
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

	histo->SetName(TString(name + "_0").Data());
	histo->SetTitle(TString(title + "_0").Data());
	fGraph.insert(pair<TString,TGraph*>(baseName + "_0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "_0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TGraph*)histo->Clone();
		h->SetName(TString(name + "_" + (Long_t)i).Data());
		h->SetTitle(TString(title + "_" + (Long_t)i).Data());
		fGraph.insert(pair<TString,TGraph*>(baseName + "_" + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + "_" + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_" + (Long_t)i, directory.Strip(TString::kBoth, '/')));
	}
}

void HistoHandler::FillHisto(TString name, TString x, int w){
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

void HistoHandler::FillHisto(TString name, TString x, double y, int w){
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
	else if(th1>0) cerr << name << " is a TH1. Cannot call with (TString,double,int)." << endl;
	else if(tgraph>0) cerr << name << " is a TGraph. Cannot call with (TString,double,int)." << endl;
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

void HistoHandler::FillHisto(TString name, TString x, TString y, int w){
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
	else if(th1>0) cerr << name << " is a TH1. Cannot call with (TString,TString,int)." << endl;
	else if(tgraph>0) cerr << name << " is a TGraph. Cannot call with (TString,TString,int)." << endl;
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

void HistoHandler::FillHisto(TString name, double x, int w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	//
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	int th1 = fHisto.count(name);
	int th2 = fHisto2.count(name);
	int tgraph = fGraph.count(name);

	if(th1>0) fHisto[name]->Fill(x,w);
	else if(th2>0) fHisto2[name]->Fill(x,w);
	else if(tgraph>0) {
		fPoint[name]++;
		fGraph[name]->SetPoint(fPoint[name], x, w);
	}
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

void HistoHandler::FillHisto(TString name, double x, double y, int w){
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
	else if(th1>0) cerr << name << " is a TH1. Cannot call with (double,double,int)." << endl;
	else if(tgraph>0) cerr << name << " is a TGraph. Cannot call with (double,double,int)." << endl;
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

	if(th2>0) fHisto2[name]->Fill(x,y,1);
	else if(tgraph>0) {
		fPoint[name]++;
		fGraph[name]->SetPoint(fPoint[name], x, y);
	}
	else if(th1>0) cerr << name << " is a TH1. Cannot call with (double,double)." << endl;
	else cerr << "Histogram " << name << " doesn't exist." << endl;
}

//########################################
void HistoHandler::FillHistoArray(TString baseName, int index, TString x, int w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + "_" + (Long_t)index, x, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, TString x, double y, int w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + "_" + (Long_t)index, x, y, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, TString x, TString y, int w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	FillHisto(baseName + "_" + (Long_t)index, x, y, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x, int w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	//
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + "_" + (Long_t)index, x, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	FillHisto(baseName + "_" + (Long_t)index, x);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x, double y, int w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + "_" + (Long_t)index,x,y,w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x, double y){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + "_" + (Long_t)index, x, y);
}

void HistoHandler::PrintInitSummary(){
	/// \MemberDescr
	///
	/// Print a list of booked histograms.
	/// \EndMemberDescr

	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator itGraph;

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

	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator it3;

	for(it1=fHisto.begin(); it1!=fHisto.end(); it1++){
		branches[(*it1).second->ClassName()] = (*it1).second;
		trees[(*it1).second->ClassName()]->Fill();
	}
	for(it2=fHisto2.begin(); it2!=fHisto2.end(); it2++){
		branches[(*it2).second->ClassName()] = (*it2).second;
		trees[(*it2).second->ClassName()]->Fill();
	}
	for(it3=fGraph.begin(); it3!=fGraph.end(); it3++){
		branches[(*it3).second->ClassName()] = (*it3).second;
		trees[(*it3).second->ClassName()]->Fill();
	}
};

void HistoHandler::DrawAllPlots(TString analyzerName){
	/// \MemberDescr
	/// \param analyzerName : Name of the analyzer calling the method
	///
	/// Draw all booked histograms on the screen
	/// \EndMemberDescr

	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator it3;

	for(it1 = fHisto.begin(); it1!=fHisto.end(); it1++){
		new TCanvas(TString("c_" + analyzerName + "_") + it1->first);
		cout << "Drawing " << (*it1).second->GetName() << endl;
		(*it1).second->Draw();
	}

	for(it2 = fHisto2.begin(); it2!=fHisto2.end(); it2++){
		new TCanvas(TString("c_" + analyzerName + "_") + it2->first);
		(*it2).second->Draw("COLZ");
	}

	for(it3 = fGraph.begin(); it3!=fGraph.end(); it3++){
		new TCanvas(TString("c_" + analyzerName + "_") + it3->first);
		(*it3).second->Draw("A*");
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
	/// Write all the booked histograms into the output file
	/// \EndMemberDescr

	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator it3;
	TString name;
	map<TString, TString>::iterator itDirectory;

	for(it1=fHisto.begin(); it1!=fHisto.end(); it1++){
		name = (*it1).second->GetName();
		itDirectory = fPlotsDirectory.find(name);
		if(itDirectory != fPlotsDirectory.end()){
			Mkdir(itDirectory->second, analyzerName);
			gFile->Cd(itDirectory->second);
		}
		(*it1).second->Write();
		if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
	}
	for(it2=fHisto2.begin(); it2!=fHisto2.end(); it2++){
		name = (*it2).second->GetName();
		itDirectory = fPlotsDirectory.find(name);
		if(itDirectory != fPlotsDirectory.end()){
			Mkdir(itDirectory->second, analyzerName);
			gFile->Cd(itDirectory->second);
		}
		(*it2).second->Write();
		if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
	}
	for(it3=fGraph.begin(); it3!=fGraph.end(); it3++){
		name = (*it3).second->GetName();
		itDirectory = fPlotsDirectory.find(name);
		if(itDirectory != fPlotsDirectory.end()){
			Mkdir(itDirectory->second, analyzerName);
			gFile->Cd(itDirectory->second);
		}
		(*it3).second->Write();
		if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
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

int HistoHandler::GetUpdateInterval(){
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

double HistoHandler::compareToReferencePlot(TH1* hRef, bool hRefWeighted, TH1* h2, bool h2Weighted) {
	int nBins = h2->GetNbinsX();
	double *res = new double[nBins];
	TString params = "U";
	TString name = hRef->GetName();
	double probability;

	BookHisto(name, hRef);
	BookHisto(name + "_res", new TGraph());

	if(h2Weighted) params += "W";
	else params += "U";
	probability = hRef->Chi2Test(h2, params + " P", res);
	for(int i=0;i<hRef->GetNbinsX();i++){
		FillHisto(name+"_res", hRef->GetBinCenter(i+1), res[i]);
	}

	delete[] res;
	return probability;
}

TH1* HistoHandler::GetTH1(TString name) {
	if(fHisto.count(name)>0){
		return fHisto[name];
	}
	else{
		cerr << "1D Plot " << name << " does not exist." << endl;
		return NULL;
	}
}

TH2* HistoHandler::GetTH2(TString name) {
	if(fHisto2.count(name)>0){
		return fHisto2[name];
	}
	else{
		cerr << "2D Plot " << name << " does not exist." << endl;
		return NULL;
	}
}

TGraph* HistoHandler::GetTGraph(TString name) {
	if(fGraph.count(name)>0){
		return fGraph[name];
	}
	else{
		cerr << "Graph " << name << " does not exist." << endl;
		return NULL;
	}
}

void HistoHandler::Mkdir(TString name, TString analyzerName){
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
