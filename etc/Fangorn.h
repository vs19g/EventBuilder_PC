#ifndef Fangorn_h
#define Fangorn_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iomanip>
#include <iostream>


    int i, j;
    float BDZoffset = 222.76+40.; // mm, edge of strip 0 (most downstream strip) //40mm offset until measurement comes
    float BUZoffset = 122.65; // mm, edge of strip 0 (most upstream strip)
    float QQQzpos = 296.; // mm
    float SXZoffset = 161.;//mm
    int innermult, outermult;
    float inner_E[30], outer_E[30]; // no idea the size - this should be big enough though
    float inner_phi[30], outer_phi[30]; 

int QFmult, QBmult;
float QFenergy[40], QBenergy[40]; // chosen as an improbably high multiplicity
int QFdetnum[40], QBdetnum[40]; // detector number
float Qz[40], Qrho[40], Qphi[40];
int QFnum[40], QBnum[40];
long QFtime[40], QBtime[40];

bool ringHit, BUhit, BDhit;


int BUmult, BDmult;
float BUenergy[60], BDenergy[60];
int BUdetnum[60], BDdetnum[60];
int BUnum[60], BDnum[60];
long BUtime[60], BDtime[60];
float BUz[60], BUrho[60], BUphi[60];
float BDz[60], BDrho[60], BDphi[60];

int SXFmult, SXBmult;
float SXFenergy[50], SXBenergy[50];
int SXFdetnum[50], SXBdetnum[50];
int SXFnum[50], SXBnum[50];
long SXFtime[50], SXBtime[50];
float SXBz[50], SXrho[50], SXphi[50];
float SXFz[50];
bool up, down;
float Efrntup[50], Efrntdwn[50], Eback;
int upstrp, dwnstrp,upnum, downnum;
float Estrip, diff, Eratio, coeff;

int SXBch[48];
float SXBgain[48], SXBoffset[48];
//float SXBenergyMax; int SXBnumMax; int SXBdetMax;
//float QFenergyMax; int QFnumMax; int QFdetMax;
//float BUenergyMax; float BDenergyMax; int BUdetMax; int BDdetMax; int BUnumMax; int BDnumMax;
//float dE; float dEtheta; float dEphi; 
//float E; float Etheta; float Ephi;
#endif

// ***** output tree *****

// SX3 branches
// SX3 fronts will need to be dealt with in a different code if the front strips are a
// combination of the upstream and downstream ends... maybe.
// Then I'd also need a branch for position and a reconstruction.

// Will need coordinate branches and time
    
    // This is a condensed method but I can't figure out what happens for multiple detector hits, multiple strip hits in a detector, etc. I guess everything could be an array of 12?
   // outT->Branch("SX",&sx,"detnum/I:Fmult/I:Bmult/I:Fenergy/F:Benergy/F:Fnum/I:Bnum/I");
    // *********
    
/*outT->Branch("SX0Fmult",&dSX[0].Fmult,"SX0Fmult/i"); outT->Branch("SX0Bmult",&dSX[0].Bmult,"SX0Bmult/i");
outT->Branch("SX1Fmult",&dSX[1].Fmult,"SX1Fmult/i"); outT->Branch("SX1Bmult",&dSX[1].Bmult,"SX1Bmult/i");
outT->Branch("SX2Fmult",&dSX[2].Fmult,"SX2Fmult/i"); outT->Branch("SX2Bmult",&dSX[2].Bmult,"SX2Bmult/i");
outT->Branch("SX3Fmult",&dSX[3].Fmult,"SX3Fmult/i"); outT->Branch("SX3Bmult",&dSX[3].Bmult,"SX3Bmult/i");
outT->Branch("SX4Fmult",&dSX[4].Fmult,"SX4Fmult/i"); outT->Branch("SX4Bmult",&dSX[4].Bmult,"SX4Bmult/i");
outT->Branch("SX5Fmult",&dSX[5].Fmult,"SX5Fmult/i"); outT->Branch("SX5Bmult",&dSX[5].Bmult,"SX5Bmult/i");
outT->Branch("SX6Fmult",&dSX[6].Fmult,"SX6Fmult/i"); outT->Branch("SX6Bmult",&dSX[6].Bmult,"SX6Bmult/i");
outT->Branch("SX7Fmult",&dSX[7].Fmult,"SX7Fmult/i"); outT->Branch("SX7Bmult",&dSX[7].Bmult,"SX7Bmult/i");
outT->Branch("SX8Fmult",&dSX[8].Fmult,"SX8Fmult/i"); outT->Branch("SX8Bmult",&dSX[8].Bmult,"SX8Bmult/i");
outT->Branch("SX9Fmult",&dSX[9].Fmult,"SX9Fmult/i"); outT->Branch("SX9Bmult",&dSX[9].Bmult,"SX9Bmult/i");
outT->Branch("SX10Fmult",&dSX[10].Fmult,"SX10Fmult/i"); outT->Branch("SX10Bmult",&dSX[10].Bmult,"SX10Bmult/i");
outT->Branch("SX11Fmult",&dSX[11].Fmult,"SX11Fmult/i"); outT->Branch("SX11Bmult",&dSX[11].Bmult,"SX11Bmult/i");
*/
/*outT->Branch("SX0Fnum",dSX[0].Fnum,"SX0Fnum[SX0Fmult]/i");
outT->Branch("SX0Bnum",dSX[0].Bnum,"SX0Bnum[SX0Bmult]/i");
outT->Branch("SX0Fenergy",dSX[0].Fenergy,"SX0Fenergy[SX0Fmult]/f");
outT->Branch("SX0Benergy",dSX[0].Benergy,"SX0Benergy[SX0Bmult]/f");

outT->Branch("SX1Fnum",dSX[1].Fnum,"SX1Fnum[SX1Fmult]/i");
outT->Branch("SX1Bnum",dSX[1].Bnum,"SX1Bnum[SX1Bmult]/i");
outT->Branch("SX1Fenergy",dSX[1].Fenergy,"SX1Fenergy[SX1Fmult]/f");
outT->Branch("SX1Benergy",dSX[1].Benergy,"SX1Benergy[SX1Bmult]/f");

outT->Branch("SX2Fnum",dSX[2].Fnum,"SX2Fnum[SX2Fmult]/i");
outT->Branch("SX2Bnum",dSX[2].Bnum,"SX2Bnum[SX2Bmult]/i");
outT->Branch("SX2Fenergy",dSX[2].Fenergy,"SX2Fenergy[SX2Fmult]/f");
outT->Branch("SX2Benergy",dSX[2].Benergy,"SX2Benergy[SX2Bmult]/f");

outT->Branch("SX3Fnum",dSX[3].Fnum,"SX3Fnum[SX3Fmult]/i");
outT->Branch("SX3Bnum",dSX[3].Bnum,"SX3Bnum[SX3Bmult]/i");
outT->Branch("SX3Fenergy",dSX[3].Fenergy,"SX3Fenergy[SX3Fmult]/f");
outT->Branch("SX3Benergy",dSX[3].Benergy,"SX3Benergy[SX3Bmult]/f");

outT->Branch("SX4Fnum",dSX[4].Fnum,"SX4Fnum[SX4Fmult]/i");
outT->Branch("SX4Bnum",dSX[4].Bnum,"SX4Bnum[SX4Bmult]/i");
outT->Branch("SX4Fenergy",dSX[4].Fenergy,"SX4Fenergy[SX4Fmult]/f");
outT->Branch("SX4Benergy",dSX[4].Benergy,"SX4Benergy[SX4Bmult]/f");

outT->Branch("SX5Fnum",dSX[5].Fnum,"SX5Fnum[SX5Fmult]/i");
outT->Branch("SX5Bnum",dSX[5].Bnum,"SX5Bnum[SX5Bmult]/i");
outT->Branch("SX5Fenergy",dSX[5].Fenergy,"SX5Fenergy[SX5Fmult]/f");
outT->Branch("SX5Benergy",dSX[5].Benergy,"SX5Benergy[SX5Bmult]/f");

outT->Branch("SX6Fnum",dSX[6].Fnum,"SX6Fnum[SX6Fmult]/i");
outT->Branch("SX6Bnum",dSX[6].Bnum,"SX6Bnum[SX6Bmult]/i");
outT->Branch("SX6Fenergy",dSX[6].Fenergy,"SX6Fenergy[SX6Fmult]/f");
outT->Branch("SX6Benergy",dSX[6].Benergy,"SX6Benergy[SX6Bmult]/f");

outT->Branch("SX7Fnum",dSX[7].Fnum,"SX7Fnum[SX7Fmult]/i");
outT->Branch("SX7Bnum",dSX[7].Bnum,"SX7Bnum[SX7Bmult]/i");
outT->Branch("SX7Fenergy",dSX[7].Fenergy,"SX7Fenergy[SX7Fmult]/f");
outT->Branch("SX7Benergy",dSX[7].Benergy,"SX7Benergy[SX7Bmult]/f");

outT->Branch("SX8Fnum",dSX[8].Fnum,"SX8Fnum[SX8Fmult]/i");
outT->Branch("SX8Bnum",dSX[8].Bnum,"SX8Bnum[SX8Bmult]/i");
outT->Branch("SX8Fenergy",dSX[8].Fenergy,"SX8Fenergy[SX8Fmult]/f");
outT->Branch("SX8Benergy",dSX[8].Benergy,"SX8Benergy[SX8Bmult]/f");

outT->Branch("SX9Fnum",dSX[9].Fnum,"SX9Fnum[SX9Fmult]/i");
outT->Branch("SX9Bnum",dSX[9].Bnum,"SX9Bnum[SX9Bmult]/i");
outT->Branch("SX9Fenergy",dSX[9].Fenergy,"SX9Fenergy[SX9Fmult]/f");
outT->Branch("SX9Benergy",dSX[9].Benergy,"SX9Benergy[SX9Bmult]/f");

outT->Branch("SX10Fnum",dSX[10].Fnum,"SX10Fnum[SX10Fmult]/i");
outT->Branch("SX10Bnum",dSX[10].Bnum,"SX10Bnum[SX10Bmult]/i");
outT->Branch("SX10Fenergy",dSX[10].Fenergy,"SX10Fenergy[SX10Fmult]/f");
outT->Branch("SX10Benergy",dSX[10].Benergy,"SX10Benergy[SX10Bmult]/f");

outT->Branch("SX11Fnum",dSX[11].Fnum,"SX11Fnum[SX11Fmult]/i");
outT->Branch("SX11Bnum",dSX[11].Bnum,"SX11Bnum[SX11Bmult]/i");
outT->Branch("SX11Fenergy",dSX[11].Fenergy,"SX11Fenergy[SX11Fmult]/f");
outT->Branch("SX11Benergy",dSX[11].Benergy,"SX11Benergy[SX11Bmult]/f");


outT->Branch("SXBenergyMax",&SXBenergyMax,"SXBenergyMax/f"); // largest SX3 back energy
outT->Branch("SXBnumMax",&SXBnumMax,"SXBnumMax/i"); // ... and its strip number
outT->Branch("SXBdetMax",&SXBdetMax,"SXBdetMax/i"); // ... and its detector

// QQQ branches
outT->Branch("Q0Fmult",&dQ[0].Fmult,"Q0Fmult/i"); outT->Branch("Q0Bmult",&dQ[0].Bmult,"Q0Bmult/i");
//outT->Branch("Q0Fnum",dQ[0].Fnum,"Q0Fnum[Q0Fmult]/i"); outT->Branch("Q0Bnum",dQ[0].Bnum,"Q0Bnum[Q0Bmult]/i");
//outT->Branch("Q0Fenergy",dQ[0].Fenergy,"Q0Fenergy[Q0Fmult]/f"); outT->Branch("Q0Benergy",dQ[0].Benergy,"Q0Benergy[Q0Bmult]/f");

outT->Branch("Q1Fmult",&dQ[1].Fmult,"Q1Fmult/i"); outT->Branch("Q1Bmult",&dQ[1].Bmult,"Q1Bmult/i");
//outT->Branch("Q1Fnum",dQ[1].Fnum,"Q1Fnum[Q1Fmult]/i"); outT->Branch("Q1Bnum",dQ[1].Bnum,"Q1Bnum[Q1Bmult]/i");
//outT->Branch("Q1Fenergy",dQ[1].Fenergy,"Q1Fenergy[Q1Fmult]/f"); outT->Branch("Q1Benergy",dQ[1].Benergy,"Q1Benergy[Q1Bmult]/f");

outT->Branch("Q2Fmult",&dQ[2].Fmult,"Q2Fmult/i"); outT->Branch("Q2Bmult",&dQ[2].Bmult,"Q2Bmult/i");
//outT->Branch("Q2Fnum",dQ[2].Fnum,"Q2Fnum[Q2Fmult]/i"); outT->Branch("Q2Bnum",dQ[2].Bnum,"Q2Bnum[Q2Bmult]/i");
//outT->Branch("Q2Fenergy",dQ[2].Fenergy,"Q2Fenergy[Q2Fmult]/f"); outT->Branch("Q2Benergy",dQ[2].Benergy,"Q2Benergy[Q2Bmult]/f");

outT->Branch("Q3Fmult",&dQ[3].Fmult,"Q3Fmult/i"); outT->Branch("Q3Bmult",&dQ[3].Bmult,"Q3Bmult/i");
//outT->Branch("Q3Fnum",dQ[3].Fnum,"Q3Fnum[Q3Fmult]/i"); outT->Branch("Q3Bnum",dQ[3].Bnum,"Q3Bnum[Q3Bmult]/i");
//outT->Branch("Q3Fenergy",dQ[3].Fenergy,"Q3Fenergy[Q3Fmult]/f"); outT->Branch("Q3Benergy",dQ[3].Benergy,"Q3Benergy[Q3Bmult]/f");

outT->Branch("QFenergyMax",&QFenergyMax,"QFenergyMax/f");
outT->Branch("QFdetMax",&QFdetMax,"QFdetMax/i");
outT->Branch("QFnumMax",&QFnumMax,"QFnumMax/i");

// Barcelona upstream branches

outT->Branch("BU0Fmult",&dBU[0].Fmult,"BU0Fmult/i");
//outT->Branch("BU0Fnum",dBU[0].Fnum,"BU0Fnum[BU0Fmult]/i");
//outT->Branch("BU0Fenergy",dBU[0].Fenergy,"BU0Fenergy[BU0Fmult]/f");

outT->Branch("BU1Fmult",&dBU[1].Fmult,"BU1Fmult/i");
//outT->Branch("BU1Fnum",dBU[1].Fnum,"BU1Fnum[BU1Fmult]/i");
//outT->Branch("BU1Fenergy",dBU[1].Fenergy,"BU1Fenergy[BU1Fmult]/f");

outT->Branch("BU2Fmult",&dBU[2].Fmult,"BU2Fmult/i");
//outT->Branch("BU2Fnum",dBU[2].Fnum,"BU2Fnum[BU2Fmult]/i");
//outT->Branch("BU2Fenergy",dBU[2].Fenergy,"BU2Fenergy[BU2Fmult]/f");

outT->Branch("BU3Fmult",&dBU[3].Fmult,"BU3Fmult/i");
//outT->Branch("BU3Fnum",dBU[3].Fnum,"BU3Fnum[BU3Fmult]/i");
//outT->Branch("BU3Fenergy",dBU[3].Fenergy,"BU3Fenergy[BU3Fmult]/f");

outT->Branch("BU4Fmult",&dBU[4].Fmult,"BU4Fmult/i");
//outT->Branch("BU4Fnum",dBU[4].Fnum,"BU4Fnum[BU4Fmult]/i");
//outT->Branch("BU4Fenergy",dBU[4].Fenergy,"BU4Fenergy[BU4Fmult]/f");

outT->Branch("BU5Fmult",&dBU[5].Fmult,"BU5Fmult/i");
//outT->Branch("BU5Fnum",dBU[5].Fnum,"BU5Fnum[BU5Fmult]/i");
//outT->Branch("BU5Fenergy",dBU[5].Fenergy,"BU5Fenergy[BU5Fmult]/f");

// Barcelona downstream branches

outT->Branch("BD0Fmult",&dBD[0].Fmult,"BD0Fmult/i");
//outT->Branch("BD0Fnum",dBD[0].Fnum,"BD0Fnum[BD0Fmult]/i");
//outT->Branch("BD0Fenergy",dBD[0].Fenergy,"BD0Fenergy[BD0Fmult]/f");

outT->Branch("BD1Fmult",&dBD[1].Fmult,"BD1Fmult/i");
//outT->Branch("BD1Fnum",dBD[1].Fnum,"BD1Fnum[BD1Fmult]/i");
//outT->Branch("BD1Fenergy",dBD[1].Fenergy,"BD1Fenergy[BD1Fmult]/f");

outT->Branch("BD2Fmult",&dBD[2].Fmult,"BD2Fmult/i");
//outT->Branch("BD2Fnum",dBD[2].Fnum,"BD2Fnum[BD2Fmult]/i");
//outT->Branch("BD2Fenergy",dBD[2].Fenergy,"BD2Fenergy[BD2Fmult]/f");

outT->Branch("BD3Fmult",&dBD[3].Fmult,"BD3Fmult/i");
//outT->Branch("BD3Fnum",dBD[3].Fnum,"BD3Fnum[BD3Fmult]/i");
//outT->Branch("BD3Fenergy",dBD[3].Fenergy,"BD3Fenergy[BD3Fmult]/f");

outT->Branch("BD4Fmult",&dBD[4].Fmult,"BD4Fmult/i");
//outT->Branch("BD4Fnum",dBD[4].Fnum,"BD4Fnum[BD4Fmult]/i");
//outT->Branch("BD4Fenergy",dBD[4].Fenergy,"BD4Fenergy[BD4Fmult]/f");

outT->Branch("BD5Fmult",&dBD[5].Fmult,"BD5Fmult/i");
//outT->Branch("BD5Fnum",dBD[5].Fnum,"BD5Fnum[BD5Fmult]/i");
//outT->Branch("BD5Fenergy",dBD[5].Fenergy,"BD5Fenergy[BD5Fmult]/f");

outT->Branch("BUenergyMax",&BUenergyMax,"BUenergyMax/f"); outT->Branch("BDenergyMax",&BDenergyMax,"BDenergyMax/f");
outT->Branch("BUdetMax",&BUdetMax,"BUdetMax/i"); outT->Branch("BDdetMax",&BDdetMax,"BDdetMax/i");
outT->Branch("BUnumMax",&BUnumMax,"BUnumMax/i"); outT->Branch("BDnumMax",&BDnumMax,"BDnumMax/i");

// General branches (salute)
outT->Branch("dE",&dE,"dE/f"); // highest dE signal (from any barcelona detector, up or downstream)
outT->Branch("dEtheta",&dEtheta,"dEtheta/f"); // theta from highest dE
outT->Branch("dEphi",&dEphi,"dEphi/f"); // phi from highest dE
outT->Branch("E",&E,"E/f"); // highest E signal (from any QQQ, for now. )
outT->Branch("Etheta",&Etheta,"Etheta/f"); // theta from highest E
outT->Branch("Ephi",&Ephi,"Ephi/f"); // phi from highest E
*/
