/*DataStructs.h
 *Data structures for analysis. To be implemented as a dictionary for ROOT in LinkDef
 *Based on: FocalPlane_SABRE.h
 *Gordon M. Oct. 2019
 */
#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

using namespace std;

struct DPPChannel {
  Double_t Timestamp;
  Int_t Channel, Board, Energy, EnergyShort;
  Int_t Flags;
  vector<Int_t> Samples;
};

#endif
