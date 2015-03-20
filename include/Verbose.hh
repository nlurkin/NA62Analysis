/*
 * Verbose.hh
 *
 *  Created on: Mar 20, 2015
 *      Author: ncl
 */

#ifndef VERBOSE_HH_
#define VERBOSE_HH_

#include <ostream>

#include "FWEnums.hh"

namespace NA62Analysis {

class Verbose {
public:
	Verbose();
	virtual ~Verbose();

	void SetVerbosity(Verbosity::VerbosityLevel v);

	Verbosity::VerbosityLevel GetVerbosityLevel() const;

	void SetStream(std::ostream &s) { fCurrentStream=&s;};
	std::ostream& GetStream() const { return *fCurrentStream;};
	const Verbose& PrintLevel(Verbosity::VerbosityLevel v) const { return *this; };
	bool TestLevel(Verbosity::VerbosityLevel level) const;

	const Verbose& user() const { return PrintLevel(Verbosity::kUser); };
	const Verbose& normal() const { return PrintLevel(Verbosity::kNormal); };
	const Verbose& debug() const { return PrintLevel(Verbosity::kDebug); };
	const Verbose& someLevel() const { return PrintLevel(Verbosity::kSomeLevel); };

private:
	Verbosity::VerbosityLevel fVerbosityLevel; ///< Verbosity of the program
	mutable Verbosity::VerbosityLevel fVerbosityTest; ///< Transient members. Store the currently requested verbosity output
	std::ostream *fCurrentStream; ///< Store the current ostream on which we write
};

template <class T>
Verbose& operator<<(Verbose &level, T x) { level.GetStream() << x; return level;};
Verbose& operator<<(Verbose &level, std::ostream& (*f)(std::ostream&));
Verbose& operator <<(std::ostream& s, const Verbose &level);


} /* namespace NA62Analysis */

#endif /* VERBOSE_HH_ */
