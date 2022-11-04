#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "../src/evb/ChannelMap.h"
#include "../src/evb/ChannelMap.cpp"
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
std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_barc+qqq.root";

std::cout<<"Processing data in "<<input_filename<<std::endl;

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
CoincEvent* event = new CoincEvent();	
tree->SetBranchAddress("event", &event);

// ***** output file *****
TFile* outputfile = TFile::Open(output_filename.c_str(), "RECREATE");
TTree *outT = new TTree("TreeData","TreeData");
std::cout<<"Opening "<<output_filename<<std::endl;

// ***** definitions *****
struct dataSX{ int Fmult; int Bmult; float Fenergy[4]; float Benergy[4]; int Fnum[4]; int Bnum[4];}; // members of each detector class
struct dataQ{ int Fmult; int Bmult; float Fenergy[16]; float Benergy[16]; int Fnum[16]; int Bnum[16]; float rho[16]; float theta[16]; float phi[16]; }; //Fnum & Bnum are local channel numbers
struct dataBarcUp{ int Fmult; float Fenergy[32]; int Fnum[32];};
struct dataBarcDown{ int Fmult; float Fenergy[32]; int Fnum[32];};
dataQ dQ[4]; // number of each type of detector
dataSX dSX[12];
dataBarcUp dBU[6];
dataBarcDown dBD[6]; // I think this is actually 5

int i, j;
float SXBenergyMax; int SXBnumMax; int SXBdetMax;
float QFenergyMax; int QFnumMax; int QFdetMax;
float BUenergyMax; float BDenergyMax; int BUdetMax; int BDdetMax; int BUnumMax; int BDnumMax;
float dE; float dEtheta; float dEphi; float dErho;
float E; float Etheta; float Ephi; float Erho;


// ***** output tree *****

// SX3 branches
// SX3 fronts will need to be dealt with in a different code if the front strips are a
// combination of the upstream and downstream ends... maybe. 
// Then I'd also need a branch for position and a reconstruction. 

// Will need coordinate branches and time

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

outT->Branch("SX0Fnum",dSX[0].Fnum,"SX0Fnum[SX0Fmult]/i");
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
*/

// QQQ branches

outT->Branch("Q0Fmult",&dQ[0].Fmult,"Q0Fmult/i"); outT->Branch("Q0Bmult",&dQ[0].Bmult,"Q0Bmult/i");
//outT->Branch("Q0Fnum",dQ[0].Fnum,"Q0Fnum[Q0Fmult]/i"); outT->Branch("Q0Bnum",dQ[0].Bnum,"Q0Bnum[Q0Bmult]/i");
//outT->Branch("Q0Fenergy",dQ[0].Fenergy,"Q0Fenergy[Q0Fmult]/f"); outT->Branch("Q0Benergy",dQ[0].Benergy,"Q0Benergy[Q0Bmult]/f");
//outT->Branch("Q0rho",&dQ[0].rho,"Q0rho/i"); 
//outT->Branch("Q0theta",&dQ[0].theta,"Q0theta/i");
outT->Branch("Q0phi",&dQ[0].phi,"Q0phi[Q0Fmult]/f");

outT->Branch("Q1Fmult",&dQ[1].Fmult,"Q1Fmult/i"); outT->Branch("Q1Bmult",&dQ[1].Bmult,"Q1Bmult/i");
//outT->Branch("Q1Fnum",dQ[1].Fnum,"Q1Fnum[Q1Fmult]/i"); outT->Branch("Q1Bnum",dQ[1].Bnum,"Q1Bnum[Q1Bmult]/i");
//outT->Branch("Q1Fenergy",dQ[1].Fenergy,"Q1Fenergy[Q1Fmult]/f"); outT->Branch("Q1Benergy",dQ[1].Benergy,"Q1Benergy[Q1Bmult]/f");
//outT->Branch("Q1rho",&dQ[1].rho,"Q1rho/i"); 
//outT->Branch("Q1theta",&dQ[1].theta,"Q1theta/i");
outT->Branch("Q1phi",&dQ[1].phi,"Q1phi[Q1Fmult]/f");

outT->Branch("Q2Fmult",&dQ[2].Fmult,"Q2Fmult/i"); outT->Branch("Q2Bmult",&dQ[2].Bmult,"Q2Bmult/i");
//outT->Branch("Q2Fnum",dQ[2].Fnum,"Q2Fnum[Q2Fmult]/i"); outT->Branch("Q2Bnum",dQ[2].Bnum,"Q2Bnum[Q2Bmult]/i");
//outT->Branch("Q2Fenergy",dQ[2].Fenergy,"Q2Fenergy[Q2Fmult]/f"); outT->Branch("Q2Benergy",dQ[2].Benergy,"Q2Benergy[Q2Bmult]/f");
//outT->Branch("Q2rho",&dQ[2].rho,"Q2rho/i"); 
//outT->Branch("Q2theta",&dQ[2].theta,"Q2theta/i");
outT->Branch("Q2phi",&dQ[2].phi,"Q2phi[Q2Fmult]/f");

outT->Branch("Q3Fmult",&dQ[3].Fmult,"Q3Fmult/i"); outT->Branch("Q3Bmult",&dQ[3].Bmult,"Q3Bmult/i");
//outT->Branch("Q3Fnum",dQ[3].Fnum,"Q3Fnum[Q3Fmult]/i"); outT->Branch("Q3Bnum",dQ[3].Bnum,"Q3Bnum[Q3Bmult]/i");
//outT->Branch("Q3Fenergy",dQ[3].Fenergy,"Q3Fenergy[Q3Fmult]/f"); outT->Branch("Q3Benergy",dQ[3].Benergy,"Q3Benergy[Q3Bmult]/f");
//outT->Branch("Q3rho",&dQ[3].rho,"Q3rho/i"); 
//outT->Branch("Q3theta",&dQ[3].theta,"Q3theta/i");
outT->Branch("Q3phi",&dQ[3].phi,"Q3phi[Q3Fmult]/f");

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
//outT->Branch("Erho",&Erho,"Erho/f"); // phi from highest E

/**************************************************************************************/
tree->SetBranchAddress("event",&event);
Long64_t nevents = tree->GetEntries();
//Long64_t nevents = 500; //for testing only
Long64_t jentry;

for (jentry=0; jentry<nevents; jentry++){
    tree->GetEntry(jentry);
	
/**************************************************************************************/
/***************** Call the exterminator! You've got bugs! ****************************/
/****************************/ bool ibool = 0; /***************************************/
/**************************************************************************************/

if(ibool) std::cout << " entry " << jentry << std::endl;
/**************************************************************************************/
// initialise everything
for (int j=0;j<6;j++){
	dBD[j].Fmult = 0; dBU[j].Fmult = 0;
	for (int i=0; i<32;i++){
		dBD[j].Fenergy[i] = -1.; dBD[j].Fnum[i] = -1;
		dBU[j].Fenergy[i] = -1.; dBU[j].Fnum[i] = -1;
}}

/*for (i=0;i<12;i++){
	dSX[i].Fmult = 0; dSX[i].Bmult = 0;
	for(j=0;j<4;j++){
	dSX[i].Fnum[j] = -1; dSX[i].Bnum[j] = -1;
	dSX[i].Fenergy[j] = -1.; dSX[i].Benergy[i] = -1.;
}}*/	

for(i=0; i<4;i++){
	dQ[i].Fmult = 0; dQ[i].Bmult = 0;
	for(j=0;j<16;j++){
		dQ[i].Fnum[j] = -1; dQ[i].Bnum[j] = -1;
		dQ[i].Fenergy[j] = -1.; dQ[i].Benergy[j] = -1.;
		dQ[i].phi[j] = -1; //dQ[i].rho[j] = -1; dQ[i].theta[j] = -1;
}}

SXBenergyMax = -1.; SXBnumMax = -1; SXBdetMax = -1;
QFdetMax = -1; QFenergyMax = -1.; QFnumMax = -1;
BUdetMax = -1; BUnumMax = -1; BUenergyMax = -1.; BDdetMax = -1; BDnumMax = -1; BDenergyMax = -1.;

dE = -1.; dEtheta = -1.; dEphi = -1.; E = -1.; Etheta = -1.; Ephi = -1.;
//dErho = 5;

EventBuilder::ChannelMap m_chanMap("ANASEN_TRIUMFAug_run21+_ChannelMap.txt");
// Otherwise, if this is slow, I could parse the global and local channels from the channel map and make a giant array
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


/*for (i=0;i<12;i++){
	//std::cout << "in SX3 back loop, detector " << i << ", mult = " << dSX[i].Bmult << std::endl;
	for(auto& back : event->barrel[i].backs){
	if(ibool)	std::cout << " detector " << i << std::endl;
		if(back.energy>0){
			dSX[i].Benergy[dSX[i].Bmult] = back.energy;
			if(ibool)std::cout << "mult = " << dSX[i].Bmult << std::endl;
			if(ibool)std::cout << "dSX["<<i<<"].Benergy["<<dSX[i].Bmult<<"] = " << dSX[i].Benergy[dSX[i].Bmult] << std::endl;
			auto iter = m_chanMap.FindChannel(back.globalChannel);
				if(iter == m_chanMap.End()){
				std::cout << "channel map error" << std::endl;
				}else{
				dSX[i].Bnum[dSX[i].Bmult] = iter->second.local_channel;
				if(ibool)std::cout << "dSX["<<i<<"].Bnum["<<dSX[i].Bmult<<"] = " << dSX[i].Bnum[dSX[i].Bmult] << std::endl;
				// dSX[i].Btime[mult] = back.timestamp;
				dSX[i].Bmult++; 
				if(ibool)std::cout << dSX[i].Bmult << " at end of loop" << std::endl;
	}}}
	 for(auto& frontup : event->barrel[i].frontsUp){
		dSX[i].Fenergy[dSX[i].Fmult] = frontup.energy;
		dSX[i].Fnum[dSX[i].Fmult] = frontup.globalChannel;
		// dSX[i].Ftime[mult] = front.timestamp;
		dSX[i].Fmult++;
	}
}*/

if(ibool) std::cout << std::endl;


// fill QQQs
for (i=0;i<4;i++){

	//fix the fact that QQQ2&3 are swapped (for 18F runs)
	int iQQQ, iWedge;
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
	if(ibool) std::cout << "in QQQ ring loop, detector " << iQQQ << ", mult = " << dQ[iQQQ].Fmult << std::endl;
	for(auto& ring : event->fqqq[iQQQ].rings){
		if(ibool) std::cout << "QQQ ring " << dQ[iQQQ].Fmult << std::endl;
		if(ring.energy>0){
			dQ[iQQQ].Fenergy[dQ[iQQQ].Fmult] = ring.energy;
			if(ibool) std::cout << "dQ["<<iQQQ<<"].Fenergy["<<dQ[iQQQ].Fmult<<"] = " << dQ[iQQQ].Fenergy[dQ[iQQQ].Fmult] << std::endl;
			auto iter = m_chanMap.FindChannel(ring.globalChannel);
			if(iter == m_chanMap.End()){ 
				std::cout << "channel map error" << std::endl;
			}else{
				dQ[iQQQ].Fnum[dQ[iQQQ].Fmult] = iter->second.local_channel;
				if(ibool) std::cout << "dQ["<<iQQQ<<"].Fnum["<<dQ[iQQQ].Fmult<<"] = " << dQ[iQQQ].Fnum[dQ[iQQQ].Fmult] << std::endl;
				//dQ[iQQQ].Ftime[dQ[iQQQ].Fmult] = ring.timestamp;
				//dQ[iQQQ].rho[dQ[iQQQ].Fmult] = ; //get rho from rings
				if(dQ[iQQQ].Fenergy[dQ[iQQQ].Fmult]>QFenergyMax){
					 QFenergyMax = dQ[iQQQ].Fenergy[dQ[iQQQ].Fmult];
					 QFdetMax = iQQQ;
					 QFnumMax = dQ[iQQQ].Fnum[dQ[iQQQ].Fmult];
					 if(ibool) std::cout << "Max energy is "<<QFenergyMax<<" from QQQ"<<QFdetMax<<" ring "<<QFnumMax<<std::endl;
				}
				dQ[iQQQ].Fmult++;
				if(ibool) std::cout << "ring mult after loop = " << dQ[iQQQ].Fmult << std::endl;
			
	}}}
	for(auto& wedge : event->fqqq[iQQQ].wedges){
		if(ibool) std::cout << "QQQ wedge " << dQ[iQQQ].Bmult << std::endl;
		if(wedge.energy>0){
			dQ[iQQQ].Benergy[dQ[iQQQ].Bmult] = wedge.energy;
			if(ibool) std::cout << "dQ["<<iQQQ<<"].Benergy["<<dQ[iQQQ].Bmult<<"] = " << dQ[iQQQ].Benergy[dQ[iQQQ].Bmult] << std::endl;
			auto iter = m_chanMap.FindChannel(wedge.globalChannel);
			if(iter == m_chanMap.End()){ 
				std::cout << "channel map error" << std::endl;
				return;
			}else if(iQQQ==0 || iQQQ==2){
				iWedge = 15.-iter->second.local_channel; //QQQ0&2 corrected so all wedges count CCW 0 to 15
			}else{
				iWedge = iter->second.local_channel;
			}
			dQ[iQQQ].Bnum[dQ[iQQQ].Bmult] = iWedge;
			if(ibool) std::cout << "dQ["<<iQQQ<<"].Bnum["<<dQ[iQQQ].Bmult<<"] = " << dQ[iQQQ].Bnum[dQ[iQQQ].Bmult] << std::endl;
			//dQ[iQQQ].Btime[dQ[iQQQ].Bmult] = wedge.timestamp;
			float phi = 270. - (iQQQ*16 +iWedge)*360./64.;
			dQ[iQQQ].phi[dQ[iQQQ].Bmult] = (phi < 0.) ? (phi + 360.) : (phi); //get phi from wedges; if phi < 0, then add 360
			if(ibool) std::cout << "phi for wedge "<<iWedge<<" of QQQ "<<iQQQ<<" = " << dQ[iQQQ].phi[dQ[iQQQ].Bmult] << std::endl;
			dQ[iQQQ].Bmult++;
			if(ibool) std::cout << "wedge mult after loop = " << dQ[iQQQ].Bmult << std::endl;
}}
//still inside QQQ loop
}

// Fill barcelonas 
for(i=0;i<6;i++){
 	for(auto& front : event->barcDown[i].fronts){
		if(front.energy>0){
			dBD[i].Fenergy[dBD[i].Fmult] = front.energy;
			auto iter = m_chanMap.FindChannel(front.globalChannel);
			if(iter == m_chanMap.End()){ 
				std::cout << "channel map error" << std::endl;
			}else{
				dBD[i].Fnum[dBD[i].Fmult] = iter->second.local_channel;
				//dBD[i].Ftime[dBD[i].Fmult] = front.timestamp;
				if(dBD[i].Fenergy[dBD[i].Fmult]>BDenergyMax){
					BDenergyMax = dBD[i].Fenergy[dBD[i].Fmult];
					BDnumMax = dBD[i].Fnum[dBD[i].Fmult];
					BDdetMax = i;
				}
				dBD[i].Fmult++;
			}}}

	for(auto& front : event->barcUp[i].fronts){
		if(front.energy>0){
			dBU[i].Fenergy[dBU[i].Fmult] = front.energy;
			auto iter = m_chanMap.FindChannel(front.globalChannel);
			if(iter == m_chanMap.End()){ 
				std::cout << "channel map error" << std::endl;
			}else{
				dBU[i].Fnum[dBU[i].Fmult] = iter->second.local_channel;
				//dBU[i].Ftime[dBU[i].Fmult] = front.timestamp;
				if(dBU[i].Fenergy[dBU[i].Fmult]>BUenergyMax){
					BUenergyMax = dBU[i].Fenergy[dBU[i].Fmult];
					BUnumMax = dBU[i].Fnum[dBU[i].Fmult];
					BUdetMax = i;
				}
				dBU[i].Fmult++;
			}}}
}
/**************************************************************************************/
// Sorting to get the biggest hit
// dE = largest of Barc up or Barc down

// not quite right to have theta = strip number here because of the reversing of the strip
// numbers between up and downstream. 
if (BUenergyMax>BDenergyMax){
		dE = BUenergyMax;
		dEtheta = BUnumMax; 
}else{
		dE = BDenergyMax;
		dEtheta = BDnumMax;
}

// Coordinates of world: +x is beam right, +y is towards the floor, +z is the beam direction

// For cylindrical coordinates: x = rho x cos(phi), y = rho x sin(phi), z = z.
// where rho = sqrt (x*x + y*y)
// phi = arctan(y/x)

// phi = arcsin(y/rho), if x>=0;
// phi = -1*arcsin(y/rho) + pi, if x<0 && y>=0;
// phi = -1*arcsin(y/rho)






/**************************************************************************************/
//outputfile->cd();
outT->Fill();
if(jentry%10000 == 0) std::cout << "Entry " << jentry << " of " << nevents << ", " << 100 * jentry/nevents << "\% complete";
std::cout << "\r" << std::flush;
} // end of event loop
/**************************************************************************************/
outputfile->cd();
std::cout << "Printing" << std::endl; outT->Print();
std::cout << "Writing" << std::endl; outT->Write();
outputfile->Write();

inputfile->Close();
outputfile->Close();
delete inputfile;
delete outputfile;
}