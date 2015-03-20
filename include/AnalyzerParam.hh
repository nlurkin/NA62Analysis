/*
 * AnalyzerParam.hh
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#ifndef ANALYZERPARAM_HH_
#define ANALYZERPARAM_HH_

#include "ConfigParser.hh"

namespace NA62Analysis {

class Analyzer;

namespace Configuration {

/// \class AnalyzerParam
/// \Brief
/// This class will parse a configuration file and assign the values to the declared analyzer param variables
/// \EndBrief
///
/// \Detailed
/// It can parse strings passed from command line, or configuration files. When analyzers are
/// provided to the parser, it applies the relevant parameters.\n
/// Command line string looks like
/// \code
/// 	analyzerName1:param1=val1;param2=val2&analyzerName2:param1=value1;paramx=valuex ...
/// \endcode
/// The configuration file looks like
/// \code
/// 	[[analyzerName1]]
/// 	param1 = val1
/// 	param2 = val2
///
/// 	[[analyzerName2]]
/// 	param1 = value1
/// 	paramx = valuex
///
/// 	...
/// \endcode
/// \EndDetailed

class AnalyzerParam : public ConfigParser {
public:
	AnalyzerParam();
	AnalyzerParam(const AnalyzerParam& c);
	virtual ~AnalyzerParam();

	void ApplyParams(Analyzer* const analyzer) const;
	void ParseCLI(TString params);
};

} /* namespace Configuration */
} /* namespace NA62Analysis */

#endif /* ANALYZERPARAM_HH_ */
