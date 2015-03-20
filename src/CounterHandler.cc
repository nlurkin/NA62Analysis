/*
 * CounterHandler.cc
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#include "CounterHandler.hh"
#include "StringBalancedTable.hh"

namespace NA62Analysis {

CounterHandler::CounterHandler() {
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

CounterHandler::CounterHandler(const CounterHandler& c):
		fEventFraction(c.fEventFraction),
		fCounters(c.fCounters)
{
	/// \MemberDescr
	/// \param c : Reference of the object to copy
	/// Copy constructor
	/// \EndMemberDescr
}

CounterHandler::~CounterHandler() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,EventFraction*>::type::iterator itEF;

	while(fEventFraction.size()>0){
		itEF = fEventFraction.begin();
		delete itEF->second;
		fEventFraction.erase(itEF);
	}
}

void CounterHandler::WriteEventFraction(TString fileName) const{
	/// \MemberDescr
	/// \param fileName : Path to the target file for writing
	/// Dump the EventFraction
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,EventFraction*>::type::const_iterator it;

	for(it=fEventFraction.begin(); it!=fEventFraction.end(); it++){
		it->second->DumpTable();
		it->second->WriteTable(fileName);
	}
}

void CounterHandler::NewEventFraction(TString name){
	/// \MemberDescr
	/// \param name : Name of the EventFraction table
	///
	/// Create a new EventFraction instance
	/// \EndMemberDescr

	fEventFraction.insert(std::pair<TString,EventFraction*>(name, new EventFraction(name)));
}
void CounterHandler::AddCounterToEventFraction(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Add a counter in the specified EventFraction table
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,EventFraction*>::type::iterator ptr;
	NA62Analysis::NA62Map<TString,int>::type::iterator ptr2;

	if((ptr=fEventFraction.find(efName))!=fEventFraction.end()){
		if((ptr2=fCounters.find(cName))!=fCounters.end()){
			ptr->second->AddCounter(cName, &ptr2->second);
		}
		else std::cerr << "Counter " << cName << " doesn't exist." << std::endl;
	}
	else std::cerr << "EventFraction table " << efName << " doesn't exist." << std::endl;
}
void CounterHandler::DefineSampleSizeCounter(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Define counter as SampleSize in the specified EventFraction table
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,EventFraction*>::type::iterator ptr;

	if((ptr=fEventFraction.find(efName))!=fEventFraction.end()){
		if(fCounters.count(cName)>0){
			ptr->second->DefineSampleSizeCounter(cName);
		}
		else std::cerr << "Counter " << cName << " doesn't exist." << std::endl;
	}
	else std::cerr << "EventFraction table " << efName << " doesn't exist." << std::endl;
}
void CounterHandler::SetSignificantDigits(TString efName, int v){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param v : number of significant digits
	///
	/// Set number of significant digits for the specified EventFraction table
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,EventFraction*>::type::iterator ptr;

	if((ptr=fEventFraction.find(efName))!=fEventFraction.end()){
		ptr->second->SetPrecision(v);
	}
	else std::cerr << "EventFraction table " << efName << " doesn't exist." << std::endl;
}

void CounterHandler::BookCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the Counter
	///
	/// Book a new counter
	/// \EndMemberDescr

	fCounters.insert(std::pair<TString,int>(name, 0));
}
void CounterHandler::IncrementCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the counter
	///
	/// Increment a previously booked counter by 1
	/// \EndMemberDescr

	IncrementCounter(name, 1);
}
void CounterHandler::IncrementCounter(TString name, int v){
	/// \MemberDescr
	/// \param name : Name of the counter
	/// \param v : value
	///
	/// Increment a previously booked counter by v
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,int>::type::iterator ptr;

	if((ptr=fCounters.find(name))!=fCounters.end()){
		ptr->second += v;
	}
	else std::cerr << "Counter " << name << " doesn't exist." << std::endl;
}
void CounterHandler::DecrementCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the counter
	///
	/// Decrement a previously booked counter by 1
	/// \EndMemberDescr

	DecrementCounter(name,1);
}
void CounterHandler::DecrementCounter(TString name, int v){
	/// \MemberDescr
	/// \param name : Name of the counter
	/// \param v : value
	///
	/// Decrement a previously booked counter by v
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,int>::type::iterator ptr;

	if((ptr=fCounters.find(name))!=fCounters.end()){
		ptr->second -= v;
	}
	else std::cerr << "Counter " << name << " doesn't exist." << std::endl;
}
void CounterHandler::SetCounterValue(TString name, int v){
	/// \MemberDescr
	/// \param name : Name of the counter
	/// \param v : value
	///
	/// Set the value of a previously booked counter
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,int>::type::iterator ptr;

	if((ptr=fCounters.find(name))!=fCounters.end()){
		ptr->second = v;
	}
	else std::cerr << "Counter " << name << " doesn't exist." << std::endl;
}
int CounterHandler::GetCounterValue(TString name) const{
	/// \MemberDescr
	/// \param name : Name of the counter
	///
	/// \return Value of a previously booked counter
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,int>::type::const_iterator ptr;

	if((ptr=fCounters.find(name))!=fCounters.end()){
		return ptr->second;
	}
	else std::cerr << "Counter " << name << " doesn't exist." << std::endl;
	return -1;
}

void CounterHandler::PrintInitSummary() const{
	/// \MemberDescr
	/// Print the summary of the EventFraction and Counters after initialization
	/// \EndMemberDescr

	NA62Analysis::NA62Map<TString,EventFraction*>::type::const_iterator itEvtFrac;
	NA62Analysis::NA62Map<TString,int>::type::const_iterator itCounter;

	StringBalancedTable evtFracTable("List of EventFraction");
	StringBalancedTable counterTable("List of Counters");

	for(itEvtFrac=fEventFraction.begin(); itEvtFrac!=fEventFraction.end(); itEvtFrac++){
		evtFracTable << itEvtFrac->first;
	}

	for(itCounter=fCounters.begin(); itCounter!=fCounters.end(); itCounter++){
		counterTable << itCounter->first;
	}

	evtFracTable.Print("\t");
	counterTable.Print("\t");
}

} /* namespace NA62Analysis */
