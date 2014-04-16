/*
 * ConfigParser.cpp
 *
 *  Created on: Jun 5, 2013
 *      Author: nlurkin
 */

#include "ConfigParser.hh"
#include "Analyzer.hh"
#include <string>
#include <iostream>
#include <fstream>
#include <TString.h>
#include <TPRegexp.h>
#include <TObjArray.h>
#include <TObjString.h>
using namespace std;

ConfigParser::ConfigParser() {
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

ConfigParser::ConfigParser(const ConfigParser& c):
	fList(c.fList),
	fCurrentAnalyzer(c.fCurrentAnalyzer)
{
	/// \MemberDescr
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
	string line;

	if(fileName.Length()>0){
		s.open(fileName);
		if(s.is_open()){
			while(s.good()){
				getline(s, line);
				AnalyzeLine(line);
			}
		}
		else{
			cerr << "Configuration parser: Unable to open configuration file " << fileName << endl;
		}

		s.close();
	}
}

void ConfigParser::AnalyzeLine(TString line){
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

	results = commExp.MatchS(line);
	if(results->GetEntries()>=2) tempString = ((TObjString*)results->At(1))->GetString();
	else tempString = line;
	results->Delete();
	delete results;

	results = anExp.MatchS(tempString);
	if(results->GetEntries()==2){
		//New analyzer section
		fCurrentAnalyzer = ((TObjString*)results->At(1))->GetString();
	}
	results->Delete();
	delete results;

	results = paramExp.MatchS(tempString);
	if(results->GetEntries()==3){
		paramName = ((TObjString*)results->At(1))->GetString();
		params = ((TObjString*)results->At(2))->GetString().Tokenize(" ");
		for(int i=0; i<params->GetEntries(); i++){
			paramValue = ((TObjString*)params->At(i))->GetString();
			fList[fCurrentAnalyzer].push_back(t_ParamPair(paramName, paramValue));
		}
		params->Delete();
		delete params;
	}
	results->Delete();
	delete results;
}

void ConfigParser::Print() const{
	/// \MemberDescr
	/// Print all the ParamName,ParamValue pairs
	/// \EndMemberDescr

	map<TString, t_ParamValue>::const_iterator it;
	t_ParamValue::const_iterator pIt;

	for(it = fList.begin(); it!=fList.end(); it++){
		cout << "Analyzer " << it->first << endl;
		for(pIt = it->second.begin(); pIt != it->second.end(); pIt++){
			cout << pIt->first << " : " << pIt->second << endl;
		}
	}
}

void ConfigParser::ApplyParams(Analyzer * const analyzer) const{
	/// \MemberDescr
	/// \param analyzer : pointer to the analyzer
	///
	/// Apply all the ParamName,ParamValue pairs to the specified analyzer
	/// \EndMemberDescr

	t_ParamValue::const_iterator pIt;

	if(fList.count(analyzer->GetAnalyzerName())>0){
		for(pIt = fList.find(analyzer->GetAnalyzerName())->second.begin(); pIt != fList.find(analyzer->GetAnalyzerName())->second.end(); pIt++){
			analyzer->ApplyParam(pIt->first, pIt->second);
		}
	}
}

void ConfigParser::ParseCLI(TString params){
	/// \MemberDescr
	/// Parse command line interface parameter line
	/// \EndMemberDescr

	TObjArray *ans, *p;
	TObjArray *pars, *values;
	TString paramsLine;

	TString paramValue, paramName;

	ans = params.Tokenize("&");
	for(int i=0; i<ans->GetEntries(); i++){
		p = ((TObjString*)ans->At(i))->GetString().Tokenize(":");
		if(p->GetEntries()==2){
			//Got the analyzer
			fCurrentAnalyzer = ((TObjString*)p->At(0))->GetString();
			//Parse parameters now
			paramsLine = ((TObjString*)p->At(1))->GetString();
			pars = paramsLine.Tokenize(";");
			for(int j=0; j<pars->GetEntries(); j++){
				values = ((TObjString*)pars->At(j))->GetString().Tokenize("=");
				if(values->GetEntries()==2){
					paramName = ((TObjString*)values->At(0))->GetString();
					paramValue = ((TObjString*)values->At(1))->GetString();
					fList[fCurrentAnalyzer].push_back(t_ParamPair(paramName, paramValue));
				}
				else{
					cerr << "Configuration parser: Parameter name or value not specified " << pars->At(j)->GetName() << endl;
				}
				values->Delete();
				delete values;
			}
			pars->Delete();
			delete pars;
		}
		else{
			cerr << "Configuration parser: Parameters list not specified for analyzer " << p->At(i)->GetName() << endl;
		}
		p->Delete();
		delete p;
	}

	ans->Delete();
	delete ans;
}
