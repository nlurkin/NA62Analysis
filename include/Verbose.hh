/*
 * Verbose.hh
 *
 *  Created on: Mar 20, 2015
 *      Author: ncl
 */

#ifndef VERBOSE_HH_
#define VERBOSE_HH_

#include <ostream>

#include <TString.h>

#include "FWEnums.hh"

#define PRINTVAR(v) #v << "= " << v << " "

namespace NA62Analysis {

class Verbose {
public:
	Verbose();
	Verbose(std::string name);
	virtual ~Verbose();

	void SetVerbosity(Verbosity::VerbosityLevel v);
	void SetGlobalVerbosity(Verbosity::VerbosityLevel v);

	Verbosity::VerbosityLevel GetVerbosityLevel() const { return fLocalVerbosityActive ? fLocalVerbosityLevel : fVerbosityLevel; };
	Verbosity::VerbosityLevel GetTestLevel() const { return fVerbosityTest; };
	std::string GetModuleName() const { return fModuleName; };

	void SetStream(std::ostream &s) const { fCurrentStream=&s;};
	std::ostream& GetStream() const { return *fCurrentStream;};

	const Verbose& PrintLevel(Verbosity::VerbosityLevel v) const { fVerbosityTest = v; return *this; };
	bool TestLevel(Verbosity::VerbosityLevel level) const;
	bool CanPrint() const;

	//Standard levels stream manipulators (shortcuts for PrintLevel)
	const Verbose& user() const { return PrintLevel(Verbosity::kUser); };
	const Verbose& normal() const { return PrintLevel(Verbosity::kNormal); };
	const Verbose& extended() const { return PrintLevel(Verbosity::kExtended); };
	const Verbose& debug() const { return PrintLevel(Verbosity::kDebug); };

	static std::string GetVerbosityLevelName(Verbosity::VerbosityLevel v);
	static Verbosity::VerbosityLevel GetVerbosityLevelFromName(TString v);

private:
	bool fLocalVerbosityActive; ///< Should local verbosity level be used?
	static Verbosity::VerbosityLevel fVerbosityLevel; ///< Global verbosity of the program
	Verbosity::VerbosityLevel fLocalVerbosityLevel; ///< Verbosity for this module only
	mutable Verbosity::VerbosityLevel fVerbosityTest; ///< Transient member. Store the currently requested verbosity output
	std::string fModuleName; ///< Name to display in the output
	mutable std::ostream *fCurrentStream; ///< Transient member. Store the current ostream on which we write
};

template <class T>
const Verbose& operator<<(const Verbose &level, T x) {
	if(level.CanPrint()) level.GetStream() << x;
	return level;
};
const Verbose& operator<<(const Verbose &level, std::ostream& (*f)(std::ostream&));
const Verbose& operator <<(std::ostream& s, const Verbose &level);


} /* namespace NA62Analysis */

#endif /* VERBOSE_HH_ */
