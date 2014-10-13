//Mandatory includes
#include "VertexCDA.hh"
#include <stdlib.h>
#include <iostream>
#include "MCSimple.hh"
#include "functions.hh"
#include "Event.hh"
#include <TChain.h>
#include <TFile.h>
#include "Persistency.hh"
using namespace std;

/// \class VertexCDA
/// \Brief
/// Another example analyzer to demonstrate how analyzers can be included in
///	the framework and be used by anyone. Find the vertex using a CDA between
///	a GigaTracker candidate track and a spectrometer candidate track.
/// \EndBrief
///
/// \Detailed
///	Requests a single candidate in GigaTracker and a single candidate in
///	Spectrometer and uses these tracks as input for the CDA (closest distance of
///	approach) algorithm. Outputs the resulting vertex (TVector3). If more than 1
///	track candidate in any of the two detectors, the vertex is not computed and
///	the output is set as invalid.
/// \EndDetailed

VertexCDA::VertexCDA(BaseAnalysis *ba) : Analyzer(ba)
{
	fAnalyzerName = "VertexCDA";

	//Specify the trees you want to use and the event class corresponding
	//Don't try to load MCTruth tree (RUN_0 or Event). Use the MCTruthEvent in Process function instead. Problems wen opening twice the same tree.
	//Example with RecoEvent
	//	AddTree("GigaTracker", new TRecoGigaTrackerEvent);
	//Example with MC Event
	//	AddTree("GigaTracker", new TGigaTrackerEvent);

	RequestTree("GigaTracker", new TRecoGigaTrackerEvent);
	RequestTree("Spectrometer", new TRecoSpectrometerEvent);
	/*AddTree("GigaTracker", new TRecoGigaTrackerEvent);
	AddTree("Spectrometer", new TRecoSpectrometerEvent);*/

	//Initialize DetectorAcceptance if needed
	//use of global instance
	//	fDetectorAcceptanceInstance = GetDetectorAcceptanceInstance();
	//use of local instance
	//	fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.gdml");
}


//#####################################################
//   Book and Initialize histograms in this function.
//#####################################################
void VertexCDA::InitHist(){
	//Same function to Book TH1, TH2, TGraph and TGraphAsymmErrors
	//	BookHisto(name, histogram*)
	//Example
	//	BookHisto("PartEnergy", new TH2I("PartEnergy", "Energy as a function of particle", 0, 0, 0, Bins, MinEnergy, MaxEnergy));

	BookHisto(new TH1I("VertexX", "Reconstructed vertex X position", 250, -250, 250));
	BookHisto(new TH1I("VertexY", "Reconstructed vertex Y position", 150, -150, 150));
	BookHisto(new TH1I("VertexZ", "Reconstructed vertex Z position", 100, 0, 300000));

	BookHisto(new TH1I("DiffVertexX", "X difference between reco and real vertex", 200, -50, 50));
	BookHisto(new TH1I("DiffVertexY", "Y difference between reco and real vertex", 200, -50, 50));
	BookHisto(new TH1I("DiffVertexZ", "Z difference between reco and real vertex", 200, -10000, 10000));

	BookHisto(new TH2I("VertexRecoRealX", "Reconstructed vs. Real (X)", 250, -250, 250, 250, -250, 250));
	BookHisto(new TH2I("VertexRecoRealY", "Reconstructed vs. Real (Y)", 150, -150, 150, 150, -150, 150));
	BookHisto(new TH2I("VertexRecoRealZ", "Reconstructed vs. Real (Z)", 200, 0, 300000, 200, 0, 300000));

	BookHisto(new TH1I("GTKMultiplicity", "Multiplicity in GTK", 11, -0.5, 10.5));
	BookHisto(new TH1I("StrawMultiplicity", "Multiplicity in Straw", 11, -0.5, 10.5));

	BookHistoArray(new TH2I("BeamXY", "BeamXY", 100, -100, 100, 100, -100, 100), 20);

	BookCounter("Total_Events");
	BookCounter("Good_GTK_Mult");
	BookCounter("Good_Straw_Mult");

	NewEventFraction("Selection");
	AddCounterToEventFraction("Selection", "Total_Events");
	AddCounterToEventFraction("Selection", "Good_GTK_Mult");
	AddCounterToEventFraction("Selection", "Good_Straw_Mult");
	DefineSampleSizeCounter("Selection", "Total_Events");
}

//#####################################################
//   Register the output variables of the analyzer
//#####################################################
void VertexCDA::InitOutput(){
	//Call
	//RegisterOutput("outputName", &variableName)
	//for each variable that should be in the output of the Analyzer
	//The name of the analyzer will be prepended to the outputName (to avoid collisions with other analyzers)
	//variableName should be the name of a variable declared in the definition of the class
	RegisterOutput("Vertex", &fVertex);
}

//#####################################################
//   Setup of fMCSimple. You must specify the generated MC particles you want
//#####################################################
void VertexCDA::DefineMCSimple(MCSimple *fMCSimple){
	//Add particles you want to recover from fMCSimple
	//	ID = fMCSimple->AddParticle(parentID, pdgCode)
	//parentID : 	0=no parent (=beam particle)
	//		1=beam particle (1st generation)
	//		...
	//Example
	int kID = fMCSimple->AddParticle(0, 321); //ask for beam Kaon
	fMCSimple->AddParticle(kID, 211); //ask for positive Kaon

}

void VertexCDA::PrintClass(){
	cout << fAnalyzerName << endl;
}

//#####################################################
//   Main process method. Called on each event. Write you analysis here.
//   iEvent = Event number
//   fMCSimple = Set of KinePart you asked for int GetTree method
//   MCTruthEvent = Complete set of generated KinePart
//#####################################################
void VertexCDA::Process(int){
	//Ask the fMCSimple to have the complete set of particles we specified
	//If the analysis can run without the complete set, comment the line
	//if(fMCSimple.fStatus == MCSimple::kMissing) return;
	//If the analyzer can run without MC data, comment the line
	//if(fMCSimple.fStatus == MCSimple::kEmpty) return;

	bool badEvent = false;
	TVector3 KaonPosition, KaonMomentum;
	TVector3 PipPosition, PipMomentum;
	MCSimple mcSimple = GetMCSimple();

	bool withMC = true;
	if(mcSimple.fStatus != MCSimple::kComplete) withMC = false;

	TRecoGigaTrackerEvent *GTKEvent = (TRecoGigaTrackerEvent*)GetEvent("GigaTracker");
	TRecoSpectrometerEvent *SpectrometerEvent = (TRecoSpectrometerEvent*)GetEvent("Spectrometer");

	IncrementCounter("Total_Events");
	FillHisto("GTKMultiplicity", GTKEvent->GetNCandidates());
	if(GTKEvent->GetNCandidates()==1){
		KaonPosition = ((TRecoGigaTrackerCandidate*)GTKEvent->GetCandidate(0))->GetPosition(2);
		KaonPosition.SetZ(KaonPosition.Z()+90932.5);
		KaonMomentum = ((TRecoGigaTrackerCandidate*)GTKEvent->GetCandidate(0))->GetMomentum().Vect();
		IncrementCounter("Good_GTK_Mult");
	}
	else badEvent = true;

	FillHisto("StrawMultiplicity", SpectrometerEvent->GetNCandidates());
	if(SpectrometerEvent->GetNCandidates()==1){
		PipPosition = ((TRecoSpectrometerCandidate*)SpectrometerEvent->GetCandidate(0))->GetPosition();
		PipMomentum = ((TRecoSpectrometerCandidate*)SpectrometerEvent->GetCandidate(0))->GetMomentum().Vect();
		IncrementCounter("Good_Straw_Mult");
	}
	else badEvent = true;


	if(badEvent){
		SetOutputState("Vertex", kOInvalid);
	}
	else{
		fVertex = GetIntersection(KaonPosition, KaonMomentum, PipPosition, PipMomentum);
		SetOutputState("Vertex", kOValid);

		FillHisto("VertexX", fVertex.X());
		FillHisto("VertexY", fVertex.Y());
		FillHisto("VertexZ", fVertex.Z());

		if(withMC){
			FillHisto("DiffVertexX", fVertex.X()-mcSimple["pi+"][0]->GetProdPos().X());
			FillHisto("DiffVertexY", fVertex.Y()-mcSimple["pi+"][0]->GetProdPos().Y());
			FillHisto("DiffVertexZ", fVertex.Z()-mcSimple["pi+"][0]->GetProdPos().Z());
			FillHisto("VertexRecoRealX", fVertex.X(), mcSimple["pi+"][0]->GetProdPos().X());
			FillHisto("VertexRecoRealY", fVertex.Y(), mcSimple["pi+"][0]->GetProdPos().Y());
			FillHisto("VertexRecoRealZ", fVertex.Z(), mcSimple["pi+"][0]->GetProdPos().Z());
			int cat = ((mcSimple["pi+"][0]->GetProdPos().Z()/1000)-100)/5;
			FillHistoArray("BeamXY", cat, mcSimple["pi+"][0]->GetProdPos().X(), mcSimple["pi+"][0]->GetProdPos().Y());
		}
	}

	//You can retrieve MC particles from the fMCSimple Set with
	//	fMCSimple.fKaon[0] for the kaons
	//	fMCSimple.fPip[0] for the positive pions
	//	fMCSimple.fGamma[0] for the photons
	//The number in the brackets is the index of the particle (if you asked for two photons in the set, you can ask fMCSimple.fGamma[0] for the first one and fMCSimple.fGamma[1] for the second)

	//You can retrieve the events from the trees with
	//	(eventClass*)fEvent["treeName"];

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

	//For use of fGeom, read DetectorAcceptance class

	//To use the output of a different analyzer, use
	//outputType var = *(outputType*)GetOutput("analyzerName.outputName", state);
	//Where outputType is the variable type and state is of type outputState
	//This function returns a void* and you have to explicitly cast it to the real object type
	//State is set with the state of the variable (kOUninit, kOInvalid ,kOValid). The value of the output should only be trusted if state == kOValid
	//example : TLorentzVector vertex = *(TLorentzVector*)GetOutput("simpleVertexAnalyzer.vertex", state);

	//Before starting the processing of an event, the state flag of each output variable is reset to kOUninit
	//When setting the value of an output variable, don't forget to set appropriately the state flag to either kOValid or kOInvalid
	//to indicate if the value can/can't be used in other analyzer
	//SetOutputState("outputName", kOValid);
}

void VertexCDA::PostProcess(){

}

//#####################################################
//   This method is called at the end of processing to save plots.
//#####################################################
void VertexCDA::ExportPlot(){
	//If you want to save them all, just call
	SaveAllPlots();
	//Or you can just save the ones you want with
	//	histogram->Write()
}

//#####################################################
//   This method is called at the end of processing to draw plots.
//#####################################################
void VertexCDA::DrawPlot(){
	//If you want to draw all the plots, just call
	DrawAllPlots();
	//Or do as usual (TCanvas, Draw, ...)
}

TVector3 VertexCDA::GetIntersection(TVector3 pos1, TVector3 p1, TVector3 pos2, TVector3 p2){

	TVector3 d0 = pos1-pos2;
	double a = p1.Mag2();
	double b = p1*p2;
	double c = p2.Mag2();
	double d = p1*d0;
	double e = p2*d0;

	double s = (b*e-c*d)/(a*c-b*b);
	double t = (a*e-b*d)/(a*c-b*b);

	TVector3 vdist = d0 + (s*p1 - t*p2);

	return pos1 + s*p1 - 0.5*vdist;
}
