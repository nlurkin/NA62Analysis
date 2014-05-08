/*
 * ConfigParser.h
 *
 *  Created on: Jun 5, 2013
 *      Author: nlurkin
 */

#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <TString.h>
#include <map>
using namespace std;

typedef pair<TString, TString> t_ParamPair;
typedef vector< t_ParamPair > t_ParamValue;

class Analyzer;

/// \class ConfigParser
/// \Brief 
/// This class will parse a configuration file and assign the values to the variables
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

class ConfigParser {
public:
	ConfigParser();
	ConfigParser(const ConfigParser& c);
	virtual ~ConfigParser();

	void ParseFile(TString fileName);
	void ParseCLI(TString params);
	void ApplyParams(Analyzer* const analyzer) const;
	void Print() const;
private:
	void AnalyzeLine(TString line);

	map<TString, t_ParamValue> fList;	///< Map of ParameterName,ParameterValue pairs
	TString fCurrentAnalyzer;			///< Name of the current Analyzer section
};

#endif /* CONFIGPARSER_H_ */
