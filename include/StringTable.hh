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

enum stOperators {endr, sepr};

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
	virtual ~StringTable();

	void Print(TString prefix);
	void Print(TString prefix, ostream &s);

	void AddColumn(TString id, TString title);
	void AddValue(TString column, TString value);
	void AddValue(unsigned int column, TString value);

	void operator<<(TString v);
	void operator<<(int v);
	void operator<<(stOperators op);
	void NewRow();
	void AddSeparatorRow();
protected:
	void ComputeWidth();
	TString FormatCell(TString v, char fill, int width);
	TString FormatCellMiddle(TString v, char fill, int width);
	int GetRowsMaxWidth();
	int GetRowsMaxWidth(int i);
	void AddColumn(int nbr);

	int fColumns; ///< Number of columns
	int fRows; ///< Number of rows

	int fTableWidth; ///< Table width in characters

	int fCurrCol; ///< Current column index

	vector< vector<TString> > fContent; ///< Cells content
	map<TString, int> fOrder; ///< Columns sequence
	vector<int> fColWidth; ///< Columns width in character

	TString fTitle; ///< Title of the table
};

#endif /* STRINGTABLE_HH_ */
