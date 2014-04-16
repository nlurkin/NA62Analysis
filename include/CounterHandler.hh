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

/// \class CounterHandler
/// \Brief
/// Class containing and handling Counters and EventFractions
/// \EndBrief
///
/// \Detailed
/// Implements the Counters and EventFractions booking, filling and printing methods.
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
	map<TString, EventFraction*> fEventFraction; ///< Container for event fraction
	map<TString, int> fCounters; ///< Container for counters
};

#endif /* COUNTERHANDLER_HH_ */
