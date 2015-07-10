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
using namespace NA62Analysis;
using namespace NA62Constants;


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

VertexCDA::VertexCDA(Core::BaseAnalysis *ba) : Analyzer(ba, "VertexCDA")
{
	//Request GigaTracker and Spectrometer reco trees
	RequestTree("GigaTracker", new TRecoGigaTrackerEvent);
	RequestTree("Spectrometer", new TRecoSpectrometerEvent);
}


//#####################################################
//   Book and Initialize histograms in this function.
//#####################################################
void VertexCDA::InitHist(){
	//Create histograms
	BookHisto(new TH1I("VertexX", "Reconstructed vertex X position; vtx_{x}^{reco}", 250, -250, 250));
	BookHisto(new TH1I("VertexY", "Reconstructed vertex Y position; vtx_{y}^{reco}", 150, -150, 150));
	BookHisto(new TH1I("VertexZ", "Reconstructed vertex Z position; vtx_{z}^{reco}", 100, 0, 300000));

	BookHisto(new TH1I("DiffVertexX", "X difference between reco and real vertex; vtx_{x}^{reco}-vtx_{x}", 200, -50, 50));
	BookHisto(new TH1I("DiffVertexY", "Y difference between reco and real vertex; vtx_{y}^{reco}-vtx_{y}", 200, -50, 50));
	BookHisto(new TH1I("DiffVertexZ", "Z difference between reco and real vertex; vtx_{z}^{reco}-vtx_{z}", 200, -10000, 10000));

	BookHisto(new TH2I("VertexRecoRealX", "Reconstructed vs. Real (X)", 250, -250, 250, 250, -250, 250));
	BookHisto(new TH2I("VertexRecoRealY", "Reconstructed vs. Real (Y)", 150, -150, 150, 150, -150, 150));
	BookHisto(new TH2I("VertexRecoRealZ", "Reconstructed vs. Real (Z)", 200, 0, 300000, 200, 0, 300000));

	BookHisto(new TH1I("GTKMultiplicity", "Multiplicity in GTK", 11, -0.5, 10.5));
	BookHisto(new TH1I("StrawMultiplicity", "Multiplicity in Straw", 11, -0.5, 10.5));

	BookHistoArray(new TH2I("BeamXY", "BeamXY", 100, -100, 100, 100, -100, 100), 20);

	//Create counters
	BookCounter("Total_Events");
	BookCounter("Good_GTK_Mult");
	BookCounter("Good_Straw_Mult");

	//Create event fraction tables and add the counters we just created in the
	//table. Define Total_Events as the sample size counter. The fractions will
	//be computed relative to Total_Events
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
	//Register the reconstructed vertex as output of the analyzer
	RegisterOutput("Vertex", &fVertex);
}

//#####################################################
//   Setup of fMCSimple. You must specify the generated MC particles you want
//#####################################################
void VertexCDA::DefineMCSimple(){
	//As MC input, request only a beam Kaon decaying into positive pion
	// + anything else
	int kID = fMCSimple.AddParticle(0, 321); //ask for beam Kaon
	fMCSimple.AddParticle(kID, 211); //ask for positive Kaon
}

//#####################################################
//   Main process method. Called on each event. Write you analysis here.
//   iEvent = Event number
//   fMCSimple = Set of KinePart you asked for int GetTree method
//   MCTruthEvent = Complete set of generated KinePart
//#####################################################
void VertexCDA::Process(int){
	bool badEvent = false;
	TVector3 KaonPosition, KaonMomentum;
	TVector3 PipPosition, PipMomentum;

	bool withMC = true;
	//If MC is not present or not as we requested (K->pi+ + ...), don't do the MC comparison part
	if(fMCSimple.fStatus != MCSimple::kComplete) withMC = false;

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
		PipPosition = ((TRecoSpectrometerCandidate*)SpectrometerEvent->GetCandidate(0))->GetPositionBeforeMagnet();
		PipMomentum.SetXYZ(((TRecoSpectrometerCandidate*)SpectrometerEvent->GetCandidate(0))->GetSlopeXBeforeMagnet(),
								 ((TRecoSpectrometerCandidate*)SpectrometerEvent->GetCandidate(0))->GetSlopeYBeforeMagnet(),
								 1);
		PipMomentum.SetMag(((TRecoSpectrometerCandidate*)SpectrometerEvent->GetCandidate(0))->GetMomentum());
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
			FillHisto("DiffVertexX", fVertex.X()-fMCSimple["pi+"][0]->GetProdPos().X());
			FillHisto("DiffVertexY", fVertex.Y()-fMCSimple["pi+"][0]->GetProdPos().Y());
			FillHisto("DiffVertexZ", fVertex.Z()-fMCSimple["pi+"][0]->GetProdPos().Z());
			FillHisto("VertexRecoRealX", fVertex.X(), fMCSimple["pi+"][0]->GetProdPos().X());
			FillHisto("VertexRecoRealY", fVertex.Y(), fMCSimple["pi+"][0]->GetProdPos().Y());
			FillHisto("VertexRecoRealZ", fVertex.Z(), fMCSimple["pi+"][0]->GetProdPos().Z());
			int cat = ((fMCSimple["pi+"][0]->GetProdPos().Z()/1000)-100)/5;
			FillHistoArray("BeamXY", cat, fMCSimple["pi+"][0]->GetProdPos().X(), fMCSimple["pi+"][0]->GetProdPos().Y());
		}
	}
}

void VertexCDA::PostProcess(){

}

//#####################################################
//   This method is called at the end of processing to save plots.
//#####################################################
void VertexCDA::ExportPlot(){
	SaveAllPlots();
}

//#####################################################
//   This method is called at the end of processing to draw plots.
//#####################################################
void VertexCDA::DrawPlot(){
	DrawAllPlots();
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
