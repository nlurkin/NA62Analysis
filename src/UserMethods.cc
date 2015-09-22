/*
 * UserMethods.cc
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#include "UserMethods.hh"

#include <iostream>

#include "BaseAnalysis.hh"
#include "CanvasOrganizer.hh"

namespace NA62Analysis {

UserMethods::UserMethods(Core::BaseAnalysis *ba):
		Verbose("UserMethods"),
		fParent(ba)
{
	/// \MemberDescr
	/// \param ba : Pointer to the parent BaseAnalysis instance
	///
	/// Constructor
	/// \EndMemberDescr
}

UserMethods::UserMethods(Core::BaseAnalysis *ba, std::string name):
		Verbose(name),
		fAnalyzerName(name),
		fParent(ba)
{
	/// \MemberDescr
	/// \param ba : Pointer to the parent BaseAnalysis instance
	/// \param name : Display name
	///
	/// Constructor with name
	/// \EndMemberDescr
}

UserMethods::UserMethods(const UserMethods &c):
		Verbose(c),
		fParent(c.fParent)
{
	/// \MemberDescr
	/// \param c : Reference of the object to copy
	///
	/// Constructor
	/// \EndMemberDescr
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
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
	/// \param x : abscissa
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, w);
}

void UserMethods::FillHistoArray(TString baseName, int index, TString x, double y, double w){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
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
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
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
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
	/// \param x : abscissa
	/// \param w : weight
	//
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, w);
}

void UserMethods::FillHistoArray(TString baseName, int index, double x){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
	/// \param x : abscissa
	///
	/// Fill a previously booked histogram with a weight of 1
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x);
}

void UserMethods::FillHistoArray(TString baseName, int index, double x, double y, double w){
	/// \MemberDescr
	/// \param baseName : Name of the histogram. The index will be appended
	///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
	/// \param x : abscissa
	/// \param y : ordinate
	/// \param w : weight
	///
	/// Fill a previously booked histogram
	/// \EndMemberDescr

	fHisto.FillHistoArray(baseName, index, x, y, w);
}

void UserMethods::ExportAllPlot(std::map<TString,TTree*> &trees, std::map<TString,void*> &branches){
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

	std::cout << normal() << "Setting plot update interval to " << interval << std::endl;
	fHisto.SetUpdateInterval(interval);
}

void UserMethods::BookCounter(TString cName){
	/// \MemberDescr
	/// \param cName : Name of the Counter
	///
	/// Book a new counter
	/// \EndMemberDescr

	fParent->GetCounterHandler()->BookCounter(fAnalyzerName + TString(".") + cName);
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
int UserMethods::GetCounterValue(TString cName) const{
	/// \MemberDescr
	/// \param cName : Name of the counter
	/// \return Value of the requested counter
	///
	/// Get counter value
	/// \EndMemberDescr

	return fParent->GetCounterHandler()->GetCounterValue(fAnalyzerName + TString(".") + cName);
}

void UserMethods::RegisterOutput(TString name, const void* const address){
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

const void *UserMethods::GetOutput(TString name, OutputState &state) const{
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : is filled with the current state of the output
	/// \return Output variable and the corresponding state
	/// \EndMemberDescr

	return fParent->GetOutput(name, state);
}

DetectorAcceptance *UserMethods::GetDetectorAcceptanceInstance(){
	/// \MemberDescr
	/// \return Global instance of DetectorAcceptance
	/// \EndMemberDescr

	return fParent->GetDetectorAcceptanceInstance();
}

const void* UserMethods::GetOutputVoid(TString name, OutputState &state) const {
	/// \MemberDescr
	/// \param name: Name of the output variable to get
	/// \param state: Reference to the outputState of the output variable
	///
	/// Internal interface to BaseAnalysis for GetOutput method
	/// \EndMemberDescr

	return fParent->GetOutput(name, state);
}

void UserMethods::RequestL0Data(){
	/// \MemberDescr
	/// Request the L0 data branch
	/// \EndMemberDescr

	if(fParent->IsTreeType()) fParent->GetIOTree()->RequestTree("Reco", "L0TPData", "L0TPData", new L0TPData);
	else std::cout << user() << "[WARNING] Not reading TTrees" << std::endl;
}

void UserMethods::RequestL1Data(){
	/// \MemberDescr
	/// Request the L1 data branch
	/// \EndMemberDescr

	if(fParent->IsTreeType()) fParent->GetIOTree()->RequestTree("Reco", "L1TPData", "L1TPData", new L1TPData);
	else std::cout << user() << "[WARNING] Not reading TTrees" << std::endl;
}

void UserMethods::RequestL2Data(){
	/// \MemberDescr
	/// Request the L2 data branch
	/// \EndMemberDescr

	if(fParent->IsTreeType()) fParent->GetIOTree()->RequestTree("Reco", "L2EBData", "L2EBData", new L2EBData);
	else std::cout << user() << "[WARNING] Not reading TTrees" << std::endl;
}

void UserMethods::RequestTree(TString detectorName, TDetectorVEvent *evt, TString outputStage){
	/// \MemberDescr
	/// \param detectorName : Name of the Detector branch to open
	/// \param evt : Pointer to an instance of a detector event (MC or Reco)
	/// \param branchName : Name of the tree to request (outputStage = Reco, Digis, MC)
	///
	/// Request a branch in a tree in the input file. If the tree has already been requested before,
	/// only add the new branch.
	/// If outputStage is not specified, the branch "Reco" or "Digis" or "MC" will be used (depending on the
	/// TDetectorVEvent class instance).
	/// \EndMemberDescr

	if(fParent->IsTreeType()) fParent->GetIOTree()->RequestTree(detectorName, evt, outputStage);
	else std::cout << user() << "[WARNING] Not reading TTrees" << std::endl;
}

L0TPData* UserMethods::GetL0Data(){
	/// \MemberDescr
	/// \return L0TPData object
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return static_cast<L0TPData*>(fParent->GetIOTree()->GetObject("L0TPData", "Reco"));
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;
	return nullptr;
}

L1TPData* UserMethods::GetL1Data(){
	/// \MemberDescr
	/// \return L0TPData object
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return static_cast<L1TPData*>(fParent->GetIOTree()->GetObject("L1TPData", "Reco"));
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;
	return nullptr;
}

L2EBData* UserMethods::GetL2Data(){
	/// \MemberDescr
	/// \return L0TPData object
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return static_cast<L2EBData*>(fParent->GetIOTree()->GetObject("L2EBData", "Reco"));
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;
	return nullptr;
}

TDetectorVEvent *UserMethods::GetEvent(TString name, TString branchName){
	/// \MemberDescr
	/// \param name : Name of the tree from which the event is read
	/// \param branchName : Name of the branch
	/// \return the pointer to the event corresponding to the given tree and the given branch.
	///
	/// If branchName is left empty and there is only 1 branch requested on this tree, this
	/// single branch is returned. If there is more than 1 branch requested on this tree,
	/// return either the "Reco" or the "Hits" branch (the first one found - undefined behaviour
	/// if both "Reco" and "Hits" branches have been requested).
	/// If branchName is specified, try to return the specified branch.
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return fParent->GetIOTree()->GetEvent(name, branchName);
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;
	return nullptr;
}

Event* UserMethods::GetMCEvent(TString treeName){
	/// \MemberDescr
	/// \param treeName Tree from which the MCEvent should be retrieved
	/// \return Pointer to the MC event.
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return fParent->GetIOTree()->GetMCTruthEvent(treeName);
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;
	return nullptr;
}

RawHeader* UserMethods::GetRawHeader(){
	/// \MemberDescr
	/// \return Pointer to the RawHeader.
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return fParent->GetIOTree()->GetRawHeaderEvent();
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;
	return nullptr;
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

	if(!fParent->IsHistoType()){
		std::cout << normal() << "[WARNING] Not reading Histos" << std::endl;
		return nullptr;
	}
	TH1* histo = fParent->GetIOHisto()->GetInputHistogram(directory, name, appendOnNewFile);

	if(!histo) std::cout << normal() << "Requested input histogram was not found " << directory << "/" << name << std::endl;
	return histo;
}

Core::HistoHandler::IteratorTH1 UserMethods::GetIteratorTH1() {
	/// \MemberDescr
	/// \return Iterator to TH1
	///
	/// Create a TH1Iterator over all the TH1 stored in this instance of HistoHandler.
	/// \EndMemberDescr

	return fHisto.GetIteratorTH1();
}

Core::HistoHandler::IteratorTH1 UserMethods::GetIteratorTH1(TString baseName) {
	/// \MemberDescr
	/// \param baseName: BaseName of the histograms to iterate over.
	/// \return Iterator to TH1
	///
	/// Create a TH1Iterator over all the TH1 whose name is starting with baseName and stored in this instance of HistoHandler.
	/// \EndMemberDescr

	return fHisto.GetIteratorTH1(baseName);
}

Core::HistoHandler::IteratorTH2 UserMethods::GetIteratorTH2() {
	/// \MemberDescr
	/// \return Iterator to TH2
	///
	/// Create a TH2Iterator over all the TH2 stored in this instance of HistoHandler.
	/// \EndMemberDescr

	return fHisto.GetIteratorTH2();
}

Core::HistoHandler::IteratorTH2 UserMethods::GetIteratorTH2(TString baseName) {
	/// \MemberDescr
	/// \param baseName: BaseName of the histograms to iterate over.
	/// \return Iterator to TH2
	///
	/// Create a TH2Iterator over all the TH2 whose name is starting with baseName and stored in this instance of HistoHandler.
	/// \EndMemberDescr

	return fHisto.GetIteratorTH2(baseName);
}

Core::HistoHandler::IteratorTGraph UserMethods::GetIteratorTGraph() {
	/// \MemberDescr
	/// \return Iterator to TGraph
	///
	/// Create a TGraphIterator over all the TGraph stored in this instance of HistoHandler.
	/// \EndMemberDescr

	return fHisto.GetIteratorTGraph();
}

Core::HistoHandler::IteratorTGraph UserMethods::GetIteratorTGraph(TString baseName) {
	/// \MemberDescr
	/// \param baseName: BaseName of the histograms to iterate over.
	/// \return Iterator to TGraph
	///
	/// Create a TGraphIterator over all the TGraph whose name is starting with baseName and stored in this instance of HistoHandler.
	/// \EndMemberDescr

	return fHisto.GetIteratorTGraph(baseName);
}

TChain* UserMethods::GetTree(TString name) {
	/// \MemberDescr
	/// \param name: Name of the TTree
	/// \return  Pointer to the specified TChain
	/// \EndMemberDescr

	return fParent->GetTree(name);
}

void* UserMethods::GetObjectVoid(TString name){
	/// \MemberDescr
	/// \param name: Name of the object
	/// Internal interface to BaseAnalysis for GetObject method
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return fParent->GetIOTree()->GetObject(name);
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;

	return nullptr;
}

bool UserMethods::RequestTreeVoid(TString name, TString branchName, TString className, void* obj){
	/// \MemberDescr
	/// \param name: Name of the tree
	/// \param branchName: Name of the branch in the requested tree
	/// \param className: Name of the class in the requested branch
	/// \param obj: Pointer to an object of class className
	/// \return True if successful
	///
	/// Internal interface to BaseAnalysis for RequestTree method
	/// \EndMemberDescr

	if(fParent->IsTreeType()) return fParent->GetIOTree()->RequestTree(name, branchName, className, obj);
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;

	return false;
}

TH1* UserMethods::GetReferenceTH1(TString name){
	/// \MemberDescr
	/// \param name : Name of the reference plot
	/// \return Pointer to the specified reference histogram. If not found, return NULL.
	/// \EndMemberDescr

	if(fParent->IsHistoType()) return fParent->GetIOHisto()->GetReferenceTH1(name);
	else std::cout << normal() << "[WARNING] Not reading Histos" << std::endl;

	return nullptr;
}

TH2* UserMethods::GetReferenceTH2(TString name){
	/// \MemberDescr
	/// \param name : Name of the reference plot
	/// \return Pointer to the specified reference histogram. If not found, return NULL.
	/// \EndMemberDescr

	if(fParent->IsHistoType()) return fParent->GetIOHisto()->GetReferenceTH2(name);
	else std::cout << normal() << "[WARNING] Not reading Histos" << std::endl;

	return nullptr;
}

TGraph* UserMethods::GetReferenceTGraph(TString name){
	/// \MemberDescr
	/// \param name : Name of the reference plot
	/// \return Pointer to the specified reference histogram. If not found, return NULL.
	/// \EndMemberDescr

	if(fParent->IsHistoType()) return fParent->GetIOHisto()->GetReferenceTGraph(name);
	else std::cout << normal() << "[WARNING] Not reading Histos" << std::endl;

	return nullptr;
}

int UserMethods::GetNEvents(){
	/// \MemberDescr
	/// \return Total number of events loaded from input trees.
	/// \EndMemberDescr
	return fParent->GetNEvents();
}

bool UserMethods::GetWithMC(){
	/// \MemberDescr
	/// \return true if the input file contains MC events
	/// \EndMemberDescr
	if(fParent->IsTreeType()) return fParent->GetIOTree()->GetWithMC();
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;

	return false;
}

bool UserMethods::GetWithRawHeader(){
	/// \MemberDescr
	/// \return true if the input file contains RawHeader
	/// \EndMemberDescr
	if(fParent->IsTreeType()) return fParent->GetIOTree()->GetWithRawHeader();
	else std::cout << normal() << "[WARNING] Not reading TTrees" << std::endl;

	return false;
}

bool UserMethods::GetIsTree() {
  /// \MemberDescr
  /// \return True if the IO Handler is able to read TTrees
  /// \EndMemberDescr
  return fParent->IsTreeType();
}

bool UserMethods::GetIsHisto() {
  /// \MemberDescr
  /// \return True if the IO Handler is only able to read histograms (--histo flag)
  /// \EndMemberDescr
  return fParent->IsHistoType();
}

TH1* UserMethods::GetInputHistogram(TString directory, TString name) {
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file where this histogram will be searched
	/// \param name : Name of the searched histogram
	/// \return A pointer to the requested histogram if it was found, else a null pointer.
	///
	/// Request histograms from input file.
	/// \EndMemberDescr

	if(!fParent->IsHistoType()){
		std::cout << normal() << "[WARNING] Not reading Histos" << std::endl;
		return nullptr;
	}
	TH1* histo = fParent->GetIOHisto()->GetInputHistogram(directory, name, false);

	if(!histo) std::cout << normal() << "Requested input histogram was not found " << directory << "/" << name << std::endl;
	return histo;
}

std::vector<Core::IOHandler::keyPair> UserMethods::GetListOfKeys(TString directory) {
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file
	/// \return A vector of keys available in the given directory. The key contains
	/// the name of the object (key.name) and the className of the object (key.className)
	///
	/// Request list of keys in the input file.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetListOfKeys(directory);
}

std::vector<TString> UserMethods::GetListOfDirs(TString directory) {
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file
	/// \return A vector of directories available in the given directory
	///
	/// Request list of directories in the input file.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetListOfDirs(directory);
}

std::vector<TString> UserMethods::GetListOfTH1(TString directory) {
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file
	/// \return A vector of TH1 type histograms available in the given directory
	///
	/// Request list of TH1 type histograms in the input file.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetListOfTH1(directory);
}

std::vector<TString> UserMethods::GetListOfTH2(TString directory) {
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file
	/// \return A vector of TH2 type histograms available in the given directory
	///
	/// Request list of TH2 type histograms in the input file.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetListOfTH2(directory);
}

std::vector<TString> UserMethods::GetListOfTGraph(TString directory) {
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file
	/// \return A vector of TGraph type histograms available in the given directory
	///
	/// Request list of TGraph type histograms in the input file.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetListOfTGraph(directory);
}

std::vector<TString> UserMethods::GetListOfHistos(TString directory) {
	/// \MemberDescr
	/// \param directory : Directory in the input ROOT file
	/// \return A vector of histograms available in the given directory
	///
	/// Request list of histograms in the input file.
	/// \EndMemberDescr

	return fParent->GetIOHandler()->GetListOfHistos(directory);
}

NA62Analysis::NA62Map<TString, Core::CanvasOrganizer*>::type UserMethods::GetCanvases() {
	/// \MemberDescr
	/// \return Map containing the list of CanvasOrganizer for this analyzer
	/// \EndMemberDescr
	return fHisto.GetCanvases();
}

int UserMethods::GetUpdateInterval() const {
	/// \MemberDescr
	/// \return Update interval
	/// \EndMemberDescr
	return fHisto.GetUpdateInterval();
}

void UserMethods::CreateCanvas(TString name, int width, int height) {
	/// \MemberDescr
	/// \param name: Name of the canvas
	/// \param width: width of the canvas (default=0=automatic)
	/// \param height: height of the canvas (default=0=automatic)
	///
	/// Create a new named canvas in the analyzer
	/// \EndMemberDescr

	fHisto.CreateCanvas(name, width, height);
}

bool UserMethods::PlacePlotOnCanvas(TString histoName, TString canvasName, int row, int col) {
	/// \MemberDescr
	/// \param histoName: Name of the plot
	/// \param canvasName: Name of the canvas
	/// \param row: Row position on the canvas
	/// \param col: Column position on the canvas
	/// \return True if canvas and histograms were found
	///
	/// Add a plot to the list of Plots managed by the specified CanvasOrganizer
	/// \EndMemberDescr

	return fHisto.PlacePlotOnCanvas(histoName, canvasName, row, col);
}

bool UserMethods::SetCanvasAutoUpdate(TString canvasName) {
	/// \MemberDescr
	/// \param canvasName: Name of the canvas
	/// \return True if canvas was found
	///
	/// Mark a canvas as AutoUpdate (will be redrawn every fAutoUpdateInterval events)
	/// \EndMemberDescr

	return fHisto.SetCanvasAutoUpdate(canvasName);
}

bool UserMethods::UpdateCanvas(TString canvasName) {
	/// \MemberDescr
	/// \param canvasName: Name of the canvas
	/// \return True if canvas was found
	///
	/// Force the update of a canvas
	/// \EndMemberDescr

	return fHisto.UpdateCanvas(canvasName);
}

void UserMethods::SetCanvasReference(TString canvasName, TString histo, TH1* refPtr) {
	/// \MemberDescr
	/// \param canvasName Name of the canvas that contains the histogram to which the reference will be added
	/// \param histo Name of the histogram to which the reference will be added
	/// \param refPtr Pointer to the reference histogram to link to histo.
	///
	/// Add a reference histogram to the specified histogram in the specified canvas
	/// \EndMemberDescr

	fHisto.SetCanvasReference(canvasName, histo, refPtr);
}

void UserMethods::SetCanvasReference(TString canvasName, TString histo, TGraph* refPtr) {
	/// \MemberDescr
	/// \param canvasName Name of the canvas that contains the histogram to which the reference will be added
	/// \param histo Name of the histogram to which the reference will be added
	/// \param refPtr Pointer to the reference histogram to link to histo.
	///
	/// Add a reference histogram to the specified histogram in the specified canvas
	/// \EndMemberDescr

	fHisto.SetCanvasReference(canvasName, histo, refPtr);
}

void UserMethods::CallReconfigureAnalyer(TString analyerName,
		TString parameterName, TString parameterValue) {
	fParent->ReconfigureAnalyzer(analyerName, parameterName, parameterValue);
}

} /* namespace NA62Analysis */

