#ifndef ANALYZER_HH
#define ANALYZER_HH

#include "MCSimple.hh"
#include "FWEnums.hh"
#include "NA62Constants.hh"
#include "UserMethods.hh"

using namespace std;
typedef pair<TString, void*> param_t;

/// \class Analyzer
/// \Brief 
/// Abstract base class for every analyzer
/// \EndBrief 
///
/// \Detailed
/// Implements the common methods and define abstract methods signature.
/// \EndDetailed
class Analyzer : public UserMethods
{
public:
	/// Possible states for the analyzer
	enum AnalyzerState {kReady, kUninit};

	Analyzer(BaseAnalysis* ba);
	Analyzer(const Analyzer& c);
	virtual ~Analyzer();

	//Run/burst events
	void StartOfBurst();
	void EndOfBurst();
	void StartOfRun();
	void EndOfRun();
	void PreProcess();

	//Abstract methods to be implemented in each Analyzer
	virtual void InitHist() = 0; ///< Pure virtual method to be implemented by user analyzer
	virtual void DefineMCSimple(MCSimple *fMCSimple) = 0; ///< Pure virtual method to be implemented by user analyzer
	virtual void InitOutput() = 0; ///< Pure virtual method to be implemented by user analyzer
	virtual void Process(int iEvent) = 0; ///< Pure virtual method to be implemented by user analyzer
	virtual void PostProcess() = 0; ///< Pure virtual method to be implemented by user analyzer
	virtual void ExportPlot() = 0; ///< Pure virtual method to be implemented by user analyzer
	virtual void DrawPlot() = 0; ///< Pure virtual method to be implemented by user analyzer
	virtual void StartOfBurstUser() {
		/// \MemberDescr Executed at start of burst (new file). No default action to do.
		/// \EndMemberDescr
	};
	virtual void EndOfBurstUser() {
		/// \MemberDescr Executed at start of burst (new file). No default action to do.
		/// \EndMemberDescr
	};
	virtual void StartOfRunUser() {
		/// \MemberDescr Executed at start of burst (new file). No default action to do.
		/// \EndMemberDescr
	};
	virtual void EndOfRunUser() {
		/// \MemberDescr Executed at start of burst (new file). No default action to do.
		/// \EndMemberDescr
	};

	//Analyzer name
	void PrintName() const;
	TString GetAnalyzerName() const ;
	void PrintInitSummary(const MCSimple * const fMCSimple) const;
	void SetVerbosity(AnalysisFW::VerbosityLevel l);

	//Methods for exporting an event
	void ExportEvent();
	bool GetExportEvent() const;

	//Methods for output trees
	void OpenNewTree(TString name, TString title);
	template <class T>
	void AddBranch(TString name, TString branchName, T *pointer){
		/// \MemberDescr
		/// \param name : Name of the output tree
		///	\param branchName : Name of the branch
		/// \param pointer : Pointer to the object used for filling the tree
		///
		/// Add a new branch to an existing output tree.
		/// \EndMemberDescr
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
		/// \MemberDescr
		/// \param name : Name of the parameter
		///	\param type : Type of the parameter (allowed: char, int, long, bool, float, double, string, TString)
		/// \param address : Pointer to the variable that will be assigned the parameter value
		/// \param defaultValue : Default value if the parameter is not specified
		///
		/// Add a new branch to an existing output tree.
		/// \EndMemberDescr
		if(!CheckType(type)){
			cout << "Error when adding parameter " << name << " : type not supported " << type << endl;
			return;
		}
		*address = defaultValue;
		fParams.insert(pair<TString, param_t>(name, param_t(type, address)));
	}
	void ApplyParam(TString paramName, TString paramValue);

	double compareToReferencePlot(TString h1, bool KS);

protected:
	//Internal methods used for setting parameters
	bool CheckType(TString type) const;
	void SetParamValue(TString name, TString val);
	TString StringFromParam(TString name) const;

	//Collection of warning messages
	void printNoMCWarning() const;
	void printIncompleteMCWarning(int i) const;

	mutable bool fNoMCWarned; ///< Has the user been warned that No MC data have been found and that he can change this behavior?
	mutable bool fIncompleteMCWarned; ///< Has the user been warned that the MC event does not match the requested one in fMCSimple and that he can change this behavior?

	DetectorAcceptance *fDetectorAcceptanceInstance; ///< Pointer to DetectorAcceptance instance. Initialize if used.

	map<TString,TTree*> fOutTree; ///< Container for the output TTrees

	//Parameters container
	map<TString,param_t> fParams; ///< Container for parameters

	AnalyzerState fState; ///< State of the analyzer

	bool fExportEvent; ///< Do we request to export this event?

	map<TString, TClonesArray> fExportCandidates; ///< Array containing candidates to be recorded in the standard output tree
	map<TString, int> fExportCandidatesNumber; ///< Number of Export candidates

	ParticleInterface *fParticleInterface; ///< Pointer to the ParticleInterface instance
};

#endif
