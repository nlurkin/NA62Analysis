/*
 * CanvasOrganizer.cc
 *
 *  Created on: Jun 16, 2015
 *      Author: nlurkin
 */

#include <iostream>
#include "CanvasOrganizer.hh"
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TList.h>

namespace NA62Analysis {
namespace Core {

CanvasOrganizer::CanvasOrganizer(TString name):
	fUpdateFrequency(1),
	fWidth(0),
	fLength(0),
	fChanged(true),
	fName(name),
	fCanvas(NULL)
{
}

CanvasOrganizer::~CanvasOrganizer() {
	if(fCanvas) delete fCanvas;
}

void CanvasOrganizer::Draw() {
	if(fCanvas) delete fCanvas;
	fCanvas = new TCanvas(fName, fName);
	fCanvas->Draw();
	fCanvas->Divide(fHistos.size(), 1);
	int i=0;
	for(auto it : fHistos){
		i++;
		fCanvas->cd(i);
		if(it.tag == TTH1) it.ptr.histo->Draw();
		else if(it.tag == TTH2) it.ptr.histo->Draw("colz");
		else if(it.tag == TTGraph) it.ptr.graph->Draw("A*");
	}
	fChanged = false;
}

void CanvasOrganizer::Update(int currentEvent) {
	if(fCanvas) {
		std::cout << fCanvas << std::endl;
		if(currentEvent % fUpdateFrequency==0){
			if(fChanged) Draw();
			fCanvas->Update();
			fCanvas->Draw();
		}
	}
}

void CanvasOrganizer::AddHisto(TH1* histoPtr) {
	plot_t t;
	t.ptr.histo = histoPtr;
	t.tag = TTH1;
	fHistos.push_back(t);
	fChanged=true;
};

void CanvasOrganizer::AddHisto(TH2* histoPtr) {
	plot_t t;
	t.ptr.histo = histoPtr;
	t.tag = TTH2;
	fHistos.push_back(t);
	fChanged=true;
};

void CanvasOrganizer::AddHisto(TGraph* histoPtr) {
	plot_t t;
	t.ptr.graph = histoPtr;
	t.tag = TTGraph;

	fHistos.push_back(t);
	fChanged=true;
};

} /* namespace Core */
} /* namespace NA62Analysis */
