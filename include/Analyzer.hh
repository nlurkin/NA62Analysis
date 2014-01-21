#ifndef ANALYZER_HH
#define ANALYZER_HH

#include <TString.h>
#include <TChain.h>
#include <TDetectorVEvent.hh>
#include <TTree.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>
#include <TGraph.h>
#include "MCSimple.hh"
#include "functions.hh"
#include "FWEnums.hh"
#include "ParticleInterface.hh"
#include "NA62Constants.hh"

using namespace std;
class BaseAnalysis;
class DetectorAcceptance;
typedef pair<TString, void*> param_t;

/// \class Analyzer
/// \Brief 
/// Abstract base class for every analyzer
/// \EndBrief 
///
/// \Detailed
/// Implements the common methods and define abstract methods signature
/// \EndDetailed

class Analyzer
{
public:
	enum AnalyzerState {kReady, kUninit}; ///< Possible states for the analyzer
	enum OutputState {kOUninit, kOValid, kOInvalid}; ///< List of possible states for the output variables.

	Analyzer(BaseAnalysis* ba);
	virtual ~Analyzer();

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

	//Request new tree to analyze
	void RequestTree(TString name, TDetectorVEvent *evt);
	template <class T>
	void RequestTree(TString name, TString branchName, TString className, T* obj){
		if(!RequestTreeVoid(name, branchName, className, obj)){
			delete obj;
		}
	}

	//Method to get an event
	TDetectorVEvent *GetEvent(TString name);
	template <class T>
	T* GetObject(TString name){
		return (T*)GetObjectVoid(name);
	};

	//Request input histogram
	TH1* RequestHistogram(TString directory, TString name, bool appendOnNewFile);

	void StartOfBurst();
	void EndOfBurst();
	void StartOfRun();
	void EndOfRun();
	void PreProcess();

	//Abstract methods to be implemented in each Analyzer
	virtual void InitHist() = 0;
	virtual void DefineMCSimple(MCSimple *fMCSimple) = 0;
	virtual void InitOutput() = 0;
	virtual void Process(int i, MCSimple &fMCSimple, Event* MCTruthEvent) = 0;
	virtual void StartOfBurstUser() {};
	virtual void EndOfBurstUser() {};
	virtual void StartOfRunUser() {};
	virtual void EndOfRunUser() {};
	virtual void PostProcess() = 0;
	virtual void ExportPlot() = 0;
	virtual void DrawPlot() = 0;

	//Export all histograms into output trees
	void ExportAllPlot(map<TString,TTree*> &trees, map<TString,void*> &branches);

	//Methods for drawing plots on screen
	void DrawAllPlots();
	void UpdatePlots(int evtNbr);
	void SetUpdateInterval(int interval);

	//Save all plots into output file
	void SaveAllPlots();

	//Analyzer name
	void PrintName();
	TString GetAnalyzerName();
	void PrintInitSummary(MCSimple *fMCSimple);
	void SetVerbosity(AnalysisFW::VerbosityLevel l);

	//Methods for setting output
	void RegisterOutput(TString name, void* address);
	void SetOutputState(TString name, OutputState state);

	//Methods for getting output
	const void *GetOutput(TString name, OutputState &state);
	template <class T>
	const T* GetOutput(TString name, OutputState &state){
		return (T*)GetOutputVoid(name, state);
	};

	//Get global DetectorAcceptance instance
	DetectorAcceptance *GetDetectorAcceptanceInstance();

	//Methods for exporting an event
	void ExportEvent();
	bool GetExportEvent();

	//Methods for output trees
	void OpenNewTree(TString name, TString title);
	template <class T>
	void AddBranch(TString name, TString branchName, T *pointer){
		if(fOutTree.count(name)==0){
			cerr << fAnalyzerName << ": Output TTree " << name << " does not exist" << endl;
		}
		else{
			fOutTree[name]->Branch(branchName, pointer);
		}
	}
	void FillTrees();
	void WriteTrees();
	void CreateStandardTree(TString name, TString title);
	KinePart* CreateStandardCandidate(TString treeName);

	//Methods to handle parameters
	template <class T>
	void AddParam(TString name, TString type, T* address, T defaultValue){
		if(!CheckType(type)){
			cout << "Error when adding parameter " << name << " : type not supported " << type << endl;
			return;
		}
		*address = defaultValue;
		fParams.insert(pair<TString, param_t>(name, param_t(type, address)));
	}
	void ApplyParam(TString paramName, TString value);
	bool PrintVerbose(AnalysisFW::VerbosityLevel printAbove);


protected:
	//Internal methods used for setting parameters
	bool CheckType(TString type);
	void SetParamValue(TString name, TString val);
	TString StringFromParam(TString name);

	//Internal method to draw the plot and allow auto-update
	//void SetPlotAutoUpdate(TString name);

	//Collection of warning messages
	void printNoMCWarning();
	void printIncompleteMCWarning(int i);

	void* GetObjectVoid(TString name);
	const void* GetOutputVoid(TString name, OutputState &state);
	bool RequestTreeVoid(TString name, TString branchName, TString className, void* obj);
	//void Mkdir(TString name);

	bool fNoMCWarned; ///< Has the user been warned that No MC data have been found and that he can change this behavior?
	bool fIncompleteMCWarned; ///< Has the user been warned that the MC event does not match the requested one in fMCSimple and that he can change this behavior?

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Pointer to DetectorAcceptance instance. Initialize if used.

	/*//Histogram containers
	map<TString,TH1*> fHisto; ///< Container for TH1
	map<TString,TH2*> fHisto2; ///< Container for the TH2
	map<TString,TGraph*> fGraph; ///< Container for the TGraph
	map<TString,int> fPoint; ///< Container for the number of points in each TGraph
	vector<TCanvas*> fCanvas; ///< Container for the TCanvas
	map<TString,TTree*> fOutTree; ///< Container for the output TTrees

	set<TString> fAutoUpdateList;
	map<TString, TString> fPlotsDirectory; ///< Matching between plot name and directory name*/
	HistoHandling fHisto;

	//Parameters container
	map<TString,param_t> fParams; ///< Container for parameters

	TString fAnalyzerName; ///< Name of the analyzer
	AnalyzerState fState; ///< State of the analyzer
	BaseAnalysis *fParent; ///< Pointer to the BaseAnalysis instance containing the analyze

	bool fExportEvent; ///< Do we request to export this event?
	int fUpdateRate; ///< Event interval at which the plots should be updated
	AnalysisFW::VerbosityLevel fVerbosity; ///< Verbosity level

	map<TString, TClonesArray> fExportCandidates; ///< Array containing candidates to be recorded in the standard output tree
	map<TString, int> fExportCandidatesNumber; ///< Number of Export candidates

	ParticleInterface *fParticleInterface; ///< Pointer to the ParticleInterface instance
};

#endif
