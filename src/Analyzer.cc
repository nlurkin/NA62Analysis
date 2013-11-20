#include "Analyzer.hh"

#include <TString.h>
#include <TChain.h>
#include <TDetectorVEvent.hh>
#include <TTree.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TFile.h>

#include "MCSimple.hh"
#include <iostream>
#include "BaseAnalysis.hh"
#include "functions.hh"
#include "StringTable.hh"

using namespace std;

Analyzer::Analyzer(BaseAnalysis *ba){
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr

	fState = kUninit;
	fParent = ba;

	fVerbosity = AnalysisFW::kNo;
	fUpdateRate = 10;
	fDetectorAcceptanceInstance = NULL;
	fParticleInterface = ParticleInterface::GetParticleInterface();

	fNoMCWarned = false;
	fIncompleteMCWarned = false;
}

Analyzer::~Analyzer(){
	/// \MemberDescr
	/// Destructor
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

void Analyzer::BookHisto(TString name, TH1* histo, bool refresh){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto.insert(pair<TString,TH1*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name);
}

void Analyzer::BookHisto(TString name, TH2* histo, bool refresh){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fHisto2.insert(pair<TString,TH2*>(name, histo));
	if(refresh) SetPlotAutoUpdate(name);
}

void Analyzer::BookHisto(TString name, TGraph* histo, bool refresh){
	/// \MemberDescr
	/// \param name : Name of the histogram
	/// \param histo : Pointer to the histogram
	/// \param refresh : Set the plot as AutoUpdate
	///
	/// Book a new histogram and make it available in the whole analyzer
	/// \EndMemberDescr

	fGraph.insert(pair<TString,TGraph*>(name, histo));
	fGraph[name]->SetNameTitle(name, name);
	fPoint.insert(pair<TString,int>(name,0));
	if(refresh) SetPlotAutoUpdate(name);
}

void Analyzer::FillHisto(TString name, TString x, int w){
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

void Analyzer::FillHisto(TString name, TString x, double y, int w){
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

void Analyzer::FillHisto(TString name, TString x, TString y, int w){
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

void Analyzer::FillHisto(TString name, double x, int w){
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
void Analyzer::FillHisto(TString name, double x){
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

void Analyzer::FillHisto(TString name, double x, double y, int w){
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
void Analyzer::FillHisto(TString name, double x, double y){
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

void Analyzer::BookCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the Counter
	///
	/// Book a new counter
	/// \EndMemberDescr

	fParent->BookCounter(fAnalyzerName + TString(".") + name);
}

void Analyzer::AddCounterToEventFraction(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Add a counter in the specified EventFraction table
	/// \EndMemberDescr

	fParent->AddCounterToEventFraction(efName, fAnalyzerName + TString(".") + cName);
}
void Analyzer::NewEventFraction(TString name){
	/// \MemberDescr
	/// \param name : Name of the eventFraction table
	///
	/// Create a new EventFraction table
	/// \EndMemberDescr

	fParent->NewEventFraction(name);
}
void Analyzer::DefineSampleSizeCounter(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Define counter as SampleSize in the specified EventFraction table
	/// \EndMemberDescr

	fParent->DefineSampleSizeCounter(efName, fAnalyzerName + TString(".") + cName);
}
void Analyzer::SetSignificantDigits(TString efName, int v){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param v : Number of significant digits
	///
	/// Set the number of significant digits for the specified EventFraction table
	/// \EndMemberDescr

	fParent->SetSignificantDigits(efName, v);
}

void Analyzer::SetCounterValue(TString cName, int v){
	/// \MemberDescr
	/// \param cName : Name of the counter
	/// \param v : value
	///
	/// Set the value of a previously booked counter
	/// \EndMemberDescr

	fParent->SetCounterValue(fAnalyzerName + TString(".") + cName, v);
}
void Analyzer::IncrementCounter(TString cName, int delta){
	/// \MemberDescr
	/// \param cName : Name of the counter
	/// \param delta : value
	///
	/// Increment a previously booked counter by delta
	/// \EndMemberDescr

	fParent->IncrementCounter(fAnalyzerName + TString(".") + cName, delta);
}
void Analyzer::DecrementCounter(TString cName, int delta){
	/// \MemberDescr
	/// \param cName : Name of the counter
	/// \param delta : value
	///
	/// Decrement a previously booked counter by delta
	/// \EndMemberDescr

	fParent->DecrementCounter(fAnalyzerName + TString(".") + cName, delta);
}
void Analyzer::IncrementCounter(TString cName){
	/// \MemberDescr
	/// \param cName : Name of the counter
	///
	/// Increment a previously booked counter by 1
	/// \EndMemberDescr

	fParent->IncrementCounter(fAnalyzerName + TString(".") + cName);
}
void Analyzer::DecrementCounter(TString cName){
	/// \MemberDescr
	/// \param cName : Name of the counter
	///
	/// Decrement a previously booked counter by 1
	/// \EndMemberDescr

	fParent->DecrementCounter(fAnalyzerName + TString(".") + cName);
}
int Analyzer::GetCounterValue(TString cName){
	/// \MemberDescr
	/// \param cName : Name of the counter
	///
	/// Get counter value
	/// \EndMemberDescr

	return fParent->GetCounterValue(fAnalyzerName + TString(".") + cName);
}

void Analyzer::ExportAllPlot(map<TString,TTree*> &trees, map<TString,void*> &branches){
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

void Analyzer::DrawAllPlots(){
	/// \MemberDescr
	/// Draw all booked histograms on the screen
	/// \EndMemberDescr

	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator it3;

	for(it1 = fHisto.begin(); it1!=fHisto.end(); it1++){
		new TCanvas(TString("c_" + fAnalyzerName + "_") + it1->first);
		cout << "Drawing " << (*it1).second->GetName() << endl;
		(*it1).second->Draw();
	}

	for(it2 = fHisto2.begin(); it2!=fHisto2.end(); it2++){
		new TCanvas(TString("c_" + fAnalyzerName + "_") + it2->first);
		(*it2).second->Draw("COLZ");
	}

	for(it3 = fGraph.begin(); it3!=fGraph.end(); it3++){
		new TCanvas(TString("c_" + fAnalyzerName + "_") + it3->first);
		(*it3).second->Draw("A*");
	}
}

void Analyzer::UpdatePlots(int evtNbr){
	/// \MemberDescr
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

void Analyzer::SaveAllPlots(){
	/// \MemberDescr
	/// Write all the booked histograms into the output file
	/// \EndMemberDescr

	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator it3;
	TString name;

	for(it1=fHisto.begin(); it1!=fHisto.end(); it1++){
		name = (*it1).second->GetName();
		(*it1).second->Write();
	}
	for(it2=fHisto2.begin(); it2!=fHisto2.end(); it2++){
		name = (*it2).second->GetName();
		(*it2).second->Write();
	}
	for(it3=fGraph.begin(); it3!=fGraph.end(); it3++){
		(*it3).second->Write();
	}
}

void Analyzer::PrintName(){
	/// \MemberDescr
	/// Print the name of the Analyzer
	/// \EndMemberDescr

	cout << fAnalyzerName << endl;
}

TString Analyzer::GetAnalyzerName(){
	/// \MemberDescr
	/// Return the name of the Analyzer
	/// \EndMemberDescr

	return fAnalyzerName;
}

void Analyzer::RegisterOutput(TString name, void* address){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param address : pointer to the variable that is registered
	///
	/// Register a variable as output of the analyzer
	/// \EndMemberDescr

	fParent->RegisterOutput(fAnalyzerName + TString(".") + name, address);
}

void Analyzer::SetOutputState(TString name, OutputState state){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : state to be set
	///
	/// Set the state of the output variable
	/// \EndMemberDescr

	fParent->SetOutputState(fAnalyzerName + TString(".") + name, state);
}

const void *Analyzer::GetOutput(TString name, OutputState &state){
	/// \MemberDescr
	/// \param name : name of the output
	/// \param state : is filled with the current state of the output
	///
	/// Return an output variable and the corresponding state
	/// \EndMemberDescr

	return fParent->GetOutput(name, state);
}

DetectorAcceptance *Analyzer::GetDetectorAcceptanceInstance(){
	/// \MemberDescr
	/// Return the global instance of DetectorAcceptance
	/// \EndMemberDescr

	return fParent->GetDetectorAcceptanceInstance();
}

void Analyzer::ExportEvent(){
	/// \MemberDescr
	/// Ask BaseAnalysis to store the event in the output file
	/// \EndMemberDescr

	fExportEvent = true;
}

bool Analyzer::GetExportEvent(){
	/// \MemberDescr
	/// Did the analyzer asked to export the event?
	/// \EndMemberDescr

	return fExportEvent;
}

void Analyzer::RequestTree(TString name, TDetectorVEvent *evt){
	/// \MemberDescr
	/// \param name : Name of the TTree to open
	/// \param evt : Pointer to an instance of a detector event (MC or Reco)
	///
	/// Request to open a tree in the input file
	/// \EndMemberDescr

	fParent->RequestTree(name, evt);
}

TDetectorVEvent *Analyzer::GetEvent(TString name){
	/// \MemberDescr
	/// \param name : Name of the tree from which the event is read
	///
	/// Get the event from input file
	/// \EndMemberDescr

	return fParent->GetEvent(name);
}

void Analyzer::PreProcess(){
	/// \MemberDescr
	/// Called before Process(). Resets the fExportEvent variable.
	/// \EndMemberDescr

	map<TString, TClonesArray>::iterator itArr;
	map<TString, int>::iterator itNum;

	fExportEvent = false;

	for(itArr=fExportCandidates.begin(); itArr!=fExportCandidates.end(); itArr++){
		itArr->second.Clear();
	}
	for(itNum=fExportCandidatesNumber.begin(); itNum!=fExportCandidatesNumber.end(); itNum++){
		itNum->second = 0;
	}
}

void Analyzer::SetParamValue(TString name, TString val){
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param val : string value to be set
	///
	/// Set the value of a parameter. The parameter must have been registered with addParam
	/// \EndMemberDescr

	if(fParams.count(name)==0){
		cerr << "Parameter " << name << " does not exist for analyzer " << fAnalyzerName << endl;
		return;
	}
	param_t p = fParams[name];
	if(PrintVerbose(AnalysisFW::kNormal)) cout << "Setting parameter " << name << " of type " << p.first << " with value " << val << endl;

	if(p.first.CompareTo("char")==0){
		*(char*)(p.second) = *(val.Data());
	}
	else if(p.first.CompareTo("int")==0){
		if(val.IsDigit()) *(int*)(p.second) = val.Atoi();
		else cerr << fAnalyzerName << ": Unable to set parameter " << name << " of type " << p.first << ". The value does not have the correct type" << val << endl;
	}
	else if(p.first.CompareTo("long")==0){
		if(val.IsDigit()) *(long*)(p.second) = val.Atoll();
		else cerr << fAnalyzerName << ": Unable to set parameter " << name << " of type " << p.first << ". The value does not have the correct type" << val << endl;
	}
	else if(p.first.CompareTo("bool")==0){
		if(val.IsDigit()) *(bool*)(p.second) = val.Atoi();
		else if(val.CompareTo("true", TString::kIgnoreCase)==0) *(bool*)(p.second) = true;
		else if(val.CompareTo("false", TString::kIgnoreCase)==0) *(bool*)(p.second) = false;
	}
	else if(p.first.CompareTo("float")==0){
		if(val.IsFloat()) *(float*)(p.second) = val.Atof();
		else cerr << fAnalyzerName << ": Unable to set parameter " << name << " of type " << p.first << ". The value does not have the correct type" << val << endl;
	}
	else if(p.first.CompareTo("double")==0){
		if(val.IsFloat()) *(double*)(p.second) = val.Atof();
		else cerr << fAnalyzerName << ": Unable to set parameter " << name << " of type " << p.first << ". The value does not have the correct type" << val << endl;
	}
	else if(p.first.CompareTo("string")==0){
		(*(string*)(p.second)) = val.Data();
	}
	else if(p.first.CompareTo("TString")==0){
		*(TString*)(p.second) = val;
	}
}

TString Analyzer::StringFromParam(TString name){
	/// \MemberDescr
	/// \param name : name of the parameter (previously defined with AddParam)
	///
	/// Return the value of a parameter as TString
	/// \EndMemberDescr

	param_t p = fParams[name];

	TString r("");

	if(p.first.CompareTo("char")==0){
		r+=(*(char*)(p.second));
	}
	else if(p.first.CompareTo("int")==0){
		r+=(*(int*)(p.second));
	}
	else if(p.first.CompareTo("long")==0){
		r+=(*(Long_t*)(p.second));
	}
	else if(p.first.CompareTo("bool")==0){
		r+=(*(bool*)(p.second));
	}
	else if(p.first.CompareTo("float")==0){
		r+=(*(Float_t*)(p.second));
	}
	else if(p.first.CompareTo("double")==0){
		r+=(*(double*)(p.second));
	}
	else if(p.first.CompareTo("string")==0){
		r+=(*(string*)(p.second));
	}
	else if(p.first.CompareTo("TString")==0){
		r+=(*(TString*)(p.second));
	}
	return r;
}

bool Analyzer::CheckType(TString type){
	/// \MemberDescr
	/// \param type : type to check
	///
	/// Check that the given param type is allowed
	/// \EndMemberDescr

	if(type.CompareTo("char")) return true;
	else if(type.CompareTo("int")) return true;
	else if(type.CompareTo("long")) return true;
	else if(type.CompareTo("bool")) return true;
	else if(type.CompareTo("float")) return true;
	else if(type.CompareTo("double")) return true;
	else if(type.CompareTo("string")) return true;
	else if(type.CompareTo("TString")) return true;
	else return false;
}

void Analyzer::ApplyParam(TString paramName, TString paramValue){
	/// \MemberDescr
	/// \param paramName : Parameter name
	/// \param paramValue : Parameter value
	///
	/// Apply the parameter value to the analyzer
	/// \EndMemberDescr

	if(paramName.CompareTo("AutoUpdate", TString::kIgnoreCase)==0){
		//Add AutoUpdate plot
		SetPlotAutoUpdate(paramValue);
	}
	else if(paramName.CompareTo("UpdateInterval", TString::kIgnoreCase)==0){
		SetUpdateInterval(paramValue.Atoi());
	}
	else if(paramName.CompareTo("Verbose", TString::kIgnoreCase)==0){
		if(paramValue.IsDec()) SetVerbosity((AnalysisFW::VerbosityLevel)paramValue.Atoi());
		else{
			if(paramValue.CompareTo("kNo", TString::kIgnoreCase)==0) SetVerbosity(AnalysisFW::kNo);
			if(paramValue.CompareTo("kUser", TString::kIgnoreCase)==0) SetVerbosity(AnalysisFW::kUser);
			if(paramValue.CompareTo("kNormal", TString::kIgnoreCase)==0) SetVerbosity(AnalysisFW::kNormal);
			if(paramValue.CompareTo("kDebug", TString::kIgnoreCase)==0) SetVerbosity(AnalysisFW::kDebug);
		}
	}
	else SetParamValue(paramName, paramValue);
}

void Analyzer::SetUpdateInterval(int interval){
	/// \MemberDescr
	/// \param interval : Events interval at which the plots should be updated
	//
	/// Set the update interval for the plots
	/// \EndMemberDescr

	if(PrintVerbose(AnalysisFW::kNormal)) cout << "Setting plot update interval to " << interval << endl;
	fUpdateRate = interval;
}

void Analyzer::SetPlotAutoUpdate(TString name){
	/// \MemberDescr
	/// \param name : Name of the plot
	///
	/// Define the plot as AutoUpdate. Create the corresponding Canvas and Draw the plot
	/// \EndMemberDescr

	TCanvas *c;
	TString canvasName = TString("c_" + fAnalyzerName + "_") + name;
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
	if(PrintVerbose(AnalysisFW::kNormal)) cout << "Setting plot " << name << " as AutoUpdate." << endl;
}

void Analyzer::PrintInitSummary(MCSimple *fMCSimple){
	/// \MemberDescr
	/// \param fMCSimple : MCSimple instance corresponding to this Analyzer
	///
	/// Print a summary of the Analyzer after initialization. List of booked histograms, list of parameters, ...
	/// \EndMemberDescr

	map<TString, param_t>::iterator it;
	map<TString,TH1*>::iterator it1;
	map<TString,TH2*>::iterator it2;
	map<TString,TGraph*>::iterator itGraph;

	StringTable paramTable("List of parameters");
	StringTable histoTable("List of booked histograms");

	TString sDetAcc;

	//Fill parameters table
	paramTable.AddColumn("name", "Name");
	paramTable.AddColumn("type", "Type");
	paramTable.AddColumn("value", "Value");
	paramTable << sepr;
	paramTable << "Verbose";
	paramTable << "int";
	paramTable << fVerbosity;
	paramTable << "AutoUpdate Rate";
	paramTable << "int";
	paramTable << fUpdateRate;
	for(it=fParams.begin(); it!=fParams.end(); it++){
		paramTable << it->first;
		paramTable << it->second.first;
		paramTable << StringFromParam(it->first);
	}

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

	//About DetectorAcceptance
	if(fDetectorAcceptanceInstance==NULL) sDetAcc = "No";
	else{
		if(fDetectorAcceptanceInstance==fParent->IsDetectorAcceptanceInstaciated()) sDetAcc = "Global";
		else sDetAcc = "Local";
	}

	cout << "================================================================================" << endl;
	cout << endl << "\t *** Settings for Analyzer: " << fAnalyzerName << " ***" << endl << endl;

	paramTable.Print("\t");
	histoTable.Print("\t");

	cout << "\tDetector acceptance requested ? " << sDetAcc << endl << endl;

	fMCSimple->PrintInit();

	cout << "================================================================================" << endl;
}

void Analyzer::SetVerbosity(AnalysisFW::VerbosityLevel l){
	fVerbosity = l;
}

bool Analyzer::PrintVerbose(AnalysisFW::VerbosityLevel printAbove){
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


/// \MemberDescr
/// Create a new TTree in the output file
/// \EndMemberDescr
/// \Detailed
/// Create a new TTree in the output file
/// \EndDetailed
/// \param name : Name of the TTree
/// \param title : Title of the TTree
void Analyzer::OpenNewTree(TString name, TString title){
	TTree *outTree = new TTree(name, title);
	fOutTree.insert(pair<TString, TTree*>(name, outTree));
}

void Analyzer::FillTrees(){
	/// \MemberDescr
	/// Fill the TTrees created via OpenNewTree()
	/// \EndMemberDescr

	map<TString, TTree*>::iterator it;
	map<TString, TClonesArray>::iterator itCand;

	for(it=fOutTree.begin(); it!=fOutTree.end(); it++){
		it->second->Fill();
	}

	for(itCand=fExportCandidates.begin(); itCand!=fExportCandidates.end(); itCand++){
		itCand->second.Clear();
		fExportCandidatesNumber[itCand->first] = 0;
	}
}

void Analyzer::WriteTrees(){
	/// \MemberDescr
	/// Write the TTrees created via OpenNewTree() in the output file
	/// \EndMemberDescr

	map<TString, TTree*>::iterator it;

	for(it=fOutTree.begin(); it!=fOutTree.end(); it++){
		it->second->Write();
	}
}

void Analyzer::printNoMCWarning(){
	/// \MemberDescr
	/// Print a warning message when no MC data are present and the analyzer requires them.
	/// \EndMemberDescr

	if(!fNoMCWarned){
		cout << "Warning in analyzer " << fAnalyzerName << ": No MC data detected in this file." << endl;
		cout << "This analyzer is not allowed to run without MC data. If you want to change this behavior,"
				"please comment the following line in the Process method of the analyzer :" << endl;
		cout << "\"if(fMCSimple.fStatus == MCSimple::kEmpty){printNoMCWarning();return;}\"" << endl;

		fNoMCWarned = true;
	}
}

void Analyzer::printIncompleteMCWarning(int i){
	/// \MemberDescr
	/// Print a warning message when MC event is not complete.
	/// \EndMemberDescr

	cout << fAnalyzerName << ": Incomplete MC Event for event " << i << endl;
	if(!fIncompleteMCWarned){
		cout << "Warning in analyzer " << fAnalyzerName << ": Incomplete MC event detected." << endl;
		cout << "This analyzer is not allowed to run without the exact required event. If you want to change this behavior,"
				"please comment the following line in the Process method of the analyzer :" << endl;
		cout << "\"if(fMCSimple.fStatus == MCSimple::kMissing){printIncompleteMCWarning(iEvent);return;}\"" << endl;

		fIncompleteMCWarned = true;
	}
}

void Analyzer::StartOfBurst(){
	/// \MemberDescr
	/// This method is called when a new file is opened in the ROOT TChain (usually meaning end/start of burst in the normal NA62 data taking) + at the beginning of the first file
	/// \EndMemberDescr

	//TODO do some things
	StartOfBurstUser();
}

void Analyzer::EndOfBurst(){
	/// \MemberDescr
	/// This method is called when a new file is opened in the ROOT TChain (usually meaning end/start of burst in the normal NA62 data taking) + at the end of the last file
	/// \EndMemberDescr

	//TODO do some things
	EndOfBurstUser();
}

void Analyzer::StartOfRun(){
	/// \MemberDescr
	/// This method is called at the beginning of the processing (usually corresponding the a start of run)
	/// \EndMemberDescr

	//TODO do some things
	StartOfRunUser();
}

void Analyzer::EndOfRun(){
	/// \MemberDescr
	/// This method is called at the end of the processing (usually corresponding the a end of run)
	/// \EndMemberDescr

	//TODO do some things
	EndOfRunUser();
}

TH1* Analyzer::RequestHistogram(TString directory, TString name, bool appendOnNewFile){
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

	TH1* histo = fParent->GetInputHistogram(directory, name, appendOnNewFile);

	if(!histo) cout << fAnalyzerName << " : Requested input histogram was not found " << directory << "/" << name << endl;
	return histo;
}

void Analyzer::CreateStandardTree(TString name, TString title){
	/// \MemberDescr
	/// \param name : name of the TTree (for future reference in the code)
	/// \param title : name of the TTree in the ROOT file
	///
	/// Create a standardized output tree containing a list of KineParts. Use CreateStandardCandidate() to fill it.
	/// \EndMemberDescr

	fExportCandidatesNumber.insert(pair<TString, int>(name,0));
	fExportCandidates.insert(pair<TString, TClonesArray>(name, TClonesArray("KinePart", 10)));

	OpenNewTree(name, title);
	AddBranch<TClonesArray>(name, "fCandidates", &fExportCandidates[name]);
}

KinePart* Analyzer::CreateStandardCandidate(TString treeName){
	/// \MemberDescr
	/// \param treeName : name of the TTree to which the Candidate has to be added
	///
	/// Create a new KinePart and add it in the specified standardized output tree
	/// \EndMemberDescr

	KinePart * p = (KinePart*)fExportCandidates[treeName].ConstructedAt(fExportCandidatesNumber[treeName]);
	fExportCandidatesNumber[treeName]++;
	return p;
}

void* Analyzer::GetObjectVoid(TString name){
	/// \MemberDescr
	/// Internal interface to BaseAnalysis for GetObject method
	/// \EndMemberDescr

	return fParent->GetObject(name);
}

void* Analyzer::GetOutputVoid(TString name, OutputState &state){
	/// \MemberDescr
	/// Internal interface to BaseAnalysis for GetOutput method
	/// \EndMemberDescr

	return fParent->GetObject(name);
}

bool Analyzer::RequestTreeVoid(TString name, TString branchName, TString className, void* obj){
	/// \MemberDescr
	/// Internal interface to BaseAnalysis for RequestTree method
	/// \EndMemberDescr

	return fParent->RequestTree(name, branchName, className, obj);
}
