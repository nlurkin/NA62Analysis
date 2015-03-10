/*
 * StringTable.cc
 *
 *  Created on: Jun 10, 2013
 *      Author: nlurkin
 */

#include "StringTable.hh"

#include <iostream>
#include <iomanip>

StringTable::StringTable(TString title):
		fColumns(0),
		fRows(0),
		fTableWidth(0),
		fCurrCol(0),
		fTitle(title)
{
	/// \MemberDescr
	/// \param title : Table title
	///
	/// Constructor.
	/// \EndMemberDescr

}

StringTable::StringTable(const StringTable& c):
		fColumns(c.fColumns),
		fRows(c.fRows),
		fTableWidth(c.fTableWidth),
		fCurrCol(c.fCurrCol),
		fContent(c.fContent),
		fOrder(c.fOrder),
		fColWidth(c.fColWidth),
		fTitle(c.fTitle)
{
	/// \MemberDescr
	///	\param c: StringTable to copy
	/// Copy Constructor
	/// \EndMemberDescr

}

StringTable::~StringTable() {
	/// \MemberDescr
	/// Default destructor
	/// \EndMemberDescr

}

void StringTable::AddColumn(TString id, TString title){
	/// \MemberDescr
	/// \param id : reference id of the column
	/// \param title : title of the column
	///
	/// Add a column to the table.
	/// \EndMemberDescr

	fOrder.insert(std::pair<TString, int>(id, fColumns));
	fContent.push_back(std::vector<TString>());
	fContent[fColumns].push_back(title);
	fColumns++;
	fRows++;
}

void StringTable::AddColumn(int nbr){
	/// \MemberDescr
	/// \param nbr : number of columns to add
	///
	/// Add many columns to the table. The columns reference id and titles will be the column indices
	/// \EndMemberDescr

	while(nbr>0){
		fOrder.insert(std::pair<TString, int>(fColumns, fColumns));
		fContent.push_back(std::vector<TString>());
		fColumns++;
		fRows++;
		nbr--;
	}
}

void StringTable::AddValue(TString column, TString value){
	/// \MemberDescr
	/// \param column : reference id of the column
	/// \param value : value to add
	///
	/// Add a value at the end of the given column.
	/// \EndMemberDescr

	if(fOrder.count(column)==0) AddColumn(column, column);

	fContent[fOrder[column]].push_back(value);
	fRows++;
}

void StringTable::AddValue(unsigned int column, TString value){
	/// \MemberDescr
	/// \param column : index of the column
	/// \param value : value to add
	///
	/// Add a value at the end of the given column.
	/// \EndMemberDescr

	if(column<fContent.size()){
		fContent[column].push_back(value);
		fRows++;
	}
}

StringTable& StringTable::operator<<(TString value){
	/// \MemberDescr
	/// \param value : value to add
	/// \return Reference to current StringTable instance
	///
	/// Append a value to the table at the current column. The current column is incremented and goes from left to right and up to down.
	/// \EndMemberDescr

	fContent[fCurrCol].push_back(value);
	fCurrCol++;
	fCurrCol = fCurrCol % fColumns;
	fRows++;

	return *this;
}
StringTable& StringTable::operator<<(int value){
	/// \MemberDescr
	/// \param value : value to add
	/// \return Reference to current StringTable instance
	///
	/// Append a value to the table at the current column. The current column is incremented and goes from left to right and up to down.
	/// \EndMemberDescr

	fContent[fCurrCol].push_back(TString("")+=value);
	fCurrCol++;
	fCurrCol = fCurrCol % fColumns;
	fRows++;

	return *this;
}
StringTable& StringTable::operator<<(StringTable& (*f)(StringTable&)){
	/// \MemberDescr
	/// \param f : function to apply
	/// \return Reference to current StringTable instance
	///
	/// Apply the function to the stream
	/// \EndMemberDescr

	return f(*this);
}

StringTable& endr(StringTable& s){
	s.NewRow();
	return s;
}
StringTable& sepr(StringTable& s){
	s.AddSeparatorRow();
	return s;
}

void StringTable::NewRow(){
	/// \MemberDescr
	/// End the current row and move to the next row. The current column is moved to the leftmost column.
	/// \EndMemberDescr

	while(fCurrCol<fColumns){
		fContent[fCurrCol].push_back("");
		fCurrCol++;
		fRows++;
	}
	fCurrCol=0;
}

void StringTable::AddSeparatorRow(){
	/// \MemberDescr
	/// Add a separator row (horizontal line) until the end of the current row and move to the new row.\n
	/// The current column is moved to the leftmost column.
	/// \EndMemberDescr

	while(fCurrCol<fColumns){
		fContent[fCurrCol].push_back("__separator__");
		fCurrCol++;
		fRows++;
	}
	fCurrCol=0;
}

void StringTable::Print(TString prefix) const{
	/// \MemberDescr
	/// \param prefix : prefix to add in front of each line
	///
	/// Print the table to stdout. Prefix is prepended to every line.\n
	/// Example \\t to add a tabulation in front of the whole table.
	/// \EndMemberDescr

	Print(prefix, std::cout);
}

void StringTable::Print(TString prefix, ostream &s) const{
	/// \MemberDescr
	/// \param prefix : prefix to add in front of each line
	/// \param s : stream to which the table is printed
	///
	/// Print the table to the specified stream. Prefix is prepended to every line.\n
	/// Example \\t to add a tabulation in front of the whole table.
	/// \EndMemberDescr

	ComputeWidth();

	s << prefix << FormatCellMiddle(fTitle, '-', fTableWidth) << std::endl;
	for(int i=0; i<GetRowsMaxWidth(); i++){
		s << prefix << "|";
		for(int j=0; j<fColumns; j++){
			if(i<GetRowsMaxWidth(j)) s << FormatCell(fContent[j][i], ' ', fColWidth[j]) << "|";
			else s << FormatCell("", ' ', fColWidth[j]) << "|";
		}
		s << std::endl;
	}
	s << prefix << FormatCellMiddle("", '-', fTableWidth) << std::endl << std::endl;
}

void StringTable::ComputeWidth() const{
	/// \MemberDescr
	/// Compute the width of the table in characters (in characters).
	/// \EndMemberDescr

	int length;

	fTableWidth = 0;
	fTableWidth+=2; //Add left and right border;
	fTableWidth+=fColumns-1; //Add middle borders;

	for(int i=0; i<fColumns; i++){
		length = 0;
		for(int j=0; j<GetRowsMaxWidth(i); j++){
			if(fContent[i][j].CompareTo("__separator__", TString::kIgnoreCase)!=0){
				if(length<fContent[i][j].Length()) length=fContent[i][j].Length();
			}
		}
		fColWidth.push_back(length);
		fTableWidth+=length+2;
	}
	int diff = fTitle.Length()-fTableWidth;
	int colAdd;
	if(diff>0){
		diff+=4;
		colAdd = ((diff/(fColumns))+1)*fColumns;
		fTableWidth+=fColumns*colAdd;
		for(int i=0; i<fColumns; i++){
			fColWidth[i]+=colAdd;
		}
	}
}

TString StringTable::FormatCell(TString v, char fill, int width) const{
	/// \MemberDescr
	/// \param v : value of the cell
	/// \param fill : character to append to v to fit the width
	/// \param width : width of the cell in characters
	/// \return Formatted TString
	///
	/// Format the cell content to fit the specified width.
	/// \EndMemberDescr

	TString zeros;
	if(v.CompareTo("__separator__", TString::kIgnoreCase)==0){
		zeros.Append('-', width+2);
		return zeros;
	}
	else{
		zeros.Append(fill, width-v.Length());
		return " " + v + zeros + " ";
	}
}

TString StringTable::FormatCellMiddle(TString v, char fill, int width) const{
	/// \MemberDescr
	/// \param v : value of the cell
	/// \param fill : character to prepend and append to v to fit the width
	/// \param width : width of the cell in characters
	/// \return Formatted TString
	///
	/// Format the cell content to fit the specified width. The value is aligned in the middle of the cell.
	/// \EndMemberDescr

	TString zerosL, zerosT;

	zerosL.Append(fill, (width-v.Length())/2);
	zerosT.Append(fill, width-v.Length()-zerosL.Length());
	return zerosL + v + zerosT;
}

int StringTable::GetRowsMaxWidth() const {
	/// \MemberDescr
	/// \return Width (in characters) of the largest row.
	/// \EndMemberDescr

	int max = 0;
	for(int i=0; i<fColumns; i++){
		if(max<GetRowsMaxWidth(i)) max = GetRowsMaxWidth(i);
	}

	return max;
}

int StringTable::GetRowsMaxWidth(int i) const {
	/// \MemberDescr
	/// \param i : index of the row
	/// \return Width of the specified row (in characters)
	/// \EndMemberDescr

	return fContent[i].size();
}
