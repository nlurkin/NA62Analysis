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
	/// \param startNow : if true, start the counter immediately
	///
	/// Start constructor.
	/// \EndMemberDescr

	if(startNow) gettimeofday(&fStartTime, NULL);
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
		gettimeofday(&fStartTime, NULL);
	}
}

void TimeCounter::Stop() {
	/// \MemberDescr
	/// Stop the counter if this Stop() correspond to the first Start()
	/// \EndMemberDescr
	if(DecrementStart()) fTotalTime += GetTime() - fStartTime;
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
	std::cout << "Started at: " << fStartTime.tv_sec + fStartTime.tv_usec/1000000. << std::endl;
	std::cout << "Total time is: " << fTotalTime << std::endl;
	std::cout << "IsRunning is: " << fIsRunning << std::endl;
}

struct timeval TimeCounter::GetTime() {
	/// \MemberDescr
	/// \return timeval struct representing the current time
	/// \EndMemberDescr

	struct timeval s;
	gettimeofday(&s, NULL);
	return s;
}

inline float operator-(struct timeval t1, struct timeval t2){
	return (float)(t1.tv_sec-t2.tv_sec) + (t1.tv_usec-t2.tv_usec)/1000000.;
}
} /* namespace NA62Analysis */
