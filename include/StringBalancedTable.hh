/*
 * StringBalancedTable.hh
 *
 *  Created on: Jun 11, 2013
 *      Author: nlurkin
 */

#ifndef STRINGBALANCEDTABLE_HH_
#define STRINGBALANCEDTABLE_HH_

#include "StringTable.hh"

namespace NA62Analysis {

/// \class StringBalancedTable
/// \Brief 
/// Extension of StringTable to print a list of values in a 2D array for
/// better visual than simple 1D list.<br>
/// \EndBrief 
///
/// \Detailed
/// Like StringTable but the number of columns and rows is automatically
/// computed when printing to have a nice lines/columns ratio.
/// \EndDetailed

class StringBalancedTable: public StringTable {
public:
	StringBalancedTable(TString t);
	StringBalancedTable(const StringBalancedTable& c);
	virtual ~StringBalancedTable();

	using StringTable::operator<<;
	StringBalancedTable& operator<<(TString v);

	void Print(TString prefix) const;
	void Print(TString prefix, ostream &s) const;
private:
	std::vector<TString> fValuesList; ///< List of values in the table
};

} /* namespace NA62Analysis */

#endif /* STRINGBALANCEDTABLE_HH_ */
