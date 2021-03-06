/*
 * HistoHandlerIterator.cc
 *
 *  Created on: 2 May 2014
 *      Author: ncl
 */

#include "HistoHandler.hh"

#include <vector>

namespace NA62Analysis {
namespace Core {

/// \class HistoHandler::Iterator
///
/// \Detailed
/// This template class is based on vector of pointers. When requesting an Iterator with
/// GetIterator... (replace ... with TH1, TH2 or TGraph) in the HistoHandler class, a vector
/// of pointers to histograms is created. The vector is stored in this class which implements
/// a wrapper around the vector's iterator.\n
/// \n
/// Following is an example of how to use an Iterator to loop over all TH1 histograms stored
/// in the Analyzer and whose name starts with "testHisto":
/// \code
/// 	HistoHandler::IteratorTH1 it;
///
/// 	for(it = GetIteratorTH1("testHisto"); it!=it.End(); it++){
/// 		it->Fill(1);
/// 	}
/// 	if( (it-it.Begin()) >= 5){ // Make sure the list contains at least 5 histograms before the current one
///			it -= 5;	// Go 5 histograms backwards
/// 		(*it)->Fill(10); // Fill it
///			it[2]->Fill(15); // Fill histogram #2
/// 	}
/// \endcode
/// \EndDetailed


template <typename PointerType>
HistoHandler::Iterator<PointerType>::Iterator():
	fList(new std::vector<PointerType*>()),
	fNInstances(new int(0)),
	fIterator(fList->begin())
{
	/// \MemberDescr
	///	Default constructor (new Iterator)
	/// \EndMemberDescr
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>::Iterator(const Iterator<PointerType>& c):
	fList(c.fList),
	fNInstances(c.fNInstances),
	fIterator(c.fIterator)
{
	/// \MemberDescr
	/// \param c : Iterator to copy
	///
	///	Copy constructor. Keeps track of how many copies are created
	/// \EndMemberDescr
	(*fNInstances)++;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>::Iterator(const std::vector<PointerType*>& list):
	fList(new std::vector<PointerType*>()),
	fNInstances(new int(0))
{
	/// \MemberDescr
	/// \param list : vector containing pointers to the elements to be iterated over
	///
	///	Private constructor with list of elements (new Iterator)
	/// \EndMemberDescr

	typename std::vector<PointerType*>::const_iterator it;

	for(it=list.begin(); it!=list.end(); it++){
		fList->push_back(*it);
	}
	fIterator = fList->begin();
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>::Iterator(const Iterator<PointerType>& c, typename std::vector<PointerType*>::iterator it):
	fList(c.fList),
	fNInstances(c.fNInstances),
	fIterator(it)
{
	/// \MemberDescr
	///	\param c : Iterator to copy
	/// \param it : vector iterator to new location
	///
	///	Private constructor to create a copy of Iterator referencing a different location. Keeps track of how many copies are created.
	/// \EndMemberDescr

	(*fNInstances)++;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>::~Iterator(){
	/// \MemberDescr
	///	Destructor. Deletes internal pointers only if this instance is the last copy of the Iterator. Else just decrement the number of copies.
	/// \EndMemberDescr

	if(*fNInstances==0){
		delete fList;
		delete fNInstances;
	}
	else{
		(*fNInstances)--;
	}
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>& HistoHandler::Iterator<PointerType>::operator=(const Iterator<PointerType>& c)
{
	/// \MemberDescr
	/// \param c : Iterator to copy-assign
	/// \return Reference to itself
	///
	/// Copy assignment operator. Keeps track of how many copies are created.
	/// \EndMemberDescr

	if(c==*this) return *this; // If asked to be replaced by itself stop here (itself already = to itself)
	if(fNInstances==c.fNInstances){	//This is already a copy of this Iterator. No need to re-copy everything, only fIterator can be different.
		fIterator = c.fIterator;
		return *this;
	}

	// Check if this instance is the last one before replacing it by a copy of another Iterator
	if(*fNInstances==0){
		delete fList;
		delete fNInstances;
	}
	else{
		(*fNInstances)--;
	}

	fList = c.fList;
	fIterator = c.fIterator;

	fNInstances = c.fNInstances;
	(*fNInstances)++;
	return *this;
}

template <typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator==(const Iterator<PointerType>& rhs) const{
	/// \MemberDescr
	/// \param rhs : Right-hand side Iterator to compare
	/// \return True if the vector iterator of rhs is the same as the internal vector iterator.
	///
	/// Compare this Iterator with another Iterator.
	/// \EndMemberDescr

	return fIterator==rhs.fIterator;
}

template <typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator==(const typename std::vector<PointerType*>::iterator& rhs) const{
	/// \MemberDescr
	/// \param rhs : Right-hand side Iterator to compare
	/// \return True if the vector iterator rhs is the same as the internal vector iterator.
	///
	/// Compare this Iterator with a vector iterator.
	/// \EndMemberDescr

	return fIterator==rhs;
}

template <typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator!=(const Iterator<PointerType>& rhs) const{
	/// \MemberDescr
	/// \param rhs : Right-hand side Iterator to compare
	/// \return True if the vector iterator if rhs is different of the internal vector iterator.
	///
	/// Compare this Iterator with another Iterator.
	/// \EndMemberDescr

	return fIterator!=rhs.fIterator;
}

template <typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator!=(const typename std::vector<PointerType*>::iterator& rhs) const{
	/// \MemberDescr
	/// \param rhs : Right-hand side Iterator to compare
	/// \return True if the vector iterator rhs is different of the internal vector iterator.
	///
	/// Compare this Iterator with a vector iterator.
	/// \EndMemberDescr

	return fIterator!=rhs;
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator*(){
	/// \MemberDescr
	/// \return Pointer to the object pointed by the Iterator
	///
	/// Dereference the Iterator.
	/// \EndMemberDescr

	return *fIterator;
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator->(){
	/// \MemberDescr
	/// \return Pointer to the object pointed by the Iterator
	///
	/// Dereference the Iterator.
	/// \EndMemberDescr

	return *fIterator;
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator++(int){
	/// \MemberDescr
	/// \return Pointer to the object pointed by the Iterator
	///
	/// Increment the Iterator and dereference it (postfix).
	/// \EndMemberDescr

	return *(fIterator++);
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator--(int){
	/// \MemberDescr
	/// \return Pointer to the object pointed by the Iterator
	///
	/// Decrement the Iterator and dereference it (postfix).
	/// \EndMemberDescr

	return *(fIterator--);
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator++(){
	/// \MemberDescr
	/// \return Pointer to the object pointed by the Iterator
	///
	/// Increment the Iterator and dereference it (prefix).
	/// \EndMemberDescr

	return *(++fIterator);
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator--(){
	/// \MemberDescr
	/// \return Pointer to the object pointed by the Iterator
	///
	/// Decrement the Iterator and dereference it (prefix).
	/// \EndMemberDescr

	return *(--fIterator);
}

template <typename PointerType>
int HistoHandler::Iterator<PointerType>::operator-(const Iterator<PointerType>& rhs) const{
	/// \MemberDescr
	/// \param rhs : Right hand side Iterator
	/// \return Distance between iterators (lhs-rhs).
	/// \EndMemberDescr

	return fIterator-rhs.fIterator;
}

template <typename PointerType>
int HistoHandler::Iterator<PointerType>::operator-(const typename std::vector<PointerType*>::iterator& rhs) const{
	/// \MemberDescr
	/// \param rhs : Right hand side vector iterator
	/// \return Distance between iterators (lhs-rhs).
	/// \EndMemberDescr

	return fIterator-rhs;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType> HistoHandler::Iterator<PointerType>::operator-(int rhs) const{
	/// \MemberDescr
	/// \param rhs : decrement value
	/// \return Copy of the Iterator decremented by rhs.
	/// \EndMemberDescr

	return HistoHandler::Iterator<PointerType>(*this, fIterator-rhs);
}

template <typename PointerType>
HistoHandler::Iterator<PointerType> HistoHandler::Iterator<PointerType>::operator+(int rhs) const{
	/// \MemberDescr
	/// \param rhs : increment value
	/// \return Copy of the Iterator incremented by rhs.
	/// \EndMemberDescr

	return HistoHandler::Iterator<PointerType>(*this, fIterator+rhs);
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator[](int rhs){
	/// \MemberDescr
	/// \param rhs : offset to apply
	/// \return the object at position rhs.
	///
	/// Offset dereference operator.
	/// \EndMemberDescr

	return *(fList->begin()+rhs);
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>& HistoHandler::Iterator<PointerType>::operator-=(int rhs){
	/// \MemberDescr
	/// \param rhs : decrement value
	/// \return Reference to itself
	///
	/// Decrement this Iterator by rhs.
	/// \EndMemberDescr

	while(rhs-->0) fIterator--;
	return *this;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>& HistoHandler::Iterator<PointerType>::operator+=(int rhs){
	/// \MemberDescr
	/// \param rhs : increment value
	/// \return Reference to itself
	///
	/// Increment this Iterator by rhs.
	/// \EndMemberDescr

	while(rhs-->0) fIterator++;
	return *this;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <(const Iterator<PointerType>& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is smaller than 0 (lhs < rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs.fIterator)<0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <(const typename std::vector<PointerType*>::iterator& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is smaller than 0 (lhs < rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs)<0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <=(const Iterator<PointerType>& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is smaller or equal to 0 (lhs <= rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs.fIterator)<=0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <=(const typename std::vector<PointerType*>::iterator& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is smaller or equal to 0 (lhs <= rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs)<=0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >(const Iterator<PointerType>& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is greater than 0 (lhs > rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs.fIterator)>0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >(const typename std::vector<PointerType*>::iterator& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is greater than 0 (lhs > rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs)>0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >=(const Iterator<PointerType>& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is greater or equale to 0 (lhs >= rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs.fIterator)>=0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >=(const typename std::vector<PointerType*>::iterator& rhs) const {
	/// \MemberDescr
	/// \param rhs : Iterator to compare
	/// \return True if the distance between both Iterators is greater or equale to 0 (lhs >= rhs).
	///
	/// Compare iterators.
	/// \EndMemberDescr

	return (fIterator-rhs)>=0;
}


template <typename PointerType>
typename std::vector<PointerType*>::iterator HistoHandler::Iterator<PointerType>::End(){
	/// \MemberDescr
	/// \return Past-the-end reference of this Iterator.
	///
	/// If an Iterator is equal to this reference, the traversal is over.
	/// \EndMemberDescr

	return fList->end();
}

template <typename PointerType>
typename std::vector<PointerType*>::iterator HistoHandler::Iterator<PointerType>::Begin(){
	/// \MemberDescr
	/// \return First element.
	/// \EndMemberDescr

	return fList->begin();
}

template class HistoHandler::Iterator<TH1>;
template class HistoHandler::Iterator<TH2>;
template class HistoHandler::Iterator<TGraph>;

} /* namespace Core */
} /* namespace NA62Analysis */
