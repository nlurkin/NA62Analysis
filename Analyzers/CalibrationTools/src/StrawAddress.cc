// ---------------------------------------------------------------
// History:
//
// Created by Dmitry Madigozhin (madigo@mail.cern.ch) 2015-06-02
//
// ---------------------------------------------------------------

#include "StrawAddress.hh"

StrawAddress::StrawAddress( int ichamber, int  iview, int ihalf,int ilayer,  int istraw) {
    
    int SecondSrb = (istraw<64);
    
    isrb = ichamber*8 + iview*2 + SecondSrb;
    
    if(iview == 3 && istraw == 121){
      cell = -1;
    }else if(istraw == 64){
      cell = -1;
    }else if(iview < 3 && istraw == 65){
      cell = -1;
    }else{
      if(istraw < 64){
	int n = 63-istraw;
	cell = 7 + n/8;
	letter = n%8;
      }else{
	int n = 121-istraw;
	if(iview == 3)n = 120-istraw;
	cell = n/8;
	letter = n%8;
      }
    }
    // --- cell & letter are ready

    if(cell == -1){
      letter = -1;
      isrb = -1;
      icover = -1;
      ichannel=-1;
    }else{
      
      int srbcover[15][2] = {{4,0},{12,8},{5,1},{13,9},{6,2},{14,10},{7,3},{4,0},{12,8},{5,1},{13,9},{6,2},{14,10},{7,3},{15,11}};
      
      icover = srbcover[cell][ihalf]; 
      
      if(ihalf){
	if(iview%2){
	  ichannel = (7-letter)*2 +ilayer;
	  if(ichannel == 0){
	    ichannel = 1;
	  }else if (ichannel == 1){
	    ichannel = 0;
	  }
	}else{
	  ichannel = letter*2 +(1-ilayer);
	  if(ichannel == 14){
	    ichannel = 15;
	  }else if (ichannel == 15){
	    ichannel = 14;
	  }
	}
      }else{
	if(iview%2){
	  ichannel = letter*2 +ilayer;
	  if(ichannel == 14){
	    ichannel = 15;
	  }else if (ichannel == 15){
	    ichannel = 14;
	  }
	}else{
	  ichannel = (7-letter)*2 +(1-ilayer);
	  if(ichannel == 0){
	    ichannel = 1;
	  }else if (ichannel == 1){
	    ichannel = 0;
	  }
	}
      }
    }
    return;
}
