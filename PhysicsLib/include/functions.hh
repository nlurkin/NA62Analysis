#include <iostream>
#include <TVector3.h>
#include <signal.h>
#include <set>
#include <queue>

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

#include <TChain.h>
#include <TLorentzVector.h>
#include <TH1I.h>
#include <TH2I.h>
#include <TH2F.h>

#include "NA62Constants.hh"
using namespace std;

/**
 * DISCLAIMER: This file needs reviewing. Most of the functions in this file are here by legacy
 * from early development stages of the framework. None of them should be considered validated
 * at this point.
 */


#define ENABLE_SHELL_COLOR  1
#if ENABLE_SHELL_COLOR == 1
#define SHELL_COLOR_RED     "\033[0;31m"
#define SHELL_COLOR_LRED    "\033[1;31m"
#define SHELL_COLOR_GREEN   "\033[0;32m"
#define SHELL_COLOR_LGREEN  "\033[1;32m"
#define SHELL_COLOR_BROWN   "\033[0;33m"
#define SHELL_COLOR_LBROWN  "\033[1;33m"
#define SHELL_COLOR_BLUE    "\033[0;34m"
#define SHELL_COLOR_LBLUE   "\033[1;34m"
#define SHELL_COLOR_PURPLE  "\033[0;35m"
#define SHELL_COLOR_LPURPLE "\033[1;35m"
#define SHELL_COLOR_NONE    "\033[0m"
#else
#define SHELL_COLOR_RED     ""
#define SHELL_COLOR_LRED    ""
#define SHELL_COLOR_GREEN   ""
#define SHELL_COLOR_LGREEN  ""
#define SHELL_COLOR_BROWN   ""
#define SHELL_COLOR_LBROWN  ""
#define SHELL_COLOR_BLUE    ""
#define SHELL_COLOR_LBLUE   ""
#define SHELL_COLOR_PURPLE  ""
#define SHELL_COLOR_LPURPLE ""
#define SHELL_COLOR_NONE    ""
#endif
#define CLEAN_OUTPUT true

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
