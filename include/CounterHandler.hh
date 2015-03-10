/*
 * CounterHandler.hh
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#ifndef COUNTERHANDLER_HH_
#define COUNTERHANDLER_HH_

#include <map>

#include "EventFraction.hh"
#include "containers.hh"

/// \class CounterHandler
/// \Brief
/// Class containing and handling Counters and EventFractions
/// \EndBrief
///
/// \Detailed
/// Implements the Counters and EventFractions booking, filling and printing methods.
/// Counters are integers counters and EventFraction are tables of counter providing
/// printing and exporting capabilities and able to compute ratios between counters
/// with asymetric errors.
/// \EndDetailed

class CounterHandler {
public:
	CounterHandler();
	CounterHandler(const CounterHandler& c);
	virtual ~CounterHandler();

	//Counter methods
	void BookCounter(TString name);
	void IncrementCounter(TString name);
	void IncrementCounter(TString name, int v);
	void DecrementCounter(TString name);
	void DecrementCounter(TString name, int v);
	void SetCounterValue(TString name, int v);
	int GetCounterValue(TString) const;

	//Event fraction methods
	void NewEventFraction(TString name);
	void AddCounterToEventFraction(TString efName, TString cName);
	void DefineSampleSizeCounter(TString efName, TString cName);
	void SetSignificantDigits(TString efName, int v);

	//Output and printing
	void WriteEventFraction(TString fileName) const;
	void PrintInitSummary() const;

private:
	AnalysisFW::NA62Map<TString,EventFraction*>::type fEventFraction; ///< Container for event fraction
	AnalysisFW::NA62Map<TString,int>::type fCounters; ///< Container for counters
};

#endif /* COUNTERHANDLER_HH_ */
