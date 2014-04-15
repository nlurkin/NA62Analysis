/*
 * VertexCDA.hh
 *
 *  Created on: Jan 8, 2013
 *      Author: nlurkin
 */

#ifndef VERTEXCDA_HH_
#define VERTEXCDA_HH_

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

class VertexCDA : public Analyzer
{
	public:
		VertexCDA(BaseAnalysis *ba);
		void InitHist();
		void DefineMCSimple(MCSimple *fMCSimple);
		void InitOutput();
		void Process(int i, MCSimple &fMCSimple, Event* MCTruthEvent);
		void PostProcess();
		void ExportPlot();
		void DrawPlot();
		void PrintClass();
	private:
		void Publish();
		TVector3 GetIntersection(TVector3 pos1, TVector3 p1, TVector3 pos2, TVector3 p2);
	protected:
		//Add the variables that should be registered as output
		TVector3 fVertex;
};

#endif /* VERTEXCDA_HH_ */