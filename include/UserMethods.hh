/*
 * UserMethods.hh
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#ifndef USERMETHODS_HH_
#define USERMETHODS_HH_

#include <map>
#include <TString.h>
#include <TTree.h>
#include <TGraph.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>
#include <TDetectorVEvent.hh>
#include "HistoHandler.hh"
#include "FWEnums.hh"

class BaseAnalysis;
class DetectorAcceptance;

using namespace std;

/// \class UserMethods
/// \Brief
/// Interface class for the end user for methods whose implementation is not actually done in the analyzer class.
/// \EndBrief
///
/// \Detailed
/// This class defines a list of methods already implemented in members objects of the analyzer but that we must keep
///	in the analyzer class as well for backward compatibility and/or shorter syntax.
/// \EndDetailed
class UserMethods {
public:
	enum OutputState {kOUninit, kOValid, kOInvalid}; ///< List of possible states for the output variables.

	UserMethods(BaseAnalysis *ba);
	UserMethods(const UserMethods&);
	virtual ~UserMethods();

	//###### Histograms related
	//Histogram booking methods
	void BookHisto(TString name, TH1* const histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TH2* const histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TGraph* const histo, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH1* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH2* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TGraph* const histo, int number, bool refresh=false, TString directory="");

	void BookHisto(TH1* const histo, bool refresh=false, TString directory="");
	void BookHisto(TH2* const histo, bool refresh=false, TString directory="");
	void BookHisto(TGraph* const histo, bool refresh=false, TString directory="");
	void BookHistoArray(TH1* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TH2* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TGraph* const histo, int number, bool refresh=false, TString directory="");

	//Histogram filling methods
	void FillHisto(TString name, TString x, double w);
	void FillHisto(TString name, TString x, double y, double w);
	void FillHisto(TString name, TString x, TString y, double w);
	void FillHisto(TString name, double x, double w);
	void FillHisto(TString name, double x);
	void FillHisto(TString name, double x, double y, double w);

	void FillHistoArray(TString baseName, int index, TString x, double w);
	void FillHistoArray(TString name, int index, TString x, double y, double w);
	void FillHistoArray(TString name, int index, TString x, TString y, double w);
	void FillHistoArray(TString name, int index, double x, double w);
	void FillHistoArray(TString name, int index, double x);
	void FillHistoArray(TString name, int index, double x, double y, double w);

	//Alias for user to make difference between TH1 and TH2
	inline void FillHisto2(TString name, TString x, double y, double w) {FillHisto(name,x,y,w);};
	inline void FillHisto2(TString name, TString x, TString y, double w) {FillHisto(name,x,y,w);};
	inline void FillHisto2(TString name, double x, double w) {FillHisto(name,x,w);};
	inline void FillHisto2(TString name, double x, double y, double w) {FillHisto(name,x,y,w);};

	inline void FillHisto2Array(TString name, int index, TString x, double y, double w) {FillHistoArray(name, index, x, y, w);};
	inline void FillHisto2Array(TString name, int index, TString x, TString y, double w) {FillHistoArray(name, index, x, y, w);};
	inline void FillHisto2Array(TString name, int index, double x, double w) {FillHistoArray(name, index, x, w);};
	inline void FillHisto2Array(TString name, int index, double x, double y, double w) {FillHistoArray(name, index, x, y, w);};

	//Methods for drawing plots on screen
	void DrawAllPlots();
	void UpdatePlots(int evtNbr);
	void SetUpdateInterval(int interval);
	//Export all histograms into output trees
	void ExportAllPlot(map<TString,TTree*> &trees, map<TString,void*> &branches);
	//Save all plots into output file
	void SaveAllPlots();

	//Request input histogram
	TH1* RequestHistogram(TString directory, TString name, bool appendOnNewFile);
	TH1* GetReferenceTH1(TString name);
	TH2* GetReferenceTH2(TString name);
	TGraph* GetReferenceTGraph(TString name);

	//###### Event fraction related
	//EventFraction methods
	void NewEventFraction(TString name);
	void AddCounterToEventFraction(TString efName, TString cName);
	void DefineSampleSizeCounter(TString efName, TString cName);
	void SetSignificantDigits(TString efName, int v);

	//Counter methods
	void BookCounter(TString cName);
	void SetCounterValue(TString cName, int v);
	void IncrementCounter(TString cName, int delta);
	void DecrementCounter(TString cName, int delta);
	void IncrementCounter(TString cName);
	void DecrementCounter(TString cName);
	int GetCounterValue(TString cName) const;

	//###### Output related
	//Methods for setting output
	void RegisterOutput(TString name, const void* const address);
	void SetOutputState(TString name, OutputState state);

	//Methods for getting output
	const void *GetOutput(TString name, OutputState &state) const;
	template <class T>
	const T* GetOutput(TString name, OutputState &state) const{
		return (T*)GetOutputVoid(name, state);
	}

	//###### Input (Event/TTree) related
	//Request new tree to analyze
	void RequestTree(TString name, TDetectorVEvent *evt);
	template <class T>
	void RequestTree(TString name, TString branchName, TString className, T* obj){
		if(!RequestTreeVoid(name, branchName, className, obj)){
			delete obj;
		}
	}

	TDetectorVEvent *GetEvent(TString name);

	//###### Other methods
	bool PrintVerbose(AnalysisFW::VerbosityLevel printAbove) const;
	DetectorAcceptance *GetDetectorAcceptanceInstance();
	template <class T>
	T* GetObject(TString name){
		return (T*)GetObjectVoid(name);
	}

	int GetNEvents();

private:
	UserMethods();

	const void* GetOutputVoid(TString name, OutputState &state) const;
	bool RequestTreeVoid(TString name, TString branchName, TString className, void* obj);
	void* GetObjectVoid(TString name);

protected:
	HistoHandler fHisto;

	TString fAnalyzerName; ///< Name of the analyzer
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity level

	BaseAnalysis *fParent; ///< Pointer to the BaseAnalysis instance containing the analyze
};

#endif /* USERMETHODS_HH_ */
