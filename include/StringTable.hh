/*
 * StringTable.hh
 *
 *  Created on: Jun 10, 2013
 *      Author: nlurkin
 */
#ifndef STRINGTABLE_HH_
#define STRINGTABLE_HH_

#include <vector>
#include <map>

#include <TString.h>

/// \class StringTable
/// \Brief 
/// Class for pretty table printing. Each cell value is stored as string.
/// \EndBrief 
///
/// \Detailed
/// Print tables with rows, columns, cells, borders. Use standard stream operator<<
/// for filling and automatically print nicely formatted tables. Use also\n
/// \code
/// << endr and << sepr
/// \endcode
/// to respectively go to next row or print an horizontal line and go to next row
/// \EndDetailed

class StringTable {
public:
	StringTable(TString title);
	StringTable(const StringTable& c);
	virtual ~StringTable();

	virtual void Print(TString prefix) const;
	virtual void Print(TString prefix, ostream &s) const;

	void AddColumn(TString id, TString title);
	void AddValue(TString column, TString value);
	void AddValue(unsigned int column, TString value);

	virtual StringTable& operator<<(TString value);
	virtual StringTable& operator<<(int value);
	virtual StringTable& operator<<(StringTable& (*f)(StringTable&));

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

	std::vector< std::vector<TString> > fContent; ///< Cells content
	std::map<TString, int> fOrder; ///< Columns sequence
	mutable std::vector<int> fColWidth; ///< Columns width in character

	TString fTitle; ///< Title of the table
};

StringTable& endr(StringTable& s);
StringTable& sepr(StringTable& s);


#endif /* STRINGTABLE_HH_ */
