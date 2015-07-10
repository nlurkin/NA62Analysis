// ---------------------------------------------------------------
// History:
//
// Created by Dmitry Madigozhin (madigo@mail.cern.ch) 2015-06-02
//
// ---------------------------------------------------------------

#ifndef STRAW_ADDRESS_H
#define STRAW_ADDRESS_H

/// \class StrawAddress
/// \Brief
/// A convertor from the straw geometric position to the straw readout channel address.
/// Additional geometrical positioning data (a cell and a letter inside cell) are also provided.
/// \EndBrief
///
/// \Detailed
///\n
/// The constructor of this class requies a straw geometric position defined by the variables:\n
///\n
///  int ichamber;   // Chamber number in the spectrometer along the beam\n
///  int iview;      // view index in the chamber along the beam (0-3) = (U,V,X,Y)\n
///  int ihalf;      // half of the view (0-1) along the beam\n
///  int ilayer;     // layer in the half (0-1) along the beam\n
///  int istraw;     // straw index in the layer  (0-122), starting from the end of last (14-th) cell\n
///\n
///  As a result of constructor work, the following public variables of new object are filled in:\n
///\n
///  int isrb;       // SRB index (0-31)  == ichamber*8 + iview*2 + SecondSrb\n
///  int icover;     // cover index inside SRB (0-15), sometimes it was called "SRB channel"\n
///  int ichannel;   // straw channel index inside SRB cover (0-15)\n
///  int cell;       // cell index (0-14) along the cells arrow direction\n
///  int letter;     // letter index of the straw in the cell (0-7) along the cells arrow direction\n
///\n
/// If there is no any valid readout channel connected to the requested straw position, all
/// these public variables become set to -1, that should be checked before their usage.\n
///\n 
/// \EndDetailed

class StrawAddress {

public:  
  int isrb;     ///< SRB index (0-31)  == ichamber*8 + iview*2 + SecondSrb
  int icover;   ///< cover index inside SRB (0-15), sometimes it was called "SRB channel"
  int ichannel; ///< straw channel index inside SRB cover (0-15)
  int cell;     ///< cell index (0-14) along the cells arrow direction
  int letter;   ///< letter index of the straw in the cell (0-7) along the cells arrow direction

  StrawAddress (int ichamber, int  iview, int ihalf,int ilayer,  int istraw);
  ~StrawAddress() {}
};

#endif
