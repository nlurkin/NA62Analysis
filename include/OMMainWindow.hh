/*
 * OMMainWindow.hh
 *
 *  Created on: Jun 17, 2015
 *      Author: nlurkin
 */

#ifndef INCLUDE_OMMAINWINDOW_HH_
#define INCLUDE_OMMAINWINDOW_HH_

#include <RQ_OBJECT.h>

#include "containers.hh"

class TGMainFrame;
class TGTab;
class TGWindow;
class TCanvas;

namespace NA62Analysis {
namespace Core {

class OMAnalyzerWindow;

/// \class OMMainWindow
/// \Brief
/// Main window of the Online Monitor GUI
/// \EndBrief
///
/// \Detailed
/// Enables to create the main window and add Tabs for analyzer and tabs for canvases
/// \EndDetailed
class OMMainWindow {

RQ_OBJECT("TestFrame")
public:
	OMMainWindow(const TGWindow *p, UInt_t w, UInt_t h);
	virtual ~OMMainWindow();

	void Create();
	void AddAnalyzerTab(TString analyzerName);
	TCanvas * AddAnalyzerCanvas(TString analyzerName, TString canvasName);
private:
    TGMainFrame* fMainFrame; ///< Pointer to the main frame
    TGTab *fAnalyzerTab; ///< Pointer to the tab element containing the analyzer tabs
    NA62Analysis::NA62Map<TString, OMAnalyzerWindow*>::type fListAnalyzerTab; ///< Container for the Analyzer windows
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* INCLUDE_OMMAINWINDOW_HH_ */
