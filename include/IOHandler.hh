/*
  * IOHandler.hh
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#ifndef IOHANDLER_HH_
#define IOHANDLER_HH_

#include <fstream>

#include <TString.h>
#include <TFile.h>
#include "TMutex.h"
#include "Verbose.hh"
#include "TimeCounter.h"

namespace NA62Analysis {
namespace Core {

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
class IOHandler : public Verbose {
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
	IOHandler(std::string name);
	IOHandler(const IOHandler& c);
	virtual ~IOHandler();

	//IO Files
	virtual bool OpenInput(TString inFileName, int nFiles);
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
	virtual bool LoadEvent(int) {
		/// \MemberDescr
		/// \param : Event index to load
		/// \return true
		///
		/// Dummy LoadEvent
		/// \EndMemberDescr

		return true;
	};
	std::vector<keyPair> GetListOfKeys(TString dir);
	std::vector<TString> GetListOfDirs(TString dir);
	std::vector<TString> GetListOfTH1(TString dir);
	std::vector<TString> GetListOfTH2(TString dir);
	std::vector<TString> GetListOfTGraph(TString dir);
	std::vector<TString> GetListOfHistos(TString dir);
	virtual long long GetNEvents();

	bool CheckDirExists(TString dir) const;
	void FileSkipped(TString fileName);
	bool IsLastFileReached() const;

	//Writing
	void MkOutputDir(TString name) const;
	void Finalise();

	//Printing
	virtual void PrintInitSummary() const;

	IOHandlerType GetIOType() const {
		/// \MemberDescr
		/// \return Type of IO handler
		/// \EndMemberDescr
		return fIOType;
	}

	const TimeCounter& GetIoTimeCount() const {
		/// \MemberDescr
		/// \return Reference to the internal IOTimeCount
		/// \EndMemberDescr
		return fIOTimeCount;
	}

	void SetContinuousReading(bool continuousReading) {
		/// \MemberDescr
		/// \param continuousReading: Value to set
		///
		/// Set continuous reading flag
		/// \EndMemberDescr
		fContinuousReading = continuousReading;
	}

	void SetOutputFileAsCurrent() const {
		/// \MemberDescr
		/// Set outputFile as current file (gFile)
		/// \EndMemberDescr
		fOutFile->cd();
	}

	void SignalExit() const {
		/// \MemberDescr
		/// Raise exit signal
		/// \EndMemberDescr

		fSignalExit = true;
	}

	int ReadCurrentFileRevision();

	void SetMutex(TMutex *m){
		/// \MemberDescr
		/// \param m: Pointer to the mutex to use
		///
		/// Set the mutex
		/// \EndMemberDescr

		fGraphicalMutex = m;
	}

	void SetFastStart(bool fastStart) {
		fFastStart = fastStart;
	}

	bool IsFastStart() const {
		return fFastStart;
	}

protected:
	void NewFileOpened(int index, TFile* currFile);

	bool fContinuousReading; ///< Continuous reading enabled?
	bool fFastStart; ///< Fast start flag enabled? (Start processing directly without checking files)
	mutable bool fSignalExit; ///< Signal from main thread to exit
	IOHandlerType fIOType; ///< Type of IO handler

	int fCurrentFileNumber; ///< Index of the current opened file in the TChain

	TFile *fOutFile; ///< Output file
	TString fOutFileName; ///< Output fileName
	char *fCurrentDir; ///< Current work directory

	TFile *fCurrentFile; ///< Pointer to the currently opened file in the TChain

	std::vector<TString> fInputfiles; ///< Vector of input file path

	std::ofstream fSkippedFD; ///< Skipped files output stream

	mutable TimeCounter fIOTimeCount; ///< Counter for the time spent in IO
	TMutex *fGraphicalMutex; ///< Mutex to prevent graphical objects to change while processing GUI events
};

bool TestIsTextFile(TString fileName);
bool TestASCIIChar(unsigned char c);
bool TestMultiByteChar(unsigned char c);

} /* namespace Core */
} /* namespace NA62Analysis */
#endif /* IOHANDLER_HH_ */
