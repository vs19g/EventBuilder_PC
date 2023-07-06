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

R__LOAD_LIBRARY(../lib/libEVBDict.so);

void BasicPlotPoint(int runNumber){
std::string input_filename = "/home/vigneshsitaraman/PCTesting/WorkingData/built/run_"+std::to_string(runNumber)+".root";
std::string output_filename = "/home/vigneshsitaraman/PCTesting/WorkingData/histograms/run_"+std::to_string(runNumber)+".root";

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
struct dataQ{ int Fmult; int Bmult; float Fenergy[16]; float Benergy[16]; int Fnum[16]; int Bnum[16];};
struct dataPC{ int Amult; int Cmult; float Aenergy[32]; float Cenergy[32]; int Anum[32];int Cnum[32];};
dataQ dQ[4]; // number of each type of detector
dataSX dSX[12];
dataPC dPC[1];

int i, j;
float SXBenergyMax; int SXBnumMax; int SXBdetMax;
float QFenergyMax; int QFnumMax; int QFdetMax;
float PCAenergyMax; float PCCenergyMax; int PCAdetMax; int PCCdetMax; int PCAnumMax; int PCCnumMax;
float dE; float dEtheta; float dEphi; 
float E; float Etheta; float Ephi;


// ***** output tree *****

// SX3 branches
// SX3 fronts will need to be dealt with in a different code if the front strips are a
// combination of the upstream and downstream ends... maybe. 
// Then I'd also need a branch for position and a reconstruction. 

// Will need coordinate branches and time

outT->Branch("SX0Fmult",&dSX[0].Fmult,"SX0Fmult/i"); outT->Branch("SX0Bmult",&dSX[0].Bmult,"SX0Bmult/i");
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

// PC branches

outT->Branch("PCAmult",&dPC[0].Amult,"PCAmult/i");
outT->Branch("PCAnum",dPC[0].Anum,"PCAnum[PCAmult]/i");
outT->Branch("PCAenergy",dPC[0].Aenergy,"PCAenergy[PCAmult]/f");

outT->Branch("PCCmult",&dPC[0].Cmult,"PCCmult/i");
outT->Branch("PCCnum",dPC[0].Cnum,"PCCnum[PCCmult]/i");
outT->Branch("PCCenergy",dPC[0].Cenergy,"PCCenergy[PCCmult]/f");


outT->Branch("PCAenergyMax",&PCAenergyMax,"PCAenergyMax/f"); outT->Branch("PCCenergyMax",&PCCenergyMax,"PCCenergyMax/f");
outT->Branch("PCAdetMax",&PCAdetMax,"PCAdetMax/i"); outT->Branch("PCCdetMax",&PCCdetMax,"PCCdetMax/i");
outT->Branch("PCAnumMax",&PCAnumMax,"PCAnumMax/i"); outT->Branch("PCCnumMax",&PCCnumMax,"PCCnumMax/i"); 

// General branches (salute)
outT->Branch("dE",&dE,"dE/f"); // highest dE signal (from any barcelona detector, up or downstream)
outT->Branch("dEtheta",&dEtheta,"dEtheta/f"); // theta from highest dE
outT->Branch("dEphi",&dEphi,"dEphi/f"); // phi from highest dE
outT->Branch("E",&E,"E/f"); // highest E signal (from any QQQ, for now. )
outT->Branch("Etheta",&Etheta,"Etheta/f"); // theta from highest E
outT->Branch("Ephi",&Ephi,"Ephi/f"); // phi from highest E

/**************************************************************************************/
tree->SetBranchAddress("event",&event);
Long64_t nevents = tree->GetEntries();
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

dPC[0].Amult = 0; dPC[0].Cmult = 0;
for (int i=0; i<24;i++)
{
    dPC[0].Aenergy[i] = -1.; dPC[0].Anum[i] = -1;
	dPC[0].Cenergy[i] = -1.; dPC[0].Cnum[i] = -1;
}

for (i=0;i<12;i++){
	dSX[i].Fmult = 0; dSX[i].Bmult = 0;
	for(j=0;j<4;j++){
	dSX[i].Fnum[j] = -1; dSX[i].Bnum[j] = -1;
	dSX[i].Fenergy[j] = -1.; dSX[i].Benergy[i] = -1.;
}}	

for(i=0; i<4;i++){
	dQ[i].Fmult = 0; dQ[i].Bmult = 0;
	for(j=0;j<16;j++){
		dQ[i].Fnum[j] = -1; dQ[i].Bnum[j] = -1;
		dQ[i].Fenergy[j] = -1.; dQ[i].Benergy[j] = -1.;
}}

SXBenergyMax = -1.; SXBnumMax = -1; SXBdetMax = -1;
QFdetMax = -1; QFenergyMax = -1.; QFnumMax = -1;
PCAdetMax = -1; PCAnumMax = -1; PCAenergyMax = -1.; PCCdetMax = -1; PCCnumMax = -1; PCCenergyMax = -1.;

dE = -1.; dEtheta = -1.; dEphi = -1.; E = -1.; Etheta = -1.; Ephi = -1.;

EventBuilder::ChannelMap m_chanMap("ANASEN_PC_test.txt");
// Otherwise, if this is slow, I could parse the global and local channels from the channel map and make a giant array
/**************************************************************************************/

// Gordon's tree contains the following:
// &event.fqqq[i].rings; &event.fqqq[i].wedges
// &event.barrel[0].frontsUp; &event.barrel[0].frontsDown; &event.barrel[0].backs;
// &event.pc[0].anodes; &event.pc[0].cathodes;
// Each one of these has associated with it: 
// globalChannel; energy; timestamp
// e.g. &event.fqqq[2].rings[3].energy

// fill SX3s
// BUTTTTTT Gordon feeds in FrontsUp and Fronts down. So you could read them both in and combine them here?! or not. 
// Pattern is Down ch 0, 2, 5, 7; Up ch 1, 3, 4, 6
// but the front multiplicity should be for the entire front strip, not the individual ends.


for (i=0;i<12;i++){
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
	/* for(auto& frontup : event->barrel[i].frontsUp){
		dSX[i].Fenergy[dSX[i].Fmult] = frontup.energy;
		dSX[i].Fnum[dSX[i].Fmult] = frontup.globalChannel;
		// dSX[i].Ftime[mult] = front.timestamp;
		dSX[i].Fmult++;
	} */
}

if(ibool) std::cout << std::endl;
// fill QQQs
for (i=0;i<4;i++){
	if(ibool) std::cout << "in QQQ ring loop, detector " << i << ", mult = " << dQ[i].Fmult << std::endl;
	for(auto& ring : event->fqqq[i].rings){
		if(ibool) std::cout << "QQQ ring " << i << std::endl;
		if(ring.energy>0){
			dQ[i].Fenergy[dQ[i].Fmult] = ring.energy;
			if(ibool) std::cout << "dQ["<<i<<"].Fenergy["<<dQ[i].Fmult<<"] = " << dQ[i].Fenergy[dQ[i].Fmult] << std::endl;
			auto iter = m_chanMap.FindChannel(ring.globalChannel);
			if(iter == m_chanMap.End()){ 
				std::cout << "channel map error" << std::endl;
			}else{
				dQ[i].Fnum[dQ[i].Fmult] = iter->second.local_channel;
				if(ibool) std::cout << "dQ["<<i<<"].Fnum["<<dQ[i].Fmult<<"] = " << dQ[i].Fnum[dQ[i].Fmult] << std::endl;
				//dQ[i].Ftime[dQ[i].Fmult] = ring.timestamp;
				if(dQ[i].Fenergy[dQ[i].Fmult]>QFenergyMax){
					 QFenergyMax = dQ[i].Fenergy[dQ[i].Fmult];
					 QFdetMax = i;
					 QFnumMax = dQ[i].Fnum[dQ[i].Fmult];
				}
				dQ[i].Fmult++;
				if(ibool) std::cout << "ring mult after loop = " << dQ[i].Fmult << std::endl;
			
	}}}
	for(auto& wedge : event->fqqq[i].wedges){
		if(ibool) std::cout << "QQQ wedge " << i << std::endl;
		if(wedge.energy>0){
			dQ[i].Benergy[dQ[i].Bmult] = wedge.energy;
			if(ibool) std::cout << "dQ["<<i<<"].Benergy["<<dQ[i].Bmult<<"] = " << dQ[i].Benergy[dQ[i].Bmult] << std::endl;
			auto iter = m_chanMap.FindChannel(wedge.globalChannel);
			if(iter == m_chanMap.End()){ 
				std::cout << "channel map error" << std::endl;
			}else{
				dQ[i].Bnum[dQ[i].Bmult] = iter->second.local_channel;
				if(ibool) std::cout << "dQ["<<i<<"].Bnum["<<dQ[i].Bmult<<"] = " << dQ[i].Bnum[dQ[i].Bmult] << std::endl;
				//dQ[i].Btime[dQ[i].Bmult] = wedge.timestamp;
				dQ[i].Bmult++;
				if(ibool) std::cout << "W mult after loop = " << dQ[i].Bmult << std::endl;

}}}
}

// Fill PC 
 	for(auto& anode : event->pc[0].anodes){
		if(anode.energy>0){
			dPC[0].Aenergy[dPC[0].Amult] = anode.energy;
			auto iter = m_chanMap.FindChannel(anode.globalChannel);
			if(iter == m_chanMap.End()){ 
				std::cout << "channel map error" << std::endl;
			}
            else{
				dPC[0].Anum[dPC[0].Amult] = iter->second.local_channel;
				//dBD[i].Ftime[dBD[i].Fmult] = front.timestamp;
				if(dPC[0].Aenergy[dPC[0].Amult]>PCAenergyMax){
					PCAenergyMax = dPC[0].Aenergy[dPC[0].Amult];
					PCAnumMax = dPC[0].Anum[dPC[0].Amult];
					PCAdetMax = i;
				}
				dPC[0].Amult++;
			}
        }
    }

    for(auto& cathode : event->pc[0].cathodes){
	    if(cathode.energy>0){
		    dPC[0].Cenergy[dPC[0].Cmult] = cathode.energy;
		    auto iter = m_chanMap.FindChannel(cathode.globalChannel);
		    if(iter == m_chanMap.End()){ 
			std::cout << "channel map error" << std::endl;
		    }
            else{
		    	dPC[0].Cnum[dPC[0].Cmult] = iter->second.local_channel;
				//dBD[i].Ftime[dBD[i].Fmult] = front.timestamp;
				if(dPC[0].Cenergy[dPC[0].Cmult]>PCCenergyMax){
					PCCenergyMax = dPC[0].Cenergy[dPC[0].Cmult];
					PCCnumMax = dPC[0].Cnum[dPC[0].Cmult];
					PCCdetMax = i;
				}
				dPC[0].Cmult++;
		    }
        }
    }        


/**************************************************************************************/
// Sorting to get the biggest hit
// dE = largest of Barc up or Barc down

// not quite right to have theta = strip number here because of the reversing of the strip
// numbers between up and downstream. 
// if (PCAenergyMax>PCCenergyMax){
// 		dE = PCAenergyMax;
// 		dEtheta = PCAnumMax; 
// }else{
// 		dE = PCCenergyMax;
// 		dEtheta = PCCnumMax;
// }

// Coordinates of world: +x is beam left, +y is towards the ceiling, +z is the beam direction
// For cylindrical coordinates: x = rho x cos(phi), y = rho x sin(phi), z = z.
// where rho = sqrt (x*x + y*y)
// phi = arctan(y/x)

// phi = arcsin(y/rho), if x>=0;
// phi = -1*arcsin(y/rho) + pi, if x<0 && y>=0;
// phi = -1*arcsin(y/rho)






/**************************************************************************************/
outputfile->cd();
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