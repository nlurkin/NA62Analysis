/*
 * StringBalancedTable.cc
 *
 *  Created on: Jun 11, 2013
 *      Author: nlurkin
 */

#include "StringBalancedTable.hh"
#include <iostream>

StringBalancedTable::StringBalancedTable(TString t) : StringTable(t){
	/// \MemberDescr
	/// \param t : Name of the table
	///
	/// Default constructor
	/// \MemberDescr

}

StringBalancedTable::StringBalancedTable(const StringBalancedTable& c) :
		StringTable(c),
		fValuesList(c.fValuesList)
{
	/// \MemberDescr
	/// \param c : Reference of the object to copy
	///
	/// Copy constructor
	/// \MemberDescr

}

StringBalancedTable::~StringBalancedTable() {
	/// \MemberDescr
	/// Default destructor
	/// \MemberDescr

}

StringBalancedTable& StringBalancedTable::operator <<(TString v){
	/// \MemberDescr
	/// \param v : value to append
	/// \return Reference to current StringBalancedTable instance
	///
	/// Append a value to the list
	/// \MemberDescr

	fValuesList.push_back(v);
	return *this;
}

void StringBalancedTable::Print(TString prefix){
	/// \MemberDescr
	/// \param prefix : prefix to add in front of each line
	///
	/// Print the table to stdout. Prefix is prepended to every line.\n
	/// Example \\t to add a tabulation in front of the whole table.
	/// \MemberDescr

	Print(prefix, std::cout);
}

void StringBalancedTable::Print(TString prefix, ostream &s){
	/// \MemberDescr
	/// \param prefix : prefix to add in front of each line
	/// \param s : stream to which the table is printed
	///
	/// Print the table to the specified stream. Prefix is prepended to every line.\n
	/// Example \\t to add a tabulation in front of the whole table.
	/// \MemberDescr

	int size, colsNumber;
	std::vector<TString>::iterator it;

	size = fValuesList.size();

	colsNumber = (size/4) + 1;
	if(colsNumber>4) colsNumber = 4;

	AddColumn(colsNumber);

	for(it=fValuesList.begin(); it!=fValuesList.end(); it++){
		StringTable::operator <<(*it);
	}

	if(size % colsNumber != 0) StringTable::operator<<(endr);

	StringTable::Print(prefix, s);
}
