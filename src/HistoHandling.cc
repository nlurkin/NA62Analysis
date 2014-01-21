/*
 * HistoHandling.cc
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#include "HistoHandling.hh"

HistoHandling::HistoHandling() {
	// TODO Auto-generated constructor stub

}

HistoHandling::~HistoHandling() {
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

void HistoHandling::BookHisto(TString name, TH1* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.insert(pair<TString,TH1*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
}

void HistoHandling::BookHisto(TString name, TH2* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto2.insert(pair<TString,TH2*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
}

void HistoHandling::BookHisto(TString name, TGraph* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fGraph.insert(pair<TString,TGraph*>(name, histo));
	fGraph[name]->SetNameTitle(name, name);
	fPoint.insert(pair<TString,int>(name,0));
	if(refresh) SetPlotAutoUpdate(name);
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(name, directory.Strip(TString::kBoth, '/')));
}

void HistoHandling::BookHistoArray(TString baseName, TH1* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
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
	if(refresh) SetPlotAutoUpdate(baseName + "_0");
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH1*)histo->Clone();
		h->SetName(TString(name + "_" + (Long_t)i).Data());
		h->SetTitle(TString(title + "_" + (Long_t)i).Data());
		fHisto.insert(pair<TString,TH1*>(baseName + "_" + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + "_" + (Long_t)i);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_" + (Long_t)i, directory.Strip(TString::kBoth, '/')));
	}
}

void HistoHandling::BookHistoArray(TString baseName, TH2* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
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
	if(refresh) SetPlotAutoUpdate(baseName + "_0");
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TH2*)histo->Clone();
		h->SetName(TString(name + "_" + (Long_t)i).Data());
		h->SetTitle(TString(title + "_" + (Long_t)i).Data());
		fHisto2.insert(pair<TString,TH2*>(baseName + "_" + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + "_" + (Long_t)i);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_" + (Long_t)i, directory.Strip(TString::kBoth, '/')));
	}
}

void HistoHandling::BookHistoArray(TString baseName, TGraph* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
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
	if(refresh) SetPlotAutoUpdate(baseName + "_0");
	if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_0", directory.Strip(TString::kBoth, '/')));
	for(int i=1; i<number; i++){
		h = (TGraph*)histo->Clone();
		h->SetName(TString(name + "_" + (Long_t)i).Data());
		h->SetTitle(TString(title + "_" + (Long_t)i).Data());
		fGraph.insert(pair<TString,TGraph*>(baseName + "_" + (Long_t)i, h));
		if(refresh) SetPlotAutoUpdate(baseName + "_" + (Long_t)i);
		if(directory.Length()>0) fPlotsDirectory.insert(pair<TString, TString>(baseName + "_" + (Long_t)i, directory.Strip(TString::kBoth, '/')));
	}
}
