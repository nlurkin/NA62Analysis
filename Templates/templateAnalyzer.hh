#ifndef TEMPLATEANALYZER_HH
#define TEMPLATEANALYZER_HH

#include <stdlib.h>
#include <vector>
#include "Analyzer.hh"
#include "MCSimple.hh"
#include "DetectorAcceptance.hh"
#include <TCanvas.h>

class TH1I;
class TH2F;
class TGraph;
class TTree;

//Change everywhere templateAnalyzer by you Analyzer name
//Add the protected and private members and methods you want to add

class templateAnalyzer : public NA62Analysis::Analyzer
{
	public:
		templateAnalyzer(NA62Analysis::Core::BaseAnalysis *ba);
		void InitHist();
		void InitOutput();
		void DefineMCSimple();
		void Process(int iEvent);
		void StartOfBurstUser();
		void EndOfBurstUser();
		void StartOfRunUser();
		void EndOfRunUser();
		void PostProcess();
		void DrawPlot();
	protected:
		//Add the variables that should be registered as output

		//Add your parameters (allowed : bool, char, string, TString, int, long, float, double)

		//Add the variables to branch to the output trees
};
#endif
