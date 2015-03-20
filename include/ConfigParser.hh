/*
 * ConfigParser.h
 *
 *  Created on: Jun 5, 2013
 *      Author: nlurkin
 */

#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <map>

#include <TString.h>

namespace NA62Analysis {
namespace Configuration {

typedef std::pair<TString, TString> ParamPair;
class ConfigNamespace {
public:
	ConfigNamespace(TString name) : fName(name){};
	ConfigNamespace(){};

	bool ParamExists(TString paramName) const;
	void AddParam(TString name, TString value){
		fParamsList.insert(ParamPair(name, value));
	}
	const std::map<TString, TString>& GetParams() const;
	TString GetParam(TString name) const;

	void SetValue(TString name, char &ref) const;
	void SetValue(TString name, bool &ref) const;
	void SetValue(TString name, int &ref) const;
	void SetValue(TString name, long &ref) const;
	void SetValue(TString name, float &ref) const;
	void SetValue(TString name, double &ref) const;
	void SetValue(TString name, std::string &ref) const;
	void SetValue(TString name, TString &ref) const;

	void Print() const;
private:
	TString fName;
	std::map<TString, TString> fParamsList;
};

typedef std::pair<TString, ConfigNamespace> NSPair;

/// \class ConfigParser
/// \Brief 
/// This class will parse a configuration file and create a repository of pairs (param,value)
/// for each namespace.
/// \EndBrief 
///
/// \Detailed
/// It can parse strings passed from configuration files that looks like
/// The configuration file looks like
/// \code
/// 	[[ namespace1 ]]
/// 	param1 = val1
/// 	param2 = val2
///
/// 	[[ namespace2 ]]
/// 	param1 = value1
/// 	paramx = valuex
///
/// 	...
/// \endcode
/// The parameters and values are accessible in their namespace.
/// \EndDetailed

class ConfigParser {
public:
	ConfigParser();
	ConfigParser(const ConfigParser& c);
	virtual ~ConfigParser();

	virtual void ParseFile(TString fileName);
	virtual void Print() const;

	bool NamespaceExists(TString ns) const;
	const ConfigNamespace& GetNamespace(TString ns) const;

protected:
	void ParseLine(TString line);

	std::map<TString, ConfigNamespace> fNSList;	///< Map of namespaces
	ConfigNamespace fDefault; ///< Default namespace
	TString fCurrentNS; ///< Transient. Currently processed namespace
};

} /* namespace Configuration */
} /* namespace NA62Analysis */

#endif /* CONFIGPARSER_H_ */
