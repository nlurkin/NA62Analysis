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
#include <cmath>

namespace NA62Analysis {
namespace Core {

CanvasOrganizer::CanvasOrganizer(TString name):
	fUpdateFrequency(1),
	fWidth(0),
	fHeight(0),
	fChanged(true),
	fName(name),
	fCanvas(NULL)
{
	/// \MemberDescr
	/// \param name : Name of the CanvasOrganizer
	///
	/// Default constructor.
	/// \EndMemberDescr
}

CanvasOrganizer::~CanvasOrganizer() {
	/// \MemberDescr
	/// Default destructor. Delete the canvas if exists
	/// \EndMemberDescr

	if(fCanvas) delete fCanvas;
}

void CanvasOrganizer::Draw() const {
	/// \MemberDescr
	/// Draw the canvas. If dimensions are not given (or cannot fit all histograms),
	/// width and height are computed according to internal algorithm
	/// (width/height ration must be < 1).
	/// Histograms are then placed on the canvas and it is drawn.
	/// \EndMemberDescr

	//If canvas does not exist, create it.
	if(!fCanvas) fCanvas = new TCanvas(fName, fName);
	fCanvas->Clear();
	fCanvas->Draw();

	//if canvas cannot accomodate all histo, resize it
	if((unsigned int)(fWidth*fHeight)<fHistos.size()) {
		size_t s = computeSize(fHistos.size());
		fCanvas->Divide(s.width, s.height);

	}
	else fCanvas->Divide(fWidth, fHeight);

	//Place all histos on the canvas
	int i=0;
	for(auto it : fHistos){
		i++;
		fCanvas->cd(i);
		if(it.tag == TTH1){
			it.ptr.histo->Draw();
			if(it.ref.histo!=nullptr){
				it.ref.histo->SetLineColor(2);
				it.ref.histo->SetLineStyle(2);
				it.ref.histo->Draw("SAME");
			}
		}
		else if(it.tag == TTH2) it.ptr.histo->Draw("colz");
		else if(it.tag == TTGraph){
			it.ptr.graph->Draw("A*");
			if(it.ref.graph!=nullptr){
				it.ref.graph->Draw("SAMEA*");
			}
		}
	}
	fChanged = false;
}

void CanvasOrganizer::Update(int currentEvent) const {
	/// \MemberDescr
	/// \param currentEvent : Currently processed event number
	///
	/// Update the canvas if it exist and the current event number
	/// matches the update frequency
	/// \EndMemberDescr

	if(fCanvas) {
		if(currentEvent % fUpdateFrequency==0){
			if(fChanged) Draw();
			UpdateRef();
			fCanvas->Update();
			fCanvas->Draw();
		}
	}
}

void CanvasOrganizer::AddHisto(TH1* histoPtr) {
	/// \MemberDescr
	/// \param histoPtr : Pointer to added histogram
	///
	/// Add an histogram in the list of managed histograms
	/// \EndMemberDescr

	plot_t t;
	t.ptr.histo = histoPtr;
	t.tag = TTH1;
	t.ref.histo = nullptr;
	fHistos.push_back(t);
	fChanged=true;
}

void CanvasOrganizer::AddHisto(TH2* histoPtr) {
	/// \MemberDescr
	/// \param histoPtr : Pointer to added histogram
	///
	/// Add an histogram in the list of managed histograms
	/// \EndMemberDescr

	plot_t t;
	t.ptr.histo = histoPtr;
	t.tag = TTH2;
	t.ref.histo = nullptr;
	fHistos.push_back(t);
	fChanged=true;
}

void CanvasOrganizer::AddHisto(TGraph* histoPtr) {
	/// \MemberDescr
	/// \param histoPtr : Pointer to added histogram
	///
	/// Add an histogram in the list of managed histograms
	/// \EndMemberDescr

	plot_t t;
	t.ptr.graph = histoPtr;
	t.tag = TTGraph;
	t.ref.graph = nullptr;
	fHistos.push_back(t);
	fChanged=true;
}

void CanvasOrganizer::SetCanvas(TCanvas* c) {
	/// \MemberDescr
	/// \param c : Pointer to a TCanvas
	///
	/// Assign a TCanvas to the CanvasOrganizer
	/// \EndMemberDescr

	fCanvas = c;
	fChanged = true;
}

CanvasOrganizer::size_t CanvasOrganizer::computeSize(int nElements) const {
	/// \MemberDescr
	/// \param nElements : Number of elements to fit on the canvas
	/// \return size_t structure with ideal dimensions
	///
	/// Computes the ideal size for the canvas to accommodate all elements.
	/// The requirement is that the ratio width/height should never be larger than 1.
	/// \EndMemberDescr

	size_t s;
	s.width = ceil(sqrt(nElements));
	s.height = ceil(nElements/(double)s.width);
	return s;
}

void CanvasOrganizer::SetReference(TH1* refPtr, TH1* histoPtr) {
	for(auto &it : fHistos){
		if(it.ptr.histo==histoPtr) it.ref.histo=refPtr;
	}
}

void CanvasOrganizer::SetReference(TGraph* refPtr, TGraph* histoPtr) {
	for(auto &it : fHistos){
		if(it.ptr.graph==histoPtr) it.ref.graph=refPtr;
	}
}

void CanvasOrganizer::UpdateRef() const {
	for(auto it : fHistos){
		if(it.ref.histo!=nullptr){
			if(it.tag==TTH1){
				it.ref.histo->Scale(it.ptr.histo->Integral()/it.ref.histo->Integral(), "");
			}
		}
	}
}

} /* namespace Core */
} /* namespace NA62Analysis */
