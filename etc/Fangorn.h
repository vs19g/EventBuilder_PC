#ifndef FANGORN_H
#define FANGORN_H

// ***** definitions *****
// Fmult, Bmult -- front/back multiplicity
// Fnum, Bnum -- front/back local channel (i.e. strip number)
// Fenergy, Benergy -- front/back energy
// FroughCal, BroughCal -- rough energy calibration for QQQs only (keV)
// rho, phi, z -- cylindrical coordinates at center of strip (degrees & mm)
// Ftime, Btime -- front/back time

// ***** structs hold members of each detector class *****
struct dataSX{ int Fmult; int Bmult; double Fenergy[8]; double Benergy[4]; int Fnum[8]; int Bnum[4]; float z[4]; float rho[4]; float phi[4]; double Btime[4]; double Ftime[8];};
struct dataQ{ int Fmult; int Bmult; double Fenergy[16]; double Benergy[16]; double FroughCal[16]; double BroughCal[16]; int Fnum[16]; int Bnum[16]; float z[16]; float rho[16]; float phi[16]; double Btime[16]; double Ftime[16];};
struct dataBarcUp{ int Fmult; double Fenergy[32]; int Fnum[32]; float z[32]; float rho[32]; float phi[32]; double Ftime[32];};
struct dataBarcDown{ int Fmult; double Fenergy[32]; int Fnum[32]; float z[32]; float rho[32]; float phi[32]; double Ftime[32];};

// ***** constants *****
const float m_BDZoffset = 262.8; // mm, edge of strip 0 (most downstream strip);
const float m_BUZoffset = 122.65; // mm, edge of strip 0 (most upstream strip)
const float m_SXZoffset = 201.4; // mm, edge of strip 0 (most upstream strip)
const float m_SXrho[12] = {89.0354, 89.0354, 89.0247, 89.0354, 89.0354, 89.0247,
                   89.0354, 89.0354, 89.0247, 89.0354, 88.9871, 89.0601}; // mm; from Gordon

float m_roughCal = 3.6; // keV/channel for QQQ only; rough calibration, same gain for all channels

// ***** channel map *****
// How to generate:
// awk '{printf $5 ","}' ANASEN_TRIUMFAug_run21+_ChannelMap.txt
// printf = don't put new line; $5 = 5th column, then put comma.
// note there were channels missing; they now read strip 100, because chan number = array element.
int m_lookUp[640]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
#endif