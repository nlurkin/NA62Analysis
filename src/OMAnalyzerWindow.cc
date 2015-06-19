/*
 * OMAnalyzerWindow.cc
 *
 *  Created on: Jun 18, 2015
 *      Author: nlurkin
 */

#include "OMAnalyzerWindow.hh"
#include <TRootEmbeddedCanvas.h>
#include <TGTab.h>

namespace NA62Analysis {
namespace Core {

OMAnalyzerWindow::OMAnalyzerWindow(TGCompositeFrame* parent):
	fParentFrame(parent)
{
	fCanvasTab = new TGTab(fParentFrame);
	fParentFrame->AddFrame(fCanvasTab, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
}

OMAnalyzerWindow::~OMAnalyzerWindow() {
}

TCanvas* OMAnalyzerWindow::AddTab(TString tabName) {
	TGCompositeFrame *f = fCanvasTab->AddTab(tabName);
	TRootEmbeddedCanvas *c = new TRootEmbeddedCanvas(tabName, f, gClient->GetDisplayHeight()-100, gClient->GetDisplayWidth()-100);
	fCanvasList.insert(std::make_pair(tabName, f));
	f->AddFrame(c, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
	return c->GetCanvas();
}

} /* namespace Core */
} /* namespace NA62Analysis */
