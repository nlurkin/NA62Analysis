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
	/// \MemberDescr
	/// \param parent: parent element
	///
	/// Default constructor. Creates the tab element and add it to the parent frame
	/// \EndMemberDescr

	fCanvasTab = new TGTab(fParentFrame);
	fParentFrame->AddFrame(fCanvasTab, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
}

OMAnalyzerWindow::~OMAnalyzerWindow() {
	/// \MemberDescr
	/// Default destructor.
	/// \EndMemberDescr
}

TCanvas* OMAnalyzerWindow::AddTab(TString tabName) {
	/// \MemberDescr
	/// \param tabName : Name of the tab to add
	/// \return Pointer to the newly created canvas, to be used to draw in the created
	/// window.
	///
	/// Add a tab to the analyzer window
	/// \EndMemberDescr

	TGCompositeFrame *f = fCanvasTab->AddTab(tabName);
	TRootEmbeddedCanvas *c = new TRootEmbeddedCanvas(tabName, f, gClient->GetDisplayHeight()-100, gClient->GetDisplayWidth()-100);
	fCanvasList.insert(std::make_pair(tabName, f));
	f->AddFrame(c, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
	return c->GetCanvas();
}

} /* namespace Core */
} /* namespace NA62Analysis */
