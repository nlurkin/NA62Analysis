/*
 * UserMethods.cc
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#include <iostream>
#include "UserMethods.hh"
#include "BaseAnalysis.hh"

using namespace std;

UserMethods::UserMethods(BaseAnalysis *ba) {
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr

	fVerbosity = AnalysisFW::kNo;
	fParent = ba;
}

UserMethods::~UserMethods() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr
}


void UserMethods::BookHisto(TString name, TH1* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.BookHisto(name, histo, fAnalyzerName, refresh, directory);
}
void UserMethods::BookHisto(TH1* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.BookHisto(histo->GetName(), histo, fAnalyzerName, refresh, directory);
}

void UserMethods::BookHisto(TString name, TH2* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.BookHisto(name, histo, fAnalyzerName, refresh, directory);
}
void UserMethods::BookHisto(TH2* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.BookHisto(histo->GetName(), histo, fAnalyzerName, refresh, directory);
}

void UserMethods::BookHisto(TString name, TGraph* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.BookHisto(name, histo, fAnalyzerName, refresh, directory);
}
void UserMethods::BookHisto(TGraph* histo, bool refresh, TString directory){
	/// \MemberDescr
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plot when calling SaveAllPlots()
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.BookHisto(histo->GetName(), histo, fAnalyzerName, refresh, directory);
}

void UserMethods::BookHistoArray(TString baseName, TH1* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	fHisto.BookHistoArray(baseName, histo, number, fAnalyzerName, refresh, directory);
}
void UserMethods::BookHistoArray(TH1* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param histo : Pointer to the histogram to replicate
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	fHisto.BookHistoArray(histo->GetName(), histo, number, fAnalyzerName, refresh, directory);
}

void UserMethods::BookHistoArray(TString baseName, TH2* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	fHisto.BookHistoArray(baseName, histo, number, fAnalyzerName, refresh, directory);
}
void UserMethods::BookHistoArray(TH2* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param histo : Pointer to the histogram to replicate
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	fHisto.BookHistoArray(histo->GetName(), histo, number, fAnalyzerName, refresh, directory);
}

void UserMethods::BookHistoArray(TString baseName, TGraph* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	/// \param histo : Pointer to the histogram to replicate
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	fHisto.BookHistoArray(baseName, histo, number, fAnalyzerName, refresh, directory);
}
void UserMethods::BookHistoArray(TGraph* histo, int number, bool refresh, TString directory){
	/// \MemberDescr
	/// \param histo : Pointer to the histogram to replicate
	///	\param number : Number of histograms to create
	/// \param refresh : Set the plots as AutoUpdate
	/// \param directory : analyzer subdirectory to save the plots when calling SaveAllPlots()
	///
	/// Book an array of similar histograms and make it available in the whole analyzer.
	/// \EndMemberDescr

	fHisto.BookHistoArray(histo->GetName(), histo, number, fAnalyzerName, refresh, directory);
}

void UserMethods::FillHisto(TString name, TString x, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHisto(name, x, w);
}

void UserMethods::FillHisto(TString name, TString x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHisto(name, x, y, w);
}

void UserMethods::FillHisto(TString name, TString x, TString y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	fHisto.FillHisto(name, x, y, w);
}

void UserMethods::FillHisto(TString name, double x, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	//
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHisto(name, x, w);
}

void UserMethods::FillHisto(TString name, double x){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	fHisto.FillHisto(name, x);
}

void UserMethods::FillHisto(TString name, double x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHisto(name, x, y, w);
}

void UserMethods::FillHistoArray(TString baseName, int index, TString x, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, w);
}

void UserMethods::FillHistoArray(TString baseName, int index, TString x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, y, w);
}

void UserMethods::FillHistoArray(TString baseName, int index, TString x, TString y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, y, w);
}

void UserMethods::FillHistoArray(TString baseName, int index, double x, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param w : weight
	//
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, w);
}

void UserMethods::FillHistoArray(TString baseName, int index, double x){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x);
}

void UserMethods::FillHistoArray(TString baseName, int index, double x, double y, double w){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, y, w);
}

void UserMethods::ExportAllPlot(map<TString,TTree*> &trees, map<TString,void*> &branches){
	/// \MemberDescr
	/// \param trees : pointer to the list of TTrees
	/// \param branches : point to the list of branches
	///
	/// Export all booked histograms into the output file histograms trees
	/// \EndMemberDescr

	fHisto.ExportAllPlot(trees, branches);
}

void UserMethods::DrawAllPlots(){
	/// \MemberDescr
	/// Draw all booked histograms on the screen
	/// \EndMemberDescr

	fHisto.DrawAllPlots(fAnalyzerName);
}

void UserMethods::UpdatePlots(int evtNbr){
	/// \MemberDescr
	/// \param evtNbr : Current event number
	///
	/// Update all plots with refresh
	/// \EndMemberDescr

	fHisto.UpdatePlots(evtNbr);
}

void UserMethods::SaveAllPlots(){
	/// \MemberDescr
	/// Write all the booked histograms into the output file
	/// \EndMemberDescr

	fHisto.SaveAllPlots(fAnalyzerName);
}

void UserMethods::SetUpdateInterval(int interval){
	/// \MemberDescr
	/// \param interval : Events interval at which the plots should be updated
	//
	/// Set the update interval for the plots
	/// \EndMemberDescr

	if(PrintVerbose(AnalysisFW::kNormal)) cout << "Setting plot update interval to " << interval << endl;
	fHisto.SetUpdateInterval(interval);
}

bool UserMethods::PrintVerbose(AnalysisFW::VerbosityLevel printAbove){
	/// \MemberDescr
	/// \param printAbove : Verbosity level threshold
	///
	/// Check if the verbosity level is high enough to print
	/// \EndMemberDescr

	if(fVerbosity >= printAbove){
		cout << fAnalyzerName << ": ";
		return true;
	}
	return false;
}

void UserMethods::BookCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the Counter
	///
	/// Book a new counter
	/// \EndMemberDescr

	fParent->GetCounterHandler()->BookCounter(fAnalyzerName + TString(".") + name);
}

void UserMethods::AddCounterToEventFraction(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Add a counter in the specified EventFraction table
	/// \EndMemberDescr

	fParent->GetCounterHandler()->AddCounterToEventFraction(efName, fAnalyzerName + TString(".") + cName);
}
void UserMethods::NewEventFraction(TString name){
	/// \MemberDescr
	/// \param name : Name of the eventFraction table
	///
	/// Create a new EventFraction table
	/// \EndMemberDescr

	fParent->GetCounterHandler()->NewEventFraction(name);
}
void UserMethods::DefineSampleSizeCounter(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Define counter as SampleSize in the specified EventFraction table
	/// \EndMemberDescr

	fParent->GetCounterHandler()->DefineSampleSizeCounter(efName, fAnalyzerName + TString(".") + cName);
}
void UserMethods::SetSignificantDigits(TString efName, int v){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param v : Number of significant digits
	///
	/// Set the number of significant digits for the specified EventFraction table
	/// \EndMemberDescr

	fParent->GetCounterHandler()->SetSignificantDigits(efName, v);
}

void UserMethods::SetCounterValue(TString cName, int v){
	/// \MemberDescr
	/// \param cName : Name of the counter
	/// \param v : value
	///
	/// Set the value of a previously booked counter
	/// \EndMemberDescr

	fParent->GetCounterHandler()->SetCounterValue(fAnalyzerName + TString(".") + cName, v);
}
void UserMethods::IncrementCounter(TString cName, int delta){
	/// \MemberDescr
	/// \param cName : Name of the counter
	/// \param delta : value
	///
	/// Increment a previously booked counter by delta
	/// \EndMemberDescr

	fParent->GetCounterHandler()->IncrementCounter(fAnalyzerName + TString(".") + cName, delta);
}
void UserMethods::DecrementCounter(TString cName, int delta){
	/// \MemberDescr
	/// \param cName : Name of the counter
	/// \param delta : value
	///
	/// Decrement a previously booked counter by delta
	/// \EndMemberDescr

	fParent->GetCounterHandler()->DecrementCounter(fAnalyzerName + TString(".") + cName, delta);
}
void UserMethods::IncrementCounter(TString cName){
	/// \MemberDescr
	/// \param cName : Name of the counter
	///
	/// Increment a previously booked counter by 1
	/// \EndMemberDescr

	fParent->GetCounterHandler()->IncrementCounter(fAnalyzerName + TString(".") + cName);
}
void UserMethods::DecrementCounter(TString cName){
	/// \MemberDescr
	/// \param cName : Name of the counter
	///
	/// Decrement a previously booked counter by 1
	/// \EndMemberDescr

	fParent->GetCounterHandler()->DecrementCounter(fAnalyzerName + TString(".") + cName);
}
int UserMethods::GetCounterValue(TString cName){
	/// \MemberDescr
	/// \param cName : Name of the counter
	///
	/// Get counter value
	/// \EndMemberDescr

	return fParent->GetCounterHandler()->GetCounterValue(fAnalyzerName + TString(".") + cName);
}

void UserMethods::RegisterOutput(TString name, void* address){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param address : pointer to the variable that is registered
	///
	/// Register a variable as output of the analyzer
	/// \EndMemberDescr

	fParent->RegisterOutput(fAnalyzerName + TString(".") + name, address);
}

void UserMethods::SetOutputState(TString name, OutputState state){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : state to be set
	///
	/// Set the state of the output variable
	/// \EndMemberDescr

	fParent->SetOutputState(fAnalyzerName + TString(".") + name, state);
}

const void *UserMethods::GetOutput(TString name, OutputState &state){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : is filled with the current state of the output
	///
	/// Return an output variable and the corresponding state
	/// \EndMemberDescr

	return fParent->GetOutput(name, state);
}

DetectorAcceptance *UserMethods::GetDetectorAcceptanceInstance(){
	/// \MemberDescr
	/// Return the global instance of DetectorAcceptance
	/// \EndMemberDescr

	return fParent->GetDetectorAcceptanceInstance();
}

const void* UserMethods::GetOutputVoid(TString name, OutputState &state){
	/// \MemberDescr
	/// Internal interface to BaseAnalysis for GetOutput method
	/// \EndMemberDescr

	return fParent->GetOutput(name, state);
}

void UserMethods::RequestTree(TString name, TDetectorVEvent *evt){
	/// \MemberDescr
	/// \param name : Name of the TTree to open
	/// \param evt : Pointer to an instance of a detector event (MC or Reco)
	///
	/// Request to open a tree in the input file
	/// \EndMemberDescr

	fParent->GetIOHandler()->RequestTree(name, evt);
}

TDetectorVEvent *UserMethods::GetEvent(TString name){
	/// \MemberDescr
	/// \param name : Name of the tree from which the event is read
	///
	/// Get the event from input file
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetEvent(name);
}

TH1* UserMethods::RequestHistogram(TString directory, TString name, bool appendOnNewFile){
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file where this histogram will be searched
	/// \param name : Name of the searched histogram
	/// \param appendOnNewFile : <br>
	///  - If set to true : When a new file is opened by the TChain the value of the new histogram extracted from this file will be appended to the existing histogram.
	///  - If set to false : When a new file is opened by the TChain the current histogram will be replaced by the new one.
	/// \return A pointer to the requested histogram if it was found, else a null pointer.
	///
	/// Request histograms from input file.
	/// \EndMemberDescr

	TH1* histo = fParent->GetIOHandler()->GetInputHistogram(directory, name, appendOnNewFile);

	if(!histo) cout << fAnalyzerName << " : Requested input histogram was not found " << directory << "/" << name << endl;
	return histo;
}

void* UserMethods::GetObjectVoid(TString name){
	/// \MemberDescr
	/// Internal interface to BaseAnalysis for GetObject method
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetObject(name);
}

bool UserMethods::RequestTreeVoid(TString name, TString branchName, TString className, void* obj){
	/// \MemberDescr
	/// Internal interface to BaseAnalysis for RequestTree method
	/// \EndMemberDescr

	return fParent->GetIOHandler()->RequestTree(name, branchName, className, obj);
}

TH1* UserMethods::GetReferenceTH1(TString name){
	/// \MemberDescr
	/// \param name : Name of the reference plot
	///
	/// Return a pointer to the specified reference histogram. If not found, return NULL.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetReferenceTH1(name);
}

TH2* UserMethods::GetReferenceTH2(TString name){
	/// \MemberDescr
	/// \param name : Name of the reference plot
	///
	/// Return a pointer to the specified reference histogram. If not found, return NULL.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetReferenceTH2(name);
}

TGraph* UserMethods::GetReferenceTGraph(TString name){
	/// \MemberDescr
	/// \param name : Name of the reference plot
	///
	/// Return a pointer to the specified reference histogram. If not found, return NULL.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetReferenceTGraph(name);
}
