//Mandatory includes
#include "LKrPhotonMC.hh"

#include <stdlib.h>
#include <iostream>

#include <TChain.h>

#include "Event.hh"

#include "TRecoLKrCandidateMC.hh"
#include "MCSimple.hh"
#include "functions.hh"
#include "Persistency.hh"
using namespace std;
using namespace NA62Analysis;
using namespace NA62Constants;


/// \class LKrPhotonMC
/// \Brief
/// Example Analyzer linking MC photons to LKr clusters for events with
///	at least 2 photons coming from a pi0 from the beam kaon
/// \EndBrief
///
/// \Detailed
///	Get all the LKr clusters and sort them by energy. The MC photons are
///	propagated on the LKr plane. The association between MC photon and cluster
///	is done by comparing the distance between the cluster position and the
///	propagated MC photon position on LKr and associating those for which the
/// distance is minimum and lower than 50mm.
/// \EndDetailed

LKrPhotonMC::LKrPhotonMC(Core::BaseAnalysis *ba) : Analyzer(ba, "LKrPhotonMC")
{
	fCandidates = new TClonesArray("TRecoLKrCandidateMC", 10);
	RequestTree("LKr", new TRecoLKrEvent);
	RequestTree("GigaTracker", new TRecoGigaTrackerEvent);
	RequestTree("Spectrometer", new TRecoSpectrometerEvent);
}

void LKrPhotonMC::InitOutput(){
	OpenNewTree("LKrMCAssociated", "LKrMCAssociated");
	AddBranch<TClonesArray>("LKrMCAssociated", "fCandidates", fCandidates);
	CreateStandardTree("LKrCandidates", "LKrCandidates");
}

void LKrPhotonMC::InitHist(){
}

void LKrPhotonMC::DefineMCSimple(){
	int kID = fMCSimple.AddParticle(0, 321);
	int pi0ID = fMCSimple.AddParticle(kID, 111);
	fMCSimple.AddParticle(pi0ID, 22);
	fMCSimple.AddParticle(pi0ID, 22);
}

void LKrPhotonMC::StartOfRunUser(){

}

void LKrPhotonMC::StartOfBurstUser(){

}

void LKrPhotonMC::EndOfBurstUser(){

}


void LKrPhotonMC::Process(int iEvent){
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
	vertex = *(TVector3*)GetOutput("VertexCDA.Vertex", state);

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
		lkrCandMC->copy(sortedCandidates.back());
		lkrCandMC->SetMCParticleID(-1);
		candidatesNumber++;
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

	FillTrees();
	ExportEvent();
}

void LKrPhotonMC::PostProcess(){
	fCandidates->Clear();
}

void LKrPhotonMC::EndOfRunUser(){

}


void LKrPhotonMC::ExportPlot(){
}

void LKrPhotonMC::DrawPlot(){
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
