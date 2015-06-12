// ---------------------------------------------------------------
// History:
//
// Created by Dmitry Madigozhin (madigo@mail.cern.ch) 2015-06-02
//
// ---------------------------------------------------------------

#include "StrawPlace.hh"

StrawPlace::StrawPlace(int isrb, int icover, int ichannel) {

    ichamber = isrb/8; //    ichamber = (isrb & 0x18)>>3;

    iview = (isrb - ichamber*8)/2; //   iview = (isrb & 0x6)>>1;
    
    // ------  cell and half calculation

    int SecondSrb = (isrb & 0x1);

    if( (! SecondSrb) && ((icover==11)||(icover==15))){       // channel is not used
      ichamber=-1; iview = -1; ihalf = -1 ; ilayer = -1; istraw = -1; cell=-1; letter = -1;
      return;
    }else{
      ihalf = 1-((icover/4)%2); 
      cell = (icover%4)*2 -1;
      if(icover > 7){
	cell+=2;
      }else{
	cell+=1;
      }
      if(SecondSrb){cell+=7;}
    }

    // ------ the rest

    int j = ichannel;

    if(iview%2){
      if(ihalf){
	if(j == 0){ j = 1; }else if(j == 1){ j = 0;}
	letter = (15-j)/2;
      }else{
	if(j == 14){ j = 15; }else if(j == 15){ j = 14;}
	letter = j/2;
      }
      ilayer=j%2;
    }else{
      if(ihalf){
	if(j == 14){ j = 15; }else if(j == 15){ j = 14;}
	letter = j/2;
      }else{
	if(j == 0){ j = 1; }else if(j == 1){ j = 0;}
	letter = (15-j)/2;
      }
      ilayer=!(j%2);
    }
    
    // ------ istraw calculation

    istraw = (14-cell)*8 + (7 - letter);
    if(cell < 7){
      istraw++;
      if(iview < 3)istraw++;
    }

    //---------------- Check if the straw exists and active

    NoStraw=false;

    //---------------- edges
    if((iview == 0 || iview == 2)){
      if((cell == 0)&&(letter < 2))NoStraw=true;
      if((cell == 14)&&(letter > 6))NoStraw=true;
    }
    if(iview == 1){
      if((cell == 0) && (letter < 2))NoStraw=true;
      if((cell == 14) && (letter ==7))NoStraw=true;
    }
    if(iview == 3){
      if((cell == 14) && (letter > 5))NoStraw=true;
    }
    
    //-------------- hole
    int il = ihalf*2+ilayer;

    if(ichamber == 0){
      if(iview == 0){
	if(cell == 6){int m[4] = {5,4,4,5}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {3,2,2,3}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 1){
	if(cell == 6){int m[4] = {5,4,4,5}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {3,2,2,3}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 2){
	if(cell == 6){int m[4] = {3,3,2,3}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {1,1,0,1}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 3){
	if(cell == 6){int m[4] = {7,6,6,7}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {6,5,5,6}; if(letter < m[il])NoStraw=true;}
      }
    }else if(ichamber==1){
      if(iview == 0){
	if(cell == 6){int m[4] = {4,4,4,4}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {2,2,2,2}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 1){
	if(cell == 6){int m[4] = {4,4,4,4}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {2,2,2,2}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 2){
	if(cell == 6){int m[4] = {2,2,2,2}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {0,0,0,0}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 3){
	if(cell == 6){int m[4] = {7,6,6,7}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {6,5,5,6}; if(letter < m[il])NoStraw=true;}
      }
    }else if(ichamber==2){
      if(iview == 0){
	if(cell == 6){int m[4] = {5,5,4,5}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {3,3,2,3}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 1){
	if(cell == 6){int m[4] = {5,4,5,5}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {3,2,3,3}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 2){
	if(cell == 6){int m[4] = {4,3,3,3}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {2,1,1,1}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 3){
	if(cell == 6){int m[4] = {7,6,6,7}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {6,5,5,6}; if(letter < m[il])NoStraw=true;}
      }
    }else if(ichamber==3){
      if(iview == 0){
	if(cell == 6){int m[4] = {7,6,6,6}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {5,4,4,4}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 1){
	if(cell == 6){int m[4] = {6,6,6,7}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {4,4,4,5}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 2){
	if(cell == 6){int m[4] = {6,5,5,6}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {4,3,3,4}; if(letter < m[il])NoStraw=true;}
      }
      if(iview == 3){
	if(cell == 6){int m[4] = {7,6,6,7}; if(letter > m[il])NoStraw=true;}
	if(cell == 7){int m[4] = {6,5,5,6}; if(letter < m[il])NoStraw=true;}
      }
    }
    //-------------- ^ hole
    return;
}
