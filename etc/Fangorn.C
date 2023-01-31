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
#include "Fangorn.h"

R__LOAD_LIBRARY(../lib/libEVBDict.dylib);

void Fangorn(int runNumber){
std::string input_filename = "run_"+std::to_string(runNumber)+".root";
std::string output_filename = "run_"+std::to_string(runNumber)+"_gw.root";

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

/**************************************************************************************/

Long64_t nevents = tree->GetEntries();
Long64_t jentry;

for (jentry=0; jentry<nevents; jentry++){
    tree->GetEntry(jentry);
	
/**************************************************************************************/
/***************** Call the exterminator! You've got bugs! ******************/
/****************************/ bool ibool = 0; /**************************/
/**************************************************************************************/
/*************************************************************************************/
//branches here? or call to it?
//    outT->Branch("QFmult",&QFmult,"QFmult/i");
//    outT->Branch("QBmult",&QBmult,"QBmult/i");
//    outT->Branch("QFenergy",QFenergy,"QFenergy[QFmult]/f");
//    outT->Branch("QBenergy",QBenergy,"QBenergy[QBmult]/f");
//    outT->Branch("QFdetnum",QFdetnum,"QFdetnum[QFmult]/i");
//    outT->Branch("QBdetnum",QBdetnum,"QBdetnum[QBmult]/i");
//    outT->Branch("QFnum",QFnum,"QFnum[QFmult]/i");
//    outT->Branch("QBnum",QBnum,"QBnum[QBmult]/i");
//    outT->Branch("QFtime",QFtime,"QFtime[QFmult]/l");
//    outT->Branch("QBtime",QBtime,"QBtime[QBmult]/l");
//    outT->Branch("Qz",Qz,"Qz[QFmult]/f");
//    outT->Branch("Qrho",Qrho,"Qrho[QFmult]/f");
//    outT->Branch("Qphi",Qphi,"Qphi[QBmult]/f");
    
// initialise everything

for(i=0; i<4;i++){
	dQ[i].Fmult = 0; dQ[i].Bmult = 0; dQ[i].detnum = -1;
	for(j=0;j<16;j++){
		dQ[i].Fnum[j] = -1; dQ[i].Bnum[j] = -1;
		dQ[i].Fenergy[j] = -1.; dQ[i].Benergy[j] = -1.;
        dQ[i].z[j] = -1.; dQ[i].rho[j] = -1; dQ[i].phi[j] = -1000;
        dQ[i].Ftime[j] = -1; dQ[i].Btime[j] = -1;
}}


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

    for (i=0;i<4;i++){
        // fill ring data with energy, time, strip. coordinates later
	for(auto& ring : event->fqqq[i].rings){
            if(ibool) std::cout << "Entry " << jentry << " QQQ ring " << i << std::endl;
        dQ[i].Fenergy[dQ[i].Fmult] = ring.energy;
        dQ[i].Ftime[dQ[i].Fmult] = ring.timestamp;
                if(ibool) std::cout << "dQ["<<i<<"].Fenergy["<<dQ[i].Fmult<<"] = " << dQ[i].Fenergy[dQ[i].Fmult] << std::endl;
        auto iter = m_chanMap.FindChannel(ring.globalChannel);
        if(iter == m_chanMap.End()){
            std::cout << "channel map error, QQQr" << std::endl;
        }else{
            dQ[i].Fnum[dQ[i].Fmult] = iter->second.local_channel;
		if(ibool) std::cout << "dQ["<<i<<"].Fnum["<<dQ[i].Fmult<<"] = " << dQ[i].Fnum[dQ[i].Fmult] << std::endl;
	    dQ[i].Fmult++;
		if(ibool) std::cout << "ring mult after loop = " << dQ[i].Fmult << std::endl;
        }}
    
	for(auto& wedge : event->fqqq[i].wedges){
                if(ibool) std::cout << "QQQ wedge " << i << std::endl;
        dQ[i].Benergy[dQ[i].Bmult] = wedge.energy;
        dQ[i].Btime[dQ[i].Bmult] = wedge.timestamp;
                if(ibool) std::cout << "dQ["<<i<<"].Benergy["<<dQ[i].Bmult<<"] = " << dQ[i].Benergy[dQ[i].Bmult] << std::endl;
        auto iter = m_chanMap.FindChannel(wedge.globalChannel);
        if(iter == m_chanMap.End()){
				std::cout << "channel map error, QQQw" << std::endl;
        }else{
            dQ[i].Bnum[dQ[i].Bmult] = iter->second.local_channel;
                if(ibool) std::cout << "dQ["<<i<<"].Bnum["<<dQ[i].Bmult<<"] = " << dQ[i].Bnum[dQ[i].Bmult] << std::endl;
            dQ[i].Bmult++;
				if(ibool) std::cout << "W mult after loop = " << dQ[i].Bmult << std::endl;
}}
    } // end loop over 4.
    
        // now we have arrays of wedge&ring data. Maybe rho, phi and z are only calculated if conditions are met, i.e. energy threshold, ring had to fire, coincidence with barc, etc
    for(i=0;i<4;i++){
    for(j=0; j<dQ[i].Fmult; j++){
            dQ[i].z[j] = QQQzpos;
            dQ[i].rho[j] = (dQ[i].Fnum[j]+0.5)*(0.099-0.0501)/16; // rho in mm
            // rho is fixed for each ring
        }
    for(j=0; j<dQ[i].Bmult; j++){
        // each wedge is 5.625 deg, define centre of wedge as set phi
        if(i==2) dQ[2].phi[j] = ((15 - dQ[2].Bnum[j]) * 5.625) + 2.8125;
        else if (i==1) dQ[1].phi[j] = ((15 - dQ[1].Bnum[j]) * 5.625) + 92.8125;
        else if (i==0) dQ[0].phi[j] = ((15 - dQ[0].Bnum[j]) * 5.625) + 182.8125;
        else if (i==3) dQ[3].phi[j] = ((15 - dQ[3].Bnum[j]) * 5.625) + 272.8125;
            if(ibool) std::cout << "dQ["<<i<<"].phi["<<j<<"] = " << dQ[i].phi[j] << std::endl;
        }
    }
   

// Coordinates of world: +x is beam left, +y is towards the ceiling, +z is the beam direction
// For cylindrical coordinates: x = rho x cos(phi), y = rho x sin(phi), z = z.
// where rho = sqrt (x*x + y*y)
// phi = arctan(y/x)

// phi = arcsin(y/rho), if x>=0;
// phi = -1*arcsin(y/rho) + pi, if x<0 && y>=0;
// phi = -1*arcsin(y/rho)

    
          
//******************************************************
outputfile->cd();
//outT->Fill();
if(jentry%1000 == 0) std::cout << "Entry " << jentry << " of " << nevents << ", " << 100 * jentry/nevents << "\% complete";
std::cout << "\r" << std::flush;
} // end of event loop

//outputfile->cd();
//std::cout << "Printing" << std::endl; outT->Print();
//std::cout << "Writing" << std::endl; outT->Write();
//outputfile->Write();

inputfile->Close();
outputfile->Close();
delete inputfile;
delete outputfile;
}
//for (int i=0;i<6;i++){
//    dBD[i].Fmult = 0; dBU[i].Fmult = 0; dBU[i].detnum = -1;
//    for (int j=0; j<32;j++){
//        dBD[i].Fenergy[j] = -1.; dBD[i].Fnum[j] = -1;
//        dBU[i].Fenergy[j] = -1.; dBU[i].Fnum[j] = -1;
//        dBU[i].z[j] = -1.; dBU[i].phi[j] = -1000.; dBU[i].rho[j] = -1.;
//        dBU[i].Ftime[j] = -1;
//}}

//for (i=0;i<12;i++){
//    dSX[i].Fmult = 0; dSX[i].Bmult = 0; dSX[i].detnum = -1;
//    for(j=0;j<4;j++){
//    dSX[i].Fnum[j] = -1; dSX[i].Bnum[j] = -1; // fronts will need to change (frontup/front down)
//        dSX[i].Fenergy[j] = -1.; dSX[i].Benergy[j] = -1.;
//    dSX[i].z[j] = -1.; dSX[i].phi[j] = -1000.; dSX[i].rho[j] = -1.;
//        dSX[i].Ftime[j] = -1; dSX[i].Btime[j] = -1;
//}}

// outermult = 0;
//for (i=0;i<12;i++){
//    // looping over detector number. Gordon separates his into detectors and strip is looked up if there's a hit
//    for(auto& back : event->barrel[i].backs){
//                if(ibool) std::cout << " detector " << i << std::endl;
//        dSX[i].Benergy[dSX[i].Bmult] = back.energy;
//        dSX[i].Btime[dSX[i].Bmult] = back.timestamp;
//                if(ibool)std::cout << "mult = " << dSX[i].Bmult << std::endl;
//                if(ibool)std::cout << "dSX["<<i<<"].Benergy["<<dSX[i].Bmult<<"] = " << dSX[i].Benergy[dSX[i].Bmult] << std::endl;
//        auto iter = m_chanMap.FindChannel(back.globalChannel);
//        if(iter == m_chanMap.End()){
//            std::cout << "channel map error" << std::endl;
//        }else{
//            dSX[i].Bnum[dSX[i].Bmult] = iter->second.local_channel;
//                if(ibool)std::cout << "dSX["<<i<<"].Bnum["<<dSX[i].Bmult<<"] = " << dSX[i].Bnum[dSX[i].Bmult] << std::endl;
//            dSX[i].Bmult++;
//                if(ibool)std::cout << dSX[i].Bmult << " at end of loop" << std::endl;
//        }}
//
//    for(j=0;j<dSX[i].Bmult;j++){
//      if(i==0){ dSX[0].phi[j] = 247.5;
//      }else if(i>0 && i<9){ dSX[i].phi[j] = 247.5 - (i*22.5);
//      }else if(i==9){ dSX[9].phi[j] = 337.5;
//      }else if(i==10){ dSX[10].phi[j] = 315.0;
//      }else if(i==11){ dSX[11].phi[j] = 292.5;
//      }
//
//      outer_E[outermult] = dSX[i].Benergy[j];
//      outer_phi[outermult] = dSX[i].phi[j];
//      outermult++;
//    }
// } //  end loop over 12
// // need fronts


// **************************************************************************************************
// Fill barcelonas
//
//int innermult = 0;
//for(i=0;i<6;i++){
//     for(auto& front : event->barcDown[i].fronts){
//        dBD[i].Fenergy[dBD[i].Fmult] = front.energy;
//        dBD[i].Ftime[dBD[i].Fmult] = front.timestamp;
//        auto iter = m_chanMap.FindChannel(front.globalChannel);
//        if(iter == m_chanMap.End()){ std::cout << "channel map error, bdF" << std::endl;
//        }else{
//            dBD[i].Fnum[dBD[i].Fmult] = iter->second.local_channel;
//            dBD[i].Fmult++;}
//    } // exit this loop with arrays of size mult, with detector number and strip
//
//    for(j=0;j<dBD[i].Fmult;j++){
//        if(i==0){ dBD[0].phi[j] = 270;
//        }else if(i==5){ dBD[5].phi[j] = 330;
//        }else{ dBD[i].phi[j] = 270 - (i*60);
//    }
//    dBD[i].z[j] = BDzoffset + (dBD[i].Fnum[j]*2) + 1; // mm. BDZoffset is distance from z=0 to edge of strip 0.
//      // +1 mm brings z to the centre of the strip
//
//    inner_E[innermult] = dBD[i].Fenergy[j];
//    inner_phi[innermult] = dBD[i].phi[j];
//    innermult++;
//
//      }
//
//    for(auto& front : event->barcUp[i].fronts){
//        dBU[i].Fenergy[dBU[i].Fmult] = front.energy;
//        dBU[i].Ftime[dBU[i].Fmult] = front.timestamp;
//    auto iter = m_chanMap.FindChannel(front.globalChannel);
//    if(iter == m_chanMap.End()){ std::cout << "channel map error, BUf" << std::endl;
//    }else{
//        dBU[i].Fnum[dBU[i].Fmult] = iter->second.local_channel;
//        dBU[i].Fmult++;}
//    }
//
//    for(j=0;j<dBU[i].Fmult;j++){
//        if(i==0){ dBU[0].phi[j] = 270;
//        }else if(i==5){ dBU[5].phi[j] = 330;
//        }else{ dBU[i].phi[j] = 270 - (i*60);
//        }
//        dBU[i].z[j] = BUzoffset + (2*(32-dBU[i].Fnum[j])) + 1;
//        // BUzoffset needs to be distance from z=0 to upstream end of strip 32.
//
//    inner_E[innermult] = dBU[i].Fenergy[j];
//    inner_phi[innermult] = dBU[i].phi[j];
//    innermult++;
//    }
//} // end loop over 6
// ***************************************************
//    // At this point, in each event, we know which strips fired, the multiplicity
//    // the timestamp and the energy.
//    // Still need to look at interstrip hits
//    // I think we could benefit from an inner (any barc) and outer multiplicity (SX3 or QQQ)
//    // but this is really just dE-E.
//    // And then sort these to get the biggest inner and outer.
//
//
//
//    //                if(dBD[i].Fenergy[dBD[i].Fmult]>BDenergyMax){
//    //                    BDenergyMax = dBD[i].Fenergy[dBD[i].Fmult];
//    //                    BDnumMax = dBD[i].Fnum[dBD[i].Fmult];
//    //                    BDdetMax = i;
//    //                }
//
//if (BUenergyMax>BDenergyMax){
//        dE = BUenergyMax;
//        dEtheta = BUnumMax;
//}else{
//        dE = BDenergyMax;
//        dEtheta = BDnumMax;
//}
