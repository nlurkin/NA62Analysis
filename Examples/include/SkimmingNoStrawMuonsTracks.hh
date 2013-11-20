#ifndef SKIMMINGNOSTRAWMUONSTRACKS_HH
#define SKIMMINGNOSTRAWMUONSTRACKS_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include "MCSimple.hh"
#include "DetectorAcceptance.hh"
#include <TCanvas.h>
using namespace std;

class TH1I;
class TH2F;
class TGraph;
class TTree;

//Change everywhere SkimmingNoStrawMuonsTracks by you Analyzer name
//Add the protected and private members and methods you want to add

class SkimmingNoStrawMuonsTracks : public Analyzer
{
	public:
		SkimmingNoStrawMuonsTracks(BaseAnalysis *ba);
		void InitHist();
		void InitOutput();
		void DefineMCSimple(MCSimple *fMCSimple);
		void Process(int i, MCSimple &fMCSimple, Event* MCTruthEvent);
		void StartOfBurstUser();
		void EndOfBurstUser();
		void StartOfRunUser();
		void EndOfRunUser();
		void PostProcess();
		void ExportPlot();
		void DrawPlot();
	private:
		void Publish();
	protected:
		//Add the variables that should be registered as output

		//Add your parameters (allowed : bool, char, string, TString, int, long, float, double)
		bool useMUV;
		//Add the variables to branch to the output trees
};
#endif
