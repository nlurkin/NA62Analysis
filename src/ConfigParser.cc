/*
 * ConfigParser.cpp
 *
 *  Created on: Jun 5, 2013
 *      Author: nlurkin
 */

#include "ConfigParser.hh"

#include <string>
#include <iostream>
#include <fstream>

#include <TString.h>
#include <TPRegexp.h>
#include <TObjArray.h>
#include <TObjString.h>

namespace NA62Analysis {
namespace Configuration {

ConfigParser::ConfigParser() {
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

ConfigParser::ConfigParser(const ConfigParser& c):
	fNSList(c.fNSList)
{
	/// \MemberDescr
	/// \param c : Reference of the object to copy
	/// Copy constructor
	/// \EndMemberDescr
}

ConfigParser::~ConfigParser() {
	/// \MemberDescr
	/// Default destructor
	/// \EndMemberDescr
}

void ConfigParser::ParseFile(TString fileName){
	/// \MemberDescr
	/// \param fileName : Path to the file to be parsed
	///
	/// Parse a configuration file.
	/// \EndMemberDescr

	ifstream s;
	std::string line;

	if(fileName.Length()>0){
		s.open(fileName);
		if(s.is_open()){
			while(s.good()){
				getline(s, line);
				ParseLine(line);
			}
		}
		else{
			std::cerr << "Configuration parser: Unable to open configuration file " << fileName << std::endl;
		}

		s.close();
	}
}

bool ConfigParser::NamespaceExists(TString ns) const {
	return fNSList.count(ns)>0;
}

const ConfigNamespace& ConfigParser::GetNamespace(TString ns) const {
	auto nsRef = fNSList.find(ns);

	if(nsRef != fNSList.end()) return nsRef->second;
	else return fDefault;
}

void ConfigParser::ParseLine(TString line){
	/// \MemberDescr
	/// \param line : line to be processed
	///
	/// Read the line and process it
	/// \EndMemberDescr

	TPRegexp commExp("(.*?)//.*");
	TPRegexp anExp("\\[\\[(.*)\\]\\]");
	TPRegexp paramExp("(.*?)[ ]*=[ ]*(.*)");
	TPRegexp plotUpdateExp("AutoUpdate = (.*)");

	TObjArray *results;
	TObjArray *params;

	TString paramName;
	TString paramValue;
	TString tempString;

	TString currentNS;

	results = commExp.MatchS(line);
	if(results->GetEntries()>=2) tempString = ((TObjString*)results->At(1))->GetString();
	else tempString = line;
	results->Delete();
	delete results;

	results = anExp.MatchS(tempString);
	if(results->GetEntries()==2){
		//New namespace section
		currentNS = ((TObjString*)results->At(1))->GetString();
		fNSList.insert(NSPair(currentNS, ConfigNamespace(currentNS)));
	}
	results->Delete();
	delete results;

	results = paramExp.MatchS(tempString);
	if(results->GetEntries()==3){
		paramName = ((TObjString*)results->At(1))->GetString();
		params = ((TObjString*)results->At(2))->GetString().Tokenize(" ");
		for(int i=0; i<params->GetEntries(); i++){
			paramValue = ((TObjString*)params->At(i))->GetString();
			fNSList[currentNS].AddParam(paramName, paramValue);
		}
		params->Delete();
		delete params;
	}
	results->Delete();
	delete results;
}

void ConfigParser::Print() const{
	/// \MemberDescr
	/// Print all the Namespaces
	/// \EndMemberDescr

	for(auto &ns : fNSList){
		ns.second.Print();
	}
}

bool ConfigNamespace::ParamExists(TString paramName) const {
	return fParamsList.count(paramName)>0;
}

const std::map<TString, TString>& ConfigNamespace::GetParams() const {
	return fParamsList;
}

TString ConfigNamespace::GetParam(TString name) const {
	auto paramRef = fParamsList.find(name);

	if(paramRef != fParamsList.end()) return paramRef->second;
	else return "";
}

void ConfigNamespace::Print() const{
	/// \MemberDescr
	/// Print all the ParamName,ParamValue pairs
	/// \EndMemberDescr

	std::cout << "[[ " << fName << " ]]" << std::endl;
	for(auto &param : fParamsList){
		std::cout << param.first << " : " << param.second << std::endl;
	}
}

} /* namespace Configuration */
} /* namespace NA62Analysis */
