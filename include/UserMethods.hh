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
	virtual ~UserMethods();

	//Histogram booking methods
	void BookHisto(TString name, TH1* histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TH2* histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TGraph* histo, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH1* histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH2* histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TGraph* histo, int number, bool refresh=false, TString directory="");

	//Histogram filling methods
	void FillHisto(TString name, TString x, int w);
	void FillHisto(TString name, TString x, double y, int w);
	void FillHisto(TString name, TString x, TString y, int w);
	void FillHisto(TString name, double x, int w);
	void FillHisto(TString name, double x);
	void FillHisto(TString name, double x, double y, int w);
	void FillHisto(TString name, double x, double y);

	void FillHistoArray(TString baseName, int index, TString x, int w);
	void FillHistoArray(TString name, int index, TString x, double y, int w);
	void FillHistoArray(TString name, int index, TString x, TString y, int w);
	void FillHistoArray(TString name, int index, double x, int w);
	void FillHistoArray(TString name, int index, double x);
	void FillHistoArray(TString name, int index, double x, double y, int w);
	void FillHistoArray(TString name, int index, double x, double y);

	//Export all histograms into output trees
	void ExportAllPlot(map<TString,TTree*> &trees, map<TString,void*> &branches);

	//Methods for drawing plots on screen
	void DrawAllPlots();
	void UpdatePlots(int evtNbr);
	void SetUpdateInterval(int interval);

	//Save all plots into output file
	void SaveAllPlots();

	bool PrintVerbose(AnalysisFW::VerbosityLevel printAbove);

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
	int GetCounterValue(TString cName);

	//Methods for setting output
	void RegisterOutput(TString name, void* address);
	void SetOutputState(TString name, OutputState state);

	//Methods for getting output
	const void *GetOutput(TString name, OutputState &state);
	template <class T>
	const T* GetOutput(TString name, OutputState &state){
		return (T*)GetOutputVoid(name, state);
	};

	//Method to get an event
		template <class T>
		T* GetObject(TString name){
			return (T*)GetObjectVoid(name);
		};

	//Get global DetectorAcceptance instance
	DetectorAcceptance *GetDetectorAcceptanceInstance();

	//Request new tree to analyze
	void RequestTree(TString name, TDetectorVEvent *evt);
	template <class T>
	void RequestTree(TString name, TString branchName, TString className, T* obj){
		if(!RequestTreeVoid(name, branchName, className, obj)){
			delete obj;
		}
	}

	TDetectorVEvent *GetEvent(TString name);

	//Request input histogram
	TH1* RequestHistogram(TString directory, TString name, bool appendOnNewFile);

protected:
	const void* GetOutputVoid(TString name, OutputState &state);
	bool RequestTreeVoid(TString name, TString branchName, TString className, void* obj);
	void* GetObjectVoid(TString name);

	HistoHandler fHisto;

	TString fAnalyzerName; ///< Name of the analyzer
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity level

	BaseAnalysis *fParent; ///< Pointer to the BaseAnalysis instance containing the analyze
};

#endif /* USERMETHODS_HH_ */
