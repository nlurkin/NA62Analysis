/*
 * OMAnalyzerWindow.hh
 *
 *  Created on: Jun 18, 2015
 *      Author: nlurkin
 */

#ifndef INCLUDE_OMANALYZERWINDOW_HH_
#define INCLUDE_OMANALYZERWINDOW_HH_

#include <RQ_OBJECT.h>
#include "containers.hh"

class TGCompositeFrame;
class TCanvas;
class TGTab;

namespace NA62Analysis {
namespace Core {

/// \class OMAnalyzerWindow
/// \Brief
/// Describes a window for an analyzer
/// \EndBrief
///
/// \Detailed
/// Enables to add Tabs for canvases
/// \EndDetailed
class OMAnalyzerWindow {
public:
	OMAnalyzerWindow(TGCompositeFrame *parent);
	virtual ~OMAnalyzerWindow();

	TCanvas* AddTab(TString tabName);
private:
	TGCompositeFrame *fParentFrame; ///< Pointer to parent frame
	TGTab *fCanvasTab; ///< Pointer to the tab element
	NA62Analysis::NA62Map<TString, TGCompositeFrame*>::type fCanvasList; ///< Container for list of canvases for this analyzer
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* INCLUDE_OMANALYZERWINDOW_HH_ */
