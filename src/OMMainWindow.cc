/*
 * OMMainWindow.cc
 *
 *  Created on: Jun 17, 2015
 *      Author: nlurkin
 */

#include "OMMainWindow.hh"
#include <TGTab.h>
#include <TGFrame.h>

#include "OMAnalyzerWindow.hh"

namespace NA62Analysis {
namespace Core {

OMMainWindow::OMMainWindow(const TGWindow *p, UInt_t w, UInt_t h) {
	fMainFrame = new TGMainFrame(p, w, h, kMainFrame | kVerticalFrame);
	fAnalyzerTab = new TGTab(fMainFrame);

	fMainFrame->AddFrame(fAnalyzerTab, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
}

OMMainWindow::~OMMainWindow() {

}

void OMMainWindow::Create() {
	fMainFrame->MapSubwindows();
	fMainFrame->Resize();

	fMainFrame->MapWindow();
}

void OMMainWindow::AddAnalyzerTab(TString analyzerName) {
	OMAnalyzerWindow *window = new OMAnalyzerWindow(fAnalyzerTab->AddTab(analyzerName));

	fListAnalyzerTab.insert(std::make_pair(analyzerName, window));
}

TCanvas* OMMainWindow::AddAnalyzerCanvas(TString analyzerName,
		TString canvasName) {
	OMAnalyzerWindow * w = fListAnalyzerTab.find(analyzerName)->second;
	return w->AddTab(canvasName);
}

} /* namespace Core */
} /* namespace NA62Analysis */

