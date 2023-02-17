#ifndef RECONSTRUCT_H
#define RECONSTRUCT_H

// members of each detector type
struct dataDE{ uint Fmult; double Fenergy[32]; int Fnum[32]; float z[32]; float rho[32]; float phi[32]; double Ftime[32];};
struct dataE{ uint Fmult; uint Bmult; double Fenergy[16]; double Benergy[16]; double FroughCal[16]; double BroughCal[16]; int Fnum[16]; int Bnum[16]; float z[16]; float rho[16]; float phi[16]; double Btime[16]; double Ftime[16];};

// define some constants
float m_SXrho[12] = {89.0354, 89.0354, 89.0247, 89.0354, 89.0354, 89.0247,
                   89.0354, 89.0354, 89.0247, 89.0354, 88.9871, 89.0601}; // mm; from Gordon

float m_Brho = 30.0; // +/- 1mm, rho for barcs
float m_Qz = 296.; // mm, z for QQQ

#endif