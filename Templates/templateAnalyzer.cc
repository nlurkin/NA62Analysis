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
	/// Specify the trees you want to use and the event class corresponding\n
	/// Don't try to load MCTruth tree (RUN_0 or Event). Use the MCTruthEvent in Process function instead. Problems when opening twice the same tree.\n
	/// Example with RecoEvent\n
	///	\code
	///		RequestTree("GigaTracker", new TRecoGigaTrackerEvent);
	/// \endcode
	/// Example with MC Event\n
	///	\code
	///		RequestTree("GigaTracker", new TGigaTrackerEvent);
	/// \endcode
	/// Example with generic tree\n
	///	\code
	///		RequestTree<MyClass>("MyTree", "BranchName", "MyClass", new MyClass);
	/// \endcode
	//// \n
	/// Initialize DetectorAcceptance if needed\n
	/// use of global instance\n
	///	\code
	///		fDetectorAcceptanceInstance = GetDetectorAcceptanceInstance();
	/// \endcode
	/// use of local instance\n
	///	\code
	///		fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.root");
	/// \endcode

	fAnalyzerName = "templateAnalyzer";

/*$$TREEREQUEST$$*/

}

void templateAnalyzer::InitOutput(){
	/// \MemberDescr
	/// Register the output variables of the analyzer.\n
	/// Call: \n
	///	\code
	/// 	RegisterOutput("outputName", &variableName)
	/// \endcode
	/// for each variable that should be in the output of the Analyzer\n
	/// The name of the analyzer will be prepended to the outputName (to avoid collisions with other analyzers)\n
	/// variableName should be the name of a variable declared in the definition of the class\n
	/// \n
	/// Call: \n
	///	\code
	/// 	AddParam("paramName", "paramType", &variableName, defaultValue);
	/// \endcode
	/// for each parameter of the analyzer. These parameters can be set when starting the FW from the command line with the -p option.\n
	/// paramName is the name of the parameter in the command line\n
	/// paramType is the type of the parameter (allowed : bool, int, long, float, double, char, string, TString)\n
	/// variableName is the name of the variable that should be declared in the definition of the class\n
	/// defaultValue is the default value if not specified in the command line\n
	/// \n
	/// To create a new TTree in the output file, call: \n
	///	\code
	/// 	void OpenNewTree("TTreeName", "TTreeTitle");
	/// \endcode
	/// TTreeName is the name of the TTree (will be used to refer to this TTree later)\n
	/// TTreeTitle is the title of the TTree\n
	/// \n
	/// To add a branch to the newly created TTree, call: \n
	///	\code
	/// 	void AddBranch<VariableType>("TTreeName", "BranchName", &pointer);
	/// \endcode
	/// VariableType is the type of the variable for this branch (fundamental data type or class)\n
	/// TTreeName is the name of the TTree to add this branch\n
	/// BranchName is the name of the branch\n
	/// pointer is a pointer to the variable (should be declared in the header file)\n
	/// \n
	/// To create a standard TTree containing KineParts (for candidates) use\n
	///	\code
	///     CreateStandardTree("TTreeName", "TTreeTitle");
	/// \endcode
	/// \EndMemberDescr
}

void templateAnalyzer::InitHist(){
	/// \MemberDescr
	/// Book and Initialize histograms in this function.\n
	/// Same function to Book TH1, TH2, TGraph and TGraphAsymmErrors (anything derived from TH1 or TGraph)
	///	\code
	/// 	BookHisto(histogram*)
	/// \endcode
	/// If isAutotUpdate is true, this histogram will be drawn and updated regularly during the processing (default=false).\n
	/// The refresh interval can be set with (default=10):
	/// \code
	/// 	SetUpdateInterval(interval)
	/// \endcode
	/// Defining plots as AutoUpdate and setting the interval can also be done at runtime with a configuration file.\n
	/// \n
	/// Example of booking an histogram: \n
	///	\code
	/// 	BookHisto(new TH2I("PartEnergy", "Energy as a function of particle", 0, 0, 0, Bins, MinEnergy, MaxEnergy));
	/// \endcode
	/// Booking of counters and creation of EventFraction can be done here with\n
	///	\code
	///		BookCounter(name)
	///		NewEventFraction(name)
	/// \endcode
	/// Example\n
	///	\code
	/// 	BookCounter("Total");
	/// 	BookCounter("PassCuts");
	/// 	NewEventFraction("Cuts");
	/// \endcode
	/// Add the counters to the EventFraction\n
	///	\code
	/// 	AddCounterToEventFraction(EventFractionName, CounterName)
	/// \endcode
	/// Example\n
	///	\code
	/// 	AddCounterToEventFraction("Cuts", "Total");
	/// 	AddCounterToEventFraction("Cuts", "PassCuts");
	/// \endcode
	/// Then define which counter represents the sample size\n
	///	\code
	/// 	DefineSampleSizeCounter(EventFractionName, CounterName)
	/// \endcode
	/// Example\n
	///	\code
	/// 	DefineSampleSizeCounter("Cuts", "Total");
	/// \endcode
	/// You can retrieve histograms from the input ROOT file (Anything derived from TH1) with\n
	///	\code
	/// 	RequestHistogram("TDirectoryName", "HistogramName", appendOnNewFile);
	/// \endcode
	/// appendOnNewFile is a boolean. If set to true, each time a new file is opened the content
	/// of the histogram will be appended to the content of the previous one. If set to false, the content
	/// of the histogram is replaced each time a new file is opened.
	/// \EndMemberDescr
}

void templateAnalyzer::DefineMCSimple(MCSimple *fMCSimple){
	/// \MemberDescr
	/// \param fMCSimple : MCSimple
	///
	/// Setup of fMCSimple. You must specify the generated MC particles you want.\n
	/// Add particles you want to recover from fMCSimple\n
	///	\code
	/// 	int particleID = fMCSimple->AddParticle(parentID, pdgCode)
	///	\endcode
	/// parentID : 	0=no parent (=beam particle)\n
	/// 	...\n
	/// Example : you want to retrieve the kaon from the beam, the pi0 an pi+ from the beam kaon and the 2 photons coming from the previous pi0 decay :\n
	///	\code
	/// 	int kaonID = fMCSimple->AddParticle(0, 321) //Ask beam kaon (sequence ID=1)
	/// 	fMCSimple->AddParticle(kaonID, 211) //Ask pi+ from previous kaon (sequence ID=2)
	/// 	int pi0ID = fMCSimple->AddParticle(kaonID, 111) //Ask pi0 from previous kaon (sequence ID=3)
	/// 	fMCSimple->AddParticle(pi0ID, 22) //Ask first gamma from previous pi0 (sequence ID=4)
	/// 	fMCSimple->AddParticle(pi0ID, 22) //Ask second gamma from previous pi0 (sequence ID=4)
	///	\endcode
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
	/// You can retrieve MC particles from the fMCSimple set with (returns a vector<KinePart*>)\n
	/// \code
	/// 	fMCSimple["particleName"]
	/// 	fMCSimple[pdgID]
	/// \endcode
	/// Example\n
	/// \code
	/// 	fMCSimple["K+"][index]; //for the kaon
	/// 	fMCSimple["pi+"][index]; //for the positive pion
	/// 	fMCSimple["gamma"][index]; //for the photon
	/// \endcode
	/// The number in the brackets is the index of the particle (if you asked for two photons in the set, you can ask fMCSimple["gamma"][0] for the first one and fMCSimple["gamma"][1] for the second)\n
	/// \n
	/// If you need a property of a particle, you can make a call to fParticleInterface (instance of the ParticleInterface class).\n
	///	This class has two methods FindParticle that will return a TParticlePDG with the required particle. You can search by pdgID or by name.\n
	///	This class also provide two methods to switch between particle name and pdgID if necessary.\n
	///	Example\n
	/// \code
	///		double kaonMass = fParticleInterface->FindParticle(321).Mass();
	///		double pi0Lifetime = fParticleInterface->FindParticle("pi0").Lifetime();
	/// \endcode
	/// You can retrieve the events from the trees with\n
	/// \code
	/// 	(eventClass*)GetEvent("treeName");
	/// \endcode
	/// You can retrieve data from generic TTrees with\n
	/// \code
	/// 	GetObject<MyClass>("treeName");
	/// \endcode
	/// You can retrieve the histograms you booked (for drawing, changing, filling, ...) with\n
	/// \code
	/// 	fHisto.GetTH1("histoName");// for TH1
	/// 	fHisto.GetTH2("histoName");// for TH2
	/// 	fHisto.GetGraph("graphName");// for TGraph and TGraphAsymmErrors
	/// \endcode
	/// To fill the histograms you can use\n
	/// \code
	/// 	FillHisto("histoName", values)
	/// \endcode
	/// where values are the same parameters as if you call histogram->Fill(values) (x,y,weight,...)\n
	/// If the histogram is not found, an error message is printed\n
	/// \n
	/// Modify a counter with one of the following methods\n
	/// \code
	/// 	IncrementCounter(name)
	/// 	IncrementCounter(name, delta)
	/// 	DecrementCounter(name)
	/// 	DecrementCounter(name, delta)
	/// 	SetCounterValue(name, value)
	/// \endcode
	/// \n
	/// For use of fGeom, read DetectorAcceptance class.\n
	///	WARNING: this class provides "exact" results, there is not tolerance. If the particle\n
	///	passes in the sensitive volume of a detector it's considered as detected, wether it's close\n
	///	to the edge or not. But as the class gives you the position of the passage point and the \n
	///	estimated path length in the sensitive volume, you can apply further cuts from this\n
	///	information at your convenience.\n
	/// \n
	/// To use the output of a different analyzer, use\n
	/// \code
	/// 	outputType *var = GetOutput<outputType>("analyzerName.outputName", state);
	/// \endcode
	/// Where outputType is the variable type and state is of type outputState\n
	/// State is set with the state of the variable (kOUninit, kOInvalid ,kOValid). The value of the output should only be trusted if state == kOValid\n
	/// example :
	/// \code
	/// 	TLorentzVector vertex = *(TLorentzVector*)GetOutput("simpleVertexAnalyzer.vertex", state);
	/// \endcode
	/// Before starting the processing of an event, the state flag of each output variable is reset to kOUninit\n
	/// When setting the value of an output variable, don't forget to set appropriately the state flag to either kOValid or kOInvalid\n
	/// to indicate if the value can/can't be used in other analyzer\n
	/// \code
	/// 	SetOutputState("outputName", kOValid);
	/// \endcode
	/// If you want to append a candidate in one of your standard output Tree, use\n
	/// \code
	/// 	KinePart *candidate = CreateStandardCandidate("treeName");
	/// \endcode
	/// and fill the properties of your candidate. It will be automatically written in the output tree.\n
	///	\n
	/// If you want to save this event in your custom and standard TTrees (not the input tree replication), call\n
	/// \code
	/// 	FillTrees();
	/// \endcode
	/// This will call the Fill method of every TTree created in this analyzer.\n
	///	\n
	/// If you want to replicate this event in the output file, call\n
	/// \code
	/// 	ExportEvent();
	/// \endcode
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
	/// This method is called at the end of the processing (corresponding to a end of run in the normal NA62 data taking)\n
	/// Do here your end of run processing if any
	/// \EndMemberDescr

}

void templateAnalyzer::ExportPlot(){
	/// \MemberDescr
	/// This method is called at the end of processing to save plots.\n
	/// If you want to save them all, just call\n
	/// \code
	/// 	SaveAllPlots();
	/// \endcode
	/// Or you can just save the ones you want with\n
	/// \code
	/// 	histogram->Write()
	/// \endcode
	/// \n
	/// To run over a set of histograms you can use Iterators (HistoHandler::IteratorTH1,
	/// HistoHandler::IteratorTH2, HistoHandler::IteratorTGraph). You can use it to run over
	/// all the histograms or only a subset of histogram by using one of the two forms of
	/// GetIterator...  (replace ... by TH1, TH2 or TGraph)\n
	/// \code
	/// 	GetIterator...()
	/// \endcode
	/// will get an Iterator running over all histograms of this type while
	/// \code
	/// 	GetIterator...("baseName")
	/// \endcode
	/// will get an Iterator running only over the histograms of this type whose name starts
	/// with baseName.\n
	/// For more details and examples on how to use the Iterator after getting it, please refer
	/// to the HistoHandler::Iterator documentation.\n
	/// Although this is described here in ExportPlot(), Iterators can be used anywhere after the
	/// histograms have been booked.
	/// \EndMemberDescr
}

void templateAnalyzer::DrawPlot(){
	/// \MemberDescr
	/// This method is called at the end of processing to draw plots.\n
	/// If you want to draw all the plots, just call\n
	/// \code
	/// 	DrawAllPlots();
	/// \endcode
	/// Or do as usual (TCanvas, Draw, ...)\n
	/// \EndMemberDescr
}
