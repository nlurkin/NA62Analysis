//Mandatory includes
#include "SkimmingNoStrawMuonsTracks.hh"

#include <stdlib.h>
#include <iostream>

#include <TChain.h>

#include "Event.hh"

#include "Persistency.hh"
#include "MCSimple.hh"
#include "functions.hh"

using namespace NA62Analysis;
using namespace NA62Constants;

/// \class SkimmingNoStrawMuonsTracks
/// \Brief
/// Example Analyzer skimming the input file from events containing muon tracks in RICH.
/// \EndBrief
///
/// \Detailed
///
/// \EndDetailed

SkimmingNoStrawMuonsTracks::SkimmingNoStrawMuonsTracks(Core::BaseAnalysis *ba) : Analyzer(ba, "SkimmingNoStrawMuonsTracks")
{
	useMUV = false;

	RequestTree("Spectrometer", new TRecoSpectrometerEvent);
	RequestTree("RICH", new TRecoRICHEvent);
	RequestTree("MUV3", new TRecoMUV3Event);
}

void SkimmingNoStrawMuonsTracks::InitOutput(){
	AddParam("useMUV", "bool", &useMUV, false);
}

void SkimmingNoStrawMuonsTracks::InitHist(){
	BookCounter("TotalEvents");
	BookCounter("TaggedMuonEvents");
	BookCounter("NoTaggedMuonEvents");

	NewEventFraction("MuonsTrackFraction");
	AddCounterToEventFraction("MuonsTrackFraction", "TotalEvents");
	AddCounterToEventFraction("MuonsTrackFraction", "TaggedMuonEvents");
	DefineSampleSizeCounter("MuonsTrackFraction", "TotalEvents");

	NewEventFraction("NoMuonsTrackFraction");
	AddCounterToEventFraction("NoMuonsTrackFraction", "TotalEvents");
	AddCounterToEventFraction("NoMuonsTrackFraction", "NoTaggedMuonEvents");
	DefineSampleSizeCounter("NoMuonsTrackFraction", "TotalEvents");
}

void SkimmingNoStrawMuonsTracks::DefineMCSimple(){
}

void SkimmingNoStrawMuonsTracks::StartOfRunUser(){

}

void SkimmingNoStrawMuonsTracks::StartOfBurstUser(){

}

void SkimmingNoStrawMuonsTracks::EndOfBurstUser(){

}

void SkimmingNoStrawMuonsTracks::Process(int){
	TRecoRICHEvent *richEvent = (TRecoRICHEvent*)GetEvent("RICH");

	double muonThreshold = 30.0;


	TRecoRICHCandidate *richCand;
	bool hasMuon = false;

	for(int i=0; i<richEvent->GetCandidates()->GetEntries(); i++){
		richCand = (TRecoRICHCandidate*)richEvent->GetCandidate(i);

		if(richCand->GetRingRadius()<muonThreshold) hasMuon=true;
	}

	if(useMUV){
		TRecoMUV3Event *muvEvent = (TRecoMUV3Event*)GetEvent("MUV3");
		if(muvEvent->GetCandidates()>0) hasMuon=true;
	}
	IncrementCounter("TotalEvents");
	if(!hasMuon){
		IncrementCounter("NoTaggedMuonEvents");
		ExportEvent();
	}
	else{
		IncrementCounter("TaggedMuonEvents");
	}
}

void SkimmingNoStrawMuonsTracks::PostProcess(){

}

void SkimmingNoStrawMuonsTracks::EndOfRunUser(){

}


void SkimmingNoStrawMuonsTracks::ExportPlot(){
}

void SkimmingNoStrawMuonsTracks::DrawPlot(){
}
