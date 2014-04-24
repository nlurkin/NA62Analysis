// ---------------------------------------------------------------
// History:
//
// Created by Evgueni Goudzovski (eg@hep.ph.bham.ac.uk) 2014-04-16
//
// ---------------------------------------------------------------

#ifndef TwoLinesCDA_H
#define TwoLinesCDA_H

#include "TVector3.h"
#include <iostream>

/// \class TwoLinesCDA
/// \Brief
/// Computation of distance between two lines (useful for vertexing)
/// \EndBrief
/// \Detailed
/// Each of the two lines can be defined by either two points or a point and a direction vector.
/// The direction vector can be of any (non-zero) magnitude.
/// The distance between the two lines and the point of closest approach are computed
/// by the ComputeVertexCDA() methid.
/// \EndDetailed

class TwoLinesCDA {

public:

  TwoLinesCDA();
  ~TwoLinesCDA() {}

  void ComputeVertexCDA();
  void Print();

  void SetLine1Point1 (TVector3 val) { fL1p1 = val; SetDir1(fL1p2-fL1p1); }
  void SetLine1Point2 (TVector3 val) { fL1p2 = val; SetDir1(fL1p2-fL1p1); }
  void SetLine2Point1 (TVector3 val) { fL2p1 = val; SetDir2(fL2p2-fL2p1); }
  void SetLine2Point2 (TVector3 val) { fL2p2 = val; SetDir2(fL2p2-fL2p1); }
  void SetDir1        (TVector3 val) { fv1   = val; }
  void SetDir2        (TVector3 val) { fv2   = val; }

  void SetLine1Point1 (Double_t x, Double_t y, Double_t z);
  void SetLine1Point2 (Double_t x, Double_t y, Double_t z);
  void SetLine2Point1 (Double_t x, Double_t y, Double_t z);
  void SetLine2Point2 (Double_t x, Double_t y, Double_t z);
  void SetDir1        (Double_t x, Double_t y, Double_t z);
  void SetDir2        (Double_t x, Double_t y, Double_t z);

  TVector3 GetLine1Point1 () { return fL1p1; }
  TVector3 GetLine1Point2 () { return fL1p2; }
  TVector3 GetLine2Point1 () { return fL2p1; }
  TVector3 GetLine2Point2 () { return fL2p2; }
  TVector3 GetDir1()         { return fv1;   }
  TVector3 GetDir2()         { return fv2;   }

  TVector3 GetVertex()             { return fVertex; }
  void     SetVertex(TVector3 val) { fVertex = val;  }
  Double_t GetCDA()                { return fCDA;    }
  void     SetCDA(Double_t val)    { fCDA = val;     }

private:

  TVector3 fL1p1, fL1p2, fL2p1, fL2p2, fv1, fv2;
  TVector3 fVertex;
  Double_t fCDA;

};

#endif
