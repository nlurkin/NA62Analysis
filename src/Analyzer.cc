#include "Analyzer.hh"

#include "BaseAnalysis.hh"
#include "StringTable.hh"

namespace NA62Analysis {

Analyzer::Analyzer(Core::BaseAnalysis *ba) :
		UserMethods(ba),
		fNoMCWarned(false),
		fIncompleteMCWarned(false),
		fDetectorAcceptanceInstance(NULL),
		fState(kUninit),
		fExportEvent(false),
		fParticleInterface(ParticleInterface::GetParticleInterface())
{
	/// \MemberDescr
	/// \param ba : Parent BaseAnalysis instance
	/// Constructor
	/// \EndMemberDescr
}

Analyzer::Analyzer(const Analyzer& c) :
		UserMethods(c),
		fNoMCWarned(c.fNoMCWarned),
		fIncompleteMCWarned(c.fIncompleteMCWarned),
		fDetectorAcceptanceInstance(c.fDetectorAcceptanceInstance),
		fOutTree(c.fOutTree),
		fParams(c.fParams),
		fState(c.fState),
		fExportEvent(c.fExportEvent),
		fExportCandidates(c.fExportCandidates),
		fExportCandidatesNumber(c.fExportCandidatesNumber),
		fParticleInterface(ParticleInterface::GetParticleInterface())
{
	/// \MemberDescr
	/// \param c : Reference object to copy
	/// Constructor
	/// \EndMemberDescr
}

Analyzer::~Analyzer(){
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr
}

void Analyzer::PrintName() const{
	/// \MemberDescr
	/// Print the name of the Analyzer
	/// \EndMemberDescr

	cout << fAnalyzerName << endl;
}

TString Analyzer::GetAnalyzerName() const{
	/// \MemberDescr
	/// \return Name of the Analyzer
	/// \EndMemberDescr

	return fAnalyzerName;
}

void Analyzer::ExportEvent(){
	/// \MemberDescr
	/// Ask BaseAnalysis to store the event in the output file.
	/// Acts on all output TTrees (copy from inputs as well as custom and standard TTrees created with
	/// OpenNewTree() and CreateStandardTree()
	/// \EndMemberDescr

	fExportEvent = true;
}

bool Analyzer::GetExportEvent() const{
	/// \MemberDescr
	/// Did the analyzer asked to export the event?
	/// \return true if export was requested. Else false.
	/// \EndMemberDescr

	return fExportEvent;
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
	if(PrintVerbose(NA62Analysis::Verbosity::kNormal)) cout << "Setting parameter " << name << " of type " << p.first << " with value " << val << endl;

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

TString Analyzer::StringFromParam(TString name) const{
	/// \MemberDescr
	/// \param name : name of the parameter (previously defined with AddParam)
	///
	/// \return Value of the requested parameter as TString
	/// \EndMemberDescr

	param_t p = fParams.find(name)->second;

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

bool Analyzer::CheckType(TString type) const{
	/// \MemberDescr
	/// \param type : type to check
	///
	/// Check that the given param type is allowed
	/// \return true if the string type is one of the allowed ones. Else false.
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
		fHisto.SetPlotAutoUpdate(paramValue,fAnalyzerName);
		if(PrintVerbose(NA62Analysis::Verbosity::kNormal)) cout << "Setting plot " << paramValue << " as AutoUpdate." << endl;
	}
	else if(paramName.CompareTo("UpdateInterval", TString::kIgnoreCase)==0){
		fHisto.SetUpdateInterval(paramValue.Atoi());
	}
	else if(paramName.CompareTo("Verbose", TString::kIgnoreCase)==0){
		if(paramValue.IsDec()) SetVerbosity((NA62Analysis::Verbosity::VerbosityLevel)paramValue.Atoi());
		else{
			if(paramValue.CompareTo("kNo", TString::kIgnoreCase)==0) SetVerbosity(NA62Analysis::Verbosity::kNo);
			if(paramValue.CompareTo("kUser", TString::kIgnoreCase)==0) SetVerbosity(NA62Analysis::Verbosity::kUser);
			if(paramValue.CompareTo("kNormal", TString::kIgnoreCase)==0) SetVerbosity(NA62Analysis::Verbosity::kNormal);
			if(paramValue.CompareTo("kDebug", TString::kIgnoreCase)==0) SetVerbosity(NA62Analysis::Verbosity::kDebug);
		}
	}
	else SetParamValue(paramName, paramValue);
}

void Analyzer::PrintInitSummary() const{
	/// \MemberDescr
	///
	/// Print a summary of the Analyzer after initialization. List of booked histograms, list of parameters, ...
	/// \EndMemberDescr

	map<TString, param_t>::const_iterator it;

	StringTable paramTable("List of parameters");

	TString sDetAcc;

	//Fill parameters table
	paramTable.AddColumn("name", "Name");
	paramTable.AddColumn("type", "Type");
	paramTable.AddColumn("value", "Value");
	paramTable << sepr;
	paramTable << "Verbose" << "int" << GetVerbosityLevel();
	paramTable << "AutoUpdate Rate" << "int" << fHisto.GetUpdateInterval();
	for(it=fParams.begin(); it!=fParams.end(); it++){
		paramTable << it->first << it->second.first << StringFromParam(it->first);
	}

	//About DetectorAcceptance
	if(fDetectorAcceptanceInstance==NULL) sDetAcc = "No";
	else{
		if(fDetectorAcceptanceInstance==fParent->IsDetectorAcceptanceInstanciated()) sDetAcc = "Global";
		else sDetAcc = "Local";
	}

	cout << "================================================================================" << endl;
	cout << endl << "\t *** Settings for Analyzer: " << fAnalyzerName << " ***" << endl << endl;

	paramTable.Print("\t");
	fHisto.PrintInitSummary();

	cout << "\tDetector acceptance requested ? " << sDetAcc << endl << endl;

	fMCSimple.PrintInit();

	cout << "================================================================================" << endl;
}

void Analyzer::OpenNewTree(TString name, TString title){
	/// \MemberDescr
	/// \param name : Name of the TTree
	/// \param title : Title of the TTree
	///
	/// Create a new TTree in the output file
	/// \EndMemberDescr
	TTree *outTree = new TTree(name, title);
	fOutTree.insert(pair<TString, TTree*>(name, outTree));
}

void Analyzer::FillTrees(){
	/// \MemberDescr
	/// Fill the TTrees created via OpenNewTree() or CreateStandardTree()
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

void Analyzer::printNoMCWarning() const{
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

double Analyzer::compareToReferencePlot(TString h1, bool KS) {
	/// \MemberDescr
	/// \param h1 : Pointer to the 1D histogram to compare.
	/// \param KS : If true, use Kolmogorov-Smirnov test, else use chi square test
	///
	/// Compare similarity between two 1D histograms, returning the probability of
	///	the tested (h1) histogram following the same distribution as a reference
	///	histogram. The reference histogram is an histogram with same name extracted
	/// from the reference file. If no reference file is specified in the command line
	/// parameters, this method returns immediately.
	/// \return -1 in case of failure (no histogram). Else returns the value returned by HistoHandler::compareToReferencePlot.
	/// \EndMemberDescr

	TH1* h = fHisto.GetTH1(h1);
	TString name = h->GetName();

	if(!fParent->IsHistoType()) return -1;
	TH1* hRef = fParent->GetIOHisto()->GetReferenceTH1(name);

	if(!hRef || !h) return -1.;
	return fHisto.compareToReferencePlot(hRef, h, KS);
}

void Analyzer::FillMCSimple(Event* mcTruthEvent) {
	/// \MemberDescr
	/// \param mcTruthEvent : Is the event coming from the TTree. Is extracted in BaseAnalysis
	/// \param verbosity : If set to 1, will print the missing particles. If set to 2, will print also the MC particles found in the event.
	///
	/// Extract informations from current Event and store them internally for later easy access
	/// \EndMemberDescr

	fMCSimple.GetRealInfos(mcTruthEvent);
}

void Analyzer::printIncompleteMCWarning(int iEvent) const{
	/// \MemberDescr
	/// \param iEvent : Event index
	/// Print a warning message when MC event is not complete.
	/// \EndMemberDescr

	cout << fAnalyzerName << ": Incomplete MC Event for event " << iEvent << endl;
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
	/// \return Pointer to a new KinePart.
	/// \EndMemberDescr

	KinePart * p = (KinePart*)fExportCandidates[treeName].ConstructedAt(fExportCandidatesNumber[treeName]);
	fExportCandidatesNumber[treeName]++;
	return p;
}

void Analyzer::AddParam(TString name, char *address, char defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("char", address)));
}

void Analyzer::AddParam(TString name, int *address, int defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("int", address)));
}

void Analyzer::AddParam(TString name, long *address, long defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("long", address)));
}

void Analyzer::AddParam(TString name, bool *address, bool defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("bool", address)));
}

void Analyzer::AddParam(TString name, float *address, float defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("float", address)));
}

void Analyzer::AddParam(TString name, double *address, double defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("double", address)));
}

void Analyzer::AddParam(TString name, string *address, string defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("string", address)));
}

void Analyzer::AddParam(TString name, TString *address, TString defaultValue) {
	/// \MemberDescr
	/// \param name : Name of the parameter
	/// \param address : Pointer to the variable that will be assigned the parameter value
	/// \param defaultValue : Default value if the parameter is not specified
	///
	/// Add a new parameter to the analyzer. The parameter is initialized with the defaultValue.
	/// The value can be overwritten using a runtime configuration file or the -p command line option.
	/// \EndMemberDescr
	*address = defaultValue;
	fParams.insert(pair<TString, param_t>(name, param_t("TString", address)));
}

} /* namespace NA62Analysis */
