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
    
    outT->Branch("SXFmult",&SXFmult,"SXFmult/i");
    outT->Branch("SXBmult",&SXBmult,"SXBmult/i");
    outT->Branch("SXFenergy",SXFenergy,"SXFenergy[SXFmult]/f");
    outT->Branch("SXBenergy",SXBenergy,"SXBenergy[SXBmult]/f");
    outT->Branch("SXFdetnum",SXFdetnum,"SXFdetnum[SXFmult]/i");
    outT->Branch("SXBdetnum",SXBdetnum,"SXBdetnum[SXBmult]/i");
    outT->Branch("SXFnum",SXFnum,"SXFnum[SXFmult]/i");
    outT->Branch("SXBnum",SXBnum,"SXBnum[SXBmult]/i");
    outT->Branch("SXFtime",SXFtime,"SXFtime[SXFmult]/l");
    outT->Branch("SXBtime",SXBtime,"SXBtime[SXBmult]/l");
    outT->Branch("SXBz",SXBz,"SXBz[SXBmult]/f");
    outT->Branch("SXFz",SXFz,"SXFz[SXFmult]/f");
    outT->Branch("SXrho",SXrho,"SXrho[SXBmult]/f");
    outT->Branch("SXphi",SXphi,"SXphi[SXFmult]/f");
    outT->Branch("Efrntup",Efrntup,"Efrntup[SXFmult]/f");
    outT->Branch("Efrntdwn",Efrntdwn,"Efrntdwn[SXFmult]/f");
    outT->Branch("totSXBenergy",&totSXBenergy,"totSXBenergy/f");
    
Long64_t nevents = tree->GetEntries();
Long64_t jentry;
    
    float rhosx[12] = {89.0354, 89.0354, 89.0247, 89.0354, 89.0354, 89.0247, 89.0354, 89.0354, 89.0247, 89.0354, 88.9871, 89.0601}; // mm; from Gordon
    
    // make channel map into a list of strip numbers:
    // awk '{printf $5 ","}' ANASEN_TRIUMFAug_run21+_ChannelMap.txt
    // printf = don't put new line; $5 = 5th column, then put comma.
    // note there were channels missing; they now read strip 100, because chan number = array element.

    int lookUp[640]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    
    // ***** read in calibration files *****
       ifstream SXB;
       SXB.open("SX3_backs.txt");
       for(i=0;i<48;i++){
           SXB >> SXBch[i] >> SXBoffset[i] >> SXBgain[i];
       }
       SXB.close();
    
    ifstream SXfvb;
    SXfvb.open("SX3-frontback.txt");
    for(i=0;i<48;i++){
        SXfvb >> SXFch[i] >> SXFoffset[i] >> SXFgain[i];
        std::cout << SXFoffset[i] << " " << SXFgain[i] << std::endl;
    }
    SXfvb.close();

for (jentry=0; jentry<nevents; jentry++){
    tree->GetEntry(jentry);
	
//*************************************************************************
//*************** Call the exterminator! You've got bugs! *****************
/* ************************ */ bool ibool = 0; /* ********************** */
//*************************************************************************
//*************************************************************************

    ringHit = false; BUhit = false; BDhit = false;
    QFmult = QBmult = 0;
    
    for(i=0;i<40;i++){
        QFenergy[i] = QBenergy[i] = -1.;
        QFdetnum[i] = QBdetnum[i] = -1;
        QFnum[i] = QBnum[i] = -1;
        QFtime[i] = QBtime[i] = -1.;
        Qz[i] = -1.; Qrho[i] = -1.; Qphi[i] = -1000.;
    }

    BUmult = BDmult = 0;
    for(i=0;i<60;i++){
        BUenergy[i] = -1.; BDenergy[i] = -1.;
        BUdetnum[i] = 100; BDdetnum[i] = 100;
        BUnum[i] = BDnum[i] = -1;
        BUtime[i] = BDtime[i] = -1.;
        BUz[i] = BUrho[i] = -1; BUphi[i] = -1000.;
        BDz[i] = BDrho[i] = -1; BDphi[i] = -1000.;
    }
    
    SXFmult = 0; SXBmult = 0;
    for(i=0;i<50;i++){
        SXFenergy[i] = -1.; SXBenergy[i] = -1.;
        SXFdetnum[i] = 100; SXBdetnum[i] = 100;
        SXFnum[i] = -1; SXBnum[i] = -1;
        SXFtime[i] = -1.; SXBtime[i] = -1.;
        SXBz[i] = -1000.; SXrho[i] = -1.; SXphi[i] = -1000.;
        SXFz[i] = -1000.; Efrntup[i] = -1; Efrntdwn[i] = -1;
    }
    Estrip = -5.; diff = -5.; Eratio=-5.; coeff=-5.;
    Eback = -5.;
    totSXBenergy = -1.;
    
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
          if(ring.energy>50){
            QFenergy[QFmult] = ring.energy;
            QFtime[QFmult] = ring.timestamp;
            
            if(i==2){ // need to swap QQQ2 & 3
                QFdetnum[QFmult] = 3;
            } else if(i==3){
                QFdetnum[QFmult] = 2;
            } else{
                QFdetnum[QFmult] = i;
            }
            
              // swapping ring count direction for QQQ0&2
            if(QFdetnum[QFmult]==0||QFdetnum[QFmult]==2){
                QFnum[QFmult] = 15 - lookUp[ring.globalChannel];
            }else{
                QFnum[QFmult] = lookUp[ring.globalChannel];
             }
   
            Qz[QFmult] = QQQzpos;
            Qrho[QFmult]= 50.1 + (QFnum[QFmult]+0.5)*(99.-50.1)/16.; // rho in mm
            
            if(ibool){ std::cout << "E " << QFenergy[QFmult] << " t " << QFtime[QFmult] << std::endl;
            std::cout << "det " << QFdetnum[QFmult] << " strip " << QFnum[QFmult] << " z " <<Qz[QFmult];
            std::cout <<" rho " << Qrho[QFmult] << std::endl;}
             QFmult++; ringHit = true;
                                               
            }
        }
    }


    for(i=0;i<4;i++){
	for(auto& wedge : event->fqqq[i].wedges){
    if(wedge.energy>50){
        if(ibool) std::cout << "QQQ wedge " << i << std::endl;
        QBenergy[QBmult] = wedge.energy;
        QBtime[QBmult] = wedge.timestamp;
       
        if(i==2){QBdetnum[QBmult] = 3;} // need to swap QQQ2 & 3
        else if(i==3){QBdetnum[QBmult] = 2;}
        else{QBdetnum[QBmult] = i;}
        
        // swapping wedge count direction for QQQ0&2
            if(QBdetnum[QBmult]==0||QBdetnum[QBmult]==2){
                QBnum[QBmult] = 15 - lookUp[wedge.globalChannel];
            }else{
                QBnum[QBmult] = lookUp[wedge.globalChannel];
            }
                 
        float phiqb = 267.158-(90.*QBdetnum[QBmult]) - (QBnum[QBmult]+0.5)*87.158/16.; // Keilah's way
        Qphi[QBmult] = (phiqb < 0.) ? (phiqb + 360.) : (phiqb); //get phi from wedges; if phi < 0, then add 360
    if(ibool){ std::cout << "E "<<QBenergy[QBmult]<<" t "<<QBtime[QBmult] << std::endl;
               std::cout << "det "<<QBdetnum[QBmult]<<" strip "<<QBnum[QBmult]<<" phi "<<Qphi[QBmult]<<std::endl;}
             QBmult++;
            }
        }
    } // end loop over 4.
    
// ***************************************************
    
// Fill barcelonas
    //if(ringHit){ // COMMENT OUT WHEN CALIBRATING!!
    for(i=0;i<6;i++){
     for(auto& front : event->barcDown[i].fronts){
        if(front.energy>0){
            BDenergy[BDmult] = front.energy;
            BDtime[BDmult] = front.timestamp;
            BDdetnum[BDmult] = i;
            BDnum[BDmult] = lookUp[front.globalChannel];
            float phibd = 270. - (60*BDdetnum[BDmult]);
            BDphi[BDmult] = (phibd<0.) ? (phibd+360.) : (phibd);
            BDrho[BDmult] = 30.;
            BDz[BDmult] = BDZoffset - 2*(BDnum[BDmult] + 0.5); // mm.
            BDmult++; BDhit = true;
        }
        }
    }
    
    if(ibool) std::cout << "postloop dBmult = " << BDmult << std::endl;
    
    for(i=0;i<6;i++){
     for(auto& front : event->barcUp[i].fronts){
         if(front.energy>0){
            BUenergy[BUmult] = front.energy;
            BUtime[BUmult] = front.timestamp;
            BUdetnum[BUmult] = i;
            BUnum[BUmult] = lookUp[front.globalChannel];
            float phibu = 270. - (60.*BUdetnum[BUmult]);
            BUphi[BUmult] = (phibu<0.) ? (phibu+360.) : (phibu);
            BUz[BUmult] = BUZoffset + 2*(BUnum[BUmult]+0.5);
            BUrho[BUmult] = 30.;
            BUmult++; BUhit = true;
               }
     }
    }// end loop over 6
    if(ibool) std::cout << "postloop BUmult = " << BUmult << std::endl;

    // ***************************************************
        
    // Fill SX3s
    // Energy comes from the backs
    // the only thing the fronts are used for is the position
    // take the biggest signal out of frontUp and frontDown and ratio it
    // to the back energy.
    // OR do (up-down)/(up+down)
    
    // the back strips are constant in z, but the more accurate z position is from the fronts.
        
    // up 1 down 0
    // up 3 down 2
    // up 4 down 5
    // up 6 down 7
   if(ibool) std::cout << jentry << std::endl;
        Eback = -5;
for(i=0;i<12;i++){
    for(auto& back : event->barrel[i].backs){
        Eback = back.energy;
        if(Eback>0){
            SXBtime[SXBmult] = back.timestamp;
            SXBdetnum[SXBmult] = i;
            SXBnum[SXBmult] = lookUp[back.globalChannel];
            SXBenergy[SXBmult] = back.energy*SXBgain[(i*4)+SXBnum[SXBmult]] + SXBoffset[(i*4)+SXBnum[SXBmult]];
            totSXBenergy += SXBenergy[SXBmult];
            SXrho[SXBmult] = rhosx[SXBdetnum[SXBmult]];
            SXBz[SXBmult] = SXZoffset + (SXBnum[SXBmult]*75/4) + 75/8; // mm, mid strip for z
            if(ibool) std::cout << jentry << " BACK: det " << i << " strip " << SXBnum[SXBmult] << " energy " << back.energy << " t = " << back.timestamp << " mult " << SXBmult << std::endl;
            SXBmult++;
        }} // end of backs
    
    if(Eback>0){
        for(auto& frontup : event->barrel[i].frontsUp){
        for(auto & frontdown : event->barrel[i].frontsDown){
            SXFdetnum[SXFmult] = i;
                 if(ibool) std::cout << "front det " << i << std::endl;
        if(i==0){SXphi[SXFmult] = 247.5;
            }else if(i>0 && i<9){ SXphi[SXFmult] = 247.5 - (i*22.5);
            }else if(i==9){ SXphi[SXFmult] = 337.5;
            }else if(i==10){ SXphi[SXFmult] = 315.0;
            }else if(i==11){ SXphi[SXFmult] = 292.5;}
                
            upnum = -5; downnum = -5; up = false; down = false; upstrp = -5; dwnstrp = -5;
            Efrntup[SXFmult] = frontup.energy;
            if(Efrntup[SXFmult]>0){ up = true;
                     if(ibool)std::cout << "up " << Efrntup[SXFmult] << ", t " << frontup.timestamp;
                }
            
            Efrntdwn[SXFmult] = frontdown.energy;
                if(Efrntdwn[SXFmult]>0){ down = true;
                     if(ibool)std::cout << " down " << Efrntdwn[SXFmult] <<", t = " << frontdown.timestamp << std::endl;
                }
          
            upnum = lookUp[frontup.globalChannel];
            downnum = lookUp[frontdown.globalChannel];
        
                    if(ibool) std::cout << "upstrp = " << upnum << ", dwnstrp = " << downnum << std::endl;
                    
            switch (upnum) {
                case 1: upstrp = 0; break;
                case 3: upstrp = 1; break;
                case 4: upstrp = 2; break;
                case 6: upstrp = 3; break;
            }
                if(ibool)std::cout << "after up switch, strip " << upstrp;
            switch (downnum) {
                case 0: dwnstrp = 0; break;
                case 2: dwnstrp = 1; break;
                case 5: dwnstrp = 2; break;
                case 7: dwnstrp = 3; break;
            }
                 if(ibool) std::cout << ", after dwn switch, strip " << dwnstrp << std::endl;
                // this isn't going to work if you have only one end on two strips firing
            //std::cout << "before " << Efrntup[SXFmult];
            //Efrntup[SXFmult] = (Efrntup[SXFmult]*SXFgain[(i*4)+upstrp]*-1) + SXFoffset[(i*4)+upstrp];
            Efrntdwn[SXFmult] = (Efrntdwn[SXFmult]*SXFgain[(i*4)+dwnstrp]*-1) + SXFoffset[(i*4)+dwnstrp];
            //std::cout<< ", after " << Efrntup[SXFmult] << std::endl;
                    
    if(up && down && upstrp==dwnstrp){
        if(ibool)std::cout << "we have a strip with both ends firing!" << std::endl;
        SXFnum[SXFmult] = upstrp;
        SXFtime[SXFmult] = frontup.timestamp;
        Estrip = Efrntdwn[SXFmult] +Efrntup[SXFmult];
        SXFenergy[SXFmult] = Estrip;
        diff = Eback - Estrip;
        Eratio = Efrntup[SXFmult]/Efrntdwn[SXFmult];
        coeff = (Efrntup[SXFmult]+diff-(Efrntdwn[SXFmult]*Eratio))/(diff * (1+Eratio));
            if(Efrntdwn[SXFmult]>=Efrntup[SXFmult]){
                SXFz[SXFmult] = (2*(Efrntdwn[SXFmult]+(coeff*diff))/Eback) - 1;
               if(ibool) std::cout << "down>up, z = " << SXFz[SXFmult] << std::endl;
                SXFmult++;
            }else{
                SXFz[SXFmult] = 1 - (2*(Efrntup[SXFmult]+(1-coeff)*diff))/Eback;
               if(ibool) std::cout << "down<up, z = " << SXFz[SXFmult] << std::endl;
                SXFmult++;
                }
    }else if(up && down && upstrp!=dwnstrp){
                // probably nothing is going to happen here
                // this is the case that you have two different fronts firing
                // but only one end of each.
                // not going to increase the multiplicity here and just have
                // it get written over
    }else if(!up && down){
        SXFz[SXFmult] = (2*Efrntdwn[SXFmult]/Eback) - 1;
        SXFnum[SXFmult] = dwnstrp;
        SXFtime[SXFmult] = frontdown.timestamp;
        if(ibool)std::cout << "down only z = " << SXFz[SXFmult] << std::endl;
        SXFmult++;
    }else if(up && !down){
        SXFz[SXFmult] = 1 - (2*Efrntup[SXFmult]/Eback);
        SXFnum[SXFmult] = upstrp;
        SXFtime[SXFmult] = frontup.timestamp;
        if(ibool)std::cout << "up only, z = " << SXFz[SXFmult] << std::endl;
                    SXFmult++;
    }else{
        std::cout << jentry << ": no front strip" << std::endl;
        }
    }
            } // end of frontsUp
        } // end of Eback>0
      if(ibool)  if(SXFmult>0||SXBmult>0) std::cout << "front mult = " << SXFmult << ", back = " << SXBmult << std::endl;
} // end of loop over 12
          
// *****************************************************
outputfile->cd();
outT->Fill();
   // } // end of ring=true condition

if(jentry%5000 == 0) std::cout << "Entry " << jentry << " of " << nevents << ", " << 100 * jentry/nevents << "\% complete";
std::cout << "\r" << std::flush;
  //  std::cout << std::endl;
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

