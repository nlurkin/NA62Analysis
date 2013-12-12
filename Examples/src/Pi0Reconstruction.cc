//Mandatory includes
#include "Pi0Reconstruction.hh"
#include <stdlib.h>
#include <iostream>
#include "MCSimple.hh"
#include "functions.hh"
#include "Event.hh"
#include <TChain.h>
#include <TFile.h>
using namespace std;

#include "TRecoLKrEvent.hh"

Pi0Reconstruction::Pi0Reconstruction(BaseAnalysis *ba) : Analyzer(ba)
{
	fAnalyzerName = "Pi0Reconstruction";

	//Specify the trees you want to use and the event class corresponding
	//Don't try to load MCTruth tree (RUN_0 or Event). Use the MCTruthEvent in Process function instead. Problems wen opening twice the same tree.
	//Example with RecoEvent
	//	AddTree("GigaTracker", new TRecoGigaTrackerEvent);
	//Example with MC Event
	//	AddTree("GigaTracker", new TGigaTrackerEvent);

	RequestTree("LKr", new TRecoLKrEvent);

	//Initialize DetectorAcceptance if needed
	//use of global instance
	fDetectorAcceptanceInstance = GetDetectorAcceptanceInstance();
	//use of local instance
	//	fDetectorAcceptanceInstance = new DetectorAcceptance("./NA62.gdml");
}

//#####################################################
//   Book and Initialize histograms in this function.
//#####################################################
void Pi0Reconstruction::InitHist(){
	//Same function to Book TH1, TH2, TGraph and TGraphAsymmErrors
	//	BookHisto(name, histogram*)
	//Example
	//	BookHisto("PartEnergy", new TH2I("PartEnergy", "Energy as a function of particle", 0, 0, 0, Bins, MinEnergy, MaxEnergy));

	BookHisto("g1Energy", new TH1I("G1Energy", "Energy of g1", 100, 0, 75000));
	BookHisto("g2Energy", new TH1I("G2Energy", "Energy of g2", 100, 0, 75000));
	BookHisto("g1Reco", new TH2I("g1Reco", "g1 Reco vs. Real", 100, 0, 75000, 100, 0, 75000));
	BookHisto("g2Reco", new TH2I("g2Reco", "g2 Reco vs. Real", 100, 0, 75000, 100, 0, 75000));
	BookHisto("g1px", new TH2I("g1px", "g1 px Reco vs. Real", 200, 0, 2000, 200, 0, 2000));
	BookHisto("g2px", new TH2I("g2px", "g2 px Reco vs. Real", 200, 0, 2000, 200, 0, 2000));
	BookHisto("g1py", new TH2I("g1py", "g1 py Reco vs. Real", 200, 0, 2000, 200, 0, 2000));
	BookHisto("g2py", new TH2I("g2py", "g2 py Reco vs. Real", 200, 0, 2000, 200, 0, 2000));
	BookHisto("g1pz", new TH2I("g1pz", "g1 pz Reco vs. Real", 10, 240000, 250000, 10, 240000, 250000));
	BookHisto("g2pz", new TH2I("g2pz", "g2 pz Reco vs. Real", 10, 240000, 250000, 10, 240000, 250000));
	BookHisto("pi0Energy", new TH1I("pi0Energy", "Energy of pi0", 100, 0, 75000));
	BookHisto("pi0Mass", new TH1I("pi0Mass", "Reconstructed mass of pi0", 200, 0, 200));
	BookHisto("pi0MCMass", new TH1I("pi0MCMass", "MC mass of pi0", 200, 0, 200));
	BookHisto("clusterPosition", new TH2I("clusterPosition", "Cluster position on LKr", 500, -2000, 2000, 500, -2000, 2000));
	BookHisto("photonsNbr", new TH1I("photonsNbr", "Photons multiplicity/event", 10, 0, 10));
	BookHisto("g1EnergyFraction", new TH1I("g1EnergyFraction", "Fraction between real energy and reco energy", 1000, 0, 100));
	BookHisto("g2EnergyFraction", new TH1I("g2EnergyFraction", "Fraction between real energy and reco energy", 1000, 0, 100));

	BookHisto("gPairSelected", new TH1I("gPairSelected", "Pair of gamma selected for Pi0", 10, 0, 10));
	BookHisto("g1FirstVol", new TH1I("g1FirstVol", "First touched volume for g1", 15, 0, 15));
	BookHisto("g2FirstVol", new TH1I("g2FirstVol", "First touched volume for g2", 15, 0, 15));

	BookHisto("pdgID", new TH1I("pdgID", "Non complete events : pdgID", 0, 0, 0));

	BookHisto("energyCalib", new TGraph());
}

//#####################################################
//   Register the output variables of the analyzer
//#####################################################
void Pi0Reconstruction::InitOutput(){
	//Call
	//RegisterOutput("outputName", &variableName)
	//for each variable that should be in the output of the Analyzer
	//The name of the analyzer will be prepended to the outputName (to avoid collisions with other analyzers)
	//variableName should be the name of a variable declared in the definition of the class

	RegisterOutput("pi0", &pi0);
}

//#####################################################
//   Open the trees and branch them.
//   Setup of fMCSimple. You must specify the generated MC particles you want
//#####################################################
void Pi0Reconstruction::DefineMCSimple(MCSimple *fMCSimple){
	//Add particles you want to recover from fMCSimple
	//	fMCSimple->AddParticle(parentID, pdgCode)
	//parentID : 	0=no parent (=beam particle)
	//		1=beam particle (1st generation)
	//		...
	int kID = fMCSimple->AddParticle(0, 321); //ask for beam Kaon
	fMCSimple->AddParticle(kID, 211); //ask for positive pion from initial kaon decay
	int pi0ID = fMCSimple->AddParticle(kID, 111); //ask for positive pion from initial kaon decay
	fMCSimple->AddParticle(pi0ID, 22); //ask for positive pion from initial kaon decay
	fMCSimple->AddParticle(pi0ID, 22); //ask for positive pion from initial kaon decay
}

//#####################################################
//   Main process method. Called on each event. Write you analysis here.
//   iEvent = Event number
//   fMCSimple = Set of KinePart you asked for int GetTree method
//   MCTruthEvent = Complete set of generated KinePart
//#####################################################
void Pi0Reconstruction::Process(int iEvent, MCSimple &fMCSimple, Event* MCTruthEvent){

	//Temporary variables
	vector<KinePart*> photons;
	KinePart *part;
	TRecoLKrCandidate *lkrCand;
	vector<pair<KinePart*, KinePart*> > candidates;
	int i;

	//Maps
	map<double, int> photonOrder;
	map<double, int>::reverse_iterator photonIt, photonIt2;
	multimap<double, int> cID;
	multimap<double, int>::iterator cIDIterator;

	//For input from VertexCDA
	TVector3 vertex, direction;
	OutputState state;


	//Variables for reconstruction
	int LKrStartPos = 240413;
	KinePart *g1,*g2;
	double pi0Mass;
	int iLead=-1;
	int iTrail=-1;

	//Variables for MC checks
	double g1EnergyFrac = 0;
	double g2EnergyFrac=0;
	DetectorAcceptance::volume g1Vol, g2Vol;
	bool g1LKr, g2LKr;
	TVector3 g1reco, g2reco, g1real, g2real;

	//Get LKr event from TTree
	TRecoLKrEvent *LKrEvent = (TRecoLKrEvent*)GetEvent("LKr");

	//Calibration constants
	double calibMult = 0.9744;
	double calibConst = -366.5;

	//Are we working with correct MC
	bool withMC = true;
	if(fMCSimple.fStatus == MCSimple::kMissing){
		for(int i=0; i<MCTruthEvent->GetNKineParts(); i++){
			FillHisto("pdgID", ((KinePart*)MCTruthEvent->GetKineParts()->At(i))->GetParticleName(), 1);
		}
		withMC = false;
	}
	if(fMCSimple.fStatus == MCSimple::kEmpty) withMC = false;

	//Get vertex from VertexCDA analyzer
	vertex = *(TVector3*)GetOutput("VertexCDA.Vertex", state);
	SetOutputState("pi0", kOInvalid);

	//Check we got the vertex. We cannot work without the vertex
	if(state!=kOValid) return;

	//Loop over the LKr clusters and create a KinePart photon candidate
	for(int i=0; i<LKrEvent->GetNCandidates(); i++){
		lkrCand = (TRecoLKrCandidate*)LKrEvent->GetCandidate(i);
		part = new KinePart();
		//Fill the cluster position histogram
		FillHisto("clusterPosition", lkrCand->GetClusterX()*10, lkrCand->GetClusterY()*10);
		//Set the LKr position (*10 to go from cm to mm)
		direction.SetXYZ(lkrCand->GetClusterX()*10, lkrCand->GetClusterY()*10, LKrStartPos);
		//Set the direction
		direction = direction - vertex;
		//Set the magnitude of the direction to the corrected cluster energy to form the momentum (photon hypothesis)
		direction.SetMag(lkrCand->GetClusterEnergy()*1000*calibMult + calibConst);
		//Assign the properties to the KinePart
		part->SetProdPos(TLorentzVector(vertex, 0));
		part->SetInitialMomentum(direction);
		part->SetInitialEnergy(lkrCand->GetClusterEnergy()*1000*calibMult + calibConst);
		//Push the candidate in the list
		photonOrder.insert(pair<double, int>(lkrCand->GetClusterEnergy()*1000*calibMult + calibConst, photons.size()));
		photons.push_back(part);
	}

	//Fill the photon multiplicity histogram
	FillHisto("photonsNbr", photonOrder.size());

	i=0;
	//We need at leat 2 photons to reconstruct the pi0
	if(photonOrder.size()>=2){

		//Looping over possible photon pairs and computing invariant mass for each
		photonIt2 = photonOrder.rbegin();
		for(photonIt = photonOrder.rbegin(); photonIt != photonOrder.rend(); photonIt++){
			g1 = photons[photonIt->second];
			photonIt2 = photonIt;
			for(photonIt2++; photonIt2 != photonOrder.rend(); photonIt2++){
				g2 = photons[photonIt2->second];

				candidates.push_back(pair<KinePart*, KinePart*>(g1, g2));
				pi0Mass = sqrt(pow(g1->GetInitialEnergy() + g2->GetInitialEnergy(),2) - (g1->GetInitialMomentum() + g2->GetInitialMomentum()).Mag2());
				//insert the invariant mass divided by the pi0 mass in the map
				cID.insert(pair<double, int>(fabs((pi0Mass/134.9766)-1), i));
				i++;
			}
		}

		//Select the photon pair closest to the pi0 mass
		FillHisto("gPairSelected", cID.begin()->second);
		g1 = candidates[cID.begin()->second].first;
		g2 = candidates[cID.begin()->second].second;

		//Are we working with MC?
		if(withMC){

			//Select the most energetic photons coming from the pi0 decay
			if(fMCSimple["gamma"][0]->GetInitialEnergy() >= fMCSimple["gamma"][1]->GetInitialEnergy()) iLead = 0;
			else iLead = 1;
			iTrail = !iLead;

			//Compare the energy with the selected pair
			g1EnergyFrac = g1->GetInitialEnergy()/fMCSimple["gamma"][iLead]->GetInitialEnergy();
			g2EnergyFrac = g2->GetInitialEnergy()/fMCSimple["gamma"][iTrail]->GetInitialEnergy();

			//Are the 2 real photons in the LKr acceptance?
			fDetectorAcceptanceInstance->FillPath(fMCSimple["gamma"][iLead]->GetProdPos().Vect(), fMCSimple["gamma"][iLead]->GetInitialMomentum());
			g1Vol = fDetectorAcceptanceInstance->FirstTouchedDetector();
			g1LKr = fDetectorAcceptanceInstance->GetDetAcceptance(DetectorAcceptance::kLKr);
			fDetectorAcceptanceInstance->CleanDetPath();
			fDetectorAcceptanceInstance->FillPath(fMCSimple["gamma"][iTrail]->GetProdPos().Vect(), fMCSimple["gamma"][iTrail]->GetInitialMomentum());
			g2Vol = fDetectorAcceptanceInstance->FirstTouchedDetector();
			g2LKr = fDetectorAcceptanceInstance->GetDetAcceptance(DetectorAcceptance::kLKr);

			FillHisto("g1FirstVol", g1Vol);
			FillHisto("g2FirstVol", g2Vol);
		}
		else{
			g1Vol = DetectorAcceptance::kLKr;
			g2Vol = DetectorAcceptance::kLKr;
			g1LKr = true;
			g2LKr = true;
		}

		//Continue only if photons are in acceptance (forced if no MC)
		if( (g1Vol != DetectorAcceptance::kLAV && g1LKr == true)
				&& (g2Vol != DetectorAcceptance::kLAV && g2LKr == true)){

			//Expected position on LKr
			g1reco = propagate(g1->GetProdPos().Vect(), g1->GetInitialMomentum(), LKrStartPos);
			g2reco = propagate(g2->GetProdPos().Vect(), g2->GetInitialMomentum(), LKrStartPos);

			if(withMC){
				//Comparison reconstructed momenta and energies between reconstructed and real
				g1real = propagate(fMCSimple["gamma"][iLead]->GetProdPos().Vect(), fMCSimple["gamma"][iLead]->GetInitialMomentum(), LKrStartPos);
				g2real = propagate(fMCSimple["gamma"][iTrail]->GetProdPos().Vect(), fMCSimple["gamma"][iTrail]->GetInitialMomentum(), LKrStartPos);

				FillHisto("g1px", g1reco.X(), g1real.X());
				FillHisto("g2px", g2reco.X(), g2real.X());
				FillHisto("g1py", g1reco.Y(), g1real.Y());
				FillHisto("g2py", g2reco.Y(), g2real.Y());
				FillHisto("g1pz", g1reco.Z(), g1real.Z());
				FillHisto("g2pz", g2reco.Z(), g2real.Z());
				FillHisto("g1Reco", g1->GetInitialEnergy(), fMCSimple["gamma"][iLead]->GetInitialEnergy());
				FillHisto("g2Reco", g2->GetInitialEnergy(), fMCSimple["gamma"][iTrail]->GetInitialEnergy());
				//Dont do it if we are likely to have selected the wrong photon
				if(g1EnergyFrac>0.95) FillHisto("energyCalib", g1->GetInitialEnergy(), fMCSimple["gamma"][iLead]->GetInitialEnergy());
				if(g2EnergyFrac>0.95) FillHisto("energyCalib", g2->GetInitialEnergy(), fMCSimple["gamma"][iTrail]->GetInitialEnergy());
				FillHisto("g1EnergyFraction", g1EnergyFrac);
				FillHisto("g2EnergyFraction", g2EnergyFrac);
			}

			FillHisto("g1Energy", g1->GetInitialEnergy());
			FillHisto("g2Energy", g2->GetInitialEnergy());

			//Reconstruct the pi0 candidate and create the KinePart
			pi0.SetInitialEnergy(g1->GetInitialEnergy() + g2->GetInitialEnergy());
			direction = g1->GetInitialMomentum();
			direction.SetMag(g1->GetInitialEnergy());
			g1->SetInitialMomentum(direction);
			direction = g2->GetInitialMomentum();
			direction.SetMag(g2->GetInitialEnergy());
			g2->SetInitialMomentum(direction);
			pi0.SetInitialMomentum(g1->GetInitialMomentum() + g2->GetInitialMomentum());

			//Set the output state as valid (we have a candidate)
			SetOutputState("pi0", kOValid);

			//Fill the pi0 histograms
			FillHisto("pi0Energy", pi0.GetInitialEnergy());
			FillHisto("pi0Mass", sqrt(pow(pi0.GetInitialEnergy(),2) - pi0.GetInitialMomentum().Mag2()));
			FillHisto("pi0MCMass", sqrt(pow(fMCSimple[22][iLead]->GetInitialEnergy() + fMCSimple[22][iTrail]->GetInitialEnergy(), 2) - (fMCSimple[22][iLead]->GetInitialMomentum() + fMCSimple[22][iTrail]->GetInitialMomentum()).Mag2()));
		}
	}

}

//Delete all the created KinePart
while(photons.size()>0){
	delete photons.back();
	photons.pop_back();
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

void Pi0Reconstruction::PostProcess(){

}

//#####################################################
//   This method is called at the end of processing to save plots.
//#####################################################
void Pi0Reconstruction::ExportPlot(){
	//If you want to save them all, just call
	SaveAllPlots();
	//Or you can just save the ones you want with
	//	histogram->Write()
}

//#####################################################
//   This method is called at the end of processing to draw plots.
//#####################################################
void Pi0Reconstruction::DrawPlot(){
	//If you want to draw all the plots, just call
	//	DrawAllPlots();
	//Or do as usual (TCanvas, Draw, ...)
}
