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
#define SHELL_COLOR_RED     "\e[0;31m"
#define SHELL_COLOR_LRED    "\e[1;31m"
#define SHELL_COLOR_GREEN   "\e[0;32m"
#define SHELL_COLOR_LGREEN  "\e[1;32m"
#define SHELL_COLOR_BROWN   "\e[0;33m"
#define SHELL_COLOR_LBROWN  "\e[1;33m"
#define SHELL_COLOR_BLUE    "\e[0;34m"
#define SHELL_COLOR_LBLUE   "\e[1;34m"
#define SHELL_COLOR_PURPLE  "\e[0;35m"
#define SHELL_COLOR_LPURPLE "\e[1;35m"
#define SHELL_COLOR_NONE    "\e[0m"
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

void map_hit(set<int> partTree, TLKrEvent* Event, TH2I* map);

void map_hit(set<int> partTree, TSpectrometerEvent* Event, TH2I* map);

void map_hit(set<int> partTree, TLAVEvent* Event, TH2I* map);

void map_hit(set<int> partTree, TIRCEvent* Event, TH2I* map);

void map_hit(set<int> partTree, TSACEvent* Event, TH2I* map);

void map_hit(KinePart* part, TGigaTrackerEvent* Event, TH2I* map);

NA62Constants::DetectorID detection(set<int> partTree, TIRCEvent* ircEvent, TLAVEvent* lavEvent, TLKrEvent* lkrEvent, TSACEvent* sacEvent, double x_extr, double y_extr, bool& irc_det, bool& lav_det, bool& lkr_det, bool& sac_det, double& rec_energy, double &clst_x, double &clst_y/*, double &clst_xrms, double &clst_yrms*/);

TVector3 spec_propagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ, const Double_t fEndZ);

set<int> BuildParticleSet(KinePart* init_Part, Event *runEvt);

NA62Constants::DetectorID detection_straw(int part, TSpectrometerEvent* strEvent, double outer_radius[4], double center_x[4], double center_y[4], bool& straw1, bool& straw2, bool& straw3, bool& straw4);

NA62Constants::DetectorID detection_gtk(int part, TGigaTrackerEvent* gtkEvent, bool& gtk1);

vector<KinePart*> getDaughter(int id, Event *runEvt);

int getDaughter(int id, Event *runEvt, queue<KinePart*> &daughter);

int getDaughterNbr(int id, Event* runEvt);

double distance3D(TVector3 p1, TVector3 p2);
double distance2D(TVector3 p1, TVector3 p2, TString plane);
#endif
