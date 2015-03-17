/*
  * IOHandler.hh
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#ifndef IOHANDLER_HH_
#define IOHANDLER_HH_

#include <TString.h>
#include <TFile.h>
#include "FWEnums.hh"

/// Type of IOHandler: Not specified yet, histogram IO, TTree IO
enum class IOHandlerType {kNOIO, kHISTO, kTREE};

/// \class IOHandler
/// \Brief
/// Class containing and handling every IO object
/// \EndBrief
///
/// \Detailed
/// Implements the base IO methods\n
/// Manage input event files (loading, specific processing before closing a file or after opening it)\n
/// \EndDetailed
class IOHandler {
public:
	/// \struct keyPair_t
	/// \Brief
	/// Structure defining an input ROOT file key (name, class name)
	/// \EndBrief
	///
	/// \Detailed
	/// Structure defining an input ROOT file key (name, class name)
	/// \EndDetailed
	typedef struct keyPair_t {
		keyPair_t(TString name, TString className){
			/// \MemberDescr
			/// \param name: Name of the object
			/// \param className: ClassName of the object
			/// \return Constructor
			/// \EndMemberDescr
			this->name = name; this->className = className;
		};
		TString name; ///< Name of the object
		TString className; ///< Class name of the object
	} keyPair; ///< Typedef to struct keyPair_t

	IOHandler();
	IOHandler(const IOHandler& c);
	virtual ~IOHandler();

	//IO Files
	virtual bool OpenInput(TString inFileName, int nFiles, AnalysisFW::VerbosityLevel verbosity);
	virtual bool OpenOutput(TString outFileName);
	virtual bool CheckNewFileOpened();
	TString GetOutputFileName() const;
	int GetCurrentFileNumber() const;
	int GetInputFileNumber() const {
		/// \MemberDescr
		/// \return Number of input files
		/// \EndMemberDescr
		return fInputfiles.size();
	};
	virtual void LoadEvent(int iEvent) {
		/// \MemberDescr
		/// \param iEvent: Event index to load
		/// Dummy LoadEvent
		/// \EndMemberDescr
	};
	std::vector<keyPair> GetListOfKeys(TString dir);
	std::vector<TString> GetListOfDirs(TString dir);
	std::vector<TString> GetListOfTH1(TString dir);
	std::vector<TString> GetListOfTH2(TString dir);
	std::vector<TString> GetListOfTGraph(TString dir);
	std::vector<TString> GetListOfHistos(TString dir);

	bool CheckDirExists(TString dir);

	//Writing
	void MkOutputDir(TString name) const;

	//Printing
	virtual void PrintInitSummary() const;

	const IOHandlerType GetIOType() const {
		/// \MemberDescr
		/// \return Type of IO handler
		/// \EndMemberDescr
		return fIOType;
	};

protected:
	void NewFileOpened(int index, TFile* currFile);

	IOHandlerType fIOType; ///< Type of IO handler

	int fCurrentFileNumber; ///< Index of the current opened file in the TChain

	TFile *fOutFile; ///< Output file
	TString fOutFileName; ///< Output fileName

	TFile *fCurrentFile; ///< Pointer to the currently opened file in the TChain

	std::vector<TString> fInputfiles; ///< Vector of input file path
};

#endif /* IOHANDLER_HH_ */
