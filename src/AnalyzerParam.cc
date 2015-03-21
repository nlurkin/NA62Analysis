/*
 * AnalyzerParam.cc
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#include <AnalyzerParam.hh>

#include "Analyzer.hh"

namespace NA62Analysis {
namespace Configuration {

AnalyzerParam::AnalyzerParam() {
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

AnalyzerParam::AnalyzerParam(const AnalyzerParam& c):
	ConfigParser(c)
{
	/// \MemberDescr
	/// \param c : Reference of the object to copy
	/// Copy constructor
	/// \EndMemberDescr
}

AnalyzerParam::~AnalyzerParam() {
	/// \MemberDescr
	/// Default destructor
	/// \EndMemberDescr
}

void AnalyzerParam::ApplyParams(Analyzer * const analyzer) const{
	/// \MemberDescr
	/// \param analyzer : pointer to the analyzer
	///
	/// Apply all the ParamName,ParamValue pairs to the specified analyzer
	/// \EndMemberDescr
	TString name = analyzer->GetAnalyzerName();
	name.ToLower();
	if(NamespaceExists(name)){
		for(auto param : GetNamespace(name).GetParams()){
			analyzer->ApplyParam(param.first, param.second);
		}
	}
}

void AnalyzerParam::ParseCLI(TString params){
	/// \MemberDescr
	/// \param params : Parameter string passed in the command line
	///
	/// Parse command line interface parameter line
	/// \EndMemberDescr

	TObjArray *ans, *p;
	TObjArray *pars, *values;
	TString paramsLine;

	TString paramValue, paramName;
	TString currentNS;

	ans = params.Tokenize("&");
	for(int i=0; i<ans->GetEntries(); i++){
		p = ((TObjString*)ans->At(i))->GetString().Tokenize(":");
		if(p->GetEntries()==2){
			//Got the analyzer
			currentNS = ((TObjString*)p->At(0))->GetString();
			fNSList.insert(NSPair(currentNS, ConfigNamespace(currentNS)));
			//Parse parameters now
			paramsLine = ((TObjString*)p->At(1))->GetString();
			pars = paramsLine.Tokenize(";");
			for(int j=0; j<pars->GetEntries(); j++){
				values = ((TObjString*)pars->At(j))->GetString().Tokenize("=");
				if(values->GetEntries()==2){
					paramName = ((TObjString*)values->At(0))->GetString();
					paramValue = ((TObjString*)values->At(1))->GetString();
					fNSList[currentNS].AddParam(paramName, paramValue);
				}
				else{
					std::cerr << "Configuration parser: Parameter name or value not specified " << pars->At(j)->GetName() << std::endl;
				}
				values->Delete();
				delete values;
			}
			pars->Delete();
			delete pars;
		}
		else{
			std::cerr << "Configuration parser: Parameters list not specified for analyzer " << p->At(i)->GetName() << std::endl;
		}
		p->Delete();
		delete p;
	}

	ans->Delete();
	delete ans;
}

} /* namespace Configuration */
} /* namespace NA62Analysis */
