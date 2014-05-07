/*
 * HistoHandler.hh
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#ifndef HISTOHANDLER_HH_
#define HISTOHANDLER_HH_

#include <set>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TTree.h>
#include <TCanvas.h>
#include "containers.hh"
using namespace std;

/// \class HistoHandler
/// \Brief
/// Class containing and handling histograms
/// \EndBrief
///
/// \Detailed
/// Implements the histogram booking and filling methods as well as the methods for drawing and exporting.
/// \EndDetailed

class HistoHandler {
	/// \class Iterator
	/// \Brief
	/// Class for traversing sets of Histograms stored in HistoHandler.
	/// \EndBrief
	///
	/// \Detailed
	/// Class for traversing sets of Histograms stored in HistoHandler. Template class based on vector of pointers.
	/// \EndDetailed

	template <typename PointerType>
	class Iterator {
		friend class HistoHandler;
	public:
		Iterator();
		Iterator(const Iterator<PointerType>& c);
		~ Iterator();
		Iterator<PointerType>& operator=(const Iterator<PointerType>& c);
		bool operator==(const Iterator<PointerType>& rhs) const;
		bool operator==(const typename std::vector<PointerType*>::iterator& rhs) const;
		bool operator!=(const Iterator<PointerType>& rhs) const;
		bool operator!=(const typename std::vector<PointerType*>::iterator& rhs) const;
		PointerType* operator*();
		PointerType* operator->();

		PointerType* operator++(int);
		PointerType* operator--(int);
		PointerType* operator++();
		PointerType* operator--();

		int operator-(const Iterator<PointerType>& rhs) const;
		int operator-(const typename std::vector<PointerType*>::iterator& rhs) const;

		Iterator<PointerType> operator-(int rhs) const;
		Iterator<PointerType> operator+(int rhs) const;

		Iterator<PointerType>& operator-=(int rhs);
		Iterator<PointerType>& operator+=(int rhs);

		Iterator<PointerType> operator[](int rhs);

		typename std::vector<PointerType*>::iterator End();
		typename std::vector<PointerType*>::iterator Begin();

		bool operator<(const Iterator<PointerType>& rhs) const;
		bool operator<(const typename std::vector<PointerType*>::iterator& rhs) const;
		bool operator<=(const Iterator<PointerType>& rhs) const;
		bool operator<=(const typename std::vector<PointerType*>::iterator& rhs) const;
		bool operator>(const Iterator<PointerType>& rhs) const;
		bool operator>(const typename std::vector<PointerType*>::iterator& rhs) const;
		bool operator>=(const Iterator<PointerType>& rhs) const;
		bool operator>=(const typename std::vector<PointerType*>::iterator& rhs) const;
	private:
		Iterator(const std::vector<PointerType*>& list);
		Iterator(const Iterator<PointerType>& c, typename std::vector<PointerType*>::iterator it);
		std::vector<PointerType*> *fList;
		int *fNInstances;
		typename std::vector<PointerType*>::iterator fIterator;
	};
public:
	typedef Iterator<TH1> IteratorTH1; ///< Typedef for Iterator over TH1
	typedef Iterator<TH2> IteratorTH2; ///< Typedef for Iterator over TH2
	typedef Iterator<TGraph> IteratorTGraph; ///< Typedef for Iterator over TGraph

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
	void FillHistoArray(TString baseName, int index, TString x, double y, double w);
	void FillHistoArray(TString baseName, int index, TString x, TString y, double w);
	void FillHistoArray(TString baseName, int index, double x);
	void FillHistoArray(TString baseName, int index, double x, double y, double w);
	void FillHistoArray(TString baseName, int index, double x, double y);

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

	IteratorTH1 GetIteratorTH1();
	IteratorTH1 GetIteratorTH1(TString baseName);
	IteratorTH2 GetIteratorTH2();
	IteratorTH2 GetIteratorTH2(TString baseName);
	IteratorTGraph GetIteratorTGraph();
	IteratorTGraph GetIteratorTGraph(TString baseName);
private:
	void Mkdir(TString name, TString analyzerName) const;

	//Histogram containers
	AnalysisFW::NA62Map<TString,TH1*>::type fHisto; ///< Container for TH1
	AnalysisFW::NA62Map<TString,TH2*>::type fHisto2; ///< Container for the TH2
	AnalysisFW::NA62Map<TString,TGraph*>::type fGraph; ///< Container for the TGraph
	AnalysisFW::NA62Map<TString,int>::type fPoint; ///< Container for the number of points in each TGraph
	vector<TCanvas*> fCanvas; ///< Container for the TCanvas
	AnalysisFW::NA62Map<TString,TTree*>::type fOutTree; ///< Container for the output TTrees
	vector<TString> fHistoOrder; ///< Container for the booking order
	AnalysisFW::NA62Map<TString,IteratorTH1>::type fTH1IteratorsList; ///< Container for TH1 Iterators (keep them in memory rather than building them again for efficiency reasons)
	AnalysisFW::NA62Map<TString,IteratorTH2>::type fTH2IteratorsList; ///< Container for TH1 Iterators (keep them in memory rather than building them again for efficiency reasons)
	AnalysisFW::NA62Map<TString,IteratorTGraph>::type fTGraphIteratorsList; ///< Container for TH1 Iterators (keep them in memory rather than building them again for efficiency reasons)

	set<TString> fAutoUpdateList; ///< List of histogram being regularly updated on screen during processing
	AnalysisFW::NA62Map<TString,TString>::type fPlotsDirectory; ///< Matching between plot name and directory name

	int fUpdateRate; ///< Event interval at which the plots should be updated
};

template <typename PointerType>
HistoHandler::Iterator<PointerType> operator+(int lhs, HistoHandler::Iterator<PointerType> rhs){
	return rhs + lhs;
}
template <typename PointerType>
HistoHandler::Iterator<PointerType> operator-(int lhs, HistoHandler::Iterator<PointerType> rhs){
	return rhs - lhs;
}

template<typename PointerType>
bool operator<(typename vector<PointerType*>::iterator lhs, const HistoHandler::Iterator<PointerType>& rhs){
	return rhs>lhs;
}
template<typename PointerType>
bool operator<=(typename vector<PointerType*>::iterator lhs, const HistoHandler::Iterator<PointerType>& rhs){
	return rhs>=lhs;
}
template<typename PointerType>
bool operator>(typename vector<PointerType*>::iterator lhs, const HistoHandler::Iterator<PointerType>& rhs){
	return rhs<lhs;
}
template<typename PointerType>
bool operator>=(typename vector<PointerType*>::iterator lhs, const HistoHandler::Iterator<PointerType>& rhs){
	return rhs<=lhs;
}


#endif /* HISTOHANDLER_HH_ */
