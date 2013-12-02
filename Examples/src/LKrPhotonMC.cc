//Mandatory includes
#include "LKrPhotonMC.hh"
#include <stdlib.h>
#include <iostream>
#include "MCSimple.hh"
#include "functions.hh"
#include "Event.hh"
#include <TChain.h>
using namespace std;

//Include the hh files for events kinds you will use
/*#include "TRecoGigaTrackerEvent.hh"
#include "TRecoRICHEvent.hh"
#include "TRecoCHANTIEvent.hh"
#include "TRecoCedarEvent.hh"
#include "TRecoCHODEvent.hh"
#include "TRecoMUV1Event.hh"
#include "TRecoMUV2Event.hh"
#include "TRecoMUV3Event.hh"
#include "TRecoMUV0Event.hh"
#include "TRecoHACEvent.hh"
#include "TRecoIRCEvent.hh"
#include "TRecoLAVEvent.hh"
#include "TRecoSACEvent.hh"
#include "TRecoLKrEvent.hh"
#include "TRecoSpectrometerEvent.hh"*/
#include "TRecoLKrEvent.hh"
#include "TRecoLKrCandidateMC.hh"

//Additional includes
/*#include <TString.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TGraph.h>
#include <TCanvas.h>*/

LKrPhotonMC::LKrPhotonMC(BaseAnalysis *ba) : Analyzer(ba)
{
	fAnalyzerName = "LKrPhotonMC";

	fCandidates = new TClonesArray("TRecoLKrCandidateMC", 10);
	RequestTree("LKr", new TRecoLKrEvent);

	RequestTree("GigaTracker", new TRecoGigaTrackerEvent);
	RequestTree("Spectrometer", new TRecoSpectrometerEvent);
	//Specify the trees you want to use and the event class corresponding
	//Don't try to load MCTruth tree (RUN_0 or Event). Use the MCTruthEvent in Process function instead. Problems when opening twice the same tree.
	//Example with RecoEvent
	//	RequestTree("GigaTracker", new TRecoGigaTrackerEvent);
	//Example with MC Event
	//	RequestTree("GigaTracker", new TGigaTrackerEvent);
	//Example with generic tree
	//	RequestTree<MyClass>("MyTree", "BranchName", "MyClass", new MyClass);

	//Initialize DetectorAcceptance if needed
	//use of global instance
	//	fDetectorAcceptanceInstance = GetDetectorAcceptanceInstance();
	//use of local instance
	//	fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.root");
}

void LKrPhotonMC::InitOutput(){
	//Call
	//	RegisterOutput("outputName", &variableName)
	//for each variable that should be in the output of the Analyzer
	//The name of the analyzer will be prepended to the outputName (to avoid collisions with other analyzers)
	//variableName should be the name of a variable declared in the definition of the class

	//Call
	//	AddParam("paramName", "paramType", &variableName, defaultValue);
	//for each parameter of the analyzer. These parameters can be set when starting the FW from the command line with the -p option.
	//paramName is the name of the parameter in the command line
	//paramType is the type of the parameter (allowed : bool, int, long, float, double, char, string, TString)
	//variableName is the name of the variable that should be declared in the definition of the class<br>
	//defaultValue is the default value if not specified in the command line

	//To create a new TTree in the output file, call
	OpenNewTree("LKrMCAssociated", "LKrMCAssociated");
	//TTreeName is the name of the TTree (will be used to refer to this TTree later)
	//TTreeTitle is the title of the TTree
	//Call
	//fCandidates.SetClass("TRecoLKrCandidateMC", 10);
	AddBranch<TClonesArray>("LKrMCAssociated", "fCandidates", fCandidates);
	//VariableType is the type of the variable for this branch (fundamental data type or class)
	//TTreeName is the name of the TTree to add this branch
	//BranchName is the name of the branch
	//pointer is a pointer to the variable (should be declared in the header file)

	//To create a standard TTree containing KineParts (for candidates) use
	CreateStandardTree("LKrCandidates", "LKrCandidates");
}

void LKrPhotonMC::InitHist(){
	//Same function to Book TH1, TH2, TGraph and TGraphAsymmErrors (anything derived from TH1 or TGraph)
	//	BookHisto(name, histogram*, isAutoUpdate)
	//If isAutotUpdate is true, this histogram will be drawn and updated regularly during the processing. (default=false)
	//The refresh interval can be set with (default=10)
	// SetUpdateInterval(interval)
	//Defining plots as AutoUpdate and setting the interval can also be done at runtime with a configuration file
	//Example
	//	BookHisto("PartEnergy", new TH2I("PartEnergy", "Energy as a function of particle", 0, 0, 0, Bins, MinEnergy, MaxEnergy));

	//Booking of counters and creation of EventFraction can be done here
	//	BookCounter(name)
	//	NewEventFraction(name)
	//Example
	//	BookCounter("Total");
	//	BookCounter("PassCuts");
	//	NewEventFraction("Cuts");

	//Add the counters to the EventFraction
	//	AddCounterToEventFraction(EventFractionName, CounterName)
	//Example
	//	AddCounterToEventFraction("Cuts", "Total");
	//	AddCounterToEventFraction("Cuts", "PassCuts");

	//Then define which counter represents the sample size
	//	DefineSampleSizeCounter(EventFractionName, CounterName)
	//Exemple
	//	DefineSampleSizeCounter("Cuts", "Total");

	//You can retrieve histograms from the input ROOT file (Anything derived from TH1) with<br>
	//	RequestHistogram("TDirectoryName", "HistogramName", appendOnNewFile);<br>
	//appendOnNewFile is a boolean. If set to true, each time a new file is opened the content<br>
	//of the histogram will be appended to the content of the previous one. If set to false, the content <br>
	//of the histogram is replaced each time a new file is opened.
}

void LKrPhotonMC::DefineMCSimple(MCSimple *fMCSimple){
	//Add particles you want to recover from fMCSimple
	//	int particleID = fMCSimple->AddParticle(parentID, pdgCode)
	//parentID : 	0=no parent (=beam particle)
	//		...
	//Example
	//	int kaonID = fMCSimple->AddParticle(0, 321); //ask for beam Kaon
	//	fMCSimple->AddParticle(kaonID, 211); //ask for positive pion from initial kaon decay

	//see ROOT TDatabasePDG for a list of PDG codes and particle naming convention
	int kID = fMCSimple->AddParticle(0, 321);
	int pi0ID = fMCSimple->AddParticle(kID, 111);
	fMCSimple->AddParticle(pi0ID, 22);
	fMCSimple->AddParticle(pi0ID, 22);
}

void LKrPhotonMC::StartOfRunUser(){

}

void LKrPhotonMC::StartOfBurstUser(){

}

void LKrPhotonMC::EndOfBurstUser(){

}


void LKrPhotonMC::Process(int iEvent, MCSimple &fMCSimple, Event* MCTruthEvent){
	//Ask the fMCSimple to have the complete set of particles we specified
	//If the analyzer can run without the complete set, comment the line
	if(fMCSimple.fStatus == MCSimple::kMissing){printIncompleteMCWarning(iEvent);return;}
	//If the analyzer can run without MC data, comment the line
	if(fMCSimple.fStatus == MCSimple::kEmpty){printNoMCWarning();return;}

	TRecoLKrEvent *lkr = (TRecoLKrEvent*)GetEvent("LKr");

	//Candidates
	TRecoLKrCandidate *lkrCand;
	TRecoLKrCandidateMC *lkrCandMC;
	KinePart *photonCandidate;
	int candidatesNumber = 0;

	TVector3 vertex, direction;
	OutputState state;
	vertex = *(TVector3*)GetOutput("VertexCDA.vertex", state);

	//Positions on LKr
	vector<double> clusterEnergy, tempVDouble;
	vector<TVector3> clusterLKr, tempVTV3;
	vector<TRecoLKrCandidate*> sortedCandidates, tempRecoLKr;

	TVector3 cluster;

	//LKr definition
	int LKrStartPos = 240413;

	TVector3 gamma1, gamma2;
	double distance1, distance2;
	int d_sel1, d_sel2;
	double d;
	int thresholdgamma = 50;

	double calibMult = 0.9744;
	double calibConst = -366.5;
	int vId;

	//Get Clusters
	for(int i=0; i<lkr->GetNCandidates(); i++){
		lkrCand = (TRecoLKrCandidate*)lkr->GetCandidate(i);
		cluster.SetXYZ(lkrCand->GetClusterX()*10, lkrCand->GetClusterY()*10, LKrStartPos);
		tempVTV3.push_back(cluster);
		tempRecoLKr.push_back(lkrCand);
		tempVDouble.push_back(lkrCand->GetClusterEnergy()*1000);
		photonCandidate = CreateStandardCandidate("LKrCandidates");
		photonCandidate->SetPDGcode(22);
		direction.SetXYZ(lkrCand->GetClusterX()*10, lkrCand->GetClusterY()*10, LKrStartPos);
		direction = direction - vertex;
		direction.SetMag(lkrCand->GetClusterEnergy()*1000*calibMult + calibConst);
		photonCandidate->SetInitialMomentum(direction);
		photonCandidate->SetEndPos(TLorentzVector(cluster, 0));
		photonCandidate->SetInitialEnergy(lkrCand->GetClusterEnergy()*1000*calibMult + calibConst);
	}

	//Sorting cluster by energy
	while(tempVDouble.size()>0){
		vId = getMostEnergetic(tempVDouble);
		sortedCandidates.push_back(tempRecoLKr[vId]);
		clusterEnergy.push_back(tempVDouble[vId]);
		clusterLKr.push_back(tempVTV3[vId]);
		tempVDouble.erase(tempVDouble.begin()+vId);
		tempRecoLKr.erase(tempRecoLKr.begin()+vId);
		tempVTV3.erase(tempVTV3.begin()+vId);
		lkrCandMC = (TRecoLKrCandidateMC*)fCandidates->ConstructedAt(candidatesNumber);
		cout << lkrCandMC->ClassName() << endl;
		lkrCandMC->copy(sortedCandidates.back());
		lkrCandMC->SetMCParticleID(-1);
	}

	//Project MC photons on LKr plane
	gamma1 = propagate(fMCSimple["gamma"][0]->GetProdPos().Vect(), fMCSimple["gamma"][0]->GetInitialMomentum(), LKrStartPos);
	gamma2 = propagate(fMCSimple["gamma"][1]->GetProdPos().Vect(), fMCSimple["gamma"][1]->GetInitialMomentum(), LKrStartPos);

	//Associate clusters to photons
	distance1 = 10000;
	distance2 = 10000;
	d_sel1 = -1;
	d_sel2 = -1;
	for(unsigned int i=0; i<clusterLKr.size(); i++){
		d = distance3D(gamma1, clusterLKr[i]);
		if(d<distance1){
			d_sel1 = i;
			distance1 = d;
		}
		d = distance3D(gamma2, clusterLKr[i]);
		if(d<distance2){
			d_sel2 = i;
			distance2 = d;
		}
	}

	if(d_sel1!=d_sel2){
		if(candidatesNumber>=1 && d_sel1!=-1 && distance1<thresholdgamma){
			//Setting correct association
			((TRecoLKrCandidateMC*)fCandidates->At(d_sel1))->SetMCParticleID(fMCSimple["gamma"][0]->GetID());
		}
		if(candidatesNumber>=2 && d_sel2!=-1 && distance2<thresholdgamma){
			//Setting correct association
			((TRecoLKrCandidateMC*)fCandidates->At(d_sel2))->SetMCParticleID(fMCSimple["gamma"][1]->GetID());
		}
	}

	//You can retrieve MC particles from the fMCSimple Set with (return a vector<KinePart*>
	//	fMCSimple["particleName"]
	//	fMCSimple[pdgID]
	//Example
	//	fMCSimple["k+"][index] for the kaon
	//	fMCSimple["pi+"][index] for the positive pion
	//	fMCSimple["gamma"][index] for the photon
	//The number in the brackets is the index of the particle (if you asked for two photons in the set, you can ask fMCSimple["gamma"][0] for the first one and fMCSimple["gamma"][1] for the second)

	//You can retrieve the events from the trees with
	//	(eventClass*)GetEvent("treeName");
	//You can retrieve data from generic TTrees with
	//	GetObject<MyClass>("treeName");

	//You can retrieve the histograms you booked (for drawing, changing, filling, ...) with
	//	fHisto["histoName"] for TH1
	//	fHisto2["histoName"] for TH2
	//	fGraph["graphName"] for TGraph and TGraphAsymmErrors
	//Be carefull !! If the histogram you ask for doesn't exist or you ask for an existing histogram
	//in the wrong recipient (e.g. th2 in fHisto), program will segfault.
	//To fill the histograms without risk of segfault, you can use
	//	FillHisto("histoName", values)
	//where values are the same parameters as if you call histogram->Fill(values) (x,y,weight,...)
	//Instead of segfault, this function print an error message

	//Modify a counter with one of the following methods
	//	IncrementCounter(name)
	//	IncrementCounter(name, delta)
	//	DecrementCounter(name)
	//	DecrementCounter(name, delta)
	//	SetCounterValue(name, value)

	//For use of fGeom, read DetectorAcceptance class

	//To use the output of a different analyzer, use
	//	outputType var = *(outputType*)GetOutput("analyzerName.outputName", state);
	//Where outputType is the variable type and state is of type outputState
	//This function returns a void* and you have to explicitly cast it to the real object type
	//State is set with the state of the variable (kOUninit, kOInvalid ,kOValid). The value of the output should only be trusted if state == kOValid
	//example : TLorentzVector vertex = *(TLorentzVector*)GetOutput("simpleVertexAnalyzer.vertex", state);

	//Before starting the processing of an event, the state flag of each output variable is reset to kOUninit
	//When setting the value of an output variable, don't forget to set appropriately the state flag to either kOValid or kOInvalid
	//to indicate if the value can/can't be used in other analyzer
	//	SetOutputState("outputName", kOValid);

	//If you want to save this event in the output file, call
	//	ExportEvent();
	//The structure of all the trees that have been opened (by all Analyzer) will be copied in the output file
	//and the events for which at least one analyzer called ExportEvent() will be written in the output trees

	//If you want to append a candidate in one of your standard output Tree, use
	//	KinePart *candidate = CreateStandardCandidate("treeName");
	//and fill the properties of your candidate. It will be automatically written in the output tree.
}

void LKrPhotonMC::PostProcess(){
	fCandidates->Clear();
}

void LKrPhotonMC::EndOfRunUser(){

}


void LKrPhotonMC::ExportPlot(){
	//If you want to save them all, just call
	//	SaveAllPlots();
	//Or you can just save the ones you want with
	//	histogram->Write()
}

void LKrPhotonMC::DrawPlot(){
	//If you want to draw all the plots, just call
	//	DrawAllPlots();
	//Or do as usual (TCanvas, Draw, ...)
}

int LKrPhotonMC::getMostEnergetic(vector<double> e){
	vector<double>::iterator it;
	double max = 0;
	int i=0;
	int index=-1;
	for(it=e.begin(); it!=e.end(); it++){
		if(*it>max){
			max = *it;
			index = i;
		}
		i++;
	}
	return index;
}
