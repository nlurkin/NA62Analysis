#include "TRecoLKrCandidateMC.hh"

ClassImp(TRecoLKrCandidateMC)


void TRecoLKrCandidateMC::copy(TRecoLKrCandidate *c){
	SetCluster77Energy(c->GetCluster77Energy());
	SetCluster77Enoise(c->GetCluster77Enoise());
	SetClusterChi2RMS(c->GetClusterChi2RMS());
	SetClusterDDeadCell(c->GetClusterDDeadCell());
	SetClusterEnergy(c->GetClusterEnergy());
	SetClusterEnergyError(c->GetClusterEnergyError());
	SetClusterEnoise(c->GetClusterEnoise());
	SetClusterKe3Energy(c->GetClusterKe3Energy());
	SetClusterRMSX(c->GetClusterRMSX());
	SetClusterRMSY(c->GetClusterRMSY());
	SetClusterSeedEnergy(c->GetClusterSeedEnergy());
	SetClusterStatus(c->GetClusterStatus());
	SetClusterTime(c->GetClusterTime());
	SetClusterTimeLateralCell(c->GetClusterTimeLateralCell());
	SetClusterUEnergy(c->GetClusterUEnergy());
	SetClusterUTime(c->GetClusterUTime());
	SetClusterX(c->GetClusterX());
	SetClusterY(c->GetClusterY());
	for(int i=0; i<50; i++){
		SetEnergy77Cell(i, c->GetEnergy77Cell(i));
		SetFlag77Cell(i, c->GetFlag77Cell(i));
		SetId77Cell(i, c->GetId77Cell(i));
		SetTime77Cell(i, c->GetTime77Cell(i));
	}
	SetId(c->GetId());
	SetIdSeed(c->GetIdSeed());
	SetN77Cells(c->GetN77Cells());
	SetNCells(c->GetNCells());
	SetSpaceChargeCorr(c->GetSpaceChargeCorr());
}
