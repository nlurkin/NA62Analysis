/*
 * NA62Constants.hh
 *
 *  Created on: Aug 27, 2013
 *      Author: nlurkin
 */

#ifndef NA62CONSTANTS_HH_
#define NA62CONSTANTS_HH_

namespace NA62Constants {

  // Particle masses [MeV] according to PDG 2013

  static const double MKCH = 493.677;
  static const double MPI  = 139.57018;
  static const double MPI0 = 134.9766;
  static const double MEL  =   0.510998928;
  static const double MMU  = 105.6583715; 

  enum DetectorID {GTK, Straw, IRC, LAV, LKr, SAC, Missed};

  static const double cGTK3StartPos = 101800.; //mm
  static const double cLKrStartPos  = 240413.; //mm
}

#endif /* CONSTANTSREPOSITORY_HH_ */
