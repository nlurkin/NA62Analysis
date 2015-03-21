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

class TH1I;
class TH2F;
class TGraph;
class TTree;

class VertexCDA : public NA62Analysis::Analyzer
{
	public:
		VertexCDA(NA62Analysis::Core::BaseAnalysis *ba);
		void InitHist();
		void DefineMCSimple();
		void InitOutput();
		void Process(int i);
		void PostProcess();
		void ExportPlot();
		void DrawPlot();
	private:
		void Publish();
		TVector3 GetIntersection(TVector3 pos1, TVector3 p1, TVector3 pos2, TVector3 p2);
	protected:
		//Add the variables that should be registered as output
		TVector3 fVertex;
};

#endif /* VERTEXCDA_HH_ */
