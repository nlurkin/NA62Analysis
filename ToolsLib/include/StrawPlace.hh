// ---------------------------------------------------------------
// History:
//
// Created by Dmitry Madigozhin (madigo@mail.cern.ch) 2015-06-02
//
// ---------------------------------------------------------------

#ifndef STRAW_PLACE_H
#define STRAW_PLACE_H
#include <iostream>

/// \class StrawPlace
/// \Brief
/// A convertor from the straw readout channel address to the straw geometric position.
/// Additional positioning data (a cell and a letter inside cell) are also provided.
/// \EndBrief
///
/// \Detailed
///\n
/// The constructor of this class requies a straw readout channel defined by the variables:\n
///\n
/// int isrb;       // SRB index (0-31)  == ichamber*8 + iview*2 + SecondSrb\n
/// int icover;     // cover index inside SRB (0-15), sometimes it was called "SRB channel"\n
/// int ichannel;   // straw channel index inside SRB cover (0-15)\n
///\n
/// As a result of constructor work, the following public variables of new object are filled in:\n
///\n
/// int ichamber; // Chamber number in the spectrometer along the beam\n
/// int iview;    // view index in the chamber along the beam (0-3) = (U,V,X,Y)\n
/// int ihalf;    // the half of the view (0-1) along the beam\n
/// int ilayer;   // layer in the half (0-1) along the beam\n
/// int istraw;   // straw index in the layer  (0-122), starting from the end of last (14-th) cell\n
/// int cell;     // cell index (0-14) along the cells arrow direction\n
/// int letter;   // letter index of the straw in the cell (0-7) along the cells arrow direction\n
/// bool NoStraw; // straw is absent or inactive;\n
///\n
/// If there is no any valid straw position related to the requested straw readout address, all
/// these public variables are set to -1.\n
///\n
/// If the position exists in principle, but the Spectrometer drawings show the absence of 
/// straw in that position or the straw is inactive, the boolean flag NoStraw is set to 
/// the 'true' value, otherwise it is 'false'.\n
/// \n
/// The non-negativeness of the resulting public variables and NoStraw value should be checked  
/// before the usage of straw position data.\n
///\n
/// \EndDetailed

class StrawPlace {

public:
  int ichamber; ///< chamber number in the spectrometer along the beam
  int iview;    ///< view index in the chamber along the beam (0-3) = (U,V,X,Y)
  int ihalf;    ///< half of the view (0-1) along the beam
  int ilayer;   ///< layer in the half (0-1) along the beam
  int istraw;   ///< straw index in the layer  (0-122), starting from the end of last (14-th) cell 
  int cell;     ///< cell index (0-14) along the cells arrow direction
  int letter;   ///< letter index of the straw in the cell (0-7) along the cells arrow direction
  bool NoStraw; ///< straw is absent or inactive;

  StrawPlace(int isrb, int icover, int ichannel);  
  ~StrawPlace() {}
};

#endif
