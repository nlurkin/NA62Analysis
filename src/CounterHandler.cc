/*
 * CounterHandler.cc
 *
 *  Created on: 22 Jan 2014
 *      Author: ncl
 */

#include "CounterHandler.hh"
#include "StringBalancedTable.hh"

CounterHandler::CounterHandler() {
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

CounterHandler::~CounterHandler() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr

	map<TString, EventFraction*>::iterator itEF;

	while(fEventFraction.size()>0){
		itEF = fEventFraction.begin();
		delete itEF->second;
		fEventFraction.erase(itEF);
	}
}

void CounterHandler::WriteEventFraction(TString fileName){
	/// \MemberDescr
	/// Dump the EventFraction
	/// \EndMemberDescr

	map<TString,EventFraction*>::iterator it;

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

	fEventFraction.insert(pair<TString,EventFraction*>(name, new EventFraction(name)));
}
void CounterHandler::AddCounterToEventFraction(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Add a counter in the specified EventFraction table
	/// \EndMemberDescr

	if(fEventFraction.count(efName)>0){
		if(fCounters.count(cName)>0){
			fEventFraction[efName]->AddCounter(cName, &fCounters[cName]);
		}
		else cerr << "Counter " << cName << " doesn't exist." << endl;
	}
	else cerr << "EventFraction table " << efName << " doesn't exist." << endl;
}
void CounterHandler::DefineSampleSizeCounter(TString efName, TString cName){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param cName : Name of the Counter
	///
	/// Define counter as SampleSize in the specified EventFraction table
	/// \EndMemberDescr

	if(fEventFraction.count(efName)>0){
		if(fCounters.count(cName)>0){
			fEventFraction[efName]->DefineSampleSizeCounter(cName);
		}
		else cerr << "Counter " << cName << " doesn't exist." << endl;
	}
	else cerr << "EventFraction table " << efName << " doesn't exist." << endl;
}
void CounterHandler::SetSignificantDigits(TString efName, int v){
	/// \MemberDescr
	/// \param efName : Name of the EventFraction instance
	/// \param v : number of significant digits
	///
	/// Set number of significant digits for the specified EventFraction table
	/// \EndMemberDescr

	if(fEventFraction.count(efName)>0){
		fEventFraction[efName]->SetPrecision(v);
	}
	else cerr << "EventFraction table " << efName << " doesn't exist." << endl;
}

void CounterHandler::BookCounter(TString name){
	/// \MemberDescr
	/// \param name : Name of the Counter
	///
	/// Book a new counter
	/// \EndMemberDescr

	fCounters.insert(pair<TString,int>(name, 0));
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

	if(fCounters.count(name)>0){
		fCounters[name] = fCounters[name]+v;
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
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

	if(fCounters.count(name)>0){
		fCounters[name] = fCounters[name]-v;
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
}
void CounterHandler::SetCounterValue(TString name, int v){
	/// \MemberDescr
	/// \param name : Name of the counter
	/// \param v : value
	///
	/// Set the value of a previously booked counter
	/// \EndMemberDescr

	if(fCounters.count(name)>0){
		fCounters[name] = v;
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
}
int CounterHandler::GetCounterValue(TString name){
	/// \MemberDescr
	/// \param name : Name of the counter
	///
	/// Get the value of a previously booked counter
	/// \EndMemberDescr

	if(fCounters.count(name)>0){
		return fCounters[name];
	}
	else cerr << "Counter " << name << " doesn't exist." << endl;
	return -1;
}

void CounterHandler::PrintInitSummary(){
	/// \MemberDescr
	/// Print the summary of the EventFraction and Counters after initialization
	/// \EndMemberDescr

	map<TString, EventFraction*>::iterator itEvtFrac;
	map<TString, int>::iterator itCounter;

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
