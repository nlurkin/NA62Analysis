/*
 * TimeCounter.cc
 *
 *  Created on: Apr 15, 2015
 *      Author: ncl
 */

#include <TimeCounter.h>
#include <iostream>

namespace NA62Analysis {

TimeCounter::TimeCounter() :
	fIsRunning(0),
	fStartTime(),
	fTotalTime(0)
{
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

TimeCounter::TimeCounter(clock_t s) :
	fIsRunning(1),
	fStartTime(s),
	fTotalTime(0)
{
	/// \MemberDescr
	/// \param s: Start timestamp
	///
	/// Start constructor. Immediately starts the counter with the start timestamp s.
	/// \EndMemberDescr
}


TimeCounter::~TimeCounter() {
	/// \MemberDescr
	/// Default destructor
	/// \EndMemberDescr
}

void TimeCounter::Start() {
	/// \MemberDescr
	/// Start the counter if not already started
	/// \EndMemberDescr
	if(IncrementStart()) fStartTime = clock();
}

void TimeCounter::Stop() {
	/// \MemberDescr
	/// Stop the counter if this Stop() correspond to the first Start()
	/// \EndMemberDescr
	if(DecrementStart()) fTotalTime += (float)(clock()-fStartTime)/CLOCKS_PER_SEC;
}

void TimeCounter::Reset() {
	/// \MemberDescr
	/// Reset the counter to 0
	/// \EndMemberDescr
	fTotalTime = 0;
	fIsRunning = 0;
}

bool TimeCounter::IncrementStart() {
	/// \MemberDescr
	/// Increment the count of Start()
	///
	/// \return true if the counter was not running
	/// \EndMemberDescr
	return 0==fIsRunning++;
}

bool TimeCounter::DecrementStart() {
	/// \MemberDescr
	/// Increment the count of Stop()
	///
	/// \return true if the number of Stop() is equal to the number of Start()
	/// \EndMemberDescr
	fIsRunning--;
	if(fIsRunning<0) fIsRunning=0;
	return fIsRunning==0;
}

void TimeCounter::Print() const {
	/// \MemberDescr
	/// Print of the internal values of the counter
	/// \EndMemberDescr
	std::cout << "Started at: " << fStartTime << std::endl;
	std::cout << "Total time is: " << fTotalTime << std::endl;
	std::cout << "IsRunning is: " << fIsRunning << std::endl;
}

} /* namespace NA62Analysis */

