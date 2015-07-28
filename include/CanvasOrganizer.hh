/*
 * CanvasOrganizer.hh
 *
 *  Created on: Jun 16, 2015
 *      Author: nlurkin
 */

#ifndef INCLUDE_CANVASORGANIZER_HH_
#define INCLUDE_CANVASORGANIZER_HH_

#include <TString.h>
#include <set>

class TH1;
class TH2;
class TGraph;
class TCanvas;

namespace NA62Analysis {
namespace Core {

/// \class CanvasOrganizer
/// \Brief
///  Class that automatically handles canvases drawing
/// \EndBrief
///
/// \Detailed
/// Given a list of histogram to be drawn on a single canvas
/// this class will organize the canvas for the best display (unless given)
/// and will draw and refresh everything automatically.
/// \EndDetailed
class CanvasOrganizer{
public:
	CanvasOrganizer(TString name);
	virtual ~CanvasOrganizer();

	void Draw() const;
	void Update(int currentEvent=0) const;
	void UpdateRef() const;

	void AddHisto(TH1* histoPtr, int row=-1, int col=-1);
	void AddHisto(TH2* histoPtr, int row=-1, int col=-1);
	void AddHisto(TGraph* histoPtr, int row=-1, int col=-1);

	void SetReference(TH1* refPtr, TH1* histoPtr);
	void SetReference(TGraph* refPtr, TGraph* histoPtr);

	void SetSize(int width, int height) {
		/// \MemberDescr
		/// \param width : Width to be set
		/// \param height : Height to be set
		///
		/// Set the dimensions of the canvas
		/// \EndMemberDescr
		fWidth=width; fHeight=height; fChanged=true;
	};
	void SetUpdateFrequency(int frequency) {
		/// \MemberDescr
		/// \param frequency : Frequency (in units of events) with which the
		/// canvas is udpated
		///
		/// Set the update frequency of the CanvasOrganizer
		/// \EndMemberDescr
		fUpdateFrequency=frequency;
	};

	TString GetName() const {
		/// \MemberDescr
		/// \return The Name of the CanvasOrganizer
		/// \EndMemberDescr
		return fName;
	};
	void SetCanvas(TCanvas* c);
	TCanvas *const GetCanvas() const {
		/// \MemberDescr
		/// \return Pointer to the TCanvas
		/// \EndMemberDescr
		return fCanvas;
	};
private:
	/// \struct size_t
	/// \Brief
	/// Struct containing two int for width and height
	/// \EndBrief
	struct size_t{
		int width;  ///< Width
		int height; ///< Height
	};

	/// \union plotType_u
	/// \Brief
	/// Union containing a pointer to a TH1 derived or TGraph derived class
	/// \EndBrief
	union plotType_u{
		TH1* histo;    ///< Pointer to TH1 derived
		TGraph* graph; ///< Pointer to TGraph derived
	};

	/// Type of Plot in the container
	enum plotType_e {TTH1, TTH2, TTGraph};

	/// \struct plot_t
	/// \Brief
	/// Struct containing the plot plotType_t union (pointer to a plot)
	/// and the tag indicating which type of plot is contained.
	/// \EndBrief
	struct plot_t{
		plotType_u ptr; ///< Pointer to the plot
		plotType_e tag; ///< Indicated type of plot
		plotType_u ref; ///< Pointer to reference plot
		int row;          ///< x position of the plot on the grid
		int col;          ///< y position of the plot on the grid
	};

	size_t computeSize(int nElements) const;
	void drawPlot(plot_t p) const;
	static int findAvailableSlot(std::set<int> busy);

	int fUpdateFrequency;     ///< Update frequency (in events) of the canvas
	int fWidth;               ///< Width of the canvas
	int fHeight;              ///< Height of the canvas
	mutable bool fChanged;    ///< True when the canvas has been modified and must be redrawn completely
	const TString fName;      ///< Name of the CanvasOrganizer
	mutable TCanvas *fCanvas; ///< Pointer to the actual TCanvas

	std::vector<plot_t> fHistos; ///< Container for plots
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* INCLUDE_CANVASORGANIZER_HH_ */
