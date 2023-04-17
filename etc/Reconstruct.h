#ifndef RECONSTRUCT_H
#define RECONSTRUCT_H

// ***** structs for dE and E *****
struct dataDE{ int Fmult; double Fenergy[32]; int Fnum[32]; float z[32]; float rho[32]; float phi[32]; double Ftime[32];};
struct dataE{ int Fmult; int Bmult; double Fenergy[16]; double Benergy[16]; double FroughCal[16]; double BroughCal[16]; int Fnum[16]; int Bnum[16]; float z[16]; float rho[16]; float phi[16]; double Btime[16]; double Ftime[16];};

// ***** constants *****
const float m_Brho = 30.0; // +/- 1mm, rho for barcs
const float m_Qz = 296.; // mm, z for QQQ

#endif