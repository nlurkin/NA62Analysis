/*
 * EventFraction.hh
 *
 *  Created on: May 21, 2013
 *      Author: nlurkin
 */

#ifndef EVENTFRACTION_HH_
#define EVENTFRACTION_HH_

#include <iostream>
#include <map>
#include <TString.h>
using namespace std;

/// \class EventFraction
/// \Brief 
/// To do
/// \EndBrief 
///
/// \Detailed
/// To do 
/// \EndDetailed

class EventFraction{
public:
	EventFraction();
	EventFraction(TString name);
	virtual ~EventFraction();

	//Counter methods
	void AddCounter(TString name, int *v);

	//
	void DefineSampleSizeCounter(TString name);
	void SetPrecision(int v);

	//Output methods
	void DumpTable();
	void WriteTable(TString filePrefix);

private:
	map<TString,int*> fCounters; ///< Map of counters
	vector<TString> fSequence; ///< Define the display order of the counters
	TString fName; ///< Name of the EventFraction
	TString fSampleSizeCounter;	///< Name of the counter defining the sample size
	int fPrecision; ///< Display precision

	TString FormatDouble(double v);
	int GetPrecision(double v);

	void PrintToStream(ostream &s);
};

#endif /* EVENTFRACTION_HH_ */
