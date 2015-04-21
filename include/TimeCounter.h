/*
 * TimeCounter.h
 *
 *  Created on: Apr 15, 2015
 *      Author: ncl
 */

#ifndef TIMECOUNTER_H_
#define TIMECOUNTER_H_

#include <sys/time.h>

namespace NA62Analysis {

float operator-(struct timeval t1, struct timeval t2);

/// \class TimeCounter
/// \Brief
/// Time counter (stop watch)
/// \EndBrief
///
/// \Detailed
/// This class provides a basic time counter. It starts running when Start()
/// or the constructor TimeCounter(clock_t s) is called and stops when as many
/// Stop() have been called.\n
/// Example:\n
/// The following code will count the time elapsed between lines 2 and 7. Lines 3,4,5,6 and 8 have no effect.
/// \code
/// 1: TimeCounter tc;
/// 2: tc.Start();
/// 3: tc.Start();
/// 4: tc.Start();
/// 5: tc.Stop();
/// 6: tc.Stop();
/// 7: tc.Stop();
/// 8: tc.Stop();
/// \endcode
/// \EndDetailed
class TimeCounter {
public:
	TimeCounter();
	TimeCounter(bool startNow);
	virtual ~TimeCounter();

	void Start();
	void Stop();
	void Reset();

	void AddTime(float v) {
		/// \MemberDescr
		/// \param v: Amount of time to add
		///
		/// Add time to the total time.
		/// \EndMemberDescr
		fTotalTime+=v;
	};

	float GetTotalTime() const {
		/// \MemberDescr
		/// \return Total accumulated time up to now if still running, else total accumulated time up to last Stop()
		/// \EndMemberDescr
		if(fIsRunning>0) return fTotalTime + (GetTime()-fStartTime);
		else return fTotalTime;
	}

	struct timeval GetStartTime() const {
		/// \MemberDescr
		/// \return Timestamp when the counter started running
		/// \EndMemberDescr
		return fStartTime;
	}

	void Print() const;

	static struct timeval GetTime();
private:
	bool IncrementStart();
	bool DecrementStart();

	int fIsRunning; ///< Indicate how many Start() were requested without Stop() (#Start() - #Stop())
	struct timeval fStartTime; ///< Timestamp when the counter started running
	float fTotalTime; ///< Total accumulated time between all Start() and Stop()
};
} /* namespace NA62Analysis */

#endif /* TIMECOUNTER_H_ */
