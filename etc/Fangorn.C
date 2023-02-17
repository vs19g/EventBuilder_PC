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
    //branches here? or call to it?
    outT->Branch("QFmult",&QFmult,"QFmult/i");
    outT->Branch("QBmult",&QBmult,"QBmult/i");
    outT->Branch("QFenergy",QFenergy,"QFenergy[QFmult]/f");
    outT->Branch("QBenergy",QBenergy,"QBenergy[QBmult]/f");
    outT->Branch("QFdetnum",QFdetnum,"QFdetnum[QFmult]/i");
    outT->Branch("QBdetnum",QBdetnum,"QBdetnum[QBmult]/i");
    outT->Branch("QFnum",QFnum,"QFnum[QFmult]/i");
    outT->Branch("QBnum",QBnum,"QBnum[QBmult]/i");
    outT->Branch("QFtime",QFtime,"QFtime[QFmult]/l");
    outT->Branch("QBtime",QBtime,"QBtime[QBmult]/l");
    outT->Branch("Qz",Qz,"Qz[QFmult]/f");
    outT->Branch("Qrho",Qrho,"Qrho[QFmult]/f");
    outT->Branch("Qphi",Qphi,"Qphi[QBmult]/f");
    
    outT->Branch("BUmult",&BUmult,"BUmult/i");
    outT->Branch("BDmult",&BDmult,"BDmult/i");
    outT->Branch("BUenergy",BUenergy,"BUenergy[BUmult]/f");
    outT->Branch("BDenergy",BDenergy,"BDenergy[BDmult]/f");
    outT->Branch("BUdetnum",BUdetnum,"BUdetnum[BUmult]/i");
    outT->Branch("BDdetnum",BDdetnum,"BDdetnum[BDmult]/i");
    outT->Branch("BUnum",BUnum,"BUnum[BUmult]/i");
    outT->Branch("BDnum",BDnum,"BDnum[BDmult]/i");
    outT->Branch("BUtime",BUtime,"BUtime[BUmult]/l");
    outT->Branch("BDtime",BDtime,"BDtime[BDmult]/l");
    outT->Branch("BUz",BUz,"BUz[BUmult]/f");
    outT->Branch("BUrho",BUrho,"BUrho[BUmult]/f");
    outT->Branch("BUphi",BUphi,"BUphi[BUmult]/f");
    outT->Branch("BDz",BDz,"BDz[BDmult]/f");
    outT->Branch("BDrho",BDrho,"BDrho[BDmult]/f");
    outT->Branch("BDphi",BDphi,"BDphi[BDmult]/f");
    
Long64_t nevents = tree->GetEntries();
Long64_t jentry;
    
    
    // make channel map into a list of strip numbers:
    // awk '{printf $5 ","}' ANASEN_TRIUMFAug_run21+_ChannelMap.txt
    // printf = don't put new line; $5 = 5th column, then put comma.
    // note there were channels missing; they now read strip 100, because chan number = array element.
    int lookUp[640]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

for (jentry=0; jentry<nevents; jentry++){
    tree->GetEntry(jentry);
	
//*************************************************************************
//*************** Call the exterminator! You've got bugs! *****************
/* ************************ */ bool ibool = 0; /* ************************* */
//*************************************************************************
//*************************************************************************

    ringHit = false; BUhit = false; BDhit = false;
    QFmult = QBmult = 0;
    
    for(i=0;i<16;i++){
        QFenergy[i] = QBenergy[i] = -1.;
        QFdetnum[i] = QBdetnum[i] = -1;
        QFnum[i] = QBnum[i] = -1;
        QFtime[i] = QBtime[i] = -1.;
        Qz[i] = -1.; Qrho[i] = -1.; Qphi[i] = -1000.;
    }

    BUmult = BDmult = 0;
    for(i=0;i<32;i++){
        BUenergy[i] = -1.; BDenergy[i] = -1.;
        BUdetnum[i] = 100; BDdetnum[i] = 100;
        BUnum[i] = BDnum[i] = -1;
        BUtime[i] = BDtime[i] = -1.;
        BUz[i] = BUrho[i] = -1; BUphi[i] = -1000.;
        BDz[i] = BDrho[i] = -1; BDphi[i] = -1000.;
    }
    
//****************************************************************************

// Gordon's tree contains the following:
// &event.fqqq[i].rings; &event.fqqq[i].wedges
// &event.barrel[0].frontsUp; &event.barrel[0].frontsDown; &event.barrel[0].backs;
// &event.barcUp[0].fronts; &event.barcUp[0].backs;
// &event.barcDown[0].fronts; &event.barcDown[0].backs;
// Each one of these has associated with it: 
// globalChannel; energy; timestamp
// e.g. &event.fqqq[2].rings[3].energy

    for(i=0;i<4;i++){
        for(auto& ring : event->fqqq[i].rings){
        if(ibool) std::cout << std::endl << "Entry " << jentry << " QQQ ring " << i << std::endl;
          QFenergy[QFmult] = ring.energy;
          QFtime[QFmult] = ring.timestamp;
            
            if(i==2){QFdetnum[QFmult] = 3;} // need to swap QQQ2 & 3
            else if(i==3){QFdetnum[QFmult] = 2;}
            else{QFdetnum[QFmult] = i;}
            
          QFnum[QFmult] = lookUp[ring.globalChannel];
   
            Qz[QFmult] = QQQzpos;
                Qrho[QFmult]= (QFnum[QFmult]+0.5)*(0.099-0.0501)/16; // rho in mm
                if(ibool){ std::cout << "E "<<QFenergy[QFmult]<<" t "<<QFtime[QFmult] << std::endl;
                    std::cout << "det "<<QFdetnum[QFmult]<<" strip "<<QFnum[QFmult]<<" z "<<Qz[QFmult]<<" rho "<<Qrho[QFmult]<<std::endl;}
            if(ring.energy>50){ QFmult++; ringHit = true;}
        }
    }


    for(i=0;i<4;i++){
	for(auto& wedge : event->fqqq[i].wedges){
        if(ibool) std::cout << "QQQ wedge " << i << std::endl;
        QBenergy[QBmult] = wedge.energy;
        QBtime[QBmult] = wedge.timestamp;
        QBdetnum[QBmult] = i;
        if(i==2) Qphi[QBmult] = ((15 - QBnum[QBmult]) * 5.625) + 2.8125;
        else if (i==1) Qphi[QBmult] = ((15 - QBnum[QBmult]) * 5.625) + 92.8125;
        else if (i==0) Qphi[QBmult] = ((15 - QBnum[QBmult]) * 5.625) + 182.8125;
        else if (i==3) Qphi[QBmult] = ((15 - QBnum[QBmult]) * 5.625) + 272.8125;
        
        QBnum[QBmult] = lookUp[wedge.globalChannel];
     
    if(ibool){ std::cout << "E "<<QBenergy[QBmult]<<" t "<<QBtime[QBmult] << std::endl;
               std::cout << "det "<<QBdetnum[QBmult]<<" strip "<<QBnum[QBmult]<<" phi "<<Qphi[QBmult]<<std::endl;}
            if(wedge.energy>50) QBmult++;
        }
    } // end loop over 4.
    
// Fill barcelonas
    if(ringHit){
    for(i=0;i<6;i++){
         for(auto& front : event->barcDown[i].fronts){
            BDenergy[BDmult] = front.energy;
            BDtime[BDmult] = front.timestamp;
            BDdetnum[BDmult] = i;
            BDnum[BDmult] = lookUp[front.globalChannel];
            
             if(i==0){ BDphi[BDmult] = 270;
             }else if(i==5){ BDphi[BDmult] = 330;
             }else{ BDphi[BDmult] = 270 - (i*60);
             }
             BDz[BDmult] = BDzoffset + (BDnum[BDmult]*2) + 1; // mm.
             if(front.energy>0) BDmult++; BDhit = true;
        }
    }
    
    if(ibool) std::cout << "postloop dBmult = " << BDmult << std::endl;
    
    for(i=0;i<6;i++){
        for(auto& front : event->barcUp[i].fronts){
            BUenergy[BUmult] = front.energy;
            BUtime[BUmult] = front.timestamp;
            BUdetnum[BUmult] = i;
            BUnum[BUmult] = lookUp[front.globalChannel];
            
            if(i==0){ BUphi[BUmult] = 270;
                }else if(i==5){ BUphi[BUmult] = 330;
                }else{ BUphi[BUmult] = 270 - (i*60);
                }
            BUz[BUmult] = BUzoffset + (2*(32-BUnum[BUmult])) + 1;
            if(front.energy>0) BUmult++; BUhit = true;
               }
 
    }// end loop over 6
    if(ibool) std::cout << "postloop BUmult = " << BUmult << std::endl;

    // ***************************************************
    
    
    
    
// Coordinates of world: +x is beam left, +y is towards the ceiling, +z is the beam direction
// For cylindrical coordinates: x = rho x cos(phi), y = rho x sin(phi), z = z.
// where rho = sqrt (x*x + y*y)
// phi = arctan(y/x)

// phi = arcsin(y/rho), if x>=0;
// phi = -1*arcsin(y/rho) + pi, if x<0 && y>=0;
// phi = -1*arcsin(y/rho)


          
// *****************************************************
outputfile->cd();
outT->Fill();
    } // end of ring=true condition

if(jentry%5000 == 0) std::cout << "Entry " << jentry << " of " << nevents << ", " << 100 * jentry/nevents << "\% complete";
std::cout << "\r" << std::flush;
    } // end of event loop

outputfile->cd();
std::cout << "Printing" << std::endl; outT->Print();
std::cout << "Writing" << std::endl; outT->Write();
outputfile->Write();

inputfile->Close();
outputfile->Close();
delete inputfile;
delete outputfile;
}



// **************************************************************************************************

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

