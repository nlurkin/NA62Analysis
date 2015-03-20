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

#include "containers.hh"

namespace NA62Analysis {

/// \class EventFraction
/// \Brief 
/// Class containing a list of counters.
/// \EndBrief 
///
/// \Detailed
/// Class containing a list of counters. It can be used for printing and exporting at the end of the processing.
///	It can also display ratio between the counters (with asymmetric errors) if one of the counter is defined as the sample size.
/// \EndDetailed

class EventFraction{
public:
	EventFraction();
	EventFraction(const EventFraction& c);
	EventFraction(TString name);
	virtual ~EventFraction();

	//Counter methods
	void AddCounter(TString name, int* const v);

	//
	void DefineSampleSizeCounter(TString name);
	void SetPrecision(int v);

	//Output methods
	void DumpTable() const;
	void WriteTable(TString filePrefix) const;

private:
	TString FormatDouble(double v) const;
	int GetPrecision(double v) const;

	void PrintToStream(ostream &s) const;

	NA62Analysis::NA62Map<TString,int*>::type fCounters; ///< Map of counters
	std::vector<TString> fSequence; ///< Define the display order of the counters
	TString fName; ///< Name of the EventFraction
	TString fSampleSizeCounter;	///< Name of the counter defining the sample size
	int fPrecision; ///< Display precision
};

} /* namespace NA62Analysis */

#endif /* EVENTFRACTION_HH_ */
