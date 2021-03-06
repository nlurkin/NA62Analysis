/*
 * HistoHandler.cc
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#include "HistoHandler.hh"

#include <iostream>

#include <TFile.h>
#include <TGraphQQ.h>
#include <TF1.h>

#include "StringTable.hh"
#include "functions.hh"

namespace NA62Analysis {
namespace Core {

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
	/// \param c : Reference of the object to copy
	///
	/// Copy constructor
	/// \EndMemberDescr
}

HistoHandler::~HistoHandler() {
	/// \MemberDescr
	/// Destructor. Delete all the histograms and canvases.
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator it1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator it2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator it3;
	NA62Analysis::NA62Map<TString,CanvasOrganizer*>::type::iterator it4;

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
		delete it4->second;
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
	fHisto.insert(std::pair<TString,TH1*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name, analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
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
	fHisto2.insert(std::pair<TString,TH2*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name, analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
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
	fGraph.insert(std::pair<TString,TGraph*>(name, histo));
	fGraph[name]->SetNameTitle(name, name);
	fPoint.insert(std::make_pair(name,0));
	if(refresh) SetPlotAutoUpdate(name, analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
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

	TString name, title;
	name = histo->GetName();
	title = histo->GetTitle();
	TH1* h;

	histo->SetName(TString(name + "0").Data());
	histo->SetTitle(TString(title + "0").Data());
	fHistoOrder.push_back(baseName+"0");
	fHisto.insert(std::pair<TString,TH1*>(baseName + "0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(baseName + "0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH1*)histo->Clone();
		h->SetName(TString(name + (Long_t)i).Data());
		h->SetTitle(TString(title + (Long_t)i).Data());
		fHistoOrder.push_back(baseName + (Long_t)i);
		fHisto.insert(std::pair<TString,TH1*>(baseName + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(baseName + (Long_t)i, directory.Strip(TString::kBoth, '/')));
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

	TString name, title;
	name = histo->GetName();
	title = histo->GetTitle();
	TH2* h;

	histo->SetName(TString(name + "0").Data());
	histo->SetTitle(TString(title + "0").Data());
	fHistoOrder.push_back(baseName + "0");
	fHisto2.insert(std::pair<TString,TH2*>(baseName + "0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(baseName + "0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH2*)histo->Clone();
		h->SetName(TString(name + (Long_t)i).Data());
		h->SetTitle(TString(title + (Long_t)i).Data());
		fHistoOrder.push_back(baseName + (Long_t)i);
		fHisto2.insert(std::pair<TString,TH2*>(baseName + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(baseName + (Long_t)i, directory.Strip(TString::kBoth, '/')));
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

	TString name, title;
	name = histo->GetName();
	title = histo->GetTitle();
	TGraph* h;

	histo->SetName(TString(name + "0").Data());
	histo->SetTitle(TString(title + "0").Data());
	fHistoOrder.push_back(baseName+ "0");
	fGraph.insert(std::pair<TString,TGraph*>(baseName + "0", histo));
	if(refresh) SetPlotAutoUpdate(baseName + "0", analyzerName);
	if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(baseName + "0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TGraph*)histo->Clone();
		h->SetName(TString(name + (Long_t)i).Data());
		h->SetTitle(TString(title + (Long_t)i).Data());
		fHistoOrder.push_back(baseName + (Long_t)i);
		fGraph.insert(std::pair<TString,TGraph*>(baseName + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + (Long_t)i, analyzerName);
		if(directory.Length()>0) fPlotsDirectory.insert(std::pair<TString, TString>(baseName + (Long_t)i, directory.Strip(TString::kBoth, '/')));
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

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;

	if((ptr1=fHisto.find(name))!=fHisto.end()) ptr1->second->Fill(x,w);
	else if(fHisto2.count(name)>0) std::cerr << name << " is a TH2. Cannot call with (TString,double)." << std::endl;
	else if(fGraph.count(name)>0) std::cerr << name << " is a TGraph. Cannot call with (TString,double)." << std::endl;
	else std::cerr << "Histogram " << name << " doesn't exist." << std::endl;
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

	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;

	if((ptr2=fHisto2.find(name))!=fHisto2.end()) ptr2->second->Fill(x,y,w);
	else if(fHisto.count(name)>0) std::cerr << name << " is a TH1. Cannot call with (TString,double,double)." << std::endl;
	else if(fGraph.count(name)>0) std::cerr << name << " is a TGraph. Cannot call with (TString,double,double)." << std::endl;
	else std::cerr << "Histogram " << name << " doesn't exist." << std::endl;
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

	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;

	if((ptr2=fHisto2.find(name))!=fHisto2.end()) ptr2->second->Fill(x,y,w);
	else if(fHisto.count(name)>0) std::cerr << name << " is a TH1. Cannot call with (TString,TString,double)." << std::endl;
	else if(fGraph.count(name)>0) std::cerr << name << " is a TGraph. Cannot call with (TString,TString,double)." << std::endl;
	else std::cerr << "Histogram " << name << " doesn't exist." << std::endl;
}

void HistoHandler::FillHisto(TString name, double x){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;

	if((ptr1=fHisto.find(name))!=fHisto.end()) ptr1->second->Fill(x,1);
	else if(fHisto2.count(name)>0) std::cerr << name << " is a TH2. Cannot call with (double)." << std::endl;
	else if(fGraph.count(name)>0) std::cerr << name << " is a TGraph. Cannot call with (double)." << std::endl;
	else std::cerr << "Histogram " << name << " doesn't exist." << std::endl;
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

	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;

	if((ptr2=fHisto2.find(name))!=fHisto2.end()) ptr2->second->Fill(x,y,w);
	else if(fHisto.count(name)>0) std::cerr << name << " is a TH1. Cannot call with (double,double,double)." << std::endl;
	else if(fGraph.count(name)>0) std::cerr << name << " is a TGraph. Cannot call with (double,double,double)." << std::endl;
	else std::cerr << "Histogram " << name << " doesn't exist." << std::endl;
}

void HistoHandler::FillHisto(TString name, double x, double y){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator ptr3;

	if((ptr1=fHisto.find(name))!=fHisto.end()) ptr1->second->Fill(x,y);
	else if((ptr2=fHisto2.find(name))!=fHisto2.end()) ptr2->second->Fill(x,y,1);
	else if((ptr3=fGraph.find(name))!=fGraph.end()){
		fPoint[name]++;
		ptr3->second->SetPoint(fPoint[name], x, y);
	}
	else std::cerr << "Histogram " << name << " doesn't exist." << std::endl;
}

//########################################
void HistoHandler::FillHistoArray(TString baseName, int index, TString x, double w){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
	/// \param x : abscissa
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index, x, w);
}

void HistoHandler::FillHistoArray(TString baseName, int index, TString x, double y, double w){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
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
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
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
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	FillHisto(baseName + (Long_t)index, x);
}

void HistoHandler::FillHistoArray(TString baseName, int index, double x, double y, double w){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
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
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
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

	NA62Analysis::NA62Map<TString,TH1*>::type::const_iterator it1;
	NA62Analysis::NA62Map<TString,TH2*>::type::const_iterator it2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::const_iterator itGraph;

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
		if(fAutoUpdateList.count(it2->first)>0) histoTable.AddValue(3, "x");
		else histoTable.AddValue(3, "");
	}
	for(itGraph = fGraph.begin(); itGraph != fGraph.end(); itGraph++){
		histoTable.AddValue(4, itGraph->first);
		if(fAutoUpdateList.count(itGraph->first)>0) histoTable.AddValue(5, "x");
		else histoTable.AddValue(5, "");
	}

	histoTable.Print("\t");
}

void HistoHandler::ExportAllPlot(std::map<TString,TTree*> &trees, std::map<TString,void*> &branches){
	/// \MemberDescr
	/// \param trees : pointer to the list of TTrees
	/// \param branches : point to the list of branches
	///
	/// Export all booked histograms into the output file histograms trees
	/// \EndMemberDescr

	std::vector<TString>::iterator itOrder;
	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator ptr3;

	for(itOrder=fHistoOrder.begin(); itOrder!=fHistoOrder.end(); itOrder++){
		if((ptr1=fHisto.find(*itOrder))!=fHisto.end()){
			branches[ptr1->second->ClassName()] = fHisto[*itOrder];
			trees[ptr1->second->ClassName()]->Fill();
		}
		else if((ptr2=fHisto2.find(*itOrder))!=fHisto2.end()){
			branches[ptr2->second->ClassName()] = fHisto2[*itOrder];
			trees[ptr2->second->ClassName()]->Fill();
		}
		else if((ptr3=fGraph.find(*itOrder))!=fGraph.end()){
			branches[ptr3->second->ClassName()] = fGraph[*itOrder];
			trees[ptr3->second->ClassName()]->Fill();
		}
	}
}

void HistoHandler::DrawAllPlots(TString analyzerName){
	/// \MemberDescr
	/// \param analyzerName : Name of the analyzer calling the method
	///
	/// Draw all booked histograms on the screen
	/// \EndMemberDescr

	std::vector<TString>::iterator itOrder;
	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator ptr3;
	NA62Analysis::NA62Map<TString,CanvasOrganizer*>::type::iterator ptr4;
	CanvasOrganizer *c;

	for(itOrder=fHistoOrder.begin(); itOrder!=fHistoOrder.end(); itOrder++){
		if((ptr1=fHisto.find(*itOrder))!=fHisto.end()){
			c = new CanvasOrganizer(TString("c_" + analyzerName + "_") + *itOrder);
			c->AddHisto(ptr1->second);
			c->Draw();
			fCanvas.insert(std::make_pair(c->GetName(), c));
		}
		else if((ptr2=fHisto2.find(*itOrder))!=fHisto2.end()){
			c = new CanvasOrganizer(TString("c_" + analyzerName + "_") + *itOrder);
			c->AddHisto(ptr2->second);
			c->Draw();
			fCanvas.insert(std::make_pair(c->GetName(), c));
		}
		else if((ptr3=fGraph.find(*itOrder))!=fGraph.end()){
			c = new CanvasOrganizer(TString("c_" + analyzerName + "_") + *itOrder);
			c->AddHisto(ptr3->second);
			c->Draw();
			fCanvas.insert(std::make_pair(c->GetName(), c));
		}
	}
	for(ptr4=fCanvas.begin(); ptr4!=fCanvas.end(); ptr4++){
		ptr4->second->Draw();
	}
}

void HistoHandler::UpdatePlots(int evtNbr){
	/// \MemberDescr
	/// \param evtNbr : Current event number
	///
	/// Update all plots with refresh
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString, CanvasOrganizer*>::type::iterator it;

	if((evtNbr % fUpdateRate) == 0){
		for(it = fCanvas.begin(); it!=fCanvas.end(); it++){
			it->second->Update();
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
	NA62Analysis::NA62Map<TString,TString>::type::iterator itDirectory;
	std::vector<TString>::iterator itOrder;

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator ptr3;

	for(itOrder=fHistoOrder.begin(); itOrder!=fHistoOrder.end(); itOrder++){
		if((ptr1=fHisto.find(*itOrder))!=fHisto.end()){
			name = *itOrder;
			itDirectory = fPlotsDirectory.find(name);
			if(itDirectory != fPlotsDirectory.end()){
				Mkdir(itDirectory->second, analyzerName);
				gFile->Cd(itDirectory->second);
			}
			ptr1->second->Write();
			if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
		}
		else if((ptr2=fHisto2.find(*itOrder))!=fHisto2.end()){
			name = *itOrder;
			itDirectory = fPlotsDirectory.find(name);
			if(itDirectory != fPlotsDirectory.end()){
				Mkdir(itDirectory->second, analyzerName);
				gFile->Cd(itDirectory->second);
			}
			ptr2->second->Write();
			if(itDirectory != fPlotsDirectory.end()) gFile->Cd("/" + analyzerName);
		}
		else if((ptr3=fGraph.find(*itOrder))!=fGraph.end()){
			name = *itOrder;
			itDirectory = fPlotsDirectory.find(name);
			if(itDirectory != fPlotsDirectory.end()){
				Mkdir(itDirectory->second, analyzerName);
				gFile->Cd(itDirectory->second);
			}
			ptr3->second->Write();
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
	/// \return Update interval for the plots
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

	CanvasOrganizer *c;
	TString canvasName = TString("c_" + analyzerName + "_") + name;

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator ptr3;

	if((ptr1=fHisto.find(name))!=fHisto.end()){
		c = new CanvasOrganizer(canvasName);
		c->AddHisto(ptr1->second);
		c->Draw();
	}
	else if((ptr2=fHisto2.find(name))!=fHisto2.end()){
		c = new CanvasOrganizer(canvasName);
		c->AddHisto(ptr2->second);
		c->Draw();
	}
	else if((ptr3=fGraph.find(name))!=fGraph.end()){
		c = new CanvasOrganizer(canvasName);
		c->AddHisto(ptr3->second);
		c->Draw();
	}
	else{
		std::cerr << "Plot " << name << " does not exist. Unable to set AutoUpdate." << std::endl;
		return;
	}

	fCanvas.insert(std::make_pair(c->GetName(), c));
	fAutoUpdateList.insert(name);
}

double HistoHandler::compareToReferencePlot(const TH1* const hRef, const TH1* const h2, bool KS) {
	/// \MemberDescr
	/// \param hRef : Pointer to the reference plot
	/// \param h2 : Pointer to the plot to compare
	/// \param KS : If true, use Kolmogorov-Smirnov test, else use chi square test
	/// \return Probability for the histograms to be the same distribution
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
	/// \return Previously booked histogram with the specified name.
	///
	///	If the histogram does not exist or is not of TH1 type, print an error
	///	message and return NULL.
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;

	if((ptr1=fHisto.find(name))!=fHisto.end()){
		return ptr1->second;
	}
	else{
		std::cerr << "1D Plot " << name << " does not exist." << std::endl;
		return NULL;
	}
}

TH2* HistoHandler::GetTH2(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TH2 to retrieve
	/// \return Previously booked histogram with the specified name.
	///
	///	If the histogram does not exist or is not of TH2 type, print an error
	///	message and return NULL.
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;

	if((ptr2=fHisto2.find(name))!=fHisto2.end()){
		return ptr2->second;
	}
	else{
		std::cerr << "2D Plot " << name << " does not exist." << std::endl;
		return NULL;
	}
}

TGraph* HistoHandler::GetTGraph(TString name) {
	/// \MemberDescr
	/// \param name : Name of the TGraph to retrieve
	/// \return Previously booked graph with the specified name.
	///
	///	If the graph does not exist or is not of TGraph type, print an error
	///	message and return NULL.
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator ptr3;

	if((ptr3=fGraph.find(name))!=fGraph.end()){
		return ptr3->second;
	}
	else{
		std::cerr << "Graph " << name << " does not exist." << std::endl;
		return NULL;
	}
}

TH1* HistoHandler::GetHisto(TString name) {
	/// \MemberDescr
	/// \param name : Name of the histogram to retrieve (TH1, TH2)
	/// \return Previously booked histogram with the specified name.
	///
	///	If the histogram does not exist, print an error message and return NULL.
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;

	if((ptr1=fHisto.find(name))!=fHisto.end()){
		return ptr1->second;
	}
	else if((ptr2=fHisto2.find(name))!=fHisto2.end()){
		return ptr2->second;
	}
	else{
		std::cerr << "Histogram " << name << " does not exist." << std::endl;
		return NULL;
	}
}

TH1* HistoHandler::GetHistoFromArray(TString baseName, int index) {
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
	///
	///	If the histogram does not exist, print an error message and return NULL.
	/// \return Pointer to histogram if found, NULL pointer else.
	/// \EndMemberDescr

	return GetHisto(baseName + (Long_t)index);
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

HistoHandler::IteratorTH1 HistoHandler::GetIteratorTH1() {
	/// \MemberDescr
	/// \return Iterator to the TH1
	///
	/// Create a TH1Iterator over all the TH1 stored in this instance of HistoHandler.
	/// \EndMemberDescr

	std::vector<TString>::const_iterator it;
	std::vector<TH1*> list;
	NA62Analysis::NA62Map<TString,TH1*>::type::const_iterator itEl;
	NA62Analysis::NA62Map<TString,IteratorTH1>::type::iterator itList;

	if((itList=fTH1IteratorsList.find(""))!=fTH1IteratorsList.end())
		return itList->second;

	for(it=fHistoOrder.begin(); it!=fHistoOrder.end(); ++it){
		if((itEl=fHisto.find(*it))!=fHisto.end()) list.push_back(itEl->second);
	}

	itList = fTH1IteratorsList.insert(std::pair<TString,IteratorTH1>("", IteratorTH1(list))).first;
	return itList->second;
}

HistoHandler::IteratorTH1 HistoHandler::GetIteratorTH1(TString baseName) {
	/// \MemberDescr
	/// \param baseName: BaseName of the histograms to iterate over.
	/// \return Iterator to the TH1
	///
	/// Create a TH1Iterator over all the TH1 whose name is starting with baseName and stored in this instance of HistoHandler.
	/// \EndMemberDescr

	std::vector<TString>::const_iterator it;
	std::vector<TH1*> list;
	NA62Analysis::NA62Map<TString,TH1*>::type::const_iterator itEl;
	NA62Analysis::NA62Map<TString,IteratorTH1>::type::iterator itList;

	if((itList=fTH1IteratorsList.find(baseName))!=fTH1IteratorsList.end())
		return itList->second;

	for(it=fHistoOrder.begin(); it!=fHistoOrder.end(); ++it){
		if(!it->BeginsWith(baseName)) continue;
		if((itEl=fHisto.find(*it))!=fHisto.end()) list.push_back(itEl->second);
	}

	itList = fTH1IteratorsList.insert(std::pair<TString,IteratorTH1>(baseName, IteratorTH1(list))).first;
	return itList->second;
}

HistoHandler::IteratorTH2 HistoHandler::GetIteratorTH2() {
	/// \MemberDescr
	/// \return Iterator to the TH2
	///
	/// Create a TH2Iterator over all the TH2 stored in this instance of HistoHandler.
	/// \EndMemberDescr

	std::vector<TString>::const_iterator it;
	std::vector<TH2*> list;
	NA62Analysis::NA62Map<TString,TH2*>::type::const_iterator itEl;
	NA62Analysis::NA62Map<TString,IteratorTH2>::type::iterator itList;

	if((itList=fTH2IteratorsList.find(""))!=fTH2IteratorsList.end())
		return itList->second;

	for(it=fHistoOrder.begin(); it!=fHistoOrder.end(); ++it){
		if((itEl=fHisto2.find(*it))!=fHisto2.end()) list.push_back(itEl->second);
	}

	itList = fTH2IteratorsList.insert(std::pair<TString,IteratorTH2>("", IteratorTH2(list))).first;
	return itList->second;
}

HistoHandler::IteratorTH2 HistoHandler::GetIteratorTH2(TString baseName) {
	/// \MemberDescr
	/// \param baseName: BaseName of the histograms to iterate over.
	/// \return Iterator to the TH2
	///
	/// Create a TH2Iterator over all the TH2 whose name is starting with baseName and stored in this instance of HistoHandler.
	/// \EndMemberDescr

	std::vector<TString>::const_iterator it;
	std::vector<TH2*> list;
	NA62Analysis::NA62Map<TString,TH2*>::type::const_iterator itEl;
	NA62Analysis::NA62Map<TString,IteratorTH2>::type::iterator itList;

	if((itList=fTH2IteratorsList.find(baseName))!=fTH2IteratorsList.end())
		return itList->second;

	for(it=fHistoOrder.begin(); it!=fHistoOrder.end(); ++it){
		if(!it->BeginsWith(baseName)) continue;
		if((itEl=fHisto2.find(*it))!=fHisto2.end()) list.push_back(itEl->second);
	}

	itList = fTH2IteratorsList.insert(std::pair<TString,IteratorTH2>(baseName, IteratorTH2(list))).first;
	return itList->second;
}

HistoHandler::IteratorTGraph HistoHandler::GetIteratorTGraph() {
	/// \MemberDescr
	/// \return Iterator to the TGraph
	///
	/// Create a TGraphIterator over all the TGraph stored in this instance of HistoHandler.
	/// \EndMemberDescr

	std::vector<TString>::const_iterator it;
	std::vector<TGraph*> list;
	NA62Analysis::NA62Map<TString,TGraph*>::type::const_iterator itEl;
	NA62Analysis::NA62Map<TString,IteratorTGraph>::type::iterator itList;

	if((itList=fTGraphIteratorsList.find(""))!=fTGraphIteratorsList.end())
		return itList->second;

	for(it=fHistoOrder.begin(); it!=fHistoOrder.end(); ++it){
		if((itEl=fGraph.find(*it))!=fGraph.end()) list.push_back(itEl->second);
	}

	itList = fTGraphIteratorsList.insert(std::pair<TString,IteratorTGraph>("", IteratorTGraph(list))).first;
	return itList->second;
}

HistoHandler::IteratorTGraph HistoHandler::GetIteratorTGraph(TString baseName) {
	/// \MemberDescr
	/// \param baseName: BaseName of the histograms to iterate over.
	/// \return Iterator to the TGraph
	///
	/// Create a TGraphIterator over all the TGraph whose name is starting with baseName and stored in this instance of HistoHandler.
	/// \EndMemberDescr

	std::vector<TString>::const_iterator it;
	std::vector<TGraph*> list;
	NA62Analysis::NA62Map<TString,TGraph*>::type::const_iterator itEl;
	NA62Analysis::NA62Map<TString,IteratorTGraph>::type::iterator itList;

	if((itList=fTGraphIteratorsList.find(baseName))!=fTGraphIteratorsList.end())
		return itList->second;

	for(it=fHistoOrder.begin(); it!=fHistoOrder.end(); ++it){
		if(!it->BeginsWith(baseName)) continue;
		if((itEl=fGraph.find(*it))!=fGraph.end()) list.push_back(itEl->second);
	}

	itList = fTGraphIteratorsList.insert(std::pair<TString,IteratorTGraph>(baseName, IteratorTGraph(list))).first;
	return itList->second;
}


void HistoHandler::CreateCanvas(TString name, int width, int height) {
	/// \MemberDescr
	/// \param name: Name of the canvas
	/// \param width: width of the canvas (default=0=automatic)
	/// \param height: height of the canvas (default=0=automatic)
	///
	/// Create a new named canvas in the analyzer
	/// \EndMemberDescr

	CanvasOrganizer *c = new CanvasOrganizer(name);
	if(width!=0 && height!=0) c->SetSize(width, height);

	fCanvas.insert(std::make_pair(c->GetName(), c));
}

bool HistoHandler::PlacePlotOnCanvas(TString histoName, TString canvasName, int row, int col) {
	/// \MemberDescr
	/// \param histoName: Name of the plot
	/// \param canvasName: Name of the canvas
	/// \param row: Row position on the canvas
	/// \param col: Column position on the canvas
	/// \return True if canvas and histograms were found
	///
	/// Add a plot to the list of Plots managed by the specified CanvasOrganizer
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString, CanvasOrganizer*>::type::iterator it;
	NA62Analysis::NA62Map<TString,TH1*>::type::iterator ptr1;
	NA62Analysis::NA62Map<TString,TH2*>::type::iterator ptr2;
	NA62Analysis::NA62Map<TString,TGraph*>::type::iterator ptr3;

	if((it=fCanvas.find(canvasName))!=fCanvas.end()){

		if((ptr1=fHisto.find(histoName))!=fHisto.end()){
			it->second->AddHisto(ptr1->second, row, col);
		}
		else if((ptr2=fHisto2.find(histoName))!=fHisto2.end()){
			it->second->AddHisto(ptr2->second, row, col);
		}
		else if((ptr3=fGraph.find(histoName))!=fGraph.end()){
			it->second->AddHisto(ptr3->second, row, col);
		}
		else{
			std::cerr << "Histogram " << histoName << " does not exist." << std::endl;
			return false;
		}
		return true;
	}
	return false;
}

bool HistoHandler::SetCanvasAutoUpdate(TString canvasName) {
	/// \MemberDescr
	/// \param canvasName: Name of the canvas
	/// \return True if canvas was found
	///
	/// Mark a canvas as AutoUpdate (will be redrawn every fAutoUpdateInterval events)
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString, CanvasOrganizer*>::type::iterator it;

	if((it=fCanvas.find(canvasName))!=fCanvas.end()){
		it->second->SetUpdateFrequency(fUpdateRate);
		it->second->Draw();
		return true;
	}
	return false;
}

bool HistoHandler::UpdateCanvas(TString canvasName) const {
	/// \MemberDescr
	/// \param canvasName: Name of the canvas
	/// \return True if canvas was found
	///
	/// Force the update of a canvas
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString, CanvasOrganizer*>::type::const_iterator it;

	if((it=fCanvas.find(canvasName))!=fCanvas.end()){
		it->second->Update();
		return true;
	}
	return false;
}

void HistoHandler::SetCanvasReference(TString canvas, TString histo, TH1* refPtr) {
	/// \MemberDescr
	/// \param canvas Name of the canvas that contains the histogram to which the reference will be added
	/// \param histo Name of the histogram to which the reference will be added
	/// \param refPtr Pointer to the reference histogram to link to histo.
	///
	/// Add a reference histogram to the specified histogram in the specified canvas
	/// \EndMemberDescr

	TH1* h = GetTH1(histo);
	NA62Analysis::NA62Map<TString, CanvasOrganizer*>::type::const_iterator it;

	if(h && ((it=fCanvas.find(canvas))!=fCanvas.end())){
		it->second->SetReference(refPtr, h);
	}
}

void HistoHandler::SetCanvasReference(TString canvas, TString histo, TGraph* refPtr) {
	/// \MemberDescr
	/// \param canvas Name of the canvas that contains the histogram to which the reference will be added
	/// \param histo Name of the histogram to which the reference will be added
	/// \param refPtr Pointer to the reference histogram to link to histo.
	///
	/// Add a reference histogram to the specified histogram in the specified canvas
	/// \EndMemberDescr

	TGraph* h = GetTGraph(histo);
	NA62Analysis::NA62Map<TString, CanvasOrganizer*>::type::const_iterator it;

	if(h && ((it=fCanvas.find(canvas))!=fCanvas.end())){
		it->second->SetReference(refPtr, h);
	}
}

} /* namespace Core */
} /* namespace NA62Analysis */
