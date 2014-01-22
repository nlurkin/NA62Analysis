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

class CounterHandler {
public:
	CounterHandler();
	virtual ~CounterHandler();

	//Counter methods
	void BookCounter(TString name);
	void IncrementCounter(TString name);
	void IncrementCounter(TString name, int v);
	void DecrementCounter(TString name);
	void DecrementCounter(TString name, int v);
	void SetCounterValue(TString name, int v);
	int GetCounterValue(TString);

	//Event fraction methods
	void NewEventFraction(TString name);
	void AddCounterToEventFraction(TString efName, TString cName);
	void DefineSampleSizeCounter(TString efName, TString cName);
	void SetSignificantDigits(TString efName, int v);

	void WriteEventFraction(TString fileName);

	void PrintInitSummary();

private:
	map<TString, EventFraction*> fEventFraction; ///< Container for event fraction
	map<TString, int> fCounters; ///< Container for counters
};

#endif /* COUNTERHANDLER_HH_ */
