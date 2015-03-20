/*
 * Verbose.cc
 *
 *  Created on: Mar 20, 2015
 *      Author: ncl
 */

#include "Verbose.hh"
#include <iostream>

namespace NA62Analysis {

Verbose::Verbose() :
	fVerbosityLevel(Verbosity::kNo),
	fVerbosityTest(Verbosity::kNo),
	fCurrentStream(&std::cout)
{
	// TODO Auto-generated constructor stub

}

Verbose::~Verbose() {
	// TODO Auto-generated destructor stub
}

void Verbose::SetVerbosity(Verbosity::VerbosityLevel v){
	/// \MemberDescr
	/// \param v : value of the verbosity
	///
	/// Change verbosity
	/// \EndMemberDescr

	if(v >= Verbosity::kUser) std::cout << "AnalysisFW: Setting verbosity to " << v << std::endl;
	fVerbosityLevel = v;
}

Verbose& operator<<(Verbose &level, std::ostream& (*f)(std::ostream&)) { level.GetStream() << f; return level;};
Verbose& operator <<(std::ostream& s, Verbose &level) {
	level.SetStream(s);
	s << "My level: " << level.GetVerbosityLevel();
	return level;
}

} /* namespace NA62Analysis */

