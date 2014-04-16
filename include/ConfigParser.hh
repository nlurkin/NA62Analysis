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
/// This class will parse a configuration file and assign the values to the variables
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
