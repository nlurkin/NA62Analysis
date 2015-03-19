#include <signal.h>
#include <iostream>
#include <set>
#include <queue>

#include <TVector3.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>

#include "TGigaTrackerEvent.hh"
#include "TIRCEvent.hh"
#include "TLAVEvent.hh"
#include "TLKrEvent.hh"
#include "TSACEvent.hh"
#include "TSpectrometerEvent.hh"
#include "TGigaTrackerHit.hh"
#include "TIRCHit.hh"
#include "TLAVHit.hh"
#include "TLKrHit.hh"
#include "TSACHit.hh"
#include "TSpectrometerHit.hh"
#include "KinePart.hh"
#include "Event.hh"

#include "NA62Constants.hh"

/**
 * DISCLAIMER: This file needs reviewing. Most of the functions in this file are here by legacy
 * from early development stages of the framework. None of them should be considered validated
 * at this point.
 */

#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

TVector3 propagate(TVector3 pos, TVector3 p, double z);

TString printVector2( TVector2 v );

TString printVector3( TVector3 v );

TString printVector4( TLorentzVector v );

bool IsInDetector(double propagation_pos, double prod_pos, double min_pos, double des_pos, double x_front, double y_front, double x_back, double y_back, double crnrX[], double crnrY[], int crnr_nb, double center_radius);

bool IsInDetector(double propagation_pos, double prod_pos, double min_pos, double des_pos, double x_front, double y_front, double x_back, double y_back, double inner_radius, double outer_radius, double centerx, double centery);

TVector3 spec_propagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ);

double distance3D(TVector3 p1, TVector3 p2);
double distance2D(TVector3 p1, TVector3 p2, TString plane);
#endif
