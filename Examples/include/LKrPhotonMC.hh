#ifndef LKRPHOTONMC_HH
#define LKRPHOTONMC_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include "MCSimple.hh"
#include "DetectorAcceptance.hh"
#include <TCanvas.h>
#include "TRecoLKrCandidate.hh"
using namespace std;

class TH1I;
class TH2F;
class TGraph;
class TTree;

//Change everywhere LKrPhotonMC by you Analyzer name
//Add the protected and private members and methods you want to add

class LKrPhotonMC : public Analyzer
{
	public:
		LKrPhotonMC(BaseAnalysis *ba);
		void InitHist();
		void InitOutput();
		void DefineMCSimple();
		void Process(int i);
		void StartOfBurstUser();
		void EndOfBurstUser();
		void StartOfRunUser();
		void EndOfRunUser();
		void PostProcess();
		void ExportPlot();
		void DrawPlot();
	private:
		void Publish();
		int getMostEnergetic(vector<double> e);
	protected:
		//Add the variables that should be registered as output

		//Add your parameters (allowed : bool, char, string, TString, int, long, float, double)

		//Add the variables to branch to the output trees
		TClonesArray *fCandidates;
		int param1;
};
#endif
