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

#define PRINTVAR(v) #v << "= " << v << " "

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

	//if canvas cannot accommodate all histo, resize it
	if((unsigned int)(fWidth*fHeight)<fHistos.size()) {
		size_t s = computeSize(fHistos.size());
		fCanvas->Divide(s.width, s.height);

	}
	else fCanvas->Divide(fWidth, fHeight);

	std::vector<plot_t> delayedPlots;
	std::set<int> busySlots;
	//Place all histos on the canvas
	for(auto it : fHistos){
		std::cout << PRINTVAR(it.row) << " " << PRINTVAR(it.col) << std::endl;
		if(it.row!=-1 && it.row<fHeight && it.col!=-1 && it.col<fWidth){
			int canvasIndex = it.row*fWidth + it.col+1;
			std::cout << "Printing now with index " << canvasIndex << std::endl;
			fCanvas->cd(canvasIndex);
			busySlots.insert(canvasIndex);
			drawPlot(it);
		}
		else{
			std::cout << "Printing later" << std::endl;
			delayedPlots.push_back(it);
		}

	}

	int slot;
	for(auto it : delayedPlots){
		slot = findAvailableSlot(busySlots);
		busySlots.insert(slot);
		fCanvas->cd(slot);
		drawPlot(it);
	}
	fChanged = false;
}

void CanvasOrganizer::drawPlot(plot_t p) const{
	/// \MemberDescr
	/// \param p: Plot to draw
	///
	/// Properly draws the histogram depending on its type
	/// \EndMemberDescr

	if(p.tag == TTH1){
		p.ptr.histo->Draw();
		if(p.ref.histo!=nullptr){
			p.ref.histo->SetLineColor(2);
			p.ref.histo->SetLineStyle(2);
			p.ref.histo->Draw("SAME");
		}
	}
	else if(p.tag == TTH2) p.ptr.histo->Draw("colz");
	else if(p.tag == TTGraph){
		p.ptr.graph->Draw("A*");
		if(p.ref.graph!=nullptr){
			p.ref.graph->Draw("SAMEA*");
		}
	}
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

void CanvasOrganizer::AddHisto(TH1* histoPtr, int row, int col) {
	/// \MemberDescr
	/// \param histoPtr : Pointer to added histogram
	///
	/// Add an histogram in the list of managed histograms
	/// \EndMemberDescr

	plot_t t;
	t.ptr.histo = histoPtr;
	t.tag = TTH1;
	t.ref.histo = nullptr;
	t.row = row;
	t.col = col;
	fHistos.push_back(t);
	fChanged=true;
}

void CanvasOrganizer::AddHisto(TH2* histoPtr, int row, int col) {
	/// \MemberDescr
	/// \param histoPtr : Pointer to added histogram
	///
	/// Add an histogram in the list of managed histograms
	/// \EndMemberDescr

	plot_t t;
	t.ptr.histo = histoPtr;
	t.tag = TTH2;
	t.ref.histo = nullptr;
	t.row = row;
	t.col = col;
	fHistos.push_back(t);
	fChanged=true;
}

void CanvasOrganizer::AddHisto(TGraph* histoPtr, int row, int col) {
	/// \MemberDescr
	/// \param histoPtr : Pointer to added histogram
	///
	/// Add an histogram in the list of managed histograms
	/// \EndMemberDescr

	plot_t t;
	t.ptr.graph = histoPtr;
	t.tag = TTGraph;
	t.ref.graph = nullptr;
	t.row = row;
	t.col = col;
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
	/// \MemberDescr
	/// \param refPtr Pointer to a reference histogram
	/// \param histoPtr Pointer to the histogram on which the reference should be overlaid
	///
	/// Link a reference histogram to an histogram already added to the canvas.
	/// \EndMemberDescr

	for(auto &it : fHistos){
		if(it.ptr.histo==histoPtr) it.ref.histo=refPtr;
	}
}

void CanvasOrganizer::SetReference(TGraph* refPtr, TGraph* histoPtr) {
	/// \MemberDescr
	/// \param refPtr Pointer to a reference histogram
	/// \param histoPtr Pointer to the histogram on which the reference should be overlaid
	///
	/// Link a reference histogram to an histogram already added to the canvas.
	/// \EndMemberDescr

	for(auto &it : fHistos){
		if(it.ptr.graph==histoPtr) it.ref.graph=refPtr;
	}
}

void CanvasOrganizer::UpdateRef() const {
	/// \MemberDescr
	/// Update reference histograms: scale them to the same integral as the linked histogram.
	/// \EndMemberDescr

	for(auto it : fHistos){
		if(it.ref.histo!=nullptr){
			if(it.tag==TTH1){
				it.ref.histo->Scale(it.ptr.histo->Integral()/it.ref.histo->Integral(), "");
			}
		}
	}
}

int CanvasOrganizer::findAvailableSlot(std::set<int> busy){
	/// \MemberDescr
	/// \param busy: Sorted set of numbers
	///
	/// Returns the first available number not in busy (starting from 1)
	/// \EndMemberDescr

	int i=1;
	for(auto  val : busy){
		if(val>i) return i;
		i++;
	}
	return i+1;
}

} /* namespace Core */
} /* namespace NA62Analysis */
