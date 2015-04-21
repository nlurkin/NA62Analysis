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

TimeCounter::TimeCounter(bool startNow) :
	fIsRunning(1),
	fTotalTime(0)
{
	/// \MemberDescr
	/// \param s: Start timestamp
	///
	/// Start constructor. Immediately starts the counter with the start timestamp s.
	/// \EndMemberDescr
	clock_gettime(CLOCK_MONOTONIC, &fStartTime);
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
	if(IncrementStart()){
		clock_gettime(CLOCK_MONOTONIC, &fStartTime);
	}
}

void TimeCounter::Stop() {
	/// \MemberDescr
	/// Stop the counter if this Stop() correspond to the first Start()
	/// \EndMemberDescr
	if(DecrementStart()) fTotalTime += getTime() - fStartTime;
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
	std::cout << "Started at: " << fStartTime.tv_sec + fStartTime.tv_nsec/1000000000. << std::endl;
	std::cout << "Total time is: " << fTotalTime << std::endl;
	std::cout << "IsRunning is: " << fIsRunning << std::endl;
}

struct timespec TimeCounter::getTime() const {
	struct timespec s;
	clock_gettime(CLOCK_MONOTONIC, &s);
	return s;
}

inline float operator-(struct timespec t1, struct timespec t2){
	return (float)(t1.tv_sec-t2.tv_sec) + (t1.tv_nsec-t2.tv_nsec)/1000000000.;
}
} /* namespace NA62Analysis */
