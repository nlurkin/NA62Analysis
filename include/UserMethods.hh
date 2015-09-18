/*
 * UserMethods.hh
 *
 *  Created on: 21 Jan 2014
 *      Author: ncl
 */

#ifndef USERMETHODS_HH_
#define USERMETHODS_HH_

#include <map>
#include <sstream>

#include <TString.h>
#include <TChain.h>
#include <TGraph.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>
#include <TDetectorVEvent.hh>

#include "HistoHandler.hh"
#include "FWEnums.hh"
#include "MCSimple.hh"
#include "IOHandler.hh"
#include "Verbose.hh"

class RawHeader;
class Event;

namespace NA62Analysis {

class DetectorAcceptance;
namespace Core {
class BaseAnalysis;
class CanvasOrganizer;
} /* namespace Core */


/// \class UserMethods
/// \Brief
/// Interface class for the end user for methods whose implementation is not actually done in the analyzer class.
/// \EndBrief
///
/// \Detailed
/// This class defines a list of methods already implemented in members objects of the analyzer but that we must keep
///	in the analyzer class as well for backward compatibility and/or shorter syntax.
/// \EndDetailed
class UserMethods : public Verbose
{
public:
	/// List of possible states for the output variables.
	enum OutputState {kOUninit, kOValid, kOInvalid};

	UserMethods(Core::BaseAnalysis *ba);
	UserMethods(Core::BaseAnalysis *ba, std::string name);
	UserMethods(const UserMethods&);
	virtual ~UserMethods();

	//###### Histograms related
	//Histogram booking methods
	void BookHisto(TString name, TH1* const histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TH2* const histo, bool refresh=false, TString directory="");
	void BookHisto(TString name, TGraph* const histo, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH1* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TH2* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TString baseName, TGraph* const histo, int number, bool refresh=false, TString directory="");

	void BookHisto(TH1* const histo, bool refresh=false, TString directory="");
	void BookHisto(TH2* const histo, bool refresh=false, TString directory="");
	void BookHisto(TGraph* const histo, bool refresh=false, TString directory="");
	void BookHistoArray(TH1* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TH2* const histo, int number, bool refresh=false, TString directory="");
	void BookHistoArray(TGraph* const histo, int number, bool refresh=false, TString directory="");

	//Histogram filling methods
	void FillHisto(TString name, TString x, double w);
	void FillHisto(TString name, TString x, double y, double w);
	void FillHisto(TString name, TString x, TString y, double w);
	void FillHisto(TString name, double x, double w);
	void FillHisto(TString name, double x);
	void FillHisto(TString name, double x, double y, double w);

	void FillHistoArray(TString baseName, int index, TString x, double w);
	void FillHistoArray(TString baseName, int index, TString x, double y, double w);
	void FillHistoArray(TString baseName, int index, TString x, TString y, double w);
	void FillHistoArray(TString baseName, int index, double x, double w);
	void FillHistoArray(TString baseName, int index, double x);
	void FillHistoArray(TString baseName, int index, double x, double y, double w);

	Core::HistoHandler::IteratorTH1 GetIteratorTH1();
	Core::HistoHandler::IteratorTH1 GetIteratorTH1(TString baseName);
	Core::HistoHandler::IteratorTH2 GetIteratorTH2();
	Core::HistoHandler::IteratorTH2 GetIteratorTH2(TString baseName);
	Core::HistoHandler::IteratorTGraph GetIteratorTGraph();
	Core::HistoHandler::IteratorTGraph GetIteratorTGraph(TString baseName);
	NA62Analysis::NA62Map<TString, Core::CanvasOrganizer*>::type GetCanvases();

	//Alias for user to make difference between TH1 and TH2
	inline void FillHisto2(TString name, TString x, double y, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram.
		/// \param x : abscissa
		/// \param y : ordinate
		/// \param w : weight
		///
		/// Alias to FillHisto(TString, TString, double, double)
		/// \EndMemberDescr
	FillHisto(name,x,y,w);};
	inline void FillHisto2(TString name, TString x, TString y, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram.
		/// \param x : abscissa
		/// \param y : ordinate
		/// \param w : weight
		///
		/// Alias to FillHisto(TString, TString, TString, double)
		/// \EndMemberDescr
	FillHisto(name,x,y,w);};
	inline void FillHisto2(TString name, double x, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram.
		/// \param x : abscissa
		/// \param w : weight
		///
		/// Alias to FillHisto(TString, double, double)
		/// \EndMemberDescr
	FillHisto(name,x,w);};
	inline void FillHisto2(TString name, double x, double y, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram.
		/// \param x : abscissa
		/// \param y : ordinate
		/// \param w : weight
		///
		/// Alias to FillHisto(TString, double, double, double)
		/// \EndMemberDescr
	FillHisto(name,x,y,w);};

	inline void FillHisto2Array(TString name, int index, TString x, double y, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram. The index will be appended
		///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
		/// \param x : abscissa
		/// \param y : ordinate
		/// \param w : weight
		///
		/// Alias to FillHistoArray(TString, int, TString, double, double)
		/// \EndMemberDescr
	FillHistoArray(name, index, x, y, w);};
	inline void FillHisto2Array(TString name, int index, TString x, TString y, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram. The index will be appended
		///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
		/// \param x : abscissa
		/// \param y : ordinate
		/// \param w : weight
		///
		/// Alias to FillHistoArray(TString, int, TString, TString, double)
		/// \EndMemberDescr
	FillHistoArray(name, index, x, y, w);};
	inline void FillHisto2Array(TString name, int index, double x, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram. The index will be appended
		///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
		/// \param x : abscissa
		/// \param w : weight
		///
		/// Alias to FillHistoArray(TString, int, double, double)
		/// \EndMemberDescr
	FillHistoArray(name, index, x, w);};
	inline void FillHisto2Array(TString name, int index, double x, double y, double w) {
		/// \MemberDescr
		/// \param name : Name of the histogram. The index will be appended
		///	\param index : Array index of the Histogram to fill. If booked with BookHistoArray, starts at 0 to N-1.
		/// \param x : abscissa
		/// \param y : ordinate
		/// \param w : weight
		///
		/// Alias to FillHistoArray(TString, int, double, double, double)
		/// \EndMemberDescr
	FillHistoArray(name, index, x, y, w);};

	//Methods for drawing plots on screen
	void DrawAllPlots();
	void UpdatePlots(int evtNbr);
	void SetUpdateInterval(int interval);
	int GetUpdateInterval() const;
	void CreateCanvas(TString name, int width=0, int length=0);
	bool PlacePlotOnCanvas(TString histoName, TString canvasName, int row=-1, int col=-1);
	bool SetCanvasAutoUpdate(TString canvasName);
	bool UpdateCanvas(TString canvasName);
	void SetCanvasReference(TString canvasName, TString histo, TH1* refPtr);
	void SetCanvasReference(TString canvasName, TString histo, TGraph* refPtr);

	//Export all histograms into output trees
	void ExportAllPlot(std::map<TString,TTree*> &trees, std::map<TString,void*> &branches);
	//Save all plots into output file
	void SaveAllPlots();

	//Request input histogram
	TH1* RequestHistogram(TString directory, TString name, bool appendOnNewFile);
	TH1* GetInputHistogram(TString directory, TString name);
	TH1* GetReferenceTH1(TString name);
	TH2* GetReferenceTH2(TString name);
	TGraph* GetReferenceTGraph(TString name);

	//IOHandler calls
	std::vector<Core::IOHandler::keyPair> GetListOfKeys(TString directory="");
	std::vector<TString> GetListOfDirs(TString directory="");
	std::vector<TString> GetListOfTH1(TString directory="");
	std::vector<TString> GetListOfTH2(TString directory="");
	std::vector<TString> GetListOfTGraph(TString directory="");
	std::vector<TString> GetListOfHistos(TString directory="");

	//###### Event fraction related
	//EventFraction methods
	void NewEventFraction(TString name);
	void AddCounterToEventFraction(TString efName, TString cName);
	void DefineSampleSizeCounter(TString efName, TString cName);
	void SetSignificantDigits(TString efName, int v);

	//Counter methods
	void BookCounter(TString cName);
	void SetCounterValue(TString cName, int v);
	void IncrementCounter(TString cName, int delta);
	void DecrementCounter(TString cName, int delta);
	void IncrementCounter(TString cName);
	void DecrementCounter(TString cName);
	int GetCounterValue(TString cName) const;

	//###### Output related
	//Methods for setting output
	void RegisterOutput(TString name, const void* const address);
	void SetOutputState(TString name, OutputState state);

	//Methods for getting output
	const void *GetOutput(TString name, OutputState &state) const;
	template <class T>
	const T* GetOutput(TString name, OutputState &state) const{
		/// \MemberDescr
		/// \param name : name of the output
		/// \param state : is filled with the current state of the output
		/// \return an output variable and the corresponding state (template version)
		/// \EndMemberDescr

		return (T*)GetOutputVoid(name, state);
	}

	//###### Input (Event/TTree) related
	//Request new tree to analyze
	void RequestTree(TString name, TDetectorVEvent *evt, TString branchName="");
	template <class T>
	void RequestTree(TString name, TString branchName, TString className, T* obj){
		/// \MemberDescr
		/// \param name : Name of the requested TTree
		/// \param branchName : Name of the Branch to retrieve
		/// \param className : Name of the class type in this branch
		/// \param obj : Pointer to an instance of any class
		///
		/// Request a tree in the input file. If already requested before, only add the new branch.
		/// \EndMemberDescr

		if(!RequestTreeVoid(name, branchName, className, obj)){
			delete obj;
		}
	}

	TChain* GetTree(TString name);
	TDetectorVEvent *GetEvent(TString name, TString branchName = "");
	Event* GetMCEvent();
	RawHeader* GetRawHeader();
	bool GetWithMC();
	bool GetWithRawHeader();
	bool GetIsTree();
	bool GetIsHisto();

	//###### Other methods
	DetectorAcceptance *GetDetectorAcceptanceInstance();
	template <class T>
	T* GetObject(TString name, TString branchName = ""){
		/// \MemberDescr
		/// \param name : Name of the TTree from which the object is read
		/// \param branchName : name of the branch to retrieve (optional)
		/// \return the pointer to the object corresponding to the given tree (template version).
		///
		/// If branchName is left empty and there is only 1 branch requested on this tree, this
		/// single branch is returned. If there is more than 1 branch requested on this tree,
		/// return the first one.
		/// If branchName is specified, try to return the specified branch.
		/// \EndMemberDescr

		return (T*)GetObjectVoid(name);
	}

	int GetNEvents();

	template <class T>
	void ReconfigureAnalyzer(TString analyzerName, TString parameterName, T parameterValue){
		stringstream ss;
		ss << parameterValue;
		TString paramStringValue(ss.str());
		CallReconfigureAnalyer(analyzerName, parameterName, paramStringValue);
	}

private:
	UserMethods();

	const void* GetOutputVoid(TString name, OutputState &state) const;
	bool RequestTreeVoid(TString name, TString branchName, TString className, void* obj);
	void* GetObjectVoid(TString name);
	void CallReconfigureAnalyer(TString analyerName, TString parameterName, TString parameterValue);

protected:
	Core::HistoHandler fHisto; ///< Local instance of HistoHandler

	TString fAnalyzerName; ///< Name of the analyzer

	Core::BaseAnalysis *fParent; ///< Pointer to the BaseAnalysis instance containing the analyze
};

} /* namespace NA62Analysis */
#endif /* USERMETHODS_HH_ */
