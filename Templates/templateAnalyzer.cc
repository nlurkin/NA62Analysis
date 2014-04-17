//Mandatory includes
#include <stdlib.h>
#include <iostream>
#include <TChain.h>
#include "templateAnalyzer.hh"
#include "MCSimple.hh"
#include "functions.hh"
#include "Event.hh"
#include "Persistency.hh"
using namespace std;
using namespace NA62Constants;

/// \class templateAnalyzer
/// \Brief 
/// Describe your Analyzer
/// \EndBrief
///
/// \Detailed
/// Describe your Analyzer
/// \EndDetailed

templateAnalyzer::templateAnalyzer(BaseAnalysis *ba) : Analyzer(ba)
{
	/// \MemberDescr
	/// \param ba : parent BaseAnalysis
	///
	///Specify the trees you want to use and the event class corresponding\n
	///Don't try to load MCTruth tree (RUN_0 or Event). Use the MCTruthEvent in Process function instead. Problems when opening twice the same tree.\n
	///Example with RecoEvent\n
	///	RequestTree("GigaTracker", new TRecoGigaTrackerEvent);\n
	///Example with MC Event\n
	///	RequestTree("GigaTracker", new TGigaTrackerEvent);\n
	///Example with generic tree\n
	///	RequestTree<MyClass>("MyTree", "BranchName", "MyClass", new MyClass);\n
	////\n
	///Initialize DetectorAcceptance if needed\n
	///use of global instance\n
	///	fDetectorAcceptanceInstance = GetDetectorAcceptanceInstance();\n
	///use of local instance\n
	///	fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.root");

	fAnalyzerName = "templateAnalyzer";

/*$$TREEREQUEST$$*/

}

void templateAnalyzer::InitOutput(){
	/// \MemberDescr
	/// Register the output variables of the analyzer.\n
	/// Call\n
	/// 	RegisterOutput("outputName", &variableName)\n
	/// for each variable that should be in the output of the Analyzer\n
	/// The name of the analyzer will be prepended to the outputName (to avoid collisions with other analyzers)\n
	/// variableName should be the name of a variable declared in the definition of the class\n
	/// \n
	/// Call\n
	/// 	AddParam("paramName", "paramType", &variableName, defaultValue);\n
	/// for each parameter of the analyzer. These parameters can be set when starting the FW from the command line with the -p option.\n
	/// paramName is the name of the parameter in the command line\n
	/// paramType is the type of the parameter (allowed : bool, int, long, float, double, char, string, TString)\n
	/// variableName is the name of the variable that should be declared in the definition of the class\n
	/// defaultValue is the default value if not specified in the command line\n
	/// \n
	/// To create a new TTree in the output file, call\n
	/// 	void OpenNewTree("TTreeName", "TTreeTitle");\n
	/// TTreeName is the name of the TTree (will be used to refer to this TTree later)\n
	/// TTreeTitle is the title of the TTree\n
	/// Call\n
	/// 	void AddBranch<VariableType>("TTreeName", "BranchName", &pointer);\n
	/// VariableType is the type of the variable for this branch (fundamental data type or class)\n
	/// TTreeName is the name of the TTree to add this branch\n
	/// BranchName is the name of the branch\n
	/// pointer is a pointer to the variable (should be declared in the header file)\n
	/// \n
	/// To create a standard TTree containing KineParts (for candidates) use\n
	///     CreateStandardTree("TTreeName", "TTreeTitle");
	/// \EndMemberDescr
}

void templateAnalyzer::InitHist(){
	/// \MemberDescr
	/// Book and Initialize histograms in this function.\n
	/// Same function to Book TH1, TH2, TGraph and TGraphAsymmErrors (anything derived from TH1 or TGraph)\n
	///  BookHisto(histogram*)\n
	/// If isAutotUpdate is true, this histogram will be drawn and updated regularly during the processing. (default=false)
	/// The refresh interval can be set with (default=10)
	///  SetUpdateInterval(interval)
	/// Defining plots as AutoUpdate and setting the interval can also be done at runtime with a configuration file
	/// Example\n
	/// 	BookHisto(new TH2I("PartEnergy", "Energy as a function of particle", 0, 0, 0, Bins, MinEnergy, MaxEnergy));\n
	/// \n
	/// Booking of counters and creation of EventFraction can be done here\n
	///	BookCounter(name)\n
	///	NewEventFraction(name)\n
	/// Example\n
	/// 	BookCounter("Total");\n
	/// 	BookCounter("PassCuts");\n
	/// 	NewEventFraction("Cuts");\n
	/// \n
	/// Add the counters to the EventFraction\n
	/// 	AddCounterToEventFraction(EventFractionName, CounterName)\n
	/// Example\n
	/// 	AddCounterToEventFraction("Cuts", "Total");\n
	/// 	AddCounterToEventFraction("Cuts", "PassCuts");\n
	/// \n
	/// Then define which counter represents the sample size\n
	/// 	DefineSampleSizeCounter(EventFractionName, CounterName)\n
	/// Example\n
	/// 	DefineSampleSizeCounter("Cuts", "Total");\n
	/// \n
	/// You can retrieve histograms from the input ROOT file (Anything derived from TH1) with\n
	///  RequestHistogram("TDirectoryName", "HistogramName", appendOnNewFile);\n
	/// appendOnNewFile is a boolean. If set to true, each time a new file is opened the content\n
	/// of the histogram will be appended to the content of the previous one. If set to false, the content \n
	/// of the histogram is replaced each time a new file is opened.
	/// \EndMemberDescr
}

void templateAnalyzer::DefineMCSimple(MCSimple *fMCSimple){
	/// \MemberDescr
	/// \param fMCSimple : MCSimple
	///
	/// Setup of fMCSimple. You must specify the generated MC particles you want.\n
	/// Add particles you want to recover from fMCSimple\n
	/// int particleID = fMCSimple->AddParticle(parentID, pdgCode)\n
	/// parentID : 	0=no parent (=beam particle)\n
	/// 	...\n
	/// Example : you want to retrieve the kaon from the beam, the pi0 an pi+ from the beam kaon and the 2 photons coming from the previous pi0 decay :\n
	/// 	int kaonID = fMCSimple->AddParticle(0, 321) //Ask beam kaon (sequence ID=1)\n
	/// 	fMCSimple->AddParticle(kaonID, 211) //Ask pi+ from previous kaon (sequence ID=2)\n
	/// 	int pi0ID = fMCSimple->AddParticle(kaonID, 111) //Ask pi0 from previous kaon (sequence ID=3)\n
	/// 	fMCSimple->AddParticle(pi0ID, 22) //Ask first gamma from previous pi0 (sequence ID=4)\n
	/// 	fMCSimple->AddParticle(pi0ID, 22) //Ask second gamma from previous pi0 (sequence ID=4)
	///
	/// @see ROOT TDatabasePDG for a list of PDG codes and particle naming convention
	/// \EndMemberDescr
}

void templateAnalyzer::StartOfRunUser(){
	/// \MemberDescr
	/// This method is called at the beginning of the processing (corresponding to a start of run in the normal NA62 data taking)\n
	/// Do here your start of run processing if any
	/// \EndMemberDescr
}

void templateAnalyzer::StartOfBurstUser(){
	/// \MemberDescr
	/// This method is called when a new file is opened in the ROOT TChain (corresponding to a start/end of burst in the normal NA62 data taking) + at the beginning of the first file\n
	/// Do here your start/end of burst processing if any
	/// \EndMemberDescr
}

void templateAnalyzer::EndOfBurstUser(){
	/// \MemberDescr
	/// This method is called when a new file is opened in the ROOT TChain (corresponding to a start/end of burst in the normal NA62 data taking) + at the end of the last file\n
	/// Do here your start/end of burst processing if any
	/// \EndMemberDescr
}

void templateAnalyzer::Process(int iEvent, MCSimple &fMCSimple, Event* MCTruthEvent){
	/// \MemberDescr
	/// \param iEvent : Event number
	/// \param fMCSimple : MCSimple
	/// \param MCTruthEvent : Complete set of MC particles
	///
	/// Main process method. Called on each event. Write you analysis here.\n
	/// Ask the fMCSimple to have the complete set of particles we specified\n
	/// You can retrieve MC particles from the fMCSimple Set with (return a vector<KinePart*>\n
	///  fMCSimple["particleName"]\n
	///  fMCSimple[pdgID]\n
	/// Example\n
	/// 	fMCSimple["K+"][index] for the kaon\n
	/// 	fMCSimple["pi+"][index] for the positive pion\n
	/// 	fMCSimple["gamma"][index] for the photon\n
	/// The number in the brackets is the index of the particle (if you asked for two photons in the set, you can ask fMCSimple["gamma"][0] for the first one and fMCSimple["gamma"][1] for the second)\n
	/// \n
	/// If you need a property of a particle, you can make a call to fParticleInterface (instance of the ParticleInterface class).\n
	///	This class has two methods FindParticle that will return a TParticlePDG with the required particle. You can search by pdgID or by name.\n
	///	This class also provide two methods to switch between particle name and pdgID if necessary.\n
	///	Example\n
	///		double kaonMass = fParticleInterface->FindParticle(321).Mass();
	///		double pi0Lifetime = fParticleInterface->FindParticle("pi0").Lifetime();
	/// You can retrieve the events from the trees with\n
	/// 	(eventClass*)GetEvent("treeName");\n
	/// You can retrieve data from generic TTrees with\n
	/// 	GetObject<MyClass>("treeName");\n
	/// \n
	/// You can retrieve the histograms you booked (for drawing, changing, filling, ...) with\n
	/// 	fHisto.GetTH1("histoName") for TH1\n
	/// 	fHisto.GetTH2("histoName") for TH2\n
	/// 	fHisto.GetGraph("graphName") for TGraph and TGraphAsymmErrors\n
	/// To fill the histograms you can use\n
	/// 	FillHisto("histoName", values)\n
	/// where values are the same parameters as if you call histogram->Fill(values) (x,y,weight,...)\n
	/// If the histogram is not found, an error message is printed\n
	/// \n
	/// Modify a counter with one of the following methods\n
	/// 	IncrementCounter(name)\n
	/// 	IncrementCounter(name, delta)\n
	/// 	DecrementCounter(name)\n
	/// 	DecrementCounter(name, delta)\n
	/// 	SetCounterValue(name, value)\n
	/// \n
	/// For use of fGeom, read DetectorAcceptance class.\n
	///	WARNING: this class provides "exact" results, there is not tolerance. If the particle\n
	///	passes in the sensitive volume of a detector it's considered as detected, wether it's close\n
	///	to the edge or not. But as the class gives you the position of the passage point and the \n
	///	estimated path length in the sensitive volume, you can apply further cuts from this\n
	///	information at your convenience.
	/// \n
	/// To use the output of a different analyzer, use\n
	/// 	outputType *var = GetOutput<outputType>("analyzerName.outputName", state);\n
	/// Where outputType is the variable type and state is of type outputState\n
	/// This function returns a void* and you have to explicitly cast it to the real object type\n
	/// State is set with the state of the variable (kOUninit, kOInvalid ,kOValid). The value of the output should only be trusted if state == kOValid\n
	/// example : TLorentzVector vertex = *(TLorentzVector*)GetOutput("simpleVertexAnalyzer.vertex", state);\n
	/// \n
	/// Before starting the processing of an event, the state flag of each output variable is reset to kOUninit\n
	/// When setting the value of an output variable, don't forget to set appropriately the state flag to either kOValid or kOInvalid\n
	/// to indicate if the value can/can't be used in other analyzer\n
	/// 	SetOutputState("outputName", kOValid);\n
	/// \n
	/// If you want to append a candidate in one of your standard output Tree, use\n
	/// 	KinePart *candidate = CreateStandardCandidate("treeName");\n
	/// and fill the properties of your candidate. It will be automatically written in the output tree.\n
	///	\n
	/// If you want to save this event in your custom and standard TTrees (not the input tree replication), call\n
	/// 	FillTrees();\n
	/// This will call the Fill method of every TTree created in this analyzer.\n
	///	\n
	/// If you want to replicate this event in the output file, call\n
	/// 	ExportEvent();\n
	/// The structure of all the trees that have been opened (by all Analyzer) will be copied in the output file\n
	/// and the events for which at least one analyzer called ExportEvent() will be replicated in the output trees.
	/// @see ROOT TParticlePDG for the particle properties
	/// @see ROOT TDatabasePDG for a list of PDG codes and particle naming convention
	/// \EndMemberDescr
	//
	//Ask the fMCSimple to have the complete set of particles we specified
	//If the analyzer can run without the complete set, comment the line
	if(fMCSimple.fStatus == MCSimple::kMissing){printIncompleteMCWarning(iEvent);return;}
	//If the analyzer can run without MC data, comment the line
	if(fMCSimple.fStatus == MCSimple::kEmpty){printNoMCWarning();return;}

/*$$GETEVENTS$$*/

}

void templateAnalyzer::PostProcess(){
	/// \MemberDescr
	/// This function is called after an event has been processed by all analyzers. It could be used to free some memory allocated
	/// during the Process.
	/// \EndMemberDescr

}

void templateAnalyzer::EndOfRunUser(){
	/// \MemberDescr
	/// This method is called at the end of the processing (corresponding to a end of run in the normal NA62 data taking\n)
	/// Do here your end of run processing if any
	/// \EndMemberDescr

}

void templateAnalyzer::ExportPlot(){
	/// \MemberDescr
	/// This method is called at the end of processing to save plots.\n
	/// If you want to save them all, just call\n
	/// 	SaveAllPlots();\n
	/// Or you can just save the ones you want with\n
	/// 	histogram->Write()\n
	/// \EndMemberDescr
}

void templateAnalyzer::DrawPlot(){
	/// \MemberDescr
	/// This method is called at the end of processing to draw plots.
	/// If you want to draw all the plots, just call
	/// 	DrawAllPlots();
	/// Or do as usual (TCanvas, Draw, ...)
	/// \EndMemberDescr
}
