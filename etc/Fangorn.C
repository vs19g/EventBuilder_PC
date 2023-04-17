#include "Fangorn.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include "../src/evbdict/DataStructs.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

//.dylib or .so
R__LOAD_LIBRARY(../lib/libEVBDict.dylib);

void Fangorn(int runNumber){
std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/built/run_"+std::to_string(runNumber)+".root";
std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F.root";

std::cout<<"Processing data from "<<input_filename<<std::endl;

// ***** Input File *****
TFile* inputfile = TFile::Open(input_filename.c_str(), "READ");
if(inputfile == nullptr || !inputfile->IsOpen()){
	std::cout<<"Could not find file "<<input_filename<<". Skipping."<<std::endl;
	delete inputfile;
	return;
}
TTree* tree = (TTree*) inputfile->Get("SortTree");
if(tree == nullptr){
	std::cout<<"No tree named SortTree found inside file "<<input_filename<<". Skipping."<<std::endl;
	inputfile->Close();
	delete inputfile;
	return;
}

// ***** output file *****
TFile* outputfile = TFile::Open(output_filename.c_str(), "RECREATE");
TTree *outT = new TTree("BarcQQQTreeData","EventBuilder data with cylindrical coords");
std::cout<<"Writing to "<<output_filename<<std::endl;

// ***** initialize *****
dataQ dQ[4]; // number of each type of detector
dataSX dSX[12];
dataBarcUp dBU[6];
dataBarcDown dBD[6];

int QQQmult; int BarcMult; //including this to make pruning easier

// ***** output tree *****

// SX3 branches
// SX3 fronts will need to be dealt with in a different code if the front strips are a
// combination of the upstream and downstream ends... maybe. 
// Then I'd also need a branch for position and a reconstruction. 
/*
//outT->Branch("SX0Fmult",&dSX[0].Fmult,"SX0Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX0Fnum",&dSX[0].Fnum,"SX0Fnum[SX0Fmult]/I");
//outT->Branch("SX0Fenergy",&dSX[0].Fenergy,"SX0Fenergy[SX0Fmult]/D");
//outT->Branch("SX0z",&dSX[0].z,"SX0z[SX0Fmult]/F"); //z from calibrated fronts
outT->Branch("SX0Bmult",&dSX[0].Bmult,"SX0Bmult/I"); //backs
outT->Branch("SX0Bnum",&dSX[0].Bnum,"SX0Bnum[SX0Bmult]/I");
outT->Branch("SX0Benergy",&dSX[0].Benergy,"SX0Benergy[SX0Bmult]/D");
outT->Branch("SX0time",&dSX[0].Btime,"SX0time[SX0Bmult]/D"); //storing time from backs
outT->Branch("SX0phi",&dSX[0].phi,"SX0phi[SX0Bmult]/F"); //phi from backs
outT->Branch("SX0rho",&dSX[0].rho,"SX0rho[SX0Bmult]/F"); //rho from backs

//outT->Branch("SX1Fmult",&dSX[1].Fmult,"SX1Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX1Fnum",&dSX[1].Fnum,"SX1Fnum[SX1Fmult]/I");
//outT->Branch("SX1Fenergy",&dSX[1].Fenergy,"SX1Fenergy[SX1Fmult]/D");
//outT->Branch("SX1z",&dSX[1].z,"SX1z[SX1Fmult]/F"); //z from calibrated fronts
outT->Branch("SX1Bmult",&dSX[1].Bmult,"SX1Bmult/I"); //backs
outT->Branch("SX1Bnum",&dSX[1].Bnum,"SX1Bnum[SX1Bmult]/I");
outT->Branch("SX1Benergy",&dSX[1].Benergy,"SX1Benergy[SX1Bmult]/D");
outT->Branch("SX1time",&dSX[1].Btime,"SX1time[SX1Bmult]/D"); //storing time from backs
outT->Branch("SX1phi",&dSX[1].phi,"SX1phi[SX1Bmult]/F"); //phi from backs
outT->Branch("SX1rho",&dSX[1].rho,"SX1rho[SX1Bmult]/F"); //rho from backs

//outT->Branch("SX2Fmult",&dSX[2].Fmult,"SX2Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX2Fnum",&dSX[2].Fnum,"SX2Fnum[SX2Fmult]/I");
//outT->Branch("SX2Fenergy",&dSX[2].Fenergy,"SX2Fenergy[SX2Fmult]/D");
//outT->Branch("SX2z",&dSX[2].z,"SX2z[SX2Fmult]/F"); //z from calibrated fronts
outT->Branch("SX2Bmult",&dSX[2].Bmult,"SX2Bmult/I"); //backs
outT->Branch("SX2Bnum",&dSX[2].Bnum,"SX2Bnum[SX2Bmult]/I");
outT->Branch("SX2Benergy",&dSX[2].Benergy,"SX2Benergy[SX2Bmult]/D");
outT->Branch("SX2time",&dSX[2].Btime,"SX2time[SX2Bmult]/D"); //storing time from backs
outT->Branch("SX2phi",&dSX[2].phi,"SX2phi[SX2Bmult]/F"); //phi from backs
outT->Branch("SX2rho",&dSX[2].rho,"SX2rho[SX2Bmult]/F"); //rho from backs

//outT->Branch("SX3Fmult",&dSX[3].Fmult,"SX3Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX3Fnum",&dSX[3].Fnum,"SX3Fnum[SX3Fmult]/I");
//outT->Branch("SX3Fenergy",&dSX[3].Fenergy,"SX3Fenergy[SX3Fmult]/D");
//outT->Branch("SX3z",&dSX[3].z,"SX3z[SX3Fmult]/F"); //z from calibrated fronts
outT->Branch("SX3Bmult",&dSX[3].Bmult,"SX3Bmult/I"); //backs
outT->Branch("SX3Bnum",&dSX[3].Bnum,"SX3Bnum[SX3Bmult]/I");
outT->Branch("SX3Benergy",&dSX[3].Benergy,"SX3Benergy[SX3Bmult]/D");
outT->Branch("SX3time",&dSX[3].Btime,"SX3time[SX3Bmult]/D"); //storing time from backs
outT->Branch("SX3phi",&dSX[3].phi,"SX3phi[SX3Bmult]/F"); //phi from backs
outT->Branch("SX3rho",&dSX[3].rho,"SX3rho[SX3Bmult]/F"); //rho from backs

//outT->Branch("SX4Fmult",&dSX[4].Fmult,"SX4Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX4Fnum",&dSX[4].Fnum,"SX4Fnum[SX4Fmult]/I");
//outT->Branch("SX4Fenergy",&dSX[4].Fenergy,"SX4Fenergy[SX4Fmult]/D");
//outT->Branch("SX4z",&dSX[4].z,"SX4z[SX4Fmult]/F"); //z from calibrated fronts
outT->Branch("SX4Bmult",&dSX[4].Bmult,"SX4Bmult/I"); //backs
outT->Branch("SX4Bnum",&dSX[4].Bnum,"SX4Bnum[SX4Bmult]/I");
outT->Branch("SX4Benergy",&dSX[4].Benergy,"SX4Benergy[SX4Bmult]/D");
outT->Branch("SX4time",&dSX[4].Btime,"SX4time[SX4Bmult]/D"); //storing time from backs
outT->Branch("SX4phi",&dSX[4].phi,"SX4phi[SX4Bmult]/F"); //phi from backs
outT->Branch("SX4rho",&dSX[4].rho,"SX4rho[SX4Bmult]/F"); //rho from backs

//outT->Branch("SX5Fmult",&dSX[5].Fmult,"SX5Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX5Fnum",&dSX[5].Fnum,"SX5Fnum[SX5Fmult]/I");
//outT->Branch("SX5Fenergy",&dSX[5].Fenergy,"SX5Fenergy[SX5Fmult]/D");
//outT->Branch("SX5z",&dSX[5].z,"SX5z[SX5Fmult]/F"); //z from calibrated fronts
outT->Branch("SX5Bmult",&dSX[5].Bmult,"SX5Bmult/I"); //backs
outT->Branch("SX5Bnum",&dSX[5].Bnum,"SX5Bnum[SX5Bmult]/I");
outT->Branch("SX5Benergy",&dSX[5].Benergy,"SX5Benergy[SX5Bmult]/D");
outT->Branch("SX5time",&dSX[5].Btime,"SX5time[SX5Bmult]/D"); //storing time from backs
outT->Branch("SX5phi",&dSX[5].phi,"SX5phi[SX5Bmult]/F"); //phi from backs
outT->Branch("SX5rho",&dSX[5].rho,"SX5rho[SX5Bmult]/F"); //rho from backs

//outT->Branch("SX6Fmult",&dSX[6].Fmult,"SX6Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX6Fnum",&dSX[6].Fnum,"SX6Fnum[SX6Fmult]/I");
//outT->Branch("SX6Fenergy",&dSX[6].Fenergy,"SX6Fenergy[SX6Fmult]/D");
//outT->Branch("SX6z",&dSX[6].z,"SX6z[SX6Fmult]/F"); //z from calibrated fronts
outT->Branch("SX6Bmult",&dSX[6].Bmult,"SX6Bmult/I"); //backs
outT->Branch("SX6Bnum",&dSX[6].Bnum,"SX6Bnum[SX6Bmult]/I");
outT->Branch("SX6Benergy",&dSX[6].Benergy,"SX6Benergy[SX6Bmult]/D");
outT->Branch("SX6time",&dSX[6].Btime,"SX6time[SX6Bmult]/D"); //storing time from backs
outT->Branch("SX6phi",&dSX[6].phi,"SX6phi[SX6Bmult]/F"); //phi from backs
outT->Branch("SX6rho",&dSX[6].rho,"SX6rho[SX6Bmult]/F"); //rho from backs

//outT->Branch("SX7Fmult",&dSX[7].Fmult,"SX7Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX7Fnum",&dSX[7].Fnum,"SX7Fnum[SX7Fmult]/I");
//outT->Branch("SX7Fenergy",&dSX[7].Fenergy,"SX7Fenergy[SX7Fmult]/D");
//outT->Branch("SX7z",&dSX[7].z,"SX7z[SX7Fmult]/F"); //z from calibrated fronts
outT->Branch("SX7Bmult",&dSX[7].Bmult,"SX7Bmult/I"); //backs
outT->Branch("SX7Bnum",&dSX[7].Bnum,"SX7Bnum[SX7Bmult]/I");
outT->Branch("SX7Benergy",&dSX[7].Benergy,"SX7Benergy[SX7Bmult]/D");
outT->Branch("SX7time",&dSX[7].Btime,"SX7time[SX7Bmult]/D"); //storing time from backs
outT->Branch("SX7phi",&dSX[7].phi,"SX7phi[SX7Bmult]/F"); //phi from backs
outT->Branch("SX7rho",&dSX[7].rho,"SX7rho[SX7Bmult]/F"); //rho from backs

//outT->Branch("SX8Fmult",&dSX[8].Fmult,"SX8Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX8Fnum",&dSX[8].Fnum,"SX8Fnum[SX8Fmult]/I");
//outT->Branch("SX8Fenergy",&dSX[8].Fenergy,"SX8Fenergy[SX8Fmult]/D");
//outT->Branch("SX8z",&dSX[8].z,"SX8z[SX8Fmult]/F"); //z from calibrated fronts
outT->Branch("SX8Bmult",&dSX[8].Bmult,"SX8Bmult/I"); //backs
outT->Branch("SX8Bnum",&dSX[8].Bnum,"SX8Bnum[SX8Bmult]/I");
outT->Branch("SX8Benergy",&dSX[8].Benergy,"SX8Benergy[SX8Bmult]/D");
outT->Branch("SX8time",&dSX[8].Btime,"SX8time[SX8Bmult]/D"); //storing time from backs
outT->Branch("SX8phi",&dSX[8].phi,"SX8phi[SX8Bmult]/F"); //phi from backs
outT->Branch("SX8rho",&dSX[8].rho,"SX8rho[SX8Bmult]/F"); //rho from backs

//outT->Branch("SX9Fmult",&dSX[9].Fmult,"SX9Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX9Fnum",&dSX[9].Fnum,"SX9Fnum[SX9Fmult]/I");
//outT->Branch("SX9Fenergy",&dSX[9].Fenergy,"SX9Fenergy[SX9Fmult]/D");
//outT->Branch("SX9z",&dSX[9].z,"SX9z[SX9Fmult]/F"); //z from calibrated fronts
outT->Branch("SX9Bmult",&dSX[9].Bmult,"SX9Bmult/I"); //backs
outT->Branch("SX9Bnum",&dSX[9].Bnum,"SX9Bnum[SX9Bmult]/I");
outT->Branch("SX9Benergy",&dSX[9].Benergy,"SX9Benergy[SX9Bmult]/D");
outT->Branch("SX9time",&dSX[9].Btime,"SX9time[SX9Bmult]/D"); //storing time from backs
outT->Branch("SX9phi",&dSX[9].phi,"SX9phi[SX9Bmult]/F"); //phi from backs
outT->Branch("SX9rho",&dSX[9].rho,"SX9rho[SX9Bmult]/F"); //rho from backs

//outT->Branch("SX10Fmult",&dSX[10].Fmult,"SX10Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX10Fnum",&dSX[10].Fnum,"SX10Fnum[SX10Fmult]/I");
//outT->Branch("SX10Fenergy",&dSX[10].Fenergy,"SX10Fenergy[SX10Fmult]/D");
//outT->Branch("SX10z",&dSX[10].z,"SX10z[SX10Fmult]/F"); //z from calibrated fronts
outT->Branch("SX10Bmult",&dSX[10].Bmult,"SX10Bmult/I"); //backs
outT->Branch("SX10Bnum",&dSX[10].Bnum,"SX10Bnum[SX10Bmult]/I");
outT->Branch("SX10Benergy",&dSX[10].Benergy,"SX10Benergy[SX10Bmult]/D");
outT->Branch("SX10time",&dSX[10].Btime,"SX10time[SX10Bmult]/D"); //storing time from backs
outT->Branch("SX10phi",&dSX[10].phi,"SX10phi[SX10Bmult]/F"); //phi from backs
outT->Branch("SX10rho",&dSX[10].rho,"SX10rho[SX10Bmult]/F"); //rho from backs

//outT->Branch("SX11Fmult",&dSX[11].Fmult,"SX11Fmult/I"); //fronts (ignoring for now)
//outT->Branch("SX11Fnum",&dSX[11].Fnum,"SX11Fnum[SX11Fmult]/I");
//outT->Branch("SX11Fenergy",&dSX[11].Fenergy,"SX11Fenergy[SX11Fmult]/D");
//outT->Branch("SX11z",&dSX[11].z,"SX11z[SX11Fmult]/F"); //z from calibrated fronts
outT->Branch("SX11Bmult",&dSX[11].Bmult,"SX11Bmult/I"); //backs
outT->Branch("SX11Bnum",&dSX[11].Bnum,"SX11Bnum[SX11Bmult]/I");
outT->Branch("SX11Benergy",&dSX[11].Benergy,"SX11Benergy[SX11Bmult]/D");
outT->Branch("SX11time",&dSX[11].Btime,"SX11time[SX11Bmult]/D"); //storing time from backs
outT->Branch("SX11phi",&dSX[11].phi,"SX11phi[SX11Bmult]/F"); //phi from backs
outT->Branch("SX11rho",&dSX[11].rho,"SX11rho[SX11Bmult]/F"); //rho from backs
*/

// QQQ branches

outT->Branch("QQQmult",&QQQmult,"QQQmult/I"); //store overall QQQ front multiplicity (for pruning)

outT->Branch("Q0Fmult",&dQ[0].Fmult,"Q0Fmult/I"); outT->Branch("Q0Bmult",&dQ[0].Bmult,"Q0Bmult/I");
outT->Branch("Q0Fnum",&dQ[0].Fnum,"Q0Fnum[Q0Fmult]/I"); outT->Branch("Q0Bnum",&dQ[0].Bnum,"Q0Bnum[Q0Bmult]/I");
outT->Branch("Q0Fenergy",&dQ[0].Fenergy,"Q0Fenergy[Q0Fmult]/D"); outT->Branch("Q0Benergy",&dQ[0].Benergy,"Q0Benergy[Q0Bmult]/D");
outT->Branch("Q0FroughCal",&dQ[0].FroughCal,"Q0FroughCal[Q0Fmult]/D"); outT->Branch("Q0BroughCal",&dQ[0].BroughCal,"Q0BroughCal[Q0Bmult]/D");
outT->Branch("Q0Btime",&dQ[0].Btime,"Q0Btime[Q0Bmult]/D"); //storing time from backs
outT->Branch("Q0Ftime",&dQ[0].Ftime,"Q0Ftime[Q0Fmult]/D"); //storing time from fronts
outT->Branch("Q0rho",&dQ[0].rho,"Q0rho[Q0Fmult]/F"); //rho from fronts
outT->Branch("Q0phi",&dQ[0].phi,"Q0phi[Q0Bmult]/F"); //phi from backs

outT->Branch("Q1Fmult",&dQ[1].Fmult,"Q1Fmult/I"); outT->Branch("Q1Bmult",&dQ[1].Bmult,"Q1Bmult/I");
outT->Branch("Q1Fnum",&dQ[1].Fnum,"Q1Fnum[Q1Fmult]/I"); outT->Branch("Q1Bnum",&dQ[1].Bnum,"Q1Bnum[Q1Bmult]/I");
outT->Branch("Q1Fenergy",&dQ[1].Fenergy,"Q1Fenergy[Q1Fmult]/D"); outT->Branch("Q1Benergy",&dQ[1].Benergy,"Q1Benergy[Q1Bmult]/D");
outT->Branch("Q1FroughCal",&dQ[1].FroughCal,"Q1FroughCal[Q1Fmult]/D"); outT->Branch("Q1BroughCal",&dQ[1].BroughCal,"Q1BroughCal[Q1Bmult]/D");
outT->Branch("Q1Btime",&dQ[1].Btime,"Q1Btime[Q1Bmult]/D"); //storing time from backs
outT->Branch("Q1Ftime",&dQ[1].Ftime,"Q1Ftime[Q1Fmult]/D"); //storing time from fronts
outT->Branch("Q1rho",&dQ[1].rho,"Q1rho[Q1Fmult]/F"); //rho from fronts
outT->Branch("Q1phi",&dQ[1].phi,"Q1phi[Q1Bmult]/F"); //phi from backs

outT->Branch("Q2Fmult",&dQ[2].Fmult,"Q2Fmult/I"); outT->Branch("Q2Bmult",&dQ[2].Bmult,"Q2Bmult/I");
outT->Branch("Q2Fnum",&dQ[2].Fnum,"Q2Fnum[Q2Fmult]/I"); outT->Branch("Q2Bnum",&dQ[2].Bnum,"Q2Bnum[Q2Bmult]/I");
outT->Branch("Q2Fenergy",&dQ[2].Fenergy,"Q2Fenergy[Q2Fmult]/D"); outT->Branch("Q2Benergy",&dQ[2].Benergy,"Q2Benergy[Q2Bmult]/D");
outT->Branch("Q2FroughCal",&dQ[2].FroughCal,"Q2FroughCal[Q2Fmult]/D"); outT->Branch("Q2BroughCal",&dQ[2].BroughCal,"Q2BroughCal[Q2Bmult]/D");
outT->Branch("Q2Btime",&dQ[2].Btime,"Q2Btime[Q2Bmult]/D"); //storing time from backs
outT->Branch("Q2Ftime",&dQ[2].Ftime,"Q2Ftime[Q2Fmult]/D"); //storing time from fronts
outT->Branch("Q2rho",&dQ[2].rho,"Q2rho[Q2Fmult]/F"); //rho from fronts
outT->Branch("Q2phi",&dQ[2].phi,"Q2phi[Q2Bmult]/F"); //phi from backs

outT->Branch("Q3Fmult",&dQ[3].Fmult,"Q3Fmult/I"); outT->Branch("Q3Bmult",&dQ[3].Bmult,"Q3Bmult/I");
outT->Branch("Q3Fnum",&dQ[3].Fnum,"Q3Fnum[Q3Fmult]/I"); outT->Branch("Q3Bnum",&dQ[3].Bnum,"Q3Bnum[Q3Bmult]/I");
outT->Branch("Q3Fenergy",&dQ[3].Fenergy,"Q3Fenergy[Q3Fmult]/D"); outT->Branch("Q3Benergy",&dQ[3].Benergy,"Q3Benergy[Q3Bmult]/D");
outT->Branch("Q3FroughCal",&dQ[3].FroughCal,"Q3FroughCal[Q3Fmult]/D"); outT->Branch("Q3BroughCal",&dQ[3].BroughCal,"Q3BroughCal[Q3Bmult]/D");
outT->Branch("Q3Btime",&dQ[3].Btime,"Q3Btime[Q3Bmult]/D"); //storing time from backs
outT->Branch("Q3Ftime",&dQ[3].Ftime,"Q3Ftime[Q3Fmult]/D"); //storing time from fronts
outT->Branch("Q3rho",&dQ[3].rho,"Q3rho[Q3Fmult]/F"); //rho from fronts
outT->Branch("Q3phi",&dQ[3].phi,"Q3phi[Q3Bmult]/F"); //phi from backs


// Barcelona upstream branches

outT->Branch("BarcMult",&BarcMult,"BarcMult/I"); //store overall Barc multiplicity (for pruning)

outT->Branch("BU0Fmult",&dBU[0].Fmult,"BU0Fmult/I");
outT->Branch("BU0Fnum",&dBU[0].Fnum,"BU0Fnum[BU0Fmult]/I");
outT->Branch("BU0Fenergy",&dBU[0].Fenergy,"BU0Fenergy[BU0Fmult]/D");
outT->Branch("BU0time",&dBU[0].Ftime,"BU0time[BU0Fmult]/D"); //storing time from fronts
outT->Branch("BU0phi",&dBU[0].phi,"BU0phi[BU0Fmult]/F"); //phi from barc #
outT->Branch("BU0z",&dBU[0].z,"BU0z[BU0Fmult]/F"); //z from fronts

outT->Branch("BU1Fmult",&dBU[1].Fmult,"BU1Fmult/I");
outT->Branch("BU1Fnum",&dBU[1].Fnum,"BU1Fnum[BU1Fmult]/I");
outT->Branch("BU1Fenergy",&dBU[1].Fenergy,"BU1Fenergy[BU1Fmult]/D");
outT->Branch("BU1time",&dBU[1].Ftime,"BU1time[BU1Fmult]/D"); //storing time from fronts
outT->Branch("BU1phi",&dBU[1].phi,"BU1phi[BU1Fmult]/F"); //phi from barc #
outT->Branch("BU1z",&dBU[1].z,"BU1z[BU1Fmult]/F"); //z from fronts

outT->Branch("BU2Fmult",&dBU[2].Fmult,"BU2Fmult/I");
outT->Branch("BU2Fnum",&dBU[2].Fnum,"BU2Fnum[BU2Fmult]/I");
outT->Branch("BU2Fenergy",&dBU[2].Fenergy,"BU2Fenergy[BU2Fmult]/D");
outT->Branch("BU2time",&dBU[2].Ftime,"BU2time[BU2Fmult]/D"); //storing time from fronts
outT->Branch("BU2phi",&dBU[2].phi,"BU2phi[BU2Fmult]/F"); //phi from barc #
outT->Branch("BU2z",&dBU[2].z,"BU2z[BU2Fmult]/F"); //z from fronts

outT->Branch("BU3Fmult",&dBU[3].Fmult,"BU3Fmult/I");
outT->Branch("BU3Fnum",&dBU[3].Fnum,"BU3Fnum[BU3Fmult]/I");
outT->Branch("BU3Fenergy",&dBU[3].Fenergy,"BU3Fenergy[BU3Fmult]/D");
outT->Branch("BU3time",&dBU[3].Ftime,"BU3time[BU3Fmult]/D"); //storing time from fronts
outT->Branch("BU3phi",&dBU[3].phi,"BU3phi[BU3Fmult]/F"); //phi from barc #
outT->Branch("BU3z",&dBU[3].z,"BU3z[BU3Fmult]/F"); //z from fronts

outT->Branch("BU4Fmult",&dBU[4].Fmult,"BU4Fmult/I");
outT->Branch("BU4Fnum",&dBU[4].Fnum,"BU4Fnum[BU4Fmult]/I");
outT->Branch("BU4Fenergy",&dBU[4].Fenergy,"BU4Fenergy[BU4Fmult]/D");
outT->Branch("BU4time",&dBU[4].Ftime,"BU4time[BU4Fmult]/D"); //storing time from fronts
outT->Branch("BU4phi",&dBU[4].phi,"BU4phi[BU4Fmult]/F"); //phi from barc #
outT->Branch("BU4z",&dBU[4].z,"BU4z[BU4Fmult]/F"); //z from fronts

outT->Branch("BU5Fmult",&dBU[5].Fmult,"BU5Fmult/I");
outT->Branch("BU5Fnum",&dBU[5].Fnum,"BU5Fnum[BU5Fmult]/I");
outT->Branch("BU5energy",&dBU[5].Fenergy,"BU5Fenergy[BU5Fmult]/D");
outT->Branch("BU5time",&dBU[5].Ftime,"BU5time[BU5Fmult]/D"); //storing time from fronts
outT->Branch("BU5phi",&dBU[5].phi,"BU5phi[BU5Fmult]/F"); //phi from barc #
outT->Branch("BU5z",&dBU[5].z,"BU5z[BU5Fmult]/F"); //z from fronts

// Barcelona downstream branches

outT->Branch("BD0Fmult",&dBD[0].Fmult,"BD0Fmult/I");
outT->Branch("BD0Fnum",&dBD[0].Fnum,"BD0Fnum[BD0Fmult]/I");
outT->Branch("BD0Fenergy",&dBD[0].Fenergy,"BD0Fenergy[BD0Fmult]/D");
outT->Branch("BD0time",&dBD[0].Ftime,"BD0time[BD0Fmult]/D"); //storing time from fronts
outT->Branch("BD0phi",&dBD[0].phi,"BD0phi[BD0Fmult]/F"); //phi from barc #
outT->Branch("BD0z",&dBD[0].z,"BD0z[BD0Fmult]/F"); //z from fronts

outT->Branch("BD1Fmult",&dBD[1].Fmult,"BD1Fmult/I");
outT->Branch("BD1Fnum",&dBD[1].Fnum,"BD1Fnum[BD1Fmult]/I");
outT->Branch("BD1Fenergy",&dBD[1].Fenergy,"BD1Fenergy[BD1Fmult]/D");
outT->Branch("BD1time",&dBD[1].Ftime,"BD1time[BD1Fmult]/D"); //storing time from fronts
outT->Branch("BD1phi",&dBD[1].phi,"BD1phi[BD1Fmult]/F"); //phi from barc #
outT->Branch("BD1z",&dBD[1].z,"BD1z[BD1Fmult]/F"); //z from fronts

outT->Branch("BD2Fmult",&dBD[2].Fmult,"BD2Fmult/I");
outT->Branch("BD2Fnum",&dBD[2].Fnum,"BD2Fnum[BD2Fmult]/I");
outT->Branch("BD2Fenergy",&dBD[2].Fenergy,"BD2Fenergy[BD2Fmult]/D");
outT->Branch("BD2time",&dBD[2].Ftime,"BD2time[BD2Fmult]/D"); //storing time from fronts
outT->Branch("BD2phi",&dBD[2].phi,"BD2phi[BD2Fmult]/F"); //phi from barc #
outT->Branch("BD2z",&dBD[2].z,"BD2z[BD2Fmult]/F"); //z from fronts

outT->Branch("BD3Fmult",&dBD[3].Fmult,"BD3Fmult/I");
outT->Branch("BD3Fnum",&dBD[3].Fnum,"BD3Fnum[BD3Fmult]/I");
outT->Branch("BD3Fenergy",&dBD[3].Fenergy,"BD3Fenergy[BD3Fmult]/D");
outT->Branch("BD3time",&dBD[3].Ftime,"BD3time[BD3Fmult]/D"); //storing time from fronts
outT->Branch("BD3phi",&dBD[3].phi,"BD3phi[BD3Fmult]/F"); //phi from barc #
outT->Branch("BD3z",&dBD[3].z,"BD3z[BD3Fmult]/F"); //z from fronts

outT->Branch("BD4Fmult",&dBD[4].Fmult,"BD4Fmult/I");
outT->Branch("BD4Fnum",&dBD[4].Fnum,"BD4Fnum[BD4Fmult]/I");
outT->Branch("BD4Fenergy",&dBD[4].Fenergy,"BD4Fenergy[BD4Fmult]/D");
outT->Branch("BD4time",&dBD[4].Ftime,"BD4time[BD4Fmult]/D"); //storing time from fronts
outT->Branch("BD4phi",&dBD[4].phi,"BD4phi[BD4Fmult]/F"); //phi from barc #
outT->Branch("BD4z",&dBD[4].z,"BD4z[BD4Fmult]/F"); //z from fronts

outT->Branch("BD5Fmult",&dBD[5].Fmult,"BD5Fmult/I");
outT->Branch("BD5Fnum",&dBD[5].Fnum,"BD5Fnum[BD5Fmult]/I");
outT->Branch("BD5Fenergy",&dBD[5].Fenergy,"BD5Fenergy[BD5Fmult]/D");
outT->Branch("BD5time",&dBD[5].Ftime,"BD5time[BD5Fmult]/D"); //storing time from fronts
outT->Branch("BD5phi",&dBD[5].phi,"BD5phi[BD5Fmult]/F"); //phi from barc #
outT->Branch("BD5z",&dBD[5].z,"BD5z[BD5Fmult]/F"); //z from fronts

/**************************************************************************************/
CoincEvent* event = new CoincEvent();	
tree->SetBranchAddress("event", &event);

ULong64_t nevents = tree->GetEntries();
//ULong64_t nevents = 50; //for testing only
ULong64_t jentry;

for (jentry=0; jentry<nevents; jentry++){
    tree->GetEntry(jentry);
	
/**************************************************************************************/
/***************** Call the exterminator! You've got bugs! ****************************/
/****************************/ bool ibool = 0; /***************************************/
/**************************************************************************************/

if(ibool) std::cout << " entry " << jentry << std::endl;
/**************************************************************************************/
// initialise everything
for (int i=0;i<6;i++){
	dBD[i].Fmult = 0; dBU[i].Fmult = 0;
	for (int j=0; j<32;j++){
		dBD[i].Fenergy[j] = -1.; dBD[i].Fnum[j] = -1; dBD[i].Ftime[j] = -1; dBD[i].phi[j] = -1; dBD[i].z[j] = -1;
		dBU[i].Fenergy[j] = -1.; dBU[i].Fnum[j] = -1; dBU[i].Ftime[j] = -1; dBU[i].phi[j] = -1; dBU[i].z[j] = -1;
}}

for (int i=0;i<12;i++){
	dSX[i].Fmult = 0; dSX[i].Bmult = 0;
	for(int j=0;j<4;j++){
	dSX[i].Fnum[j] = -1; dSX[i].Bnum[j] = -1;
	dSX[i].Benergy[i] = -1.; dSX[i].Btime[j] = -1.; dSX[i].phi[j] = -1.; dSX[i].rho[j] = -1.;
}}

for(int i=0; i<4;i++){
	dQ[i].Fmult = 0; dQ[i].Bmult = 0;
	for(int j=0;j<16;j++){
		dQ[i].Fnum[j] = -1; dQ[i].Bnum[j] = -1;
		dQ[i].Fenergy[j] = -1.; dQ[i].Benergy[j] = -1.;
		dQ[i].Btime[j] = -1.; dQ[i].Ftime[j] = -1.; dQ[i].phi[j] = -1.; dQ[i].rho[j] = -1.;
		dQ[i].FroughCal[j] = -1; dQ[i].BroughCal[j] = -1;
}}

QQQmult = 0; BarcMult = 0;

/**************************************************************************************/

// Gordon's tree contains the following:
// &event.fqqq[i].rings; &event.fqqq[i].wedges
// &event.barrel[0].frontsUp; &event.barrel[0].frontsDown; &event.barrel[0].backs;
// &event.barcUp[0].fronts; &event.barcUp[0].backs;
// &event.barcDown[0].fronts; &event.barcDown[0].backs;
// Each one of these has associated with it: 
// globalChannel; energy; timestamp
// e.g. &event.fqqq[2].rings[3].energy

// fill SX3s
// BUTTTTTT Gordon feeds in FrontsUp and Fronts down. So you could read them both in and combine them here?! or not. 
// Pattern is Down ch 0, 2, 5, 7; Up ch 1, 3, 4, 6
// but the front multiplicity should be for the entire front strip, not the individual ends.

/*
for (int i=0;i<12;i++){
	int iBack;

	if(ibool)std::cout << "in SX3 back loop, detector " << i << std::endl;
	for(auto& back : event->barrel[i].backs){
		if(back.energy>0){
			//store energy & timestamp
			dSX[i].Benergy[dSX[i].Bmult] = back.energy;
			dSX[i].Btime[dSX[i].Bmult] = back.timestamp;
			if(ibool)std::cout << "mult = " << dSX[i].Bmult << std::endl;
			if(ibool)std::cout << "dSX["<<i<<"].Benergy["<<dSX[i].Bmult<<"] = " << dSX[i].Benergy[dSX[i].Bmult] << std::endl;
			//assign Bnum to iBack (local strip number)
			iBack = m_lookUp[back.globalChannel]; //iBack is local strip # (unvetted, some may be "reversed") 
			dSX[i].Bnum[dSX[i].Bmult] = iBack;
			if(ibool)std::cout << "dSX["<<i<<"].Bnum["<<dSX[i].Bmult<<"] = " << dSX[i].Bnum[dSX[i].Bmult] << std::endl;
			//calculate physical coordinate(s) 
			float phi = 270.-(30.*i) - (iBack+0.5)*30./4.; //phi in degrees (center of strip, assuming negligible dead space)
			float rho = SXrho[i]; //TODO: rho in mm (center of SX_i detector, not center of strip)
			//store physical coordinate(s) 
			dSX[i].phi[dSX[i].Bmult] = (phi < 0.) ? (phi + 360.) : (phi); //if phi < 0, then add 360
			dSX[i].rho[dSX[i].Bmult] = rho;
			//print coordinates 
			if(ibool) std::cout << "phi for back "<<iBack<<" of SX "<<i<<" = " << dSX[i].phi[dSX[i].Bmult] << std::endl;
			if(ibool) std::cout << "rho for back "<<iBack<<" of SX "<<i<<" = " << dSX[i].rho[dSX[i].Bmult] << std::endl;
			//increment multiplicity
			dSX[i].Bmult++; 
			if(ibool)std::cout << "SX" << i << " mult = " << dSX[i].Bmult << " at end of loop" << std::endl;
	}}
	// ignore the fronts for now
}*/

if(ibool) std::cout << std::endl;

// fill QQQs
for (int i=0;i<4;i++){

	//fixes the fact that QQQ2&3 are swapped (for 18F runs)
	int iQQQ, iWedge, iRing;
	switch(i){
		case 0:
			iQQQ = i;
			break;
		case 1:
			iQQQ = i;
			break;
		case 2:
			iQQQ = 3;
			break;
		case 3:
			iQQQ = 2;
			break;
	}
	//pulling data from fqqq[i] but storing as dQ[iQQQ]
	if(ibool) std::cout << "in QQQ loop, detector " << iQQQ << std::endl;
	for(auto& ring : event->fqqq[i].rings){
		if(ibool) std::cout << "ring mult = " << dQ[iQQQ].Fmult << std::endl;
		if(ring.energy>0){
			//store energy & timestamp
			dQ[iQQQ].Fenergy[dQ[iQQQ].Fmult] = ring.energy;
			dQ[iQQQ].FroughCal[dQ[iQQQ].Fmult] = ring.energy*m_roughCal;
			dQ[iQQQ].Ftime[dQ[iQQQ].Fmult] = ring.timestamp;
			if(ibool) std::cout << "dQ["<<iQQQ<<"].Fenergy["<<dQ[iQQQ].Fmult<<"] = " << dQ[iQQQ].Fenergy[dQ[iQQQ].Fmult] << std::endl;
			//assign Fnum to iRing (local strip number)
			if(iQQQ==0 || iQQQ==2){//copying corrections for QQQ0&2 from wedges; forcing 0 to 15 to count same direction (outward?)
				iRing = 15-m_lookUp[ring.globalChannel]; //iRing is local ring #
			}else{
				iRing = m_lookUp[ring.globalChannel]; //iRing is local ring # (assuming QQQ1&3 count outward)
			}
			dQ[iQQQ].Fnum[dQ[iQQQ].Fmult] = iRing;
			if(ibool) std::cout << "dQ["<<iQQQ<<"].Fnum["<<dQ[iQQQ].Fmult<<"] = " << dQ[iQQQ].Fnum[dQ[iQQQ].Fmult] << std::endl;
			//calculate physical coordinate(s)
			float rho = 50.1 + (iRing+0.5)*(99.-50.1)/16.; //rho in mm (assuming 0to15 counts outward)
			//store physical coordinate(s)
			dQ[iQQQ].rho[dQ[iQQQ].Fmult] = rho; //get rho from rings
			//print coordinates
			if(ibool) std::cout << "rho for ring "<<iRing<<" of QQQ "<<iQQQ<<" = " << dQ[iQQQ].rho[dQ[iQQQ].Fmult] << std::endl;
        	//increment multiplicity
			dQ[iQQQ].Fmult++;
			QQQmult++;
			if(ibool) std::cout << "ring mult after loop = " << dQ[iQQQ].Fmult << std::endl;
	//still inside ring loop
		}}
	//pulling data from fqqq[i] but storing as dQ[iQQQ]
	for(auto& wedge : event->fqqq[i].wedges){
		if(ibool) std::cout << "wedge mult = " << dQ[iQQQ].Bmult << std::endl;
		if(wedge.energy>0){
			//store energy & timestamp
			dQ[iQQQ].Benergy[dQ[iQQQ].Bmult] = wedge.energy;
			dQ[iQQQ].BroughCal[dQ[iQQQ].Bmult] = wedge.energy*m_roughCal;
			dQ[iQQQ].Btime[dQ[iQQQ].Bmult] = wedge.timestamp;
			if(ibool) std::cout << "dQ["<<iQQQ<<"].Benergy["<<dQ[iQQQ].Bmult<<"] = " << dQ[iQQQ].Benergy[dQ[iQQQ].Bmult] << std::endl;
			//assign Fnum to iWedge (local strip number)
			if(iQQQ==0 || iQQQ==2){//QQQ0&2 corrected so all wedges count CCW 0 to 15
				iWedge = 15-m_lookUp[wedge.globalChannel]; //iWedge is local wedge #
			}else{
				iWedge = m_lookUp[wedge.globalChannel]; //iWedge is local wedge #
			}
			dQ[iQQQ].Bnum[dQ[iQQQ].Bmult] = iWedge;
			if(ibool) std::cout << "dQ["<<iQQQ<<"].Bnum["<<dQ[iQQQ].Bmult<<"] = " << dQ[iQQQ].Bnum[dQ[iQQQ].Bmult] << std::endl;
			//calculate physical coordinate(s)
			float phi = 267.158-(90.*iQQQ) - (iWedge+0.5)*87.158/16.; //phi in degrees (center of wedge, includes dead space)
			//store physical coordinate(s)
			dQ[iQQQ].phi[dQ[iQQQ].Bmult] = (phi < 0.) ? (phi + 360.) : (phi); //get phi from wedges; if phi < 0, then add 360
			//print coordinates
			if(ibool) std::cout << "phi for wedge "<<iWedge<<" of QQQ "<<iQQQ<<" = " << dQ[iQQQ].phi[dQ[iQQQ].Bmult] << std::endl;
			//increment multiplicity
			dQ[iQQQ].Bmult++;
			if(ibool) std::cout << "wedge mult after loop = " << dQ[iQQQ].Bmult << std::endl;
	//still inside wedge loop
		}}
} //end of QQQ loop
if(ibool) std::cout << "total QQQ (front) mult = " << QQQmult << std::endl;

// Fill barcelonas 
for(int i=0;i<6;i++){
	int iFront;
	if(ibool)std::cout << "in Barc loop, detector " << i << std::endl;
 	for(auto& front : event->barcDown[i].fronts){
 		//if(ibool) std::cout << "barcDn mult = " << dBD[i].Fmult << std::endl;
		if(front.energy>0){
			//store energy & timestamp 
			dBD[i].Fenergy[dBD[i].Fmult] = front.energy;
			dBD[i].Ftime[dBD[i].Fmult] = front.timestamp;
			if(ibool) std::cout << "dBD["<<i<<"].Fenergy["<<dBD[i].Fmult<<"] = " << dBD[i].Fenergy[dBD[i].Fmult] << std::endl;
			//assign Fnum to iFront (local strip number)
			iFront = m_lookUp[front.globalChannel]; //counts 0 to 31 from downstream to upstream (decreasing z)
			dBD[i].Fnum[dBD[i].Fmult] = iFront;
			if(ibool)std::cout << "dBD["<<i<<"].Fnum["<<dBD[i].Fmult<<"] = " << dBD[i].Fnum[dBD[i].Fmult] << std::endl;
			//calculate physical coordinate(s)
			float phi = 270. - (60.*i);
			float z = m_BDZoffset - 2*(iFront+0.5); // mm, m_BDZoffset is distance from z=0 to downstream edge of strip 0
			//store physical coordinate(s)
			dBD[i].phi[dBD[i].Fmult] = (phi < 0.) ? (phi + 360.) : (phi); //if phi < 0, then add 360
			dBD[i].z[dBD[i].Fmult] = z;
			//print coordinates
			if(ibool)std::cout << "dBD["<<i<<"].phi["<<dBD[i].Fmult<<"] = " << dBD[i].phi[dBD[i].Fmult] << std::endl;
			if(ibool)std::cout << "dBD["<<i<<"].z["<<dBD[i].Fmult<<"] = " << dBD[i].z[dBD[i].Fmult] << std::endl;
			//increment multiplicity
			dBD[i].Fmult++;
			BarcMult++;
			if(ibool) std::cout << "barcDn mult after loop = " << dBD[i].Fmult << std::endl;
			}}

	for(auto& front : event->barcUp[i].fronts){
		//if(ibool) std::cout << "barcUp mult = " << dBU[i].Fmult << std::endl;
		if(front.energy>0){
			//store energy & timestamp
			dBU[i].Fenergy[dBU[i].Fmult] = front.energy;
			dBU[i].Ftime[dBU[i].Fmult] = front.timestamp;
			if(ibool) std::cout << "dBU["<<i<<"].Fenergy["<<dBU[i].Fmult<<"] = " << dBU[i].Fenergy[dBU[i].Fmult] << std::endl;
			//assign Fnum to iFront (local strip number)
			iFront = m_lookUp[front.globalChannel]; //counts 0 to 31 from upstream to downstream (increasing z)
			dBU[i].Fnum[dBU[i].Fmult] = iFront;
			if(ibool)std::cout << "dBU["<<i<<"].Fnum["<<dBU[i].Fmult<<"] = " << dBU[i].Fnum[dBU[i].Fmult] << std::endl;
			//calculate physical coordinate(s)
			float phi = 270. - (60.*i);
			float z = m_BUZoffset + 2*(iFront+0.5); // mm, m_BUZoffset is distance from z=0 to upstream edge of strip 0
			//store physical coordinate(s)
			dBU[i].phi[dBU[i].Fmult] = (phi < 0.) ? (phi + 360.) : (phi); //if phi < 0, then add 360
			dBU[i].z[dBU[i].Fmult] = z;
			//print coordinates
			if(ibool)std::cout << "dBU["<<i<<"].phi["<<dBU[i].Fmult<<"] = " << dBU[i].phi[dBU[i].Fmult] << std::endl;
			if(ibool)std::cout << "dBU["<<i<<"].z["<<dBU[i].Fmult<<"] = " << dBU[i].z[dBU[i].Fmult] << std::endl;
			//increment multiplicity
			dBU[i].Fmult++;
			BarcMult++;
			if(ibool) std::cout << "barcUp mult after loop = " << dBU[i].Fmult << std::endl;
			}}
} //end of barc loop
if(ibool) std::cout << "total Barc mult = " << BarcMult << std::endl;

/**************************************************************************************/
outT->Fill();
if(jentry%10000 == 0) std::cout << "Entry " << jentry << " of " << nevents << ", " << 100 * jentry/nevents << "\% complete";
std::cout << "\r" << std::flush;
} // end of event loop
/**************************************************************************************/

outputfile->cd();
//std::cout << "Printing" << std::endl; outT->Print();
std::cout << "\nWriting" << std::endl; outT->Write();
inputfile->Close();
outputfile->Close();
delete inputfile;
delete outputfile;
}

/*void Fangorn(int runMin, int runMax)
{
	for(int i=runMin; i<=runMax; i++)
		Run_Fangorn(i);
}*/