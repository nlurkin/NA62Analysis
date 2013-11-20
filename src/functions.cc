#include <iostream>
#include <TVector3.h>
#include <signal.h>
#include <sstream>

#include "functions.hh"
using namespace std;

TVector3 propagate(TVector3 pos, TVector3 p, double z){
	/// \MemberDescr
	/// \param pos : position
	/// \param p : momentum
	/// \param z : projection position
	///
	/// Propagate the particle to z position
	/// \EndMemberDescr

	TVector3 result;
	result = pos + p*((z-pos.Z())/p.Z());
	return result;
}

TString printVector2( TVector2 v )
{
	/// \MemberDescr
	/// \param v : Vector to format
	///
	/// Output correct format for printing TVector2 coordinates
	/// \EndMemberDescr

	ostringstream ss;
    ss << "( " << v.X() << ", " << v.Y() << " )";
    return TString( ss.str().c_str() );
}

TString printVector3( TVector3 v )
{
	/// \MemberDescr
	/// \param v : Vector to format
	///
	/// Output correct format for printing TVector3 coordinates
	/// \EndMemberDescr

	ostringstream ss;
    ss << "( " << v.X() << ", " << v.Y() << ", " << v.Z() << " )";
    return TString( ss.str().c_str() );
}

TString printVector4( TLorentzVector v )
{
	/// \MemberDescr
	/// \param v : Vector to format
	///
	/// Output correct format for printing TLorentzVector coordinates
	/// \EndMemberDescr

	ostringstream ss;
    ss << "( " << v.T() << ", " << v.X() << ", " << v.Y() << ", " << v.Z() << " )";
    return TString( ss.str().c_str() );
}

bool IsInDetector(double propagation_pos, double prod_pos, double min_pos, double des_pos, double x_front, double y_front, double x_back, double y_back, double crnrX[], double crnrY[], int crnr_nb, double center_radius){
	/// \MemberDescr
	/// \param propagation_pos : position of the detector min z
	/// \param prod_pos : initial position of the particle
	/// \param min_pos : minimum prod_pos to be reached for the  particle to be accepted (usually max z of detector - particle has to be produced before the detector)
	/// \param des_pos : minimum decay z position to be reached for the particle to be accepted (usually detector min z or max z(if it has to cross the whole detector to be detected))
	/// \param x_front : particle x coordinate at detector front plane
	/// \param y_front : particle y coordinate at detector front plane
	/// \param x_back : particle x coordinate at detector back plane
	/// \param y_back : particle y coordinate at detector back plane
	/// \param crnrX[] : x coordinates of detector corners
	/// \param crnrY[] : y coordinates of detector corners
	/// \param crnr_nb : number of detector corners
	/// \param center_radius : radius around (x=0,y=0) corresponding to beam hole (no sensitive part). 0 if no beam hole.
	///
	/// Say if the particle is in the detector volume. For polygonal detectors.\n
	/// WARNING : Approximations are always made with this function. Use the DetectorAcceptance class instead.
	/// \EndMemberDescr

	int i, j=crnr_nb-1;
	bool oddNodes_front = false;
	bool oddNodes_back = false;
	bool outside = false;

	for (i=0; i<crnr_nb; i++) {
		if ((crnrY[i]<y_front && crnrY[j]>=y_front) || (crnrY[j]<y_front && crnrY[i]>=y_front)) {
			if (crnrX[i]+(y_front-crnrY[i])/(crnrY[j]-crnrY[i])*(crnrX[j]-crnrX[i])<x_front) {
				oddNodes_front=!oddNodes_front;
			}
		}
		j=i;
	}

	for (i=0; i<crnr_nb; i++) {
		if ((crnrY[i]<y_back && crnrY[j]>=y_back) || (crnrY[j]<y_back && crnrY[i]>=y_back)) {
			if (crnrX[i]+(y_back-crnrY[i])/(crnrY[j]-crnrY[i])*(crnrX[j]-crnrX[i])<x_back) {
				oddNodes_back=!oddNodes_back;
			}
		}
		j=i;
	}

	if((pow(x_front,2)+pow(y_front,2)<pow(center_radius,2)) && (pow(x_back,2)+pow(y_back,2)<pow(center_radius,2))) outside = true;

	if(des_pos < min_pos) outside = true;
	if(prod_pos > propagation_pos) outside = true;

	return oddNodes_front && oddNodes_back && !outside;
}

bool IsInDetector(double propagation_pos, double prod_pos, double min_pos, double des_pos, double x_front, double y_front, double x_back, double y_back, double inner_radius, double outer_radius, double centerx, double centery){
	/// \MemberDescr
	/// \param propagation_pos : position of the detector min z
	/// \param prod_pos : initial position of the particle
	/// \param min_pos : minimum prod_pos to be reached for the  particle to be accepted (usually max z of detector - particle has to be produced before the detector)
	/// \param des_pos : minimum decay z position to be reached for the particle to be accepted (usually detector min z or max z(if it has to cross the whole detector to be detected))
	/// \param x_front : particle x coordinate at detector front plane
	/// \param y_front : particle y coordinate at detector front plane
	/// \param x_back : particle x coordinate at detector back plane
	/// \param y_back : particle y coordinate at detector back plane
	/// \param inner_radius : inner radius of cylinder
	/// \param outer_radius : outer radius od cylinder
	/// \param centerx : x coordinate of cylinder center
	/// \param centery : y coordinate of cylinder center
	///
	/// Say if the particle is in the detector volume. For cylindrical detectors<br>
	/// WARNING : Approximations are always made with this function. Use the DetectorAcceptance class instead.
	/// \EndMemberDescr

	double sq_radius_front = (pow(x_front-centerx,2) + pow(y_front-centery, 2));
	double sq_radius_back = (pow(x_back-centerx,2) + pow(y_back-centery, 2));

	bool outside = false;

	if(((sq_radius_front < pow(outer_radius,2)) && (sq_radius_front > pow(inner_radius,2))) || ((sq_radius_back < pow(outer_radius,2)) && (sq_radius_back > pow(inner_radius,2)))) outside = false;
	else outside = true;

	if(des_pos < min_pos) outside = true;
	if(prod_pos > propagation_pos) outside = true;

	return !outside;
}

void map_hit(set<int> partTree, TLKrEvent* Event, TH2I* map){
	/// \MemberDescr
	/// \param partTree :
	/// \param Event :
	/// \param map :
	///
	/// Fill 2D-histogram with hits position for the specified particle.\n
	/// Histogram has to be provided.\n
	/// Version for LKr
	/// \EndMemberDescr

	TLKrHit* hit;

	int hit_nb;

	//On LKr
	hit_nb = Event->GetNHits();
	for(int m=0; m<hit_nb; m++){
		hit = (TLKrHit*)Event->Hit(m);
		map->Fill(hit->GetPosition().X(), hit->GetPosition().Y());
	}
}

void map_hit(set<int> partTree, TSpectrometerEvent* Event, TH2I* map){
	/// \MemberDescr
	/// \param partTree :
	/// \param Event :
	/// \param map :
	///
	/// Fill 2D-histogram with hits position.\n
	/// Histogram has to be provided.\n
	/// Version for Spectrometer
	/// \EndMemberDescr

	TSpectrometerHit* hit;

	int hit_nb;

	hit_nb = Event->GetNHits();
	for(int m=0; m<hit_nb; m++){
		hit = (TSpectrometerHit*)Event->Hit(m);
		if(partTree.count(hit->GetMCTrackID())>0){
			map->Fill(hit->GetPosition().X(), hit->GetPosition().Y());
		}
	}
}

void map_hit(set<int> partTree, TLAVEvent* Event, TH2I* map){
	/// \MemberDescr
	/// \param partTree :
	/// \param Event :
	/// \param map :
	///
	/// Fill 2D-histogram with hits position for the specified particle.\n
	/// Histogram has to be provided.\n
	/// Version for LAV
	/// \EndMemberDescr

	TLAVHit* hit;

	int hit_nb;

	hit_nb = Event->GetNHits();
	for(int m=0; m<hit_nb; m++){
		hit = (TLAVHit*)Event->Hit(m);
		if(partTree.count(hit->GetMCTrackID())>0) map->Fill(hit->GetPosition().X(), hit->GetPosition().Y());
	}
}

void map_hit(set<int> partTree, TIRCEvent* Event, TH2I* map){
	/// \MemberDescr
	/// \param partTree :
	/// \param Event :
	/// \param map :
	///
	/// Fill 2D-histogram with hits position for the specified particle.\n
	/// Histogram has to be provided.\n
	/// Version for IRC
	/// \EndMemberDescr

	TIRCHit* hit;

	int hit_nb;

	hit_nb = Event->GetNHits();
	for(int m=0; m<hit_nb; m++){
		hit = (TIRCHit*)Event->Hit(m);
		if(partTree.count(hit->GetMCTrackID())>0) map->Fill(hit->GetPosition().X(), hit->GetPosition().Y());
	}
}

void map_hit(set<int> partTree, TSACEvent* Event, TH2I* map){
	/// \MemberDescr
	/// \param partTree :
	/// \param Event :
	/// \param map :
	///
	/// Fill 2D-histogram with hits position for the specified particle.\n
	/// Histogram has to be provided.\n
	/// Version for SAC
	/// \EndMemberDescr

	TSACHit* hit;

	int hit_nb;

	hit_nb = Event->GetNHits();
	for(int m=0; m<hit_nb; m++){
		hit = (TSACHit*)Event->Hit(m);
		if(partTree.count(hit->GetMCTrackID())>0) map->Fill(hit->GetPosition().X(), hit->GetPosition().Y());
	}
}

void map_hit(KinePart* part, TGigaTrackerEvent* Event, TH2I* map){
	/// \MemberDescr
	/// \param part :
	/// \param Event :
	/// \param map :
	///
	/// Fill 2D-histogram with hits position for the specified particle.\n
	/// Histogram has to be provided.\n
	/// Version for Gigatracker
	/// \EndMemberDescr

	TGigaTrackerHit* hit;

	int hit_nb;

	hit_nb = Event->GetNHits();
	for(int m=0; m<hit_nb; m++){
		hit = (TGigaTrackerHit*)Event->Hit(m);
		if(part->GetID()==hit->GetMCTrackID()) map->Fill(hit->GetPosition().X(), hit->GetPosition().Y());
	}
}

DetectorID detection(set<int> partTree, TIRCEvent* ircEvent, TLAVEvent* lavEvent, TLKrEvent* lkrEvent, TSACEvent* sacEvent, double x_extr, double y_extr, bool& irc_det, bool& lav_det, bool& lkr_det, bool& sac_det, double& rec_energy, double &clst_x, double &clst_y/*, double &clst_xrms, double &clst_yrms*/){
	/// \MemberDescr
	/// \param partTree :
	/// \param ircEvent :
	/// \param lavEvent :
	/// \param lkrEvent :
	/// \param sacEvent :
	/// \param x_extr :
	/// \param y_extr :
	/// \param irc_det :
	/// \param lav_det :
	/// \param lkr_det :
	/// \param sac_det :
	/// \param rec_energy :
	/// \param clst_x :
	/// \param clst_y :
	///
	/// Say if the photon is detected, in which detector(s), the energy and cluster position.\n
	/// You must provide the decay tree of the particle\n
	/// WARNING : Don't work very well since all the particles are not stored in GEANT4
	/// \EndMemberDescr

	bool glav = false;
	bool girc = false;
	bool glkr = false;
	bool gsac = false;

	double det_energy[4] = {0,0,0,0}; //0=irc, 1=lav, 2=lkr, 3=sac, 4=miss
	DetectorID det_id[5] = {IRC, LAV, LKr, SAC, Missed};
	double det_total_energy[4] = {0,0,0,0};

	double tclst_x[4] = {0,0,0,0};
	double tclst_y[4] = {0,0,0,0};
	//double tclst_xrms[4] = {0,0,0,0};
	//double tclst_yrms[4] = {0,0,0,0};
	int used_hits[4] = {0,0,0,0};

	double ircCut = 100; //100 MeV
	double lavCut = 100; //100 MeV
	double lkrCut = 100; //100 MeV
	double sacCut = 100; //100 MeV

	int det_nb = 4;

	TIRCHit* ircHit;
	TLAVHit* lavHit;
	TLKrHit* lkrHit;
	TSACHit* sacHit;

	int hit_nb;

	//On LAV's
	hit_nb = lavEvent->GetNHits();
	for(int m=0; m<hit_nb; m++){
		lavHit = (TLAVHit*)lavEvent->Hit(m);
		det_total_energy[1] += lavHit->GetEnergy();
		if((partTree.count(lavHit->GetMCTrackID())>0) || (pow(lavHit->GetPosition().X()-x_extr, 2) + pow(lavHit->GetPosition().Y()-y_extr, 2) < 900)){
			det_energy[1] += lavHit->GetEnergy();
			tclst_x[1] += lavHit->GetPosition().X();
			tclst_y[1] += lavHit->GetPosition().Y();
			used_hits[1]++;
		}
	}
	if(det_energy[1]>lavCut){
		lav_det++;
		glav = true;
	}

	//On LKr
	hit_nb = lkrEvent->GetNHits();
	for(int m=0; m<hit_nb; m++){
		lkrHit = (TLKrHit*)lkrEvent->Hit(m);
		det_total_energy[2] += lkrHit->GetEnergy();
		if((partTree.count(lkrHit->GetMCTrackID())>0) || (pow(lkrHit->GetPosition().X()-x_extr, 2) + pow(lkrHit->GetPosition().Y()-y_extr, 2) < 900)){
			det_energy[2] += lkrHit->GetEnergy();
			tclst_x[2] += lkrHit->GetPosition().X();
			tclst_y[2] += lkrHit->GetPosition().Y();
			used_hits[2]++;
		}
	}
	if(det_energy[2]>lkrCut){
		lkr_det++;
		glkr = true;
	}

	//On SAC
	hit_nb = sacEvent->GetNHits();
	for(int m=0; m<hit_nb; m++){
		sacHit = (TSACHit*)sacEvent->Hit(m);
		det_total_energy[3] += sacHit->GetEnergy();
		if((partTree.count(sacHit->GetMCTrackID())>0) || (pow(sacHit->GetPosition().X()-x_extr, 2) + pow(sacHit->GetPosition().Y()-y_extr, 2) < 400)){
			det_energy[3] += sacHit->GetEnergy();
			tclst_x[3] += sacHit->GetPosition().X();
			tclst_y[3] += sacHit->GetPosition().Y();
			used_hits[3]++;
		}
	}
	if(det_energy[3]>sacCut){
		sac_det++;
		gsac = true;
	}

	//On IRC
	hit_nb = ircEvent->GetNHits();
	for(int m=0; m<hit_nb; m++){
		ircHit = (TIRCHit*)ircEvent->Hit(m);
		det_total_energy[0] += ircHit->GetEnergy();
		if((partTree.count(ircHit->GetMCTrackID())>0) || (pow(ircHit->GetPosition().X()-x_extr, 2) + pow(ircHit->GetPosition().Y()-y_extr, 2) < 400)){
			det_energy[0] += ircHit->GetEnergy();
			tclst_x[0] += ircHit->GetPosition().X();
			tclst_y[0] += ircHit->GetPosition().Y();
			used_hits[0]++;
		}
	}
	if(det_energy[0]>ircCut){
		irc_det++;
		girc = true;
	}

	if(girc || glav || glkr || gsac){
		//Gamma has been detected by at least one detector
		//Theta <<< 1
		int max_det = -1;
		double max_energy = 0;
		for(int i=0; i<4; i++){
			if(det_energy[i]>max_energy){
				max_det = i;
				max_energy = det_energy[i];
			}
		}
		det_nb = max_det;
		rec_energy = max_energy;
		clst_x = tclst_x[det_nb]/(double)used_hits[det_nb];
		clst_y = tclst_y[det_nb]/(double)used_hits[det_nb];
	}
	else{
		rec_energy = 0;
		if(rec_energy<det_energy[0]) rec_energy = det_energy[0];
		if(rec_energy<det_energy[1]) rec_energy = det_energy[1];
		if(rec_energy<det_energy[2]) rec_energy = det_energy[2];
		if(rec_energy<det_energy[3]) rec_energy = det_energy[3];
		//Theta too big or too small (gone in the beam pipe)
		//Theta >= 1
	}
	return det_id[det_nb];
}

TVector3 spec_propagate( const TVector3 StartPosition, const TVector3 StartMomentum, const Int_t fQ,
                      const Double_t fEndZ)
{
	/// \MemberDescr
	/// \param StartPosition : Initial position of the particle
	/// \param StartMomentum : InitialMomentum of the particle
	/// \param fQ : Charge of the particle
	/// \param fEndZ : End z coordinate of the track
	///
	/// ---------------- spec_propagate ------------------------------------------\n
	/// Analytic propagation of a charged particle. It takes into account the\n
	/// presence of the bending magnet of the Spectrometer if fZEnd is greater\n
	/// than the Z coordinate of the center of the spectrometer.\n
	/// \EndMemberDescr

	TVector3 EndPosition, EndMomentum;

    const Double_t fMagnetZLength = 1300;
    const Double_t fMagnetZPosition = 0.5*(196350+197650);      // magnet center position
    const Double_t fMagnetFieldStrength = 0.6928*1000;
    Double_t fEC = TMath::C() * 1.e-9 * 1.e-4 * 1.e-3;

    Double_t fStartX = StartPosition.X();
    Double_t fStartY = StartPosition.Y();
    Double_t fStartZ = StartPosition.Z();

    if ( StartMomentum.Z() == 0)
    {
        cerr << "[spec_propagate] Error : Perpendicular momentum." << endl;
        return -1;
    }

    Double_t fThetaX = StartMomentum.X() / StartMomentum.Z();
    Double_t fThetaY = StartMomentum.Y() / StartMomentum.Z();

    Double_t dMag = fMagnetZLength;
    Double_t zMag = fMagnetZPosition - dMag / 2.0 ;

    // fZEnd before magnet
    if (fEndZ <= zMag)
    {
        EndPosition.SetX( fStartX + fThetaX * (fEndZ - fStartZ) );
        EndPosition.SetY( fStartY + fThetaY * (fEndZ - fStartZ) );
        EndPosition.SetZ( fEndZ );
        return EndPosition;
    }

    // fZEnd after MNP33
    EndPosition.SetX( fStartX + fThetaX * (zMag - fStartZ) );
    EndPosition.SetY( fStartY + fThetaY * (zMag - fStartZ) );
    EndPosition.SetZ( zMag );

    TVector3 B, fP;
    B.SetXYZ( 0.0, fMagnetFieldStrength * 10000.0, 0.0 );
    fP = StartMomentum;

    Int_t qb = B.Y()>0 ? 1 : -1;

    Double_t rho = (fP.Cross(B)).Mag() / (fQ * fEC * B.Mag2() );
    Double_t delta = dMag / rho;
    Double_t sint = sin( atan(fThetaX) );
    Double_t cost = cos( atan(fThetaX) );
    Double_t dx = qb * rho * ( -cost + sqrt( 1 - (delta - qb*sint)*(delta - qb*sint) ) );
    EndPosition.SetX( EndPosition.X() + dx );
    EndPosition.SetY( EndPosition.Y() + fThetaY*dMag );
    EndPosition.SetZ( EndPosition.Z() + dMag );
    TVector3 TempPosition = EndPosition;
    Double_t fThetaXAfter = -qb * (delta - qb*sint) / sqrt( 1.0 - (delta - qb*sint)*(delta - qb*sint) );
    EndPosition.SetX( EndPosition.X() + fThetaXAfter * (fEndZ - EndPosition.Z()) );
    EndPosition.SetY( EndPosition.Y() + fThetaY * (fEndZ - EndPosition.Z()) );
    EndPosition.SetZ( fEndZ );

    EndMomentum = (EndPosition - TempPosition).Unit();

    return EndPosition;
}

set<int> BuildParticleSet(KinePart* init_Part, Event *runEvt){
	/// \MemberDescr
	/// \param init_Part :
	/// \param runEvt :
	///
	/// Build the particle decay tree of particle init_Part
	/// \EndMemberDescr

	set<int> st;

	KinePart *part;
	queue<KinePart*> daughter;
	queue<KinePart*> next_daughter;

	st.insert(init_Part->GetID());
	getDaughter(init_Part->GetID(), runEvt, daughter);
	int i = 0;
	while(daughter.size()>0 && i<2){
		while(daughter.size()>0){
			part = daughter.front();
			st.insert(part->GetID());
			getDaughter(part->GetID(), runEvt, next_daughter);
			daughter.pop();
		}
		daughter = next_daughter;
		while(next_daughter.size()>0){next_daughter.pop();}
	}

	return st;
}

DetectorID detection_straw(int part, TSpectrometerEvent* strEvent, double outer_radius[4], double center_x[4], double center_y[4], bool& straw1, bool& straw2, bool& straw3, bool& straw4){
	/// \MemberDescr
	/// \param part :
	/// \param strEvent :
	/// \param outer_radius :
	/// \param center_x :
	/// \param center_y :
	/// \param straw1 :
	/// \param straw2 :
	/// \param straw3 :
	/// \param straw4 :
	///
	/// Say if the particle has been detected in Spectrometer\n
	/// WARNING : Don't give good results neither
	/// \EndMemberDescr

	bool gstr = false;;

	TSpectrometerHit* strHit;

	int hit_nb;

	double x, y;


	bool planeID[4] = {false, false, false, false};

	//On Straw
	hit_nb = strEvent->GetNHits();
	for(int m=0; m<hit_nb; m++){
		strHit = (TSpectrometerHit*)strEvent->Hit(m);
		x = strHit->GetPosition().X();
		y = strHit->GetPosition().Y();
		if(pow(x-center_x[strHit->GetChamberID()],2)+pow(y-center_y[strHit->GetChamberID()],2)<pow(outer_radius[strHit->GetChamberID()], 2)){
			if(part == strHit->GetMCTrackID()){
				planeID[strHit->GetChamberID()] = true;
			}
		}
	}

	for(int m=0; m<4; m++){
		if(planeID[(m)%4] && planeID[(m+1)%4] && planeID[(m+2)%4]){ gstr = true; break;}
	}

	straw1 = planeID[0];
	straw2 = planeID[1];
	straw3 = planeID[2];
	straw4 = planeID[3];
	if(gstr) return Straw;
	else return Missed;
}

DetectorID detection_gtk(int part, TGigaTrackerEvent* gtkEvent, bool& gtk1){
	/// \MemberDescr
	/// \param part :
	/// \param gtkEvent :
	/// \param gtk1 :
	///
	/// Say if the particle has been detected in GTK\n
	/// This one gives pretty good results
	/// \EndMemberDescr

	bool ggtk = false;;

	TGigaTrackerHit* gtkHit;

	int hit_nb;

	bool planeID[3] = {false, false, false};

	//On GTK
	hit_nb = gtkEvent->GetNHits();
	for(int m=0; m<hit_nb; m++){
		gtkHit = (TGigaTrackerHit*)gtkEvent->Hit(m);
		if(part == gtkHit->GetMCTrackID()){
			planeID[gtkHit->GetStationID()] = true;
		}
	}

	if(planeID[0] && planeID[1] && planeID[2]) ggtk = true;
	gtk1 = planeID[0];

	if(ggtk) return GTK;
	else return Missed;
}

vector<KinePart*> getDaughter(int id, Event *runEvt){
	/// \MemberDescr
	/// \param id :
	/// \param runEvt :
	///
	/// Build a vector with the decay product of particle id
	/// \EndMemberDescr

	int i;
	vector<KinePart*> daughter;
	KinePart *part;

	for(i=0; i<runEvt->GetNKineParts(); i++){
		part = (KinePart*)runEvt->GetKineParts()->At(i);
		if((part->GetParentID()==id)){
#if USE_DIRECT==1
		if((part->GetDirectParent())){
#endif
#if USE_FIRST==1
		if((part->GetPDGcode()<5000)){
#endif
			daughter.push_back(part);
#if USE_DIRECT==1
		}
#endif
#if USE_FIRST==1
		}
#endif
		}
	}
	return daughter;
}

int getDaughter(int id, Event *runEvt, queue<KinePart*> &daughter){
	/// \MemberDescr
	/// \param id :
	/// \param runEvt :
	/// \param daughter :
	///
	/// Build a vector with the decay product of particle id into daughter and return the number of decay products.
	/// \EndMemberDescr

	int i;
	KinePart *part;
	int number = 0;

	for(i=0; i<runEvt->GetNKineParts(); i++){
		part = (KinePart*)runEvt->GetKineParts()->At(i);
		if((part->GetParentID()==id)){
#if USE_DIRECT==1
		if((part->GetDirectParent())){
#endif
#if USE_FIRST==1
		if((part->GetPDGcode()<5000)){
#endif
			daughter.push(part);
			number++;
#if USE_DIRECT==1
		}
#endif
#if USE_FIRST==1
		}
#endif
		}
	}
	return number;
}

int getDaughterNbr(int id, Event* runEvt){
	/// \MemberDescr
	/// \param id :
	/// \param runEvt :
	///
	/// Returns the number of decay products of particle id
	/// \EndMemberDescr

	int i;
	KinePart *part;
	int number = 0;

	for(i=0; i<runEvt->GetNKineParts(); i++){
		part = (KinePart*)runEvt->GetKineParts()->At(i);
		if((part->GetParentID()==id)){
#if USE_DIRECT==1
		if((part->GetDirectParent())){
#endif
#if USE_FIRST==1
		if((part->GetPDGcode()<5000)){
#endif
			number++;
#if USE_DIRECT==1
		}
#endif
#if USE_FIRST==1
		}
#endif
		}
	}
	return number;
}

double distance3D(TVector3 p1, TVector3 p2){
	/// \MemberDescr
	/// \param p1 : point 1
	/// \param p2 : point 2
	///
	/// Return the distance between two points in 3D space
	/// \EndMemberDescr

	return sqrt(pow(p1.X()-p2.X(), 2) + pow(p1.Y()-p2.Y(), 2) + pow(p1.Z()-p2.Z(), 2));
}

double distance2D(TVector3 p1, TVector3 p2, TString plane){
	/// \MemberDescr
	/// \param p1 : point 1
	/// \param p2 : point 2
	/// \param plane : plane on which the distance is computed
	///
	/// Return the distance between two points in 2D space
	/// \EndMemberDescr

	plane.ToUpper();
	if( (plane.CompareTo("XY")==0) || (plane.CompareTo("YX")==0) ) return sqrt(pow(p1.X()-p2.X(), 2) + pow(p1.Y()-p2.Y(), 2));
	else if( (plane.CompareTo("XZ")==0) || (plane.CompareTo("ZX")==0) ) return sqrt(pow(p1.X()-p2.X(), 2) + pow(p1.Z()-p2.Z(), 2));
	else if( (plane.CompareTo("YZ")==0) || (plane.CompareTo("ZY")==0) ) return sqrt(pow(p1.Y()-p2.Y(), 2) + pow(p1.Z()-p2.Z(), 2));
	else return -1;
}
