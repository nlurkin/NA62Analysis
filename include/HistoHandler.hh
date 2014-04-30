/*
 * HistoHandler.hh
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#ifndef HISTOHANDLER_HH_
#define HISTOHANDLER_HH_

#include <map>
#include <set>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TTree.h>
#include <TCanvas.h>
#include <unordered_map>
using namespace std;

/// \class HistoHandler
/// \Brief
/// Class containing and handling histograms
/// \EndBrief
///
/// \Detailed
/// Implements the histogram booking and filling methods as well as the methods for drawing and exporting.
/// \EndDetailed

namespace std
{
template<> struct hash<TString>
{
	size_t operator()(const TString& v) const
	{
		return v.Hash();
	}
};
}


class HistoHandler {
public:
	HistoHandler();
	HistoHandler(const HistoHandler& c);
	virtual ~HistoHandler();

	void BookHisto(TString name, TH1* const histo, TString analyzerName="", bool refresh=false, TString directory="");
	void BookHisto(TString name, TH2* const histo, TString analyzerName="", bool refresh=false, TString directory="");
	void BookHisto(TString name, TGraph* const histo, TString analyzerName="", bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH1* const histo, int number, TString analyzerName="", bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH2* const histo, int number, TString analyzerName="", bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TGraph* const histo, int number, TString analyzerName="", bool refresh=false, TString directory="");

	//Histogram filling methods
	void FillHisto(TString name, TString x, double w);
	void FillHisto(TString name, TString x, double y, double w);
	void FillHisto(TString name, TString x, TString y, double w);
	void FillHisto(TString name, double x);
	void FillHisto(TString name, double x, double y, double w);
	void FillHisto(TString name, double x, double y);

	void FillHistoArray(TString baseName, int index, TString x, double w);
	void FillHistoArray(TString name, int index, TString x, double y, double w);
	void FillHistoArray(TString name, int index, TString x, TString y, double w);
	void FillHistoArray(TString name, int index, double x);
	void FillHistoArray(TString name, int index, double x, double y, double w);
	void FillHistoArray(TString name, int index, double x, double y);

	//Export all histograms into output trees
	void ExportAllPlot(map<TString,TTree*> &trees, map<TString,void*> &branches);

	//Methods for drawing plots on screen
	void DrawAllPlots(TString analyzerName);
	void UpdatePlots(int evtNbr);
	void SetUpdateInterval(int interval);
	int GetUpdateInterval() const;

	//Save all plots into output file
	void SaveAllPlots(TString analyzerName);

	void PrintInitSummary() const;
	void SetPlotAutoUpdate(TString name, TString analyzerName);

	double compareToReferencePlot(const TH1* const hRef, const TH1* const h2, bool KS);

	TH1* GetTH1(TString name);
	TH2* GetTH2(TString name);
	TGraph* GetTGraph(TString name);

private:
	void Mkdir(TString name, TString analyzerName) const;

	//Histogram containers
	unordered_map<TString,TH1*> fHisto; ///< Container for TH1
	unordered_map<TString,TH2*> fHisto2; ///< Container for the TH2
	unordered_map<TString,TGraph*> fGraph; ///< Container for the TGraph
	unordered_map<TString,int> fPoint; ///< Container for the number of points in each TGraph
	vector<TCanvas*> fCanvas; ///< Container for the TCanvas
	unordered_map<TString,TTree*> fOutTree; ///< Container for the output TTrees
	vector<TString> fHistoOrder; ///< Container for the booking order

	set<TString> fAutoUpdateList;
	unordered_map<TString, TString> fPlotsDirectory; ///< Matching between plot name and directory name

	int fUpdateRate; ///< Event interval at which the plots should be updated
};

#endif /* HISTOHANDLER_HH_ */
