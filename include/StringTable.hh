/*
 * StringTable.hh
 *
 *  Created on: Jun 10, 2013
 *      Author: nlurkin
 */

#include <TString.h>
#include <vector>
#include <map>
using namespace std;

#ifndef STRINGTABLE_HH_
#define STRINGTABLE_HH_

/// \class StringTable
/// \Brief 
/// Class for pretty table printing. Each cell value is stored as string.
/// \EndBrief 
///
/// \Detailed
/// Class for pretty table printing. Each cell value is stored as string.
/// \EndDetailed

class StringTable {
public:
	StringTable(TString title);
	StringTable(const StringTable& c);
	virtual ~StringTable();

	void Print(TString prefix) const;
	void Print(TString prefix, ostream &s) const;

	void AddColumn(TString id, TString title);
	void AddValue(TString column, TString value);
	void AddValue(unsigned int column, TString value);

	StringTable& operator<<(TString value);
	StringTable& operator<<(int value);
	StringTable& operator<<(StringTable& (*f)(StringTable&));

	void NewRow();
	void AddSeparatorRow();
protected:
	void ComputeWidth() const;
	TString FormatCell(TString v, char fill, int width) const;
	TString FormatCellMiddle(TString v, char fill, int width) const;
	int GetRowsMaxWidth() const;
	int GetRowsMaxWidth(int i) const;
	void AddColumn(int nbr);

	int fColumns; ///< Number of columns
	int fRows; ///< Number of rows

	mutable int fTableWidth; ///< Table width in characters

	int fCurrCol; ///< Current column index

	vector< vector<TString> > fContent; ///< Cells content
	map<TString, int> fOrder; ///< Columns sequence
	mutable vector<int> fColWidth; ///< Columns width in character

	TString fTitle; ///< Title of the table
};

StringTable& endr(StringTable& s);
StringTable& sepr(StringTable& s);


#endif /* STRINGTABLE_HH_ */
