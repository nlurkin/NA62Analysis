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

class OMAnalyzerWindow {
public:
	OMAnalyzerWindow(TGCompositeFrame *parent);
	virtual ~OMAnalyzerWindow();

	TCanvas* AddTab(TString tabName);
private:
	TGCompositeFrame *fParentFrame;
	TGTab *fCanvasTab;
	NA62Analysis::NA62Map<TString, TGCompositeFrame*>::type fCanvasList;
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* INCLUDE_OMANALYZERWINDOW_HH_ */
