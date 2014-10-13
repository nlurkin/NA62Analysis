/*
 * Pi0Reconstruction.hh
 *
 *  Created on: Jan 8, 2013
 *      Author: nlurkin
 */

#ifndef Pi0Reconstruction_HH_
#define Pi0Reconstruction_HH_

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

class Pi0Reconstruction : public Analyzer
{
	public:
		Pi0Reconstruction(BaseAnalysis *ba);
		void InitHist();
		void InitOutput();
		void DefineMCSimple(MCSimple *fMCSimple);
		void Process(int i);
		void PostProcess();
		void ExportPlot();
		void DrawPlot();
		void PrintClass();
	private:
		void Publish();
	protected:
		//Add the variables that should be registered as output
		KinePart pi0;
};

#endif /* Pi0Reconstruction_HH_ */
