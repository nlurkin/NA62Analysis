/*
 * EventFraction.cc
 *
 *  Created on: May 21, 2013
 *      Author: nlurkin
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <TGraphAsymmErrors.h>
#include <TH1D.h>
#include <cmath>

#include "EventFraction.hh"
#include "StringTable.hh"
using namespace std;

EventFraction::EventFraction() {
	fName = "DefaultName";
	fPrecision = 0;
}

EventFraction::EventFraction(TString name) {
	fName = name;
	fPrecision = 0;
}

EventFraction::~EventFraction() {
}

void EventFraction::AddCounter(TString name, int *v){
	fCounters.insert(pair<TString,int*>(name, v));
	fSequence.push_back(name);
}

void EventFraction::DumpTable(){
	PrintToStream(cout);
}

void EventFraction::PrintToStream(ostream &s){
	vector<TString>::iterator it;

	TGraphAsymmErrors as;
	TH1D g("g", "", fCounters.size(), 0, fCounters.size());
	TH1D t("t", "", fCounters.size(), 0, fCounters.size());

	double x,y,eh,el;
	bool isRelative;

	int i = 0;

	StringTable table(fName);

	//Do we have the total to divide
	if(fSampleSizeCounter.Length()>0) isRelative = true;
	else isRelative = false;

	//First loop to fill histograms
	for(it=fSequence.begin(); it!=fSequence.end(); it++){
		if(isRelative){
			g.Fill(i,*(fCounters[*it]));
			t.Fill(i,*(fCounters[fSampleSizeCounter]));
		}
		i++;
	}

	//Compute ratio with errors
	if(isRelative){
		g.Sumw2();
		t.Sumw2();
		as.BayesDivide(&g,&t);
	}

	//Print title row
	//Sequence
	table.AddColumn("seq", "Seq");
	//Counter name
	table.AddColumn("name", "Counter name");
	//Counter value
	table.AddColumn("val", "Value");
	if(isRelative){
		//Fraction
		table.AddColumn("frac", "Fraction");
		//Error high
		table.AddColumn("ehigh", "ErrorHigh");
		//Error low
		table.AddColumn("elow", "ErrorLow");
	}
	table << sepr;

	//Print counters
	i=0;
	x = 0;
	y = 0;
	el = 0;
	eh = 0;
	for(it=fSequence.begin(); it!=fSequence.end(); it++){
		//Get fraction and errors if defined
		if(isRelative){
			as.GetPoint(i,x,y);
			eh = as.GetErrorYhigh(i);
			el = as.GetErrorYlow(i);
		}

		//Sequence
		table << i;
		//Counter name
		if(fSampleSizeCounter.CompareTo(*it, TString::kIgnoreCase)==0) table << (TString("*") + *it);
		else table << *it;
		//Counter value
		table << *fCounters[*it];
		if(isRelative){
			//Fraction
			table << FormatDouble(y);
			//Error high
			table << FormatDouble(eh);
			//Error low
			table << FormatDouble(el);
		}
		i++;
	}
	table.Print("", s);
}

void EventFraction::DefineSampleSizeCounter(TString name){
	if(fCounters.count(name)==0){
		cerr << "Error : " << name << " does not exist or has not been added to " << fName << " EventFraction. Cannot define it as sample size" << endl;
		return;
	}
	fSampleSizeCounter = name;
}

TString EventFraction::FormatDouble(double v){
	stringstream ss;
	ss.precision(GetPrecision(v));
	ss << fixed << v;

	return ss.str();
}

void EventFraction::SetPrecision(int v){
	fPrecision = v;
}

int EventFraction::GetPrecision(double v){
	if(fPrecision==0) return 7;
	int a = (fPrecision - int(log10(fabs(v))));

	if(a>7) a=7;
	return a;
}

void EventFraction::WriteTable(TString filePrefix){
	ofstream fd;
	TString fileName = filePrefix + "_" + fName + ".txt";
	fd.open(fileName.Data(), ofstream::out);
	PrintToStream(fd);
	fd.close();
}
