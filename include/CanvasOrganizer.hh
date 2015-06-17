/*
 * CanvasOrganizer.hh
 *
 *  Created on: Jun 16, 2015
 *      Author: nlurkin
 */

#ifndef INCLUDE_CANVASORGANIZER_HH_
#define INCLUDE_CANVASORGANIZER_HH_

#include <TString.h>


class TH1;
class TH2;
class TGraph;
class TCanvas;

namespace NA62Analysis {
namespace Core {

class CanvasOrganizer {
public:
	CanvasOrganizer(TString name);
	virtual ~CanvasOrganizer();

	void Draw();
	void Update(int currentEvent=0);
	void AddHisto(TH1* histoPtr);
	void AddHisto(TH2* histoPtr);
	void AddHisto(TGraph* histoPtr);
	void SetSize(int width, int length) { fWidth=width; fLength=length; fChanged=true; };
	void SetUpdateFrequency(int frequency) { fUpdateFrequency=frequency; };

	TString GetName() { return fName; };
private:
	int fUpdateFrequency;
	int fWidth, fLength;
	bool fChanged;
	const TString fName;
	TCanvas *fCanvas;

	union plotType_u{
		TH1* histo;
		TGraph* graph;
	};
	enum plotType_e {TTH1, TTH2, TTGraph};
	struct plot_t{
		plotType_u ptr;
		plotType_e tag;
	};

	std::vector<plot_t> fHistos;
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* INCLUDE_CANVASORGANIZER_HH_ */
