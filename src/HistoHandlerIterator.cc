/*
 * HistoHandlerIterator.cc
 *
 *  Created on: 2 May 2014
 *      Author: ncl
 */

#include "HistoHandler.hh"
#include <vector>

template <typename PointerType>
HistoHandler::Iterator<PointerType>::Iterator():
	fList(new std::vector<PointerType*>()),
	fNInstances(new int(0)),
	fIterator(fList->begin())
{
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>::Iterator(const Iterator<PointerType>& c):
	fList(c.fList),
	fNInstances(c.fNInstances),
	fIterator(c.fIterator)
{
	(*fNInstances)++;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>::Iterator(const std::vector<PointerType*>& list):
	fList(new std::vector<PointerType*>()),
	fNInstances(new int(0))
{
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
	(*fNInstances)++;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>::~Iterator(){
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
	return fIterator==rhs.fIterator;
}

template <typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator==(const typename std::vector<PointerType*>::iterator& rhs) const{
	return fIterator==rhs;
}

template <typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator!=(const Iterator<PointerType>& rhs) const{
	return fIterator!=rhs.fIterator;
}

template <typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator!=(const typename std::vector<PointerType*>::iterator& rhs) const{
	return fIterator!=rhs;
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator*(){
	return *fIterator;
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator->(){
	return *fIterator;
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator++(int){
	return *(fIterator++);
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator--(int){
	return *(fIterator--);
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator++(){
	return *(++fIterator);
}

template <typename PointerType>
PointerType* HistoHandler::Iterator<PointerType>::operator--(){
	return *(--fIterator);
}

template <typename PointerType>
int HistoHandler::Iterator<PointerType>::operator-(const Iterator<PointerType>& rhs) const{
	return fIterator-rhs.fIterator;
}

template <typename PointerType>
int HistoHandler::Iterator<PointerType>::operator-(const typename std::vector<PointerType*>::iterator& rhs) const{
	return fIterator-rhs;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType> HistoHandler::Iterator<PointerType>::operator-(int rhs) const{
	return HistoHandler::Iterator<PointerType>(*this, fIterator-rhs);
}

template <typename PointerType>
HistoHandler::Iterator<PointerType> HistoHandler::Iterator<PointerType>::operator+(int rhs) const{
	return HistoHandler::Iterator<PointerType>(*this, fIterator+rhs);
}

template <typename PointerType>
HistoHandler::Iterator<PointerType> HistoHandler::Iterator<PointerType>::operator[](int rhs){
	return HistoHandler::Iterator<PointerType>(*this, fList->begin()+rhs);
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>& HistoHandler::Iterator<PointerType>::operator-=(int rhs){
	while(rhs-->0) fIterator--;
	return *this;
}

template <typename PointerType>
HistoHandler::Iterator<PointerType>& HistoHandler::Iterator<PointerType>::operator+=(int rhs){
	while(rhs-->0) fIterator++;
	return *this;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <(const Iterator<PointerType>& rhs) const {
	return (fIterator-rhs.fIterator)<0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <(const typename std::vector<PointerType*>::iterator& rhs) const {
	return (fIterator-rhs)<0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <=(const Iterator<PointerType>& rhs) const {
	return (fIterator-rhs.fIterator)<=0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator <=(const typename std::vector<PointerType*>::iterator& rhs) const {
	return (fIterator-rhs)<=0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >(const Iterator<PointerType>& rhs) const {
	return (fIterator-rhs.fIterator)>0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >(const typename std::vector<PointerType*>::iterator& rhs) const {
	return (fIterator-rhs)>0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >=(const Iterator<PointerType>& rhs) const {
	return (fIterator-rhs.fIterator)>=0;
}

template<typename PointerType>
bool HistoHandler::Iterator<PointerType>::operator >=(const typename std::vector<PointerType*>::iterator& rhs) const {
	return (fIterator-rhs)>=0;
}


template <typename PointerType>
typename std::vector<PointerType*>::iterator HistoHandler::Iterator<PointerType>::End(){
	return fList->end();
}

template <typename PointerType>
typename std::vector<PointerType*>::iterator HistoHandler::Iterator<PointerType>::Begin(){
	return fList->begin();
}

template class HistoHandler::Iterator<TH1>;
template class HistoHandler::Iterator<TH2>;
template class HistoHandler::Iterator<TGraph>;
