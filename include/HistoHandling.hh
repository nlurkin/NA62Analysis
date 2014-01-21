/*
 * HistoHandling.hh
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#ifndef HISTOHANDLING_HH_
#define HISTOHANDLING_HH_

#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>
#include <TGraph.h>

using namespace std;

class HistoHandling {
public:
	HistoHandling();
	virtual ~HistoHandling();

	void BookHisto(TString name, TH1* histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TH2* histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TGraph* histo, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH1* histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH2* histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TGraph* histo, int number, bool refresh=false, TString directory="");

private:
	void Mkdir(TString name);
	void SetPlotAutoUpdate(TString name);

	//Histogram containers
	map<TString,TH1*> fHisto; ///< Container for TH1
	map<TString,TH2*> fHisto2; ///< Container for the TH2
	map<TString,TGraph*> fGraph; ///< Container for the TGraph
	map<TString,int> fPoint; ///< Container for the number of points in each TGraph
	vector<TCanvas*> fCanvas; ///< Container for the TCanvas
	map<TString,TTree*> fOutTree; ///< Container for the output TTrees

	set<TString> fAutoUpdateList;
	map<TString, TString> fPlotsDirectory; ///< Matching between plot name and directory name
};

#endif /* HISTOHANDLING_HH_ */
