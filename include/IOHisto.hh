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

class IOHisto : public IOHandler{
public:
	IOHisto();
	IOHisto(const IOHisto &c);
	virtual ~IOHisto();

	void SetReferenceFileName(TString fileName);
	bool CheckNewFileOpened();

	//Histogram
	TH1* GetInputHistogram(TString directory, TString name, bool append);
	TH1* GetReferenceTH1(TString name);
	TH2* GetReferenceTH2(TString name);
	TGraph* GetReferenceTGraph(TString name);
	void UpdateInputHistograms();

private:
	AnalysisFW::NA62MultiMap<TString,TH1*>::type fInputHistoAdd; ///< Container for input histograms for which we append the values of the new files
	AnalysisFW::NA62MultiMap<TString,TH1*>::type fInputHisto; ///< Container for input histograms for which we do not append the values of the new files

	TString fReferenceFileName; ///< Name of the file containing reference plots to compare with
};

#endif /* IOHISTO_H_ */
