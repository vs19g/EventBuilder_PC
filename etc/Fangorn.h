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
float m_BDZoffset = 222.76; // mm, edge of strip 0 (most downstream strip)
float m_BUZoffset = 122.65; // mm, edge of strip 0 (most upstream strip)

float m_roughCal = 3.6; // keV/channel for QQQ only; rough calibration, same gain for all channels
#endif