/*
 * IOHisto.h
 *
 *  Created on: Mar 9, 2015
 *      Author: ncl
 */

#ifndef IOHISTO_H_
#define IOHISTO_H_

#include "IOHandler.hh"
#include "containers.hh"

class TH1;
class TH2;
class TGraph;

namespace NA62Analysis {
namespace Core {

/// \class IOHisto
/// \Brief
/// Class containing and handling every IO histograms
/// \EndBrief
///
/// \Detailed
/// Inherits IOHandler and implements the IO methods for histograms\n
/// Load input files with reference histograms and give access to these histograms, update them.\n
/// \EndDetailed
class IOHisto : public IOHandler{
public:
	IOHisto();
	IOHisto(std::string name);
	IOHisto(const IOHisto &c);
	virtual ~IOHisto();

	bool OpenInput(TString inFileName, int nFiles);
	void SetReferenceFileName(TString fileName);
	bool CheckNewFileOpened();
	bool LoadEvent(int iEvent);

	//Histogram
	TH1* GetInputHistogram(TString directory, TString name, bool append);
	TH1* GetReferenceTH1(TString name);
	TH2* GetReferenceTH2(TString name);
	TGraph* GetReferenceTGraph(TString name);
	void UpdateInputHistograms();

private:
	bool fNewFileOpened; ///< Indicates if a new file has been opened

	NA62Analysis::NA62MultiMap<TString,TH1*>::type fInputHistoAdd; ///< Container for input histograms for which we append the values of the new files
	NA62Analysis::NA62MultiMap<TString,TH1*>::type fInputHisto; ///< Container for input histograms for which we do not append the values of the new files

	TString fReferenceFileName; ///< Name of the file containing reference plots to compare with
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* IOHISTO_H_ */
