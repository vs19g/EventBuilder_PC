#include "Reconstruct.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <iostream>
#include <string>
#include <cmath> // M_PI,std::cos,std::atan (in radians)

/*
	Coordinates of world: +x is beam right, +y is towards the floor, +z is the beam direction
	For cylindrical coordinates: rho, phi, z
		x = rho * cos(phi)
		y = rho * sin(phi)
		z = z 
	Math to get reaction vertex: x=y=0
		start with no straggling
		IntZ = Qz - Qrho( (Qz-Bz)/(Qrho-Brho) )
		IntTheta = arctan(Qrho/(Qz-IntZ))
		IntPath = (Qz-IntZ)/cos(IntTheta)
	units: deg, mm
*/

void Reconstruct(int runNumber)
{
	bool ibool = 0; //debug
	double eQmin = 50.; //QQQ energy threshold
	double eBmin = 0.; //barc energy threshold

	std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F_pruned.root";
	std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F_reconstructed.root";

	std::cout<<"Processing data in "<<input_filename<<std::endl;

	// ***** Input File *****
	TFile* inputfile = TFile::Open(input_filename.c_str(), "READ");
	if(inputfile == nullptr || !inputfile->IsOpen()){
		std::cout<<"Could not find file "<<input_filename<<". Skipping."<<std::endl;
		delete inputfile;
		return;
	}
	TTree* inTree = (TTree*) inputfile->Get("BarcQQQTreeData");
	if(inTree == nullptr){
		std::cout<<"No tree named BarcQQQTreeData found inside file "<<input_filename<<". Skipping."<<std::endl;
		inputfile->Close();
		delete inputfile;
		return;
	}

	// ***** Output File *****
	TFile* outputfile = TFile::Open(output_filename.c_str(), "RECREATE");
	TTree *outTree = new TTree("tBarcQQQ1","Barc+QQQ events for QQQmult=1");
	std::cout<<"Opening "<<output_filename<<std::endl;

	// ***** initialize *****
	uint dEmult, dEstrip, EmultF, EmultB, EstripF, EstripB;
	int dEdet, Edet, flagB, flagQ, flagUpDn;
	float dEz, dEphi, dErho, Ez, Ephi, Erho, IntZ, IntTheta, IntPath;
	double dEtime, dE, EtFront, EtBack, Ef, Eb, EroughF, EroughB; 

	std::string name;
	ULong64_t entryNum;

	// ***** output tree *****
	outTree->Branch("EntryNo",&entryNum,"EntryNo/l"); // row of pruned tree
	outTree->Branch("flagQ",&flagQ,"flagQ/I"); // see flag key in event loop
	outTree->Branch("flagB",&flagB,"flagB/I"); // see flag key in event loop
	
	outTree->Branch("dEmult",&dEmult,"dEmult/i"); // detector mult above threshold
	outTree->Branch("dEt",&dEtime,"dEt/D"); // time
	outTree->Branch("dE",&dE,"dE/D"); // dE from inner
	outTree->Branch("dEz",&dEz,"dEz/F"); // z from inner
	outTree->Branch("dEphi",&dEphi,"dEphi/F"); // phi from inner
	outTree->Branch("dErho",&dErho,"dErho/F"); // rho from inner
	outTree->Branch("dEdet",&dEdet,"dEdet/i"); // detector inner
	outTree->Branch("dEstrip",&dEstrip,"dEstrip/i"); // strip number
	outTree->Branch("flagUpDn",&flagUpDn,"flagUpDn/I"); // up (+1) or downstream (-1) barc
	
	outTree->Branch("EmultF",&EmultF,"EmultF/i"); outTree->Branch("EmultB",&EmultB,"EmultB/i"); // detector mult above threshold
	outTree->Branch("EtF",&EtFront,"EtF/D"); outTree->Branch("EtB",&EtBack,"EtB/D"); // front & back times
	outTree->Branch("Ef",&Ef,"Ef/D"); outTree->Branch("Eb",&Eb,"Eb/D"); // E from outer
	outTree->Branch("EroughF",&EroughF,"EroughF/D"); outTree->Branch("EroughB",&EroughB,"EroughB/D"); // rough E calibration (same gain all channels)
	outTree->Branch("Ez",&Ez,"Ez/F"); // z from outer (z for QQQ is a constant, z for SX3s requires calibration first)
	outTree->Branch("Ephi",&Ephi,"Ephi/F"); // phi from outer
	outTree->Branch("Erho",&Erho,"Erho/F"); // rho from outer
	outTree->Branch("Edet",&Edet,"Edet/i"); // detector number
	outTree->Branch("EstripF",&EstripF,"EstripF/i"); outTree->Branch("EstripB",&EstripB,"EstripB/i"); // strip number

	outTree->Branch("IntZ",&IntZ,"IntZ/F"); // z for interaction point (x=y=0)
	outTree->Branch("IntTheta",&IntTheta,"IntTheta/F"); // angle from interaction point (x=y=0) to outer detector hit
	outTree->Branch("IntPath",&IntPath,"IntPath/F"); // path length from interaction point (x=y=0) to outer detector hit

	// ***** set branch addresses for old Fangorn tree *****
	uint QQQmult,BarcMult;
	dataE dQ[4];
	dataDE dBU[6];
	dataDE dBD[6];

	inTree->SetBranchAddress("QQQmult",&QQQmult); //store overall QQQ front multiplicity (for pruning)
	
	inTree->SetBranchAddress("Q0Fmult",&dQ[0].Fmult); inTree->SetBranchAddress("Q0Bmult",&dQ[0].Bmult);
	inTree->SetBranchAddress("Q0Fnum",&dQ[0].Fnum); inTree->SetBranchAddress("Q0Bnum",&dQ[0].Bnum);
	inTree->SetBranchAddress("Q0Fenergy",&dQ[0].Fenergy); inTree->SetBranchAddress("Q0Benergy",&dQ[0].Benergy);
	inTree->SetBranchAddress("Q0FroughCal",&dQ[0].FroughCal); inTree->SetBranchAddress("Q0BroughCal",&dQ[0].BroughCal);
	inTree->SetBranchAddress("Q0Btime",&dQ[0].Btime); //storing time from backs
	inTree->SetBranchAddress("Q0Ftime",&dQ[0].Ftime); //storing time from fronts
	inTree->SetBranchAddress("Q0rho",&dQ[0].rho); //rho from fronts
	inTree->SetBranchAddress("Q0phi",&dQ[0].phi); //phi from backs

	inTree->SetBranchAddress("Q1Fmult",&dQ[1].Fmult); inTree->SetBranchAddress("Q1Bmult",&dQ[1].Bmult);
	inTree->SetBranchAddress("Q1Fnum",&dQ[1].Fnum); inTree->SetBranchAddress("Q1Bnum",&dQ[1].Bnum);
	inTree->SetBranchAddress("Q1Fenergy",&dQ[1].Fenergy); inTree->SetBranchAddress("Q1Benergy",&dQ[1].Benergy);
	inTree->SetBranchAddress("Q1FroughCal",&dQ[1].FroughCal); inTree->SetBranchAddress("Q1BroughCal",&dQ[1].BroughCal);
	inTree->SetBranchAddress("Q1Btime",&dQ[1].Btime); //storing time from backs
	inTree->SetBranchAddress("Q1Ftime",&dQ[1].Ftime); //storing time from fronts
	inTree->SetBranchAddress("Q1rho",&dQ[1].rho); //rho from fronts
	inTree->SetBranchAddress("Q1phi",&dQ[1].phi); //phi from backs

	inTree->SetBranchAddress("Q2Fmult",&dQ[2].Fmult); inTree->SetBranchAddress("Q2Bmult",&dQ[2].Bmult);
	inTree->SetBranchAddress("Q2Fnum",&dQ[2].Fnum); inTree->SetBranchAddress("Q2Bnum",&dQ[2].Bnum);
	inTree->SetBranchAddress("Q2Fenergy",&dQ[2].Fenergy); inTree->SetBranchAddress("Q2Benergy",&dQ[2].Benergy);
	inTree->SetBranchAddress("Q2FroughCal",&dQ[2].FroughCal); inTree->SetBranchAddress("Q2BroughCal",&dQ[2].BroughCal);
	inTree->SetBranchAddress("Q2Btime",&dQ[2].Btime); //storing time from backs
	inTree->SetBranchAddress("Q2Ftime",&dQ[2].Ftime); //storing time from fronts
	inTree->SetBranchAddress("Q2rho",&dQ[2].rho); //rho from fronts
	inTree->SetBranchAddress("Q2phi",&dQ[2].phi); //phi from backs

	inTree->SetBranchAddress("Q3Fmult",&dQ[3].Fmult); inTree->SetBranchAddress("Q3Bmult",&dQ[3].Bmult);
	inTree->SetBranchAddress("Q3Fnum",&dQ[3].Fnum); inTree->SetBranchAddress("Q3Bnum",&dQ[3].Bnum);
	inTree->SetBranchAddress("Q3Fenergy",&dQ[3].Fenergy); inTree->SetBranchAddress("Q3Benergy",&dQ[3].Benergy);
	inTree->SetBranchAddress("Q3FroughCal",&dQ[3].FroughCal); inTree->SetBranchAddress("Q3BroughCal",&dQ[3].BroughCal);
	inTree->SetBranchAddress("Q3Btime",&dQ[3].Btime); //storing time from backs
	inTree->SetBranchAddress("Q3Ftime",&dQ[3].Ftime); //storing time from fronts
	inTree->SetBranchAddress("Q3rho",&dQ[3].rho); //rho from fronts
	inTree->SetBranchAddress("Q3phi",&dQ[3].phi); //phi from backs
	
	inTree->SetBranchAddress("BarcMult",&BarcMult); //store overall Barc multiplicity (for pruning)
	
	inTree->SetBranchAddress("BU0Fmult",&dBU[0].Fmult);
	inTree->SetBranchAddress("BU0Fnum",&dBU[0].Fnum);
	inTree->SetBranchAddress("BU0Fenergy",&dBU[0].Fenergy);
	inTree->SetBranchAddress("BU0time",&dBU[0].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BU0phi",&dBU[0].phi); //phi from barc #
	inTree->SetBranchAddress("BU0z",&dBU[0].z); //z from fronts

	inTree->SetBranchAddress("BU1Fmult",&dBU[1].Fmult);
	inTree->SetBranchAddress("BU1Fnum",&dBU[1].Fnum);
	inTree->SetBranchAddress("BU1Fenergy",&dBU[1].Fenergy);
	inTree->SetBranchAddress("BU1time",&dBU[1].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BU1phi",&dBU[1].phi); //phi from barc #
	inTree->SetBranchAddress("BU1z",&dBU[1].z); //z from fronts

	inTree->SetBranchAddress("BU2Fmult",&dBU[2].Fmult);
	inTree->SetBranchAddress("BU2Fnum",&dBU[2].Fnum);
	inTree->SetBranchAddress("BU2Fenergy",&dBU[2].Fenergy);
	inTree->SetBranchAddress("BU2time",&dBU[2].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BU2phi",&dBU[2].phi); //phi from barc #
	inTree->SetBranchAddress("BU2z",&dBU[2].z); //z from fronts

	inTree->SetBranchAddress("BU3Fmult",&dBU[3].Fmult);
	inTree->SetBranchAddress("BU3Fnum",&dBU[3].Fnum);
	inTree->SetBranchAddress("BU3Fenergy",&dBU[3].Fenergy);
	inTree->SetBranchAddress("BU3time",&dBU[3].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BU3phi",&dBU[3].phi); //phi from barc #
	inTree->SetBranchAddress("BU3z",&dBU[3].z); //z from fronts

	inTree->SetBranchAddress("BU4Fmult",&dBU[4].Fmult);
	inTree->SetBranchAddress("BU4Fnum",&dBU[4].Fnum);
	inTree->SetBranchAddress("BU4Fenergy",&dBU[4].Fenergy);
	inTree->SetBranchAddress("BU4time",&dBU[4].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BU4phi",&dBU[4].phi); //phi from barc #
	inTree->SetBranchAddress("BU4z",&dBU[4].z); //z from fronts

	inTree->SetBranchAddress("BU5Fmult",&dBU[5].Fmult);
	inTree->SetBranchAddress("BU5Fnum",&dBU[5].Fnum);
	inTree->SetBranchAddress("BU5energy",&dBU[5].Fenergy);
	inTree->SetBranchAddress("BU5time",&dBU[5].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BU5phi",&dBU[5].phi); //phi from barc #
	inTree->SetBranchAddress("BU5z",&dBU[5].z); //z from fronts

	inTree->SetBranchAddress("BD0Fmult",&dBD[0].Fmult);
	inTree->SetBranchAddress("BD0Fnum",&dBD[0].Fnum);
	inTree->SetBranchAddress("BD0Fenergy",&dBD[0].Fenergy);
	inTree->SetBranchAddress("BD0time",&dBD[0].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BD0phi",&dBD[0].phi); //phi from barc #
	inTree->SetBranchAddress("BD0z",&dBD[0].z); //z from fronts

	inTree->SetBranchAddress("BD1Fmult",&dBD[1].Fmult);
	inTree->SetBranchAddress("BD1Fnum",&dBD[1].Fnum);
	inTree->SetBranchAddress("BD1Fenergy",&dBD[1].Fenergy);
	inTree->SetBranchAddress("BD1time",&dBD[1].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BD1phi",&dBD[1].phi); //phi from barc #
	inTree->SetBranchAddress("BD1z",&dBD[1].z); //z from fronts

	inTree->SetBranchAddress("BD2Fmult",&dBD[2].Fmult);
	inTree->SetBranchAddress("BD2Fnum",&dBD[2].Fnum);
	inTree->SetBranchAddress("BD2Fenergy",&dBD[2].Fenergy);
	inTree->SetBranchAddress("BD2time",&dBD[2].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BD2phi",&dBD[2].phi); //phi from barc #
	inTree->SetBranchAddress("BD2z",&dBD[2].z); //z from fronts

	inTree->SetBranchAddress("BD3Fmult",&dBD[3].Fmult);
	inTree->SetBranchAddress("BD3Fnum",&dBD[3].Fnum);
	inTree->SetBranchAddress("BD3Fenergy",&dBD[3].Fenergy);
	inTree->SetBranchAddress("BD3time",&dBD[3].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BD3phi",&dBD[3].phi); //phi from barc #
	inTree->SetBranchAddress("BD3z",&dBD[3].z); //z from fronts

	inTree->SetBranchAddress("BD4Fmult",&dBD[4].Fmult);
	inTree->SetBranchAddress("BD4Fnum",&dBD[4].Fnum);
	inTree->SetBranchAddress("BD4Fenergy",&dBD[4].Fenergy);
	inTree->SetBranchAddress("BD4time",&dBD[4].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BD4phi",&dBD[4].phi); //phi from barc #
	inTree->SetBranchAddress("BD4z",&dBD[4].z); //z from fronts

	inTree->SetBranchAddress("BD5Fmult",&dBD[5].Fmult);
	inTree->SetBranchAddress("BD5Fnum",&dBD[5].Fnum);
	inTree->SetBranchAddress("BD5Fenergy",&dBD[5].Fenergy);
	inTree->SetBranchAddress("BD5time",&dBD[5].Ftime); //storing time from fronts
	inTree->SetBranchAddress("BD5phi",&dBD[5].phi); //phi from barc #
	inTree->SetBranchAddress("BD5z",&dBD[5].z); //z from fronts

	// ***** loop over pruned events *****
	ULong64_t nevents = inTree->GetEntries();
	//ULong64_t nevents = 1000; //for testing only
	ULong64_t jentry;
	int iter, iter2;

	for (jentry=0; jentry<nevents; jentry++) //event loop
	{
	    inTree->GetEntry(jentry);

	    //initialize stuff
	    dEmult = 0; dEtime = -1; dE = -1.; dEz = -1.; dEphi = -1.; dErho = -1.; 
	    dEdet = -1; dEstrip = -1; flagUpDn = 0; flagQ = 0; flagB = 0;
	    EmultF = 0; EmultB = 0; EtFront = -1; EtBack = -1; Ef = -1.; Eb = -1.; 
	    EroughF = -1; EroughB = -1; Ez = -1.; Erho = -1.; Ephi = -1.; 
	    Edet = -1; EstripF = -1; EstripB = -1;
	    IntZ = -1.; IntTheta = -1.; IntPath = -1.;

	    /*event flag key
	     * option 0: default flag = 0 (below threshold)
	     * option 1: detector # not set -> set everything for first time -> flag = 1
	     * option 2: detector # matches previous -> check for adjacent strip -> sum them -> flag = 2
	     * option 3a: detector # matches previous -> not adjacent strip -> ??? -> flag = 3 [only BD4: strip 26&10, 29&13 (wire bond damage)]
	     * option 3b: detector # doesn't match prev -> ??? -> flag = 3
	     */

	    //start with QQQ (front) mult=1 events only
	    if(QQQmult==1)
	    {
	    	if(ibool)std::cout << "\nEntry = " << jentry << std::endl;
	    	//find which QQQ
	    	for(int i=0; i<=3; i++)
	    	{
	    		for(int j=0; j<dQ[i].Fmult; j++)
	    		{
	    			if(dQ[i].Fenergy[j]>eQmin) //apply energy threshold here?
	    			{
	    				Edet = i;
	    				break;
	    			}
	    		}
	    	}
	    	if(Edet>-1) //proceed if energy threshold met
	    	{
	    		if(ibool)std::cout << "\nEntry = " << jentry << std::endl;
	    		entryNum = jentry; //store row of pruned tree

	    		if(ibool)std::cout<<" QQQ "<<Edet<<std::endl;
	    		//grab all QQQ data
	    		EmultF = dQ[Edet].Fmult; EmultB = dQ[Edet].Bmult; 
	    		EtFront = dQ[Edet].Ftime[0]; EtBack = dQ[Edet].Btime[0]; 
	    		Ef = dQ[Edet].Fenergy[0]; Eb = dQ[Edet].Benergy[0]; 
	    		EroughF = dQ[Edet].FroughCal[0]; EroughB = dQ[Edet].BroughCal[0]; 
	    		EstripF = dQ[Edet].Fnum[0]; EstripB = dQ[Edet].Bnum[0];
	    		Ez = m_Qz; Erho = dQ[Edet].rho[0]; Ephi = dQ[Edet].phi[0];

	    		//TODO: list of dead wedges

	    		//if no back hit, use detector phi
	    		if(EmultB==0)
	    		{
	    			float phi = 225 - Edet *  90; //calculate phi in degrees
	    			Ephi = (phi < 0.) ? (phi + 360.) : (phi); //if phi < 0, then add 360
	    		}
	    		//look for barc (up or down) signal
	    		int count = 0;
	    		for(int i=0; i<=5; i++)
	    		{
	    			for(int j=0; j<dBU[i].Fmult; j++) //up loop
	    			{
	    				//TODO: can this be a function?
	    				if(dBU[i].Fenergy[j]>eBmin) //energy threshold
	    				{
	    					if(dEdet<0) //if first match
	    					{
	    						flagB = 1;
	    						dEdet = i;
	    						flagUpDn = 1; //store if inner hit is upstream (+1) barc
	    						iter = j; //iterator =/= strip number
	    						count++;
	    					}
	    					else if(dEdet==i && std::abs(dBU[i].Fnum[iter]-dBU[i].Fnum[j])==1) //if second match & adjacent strip
	    					{
	    						if(ibool)std::cout<<" dEdet already assigned to "<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" previous strip = "<<dBU[i].Fnum[iter]<<", current strip = "<<dBU[i].Fnum[j]<<std::endl;
	    						flagB = 2;
	    						iter2 = j;
	    						count++;
	    					}
	    					else //all other cases
	    					{
	    						if(ibool)std::cout<<" dEdet already assigned to "<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" current detector is BU"<<i<<std::endl;
	    						if(ibool)std::cout<<" previous strip = "<<dBU[i].Fnum[iter]<<", current strip = "<<dBU[i].Fnum[j]<<std::endl;
	    						flagB = 3;
	    						count++;
	    					}
	    				}
	    			}
	    			for(int j=0; j<dBD[i].Fmult; j++) //down loop
	    			{
	    				if(dBD[i].Fenergy[j]>eBmin) //energy threshold
	    				{
	    					if(dEdet<0) //if first match
	    					{
	    						flagB = 1;
	    						dEdet = i;
	    						flagUpDn = -1; //store if inner hit is downstream (-1) barc
	    						iter = j; //iterator =/= strip number
	    						count++;
	    					}
	    					else if(dEdet==i && std::abs(dBD[i].Fnum[iter]-dBD[i].Fnum[j])==1) //if second match & adjacent strip
	    					{
	    						if(ibool)std::cout<<" dEdet already assigned to "<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" previous strip = "<<dBD[i].Fnum[iter]<<", current strip = "<<dBD[i].Fnum[j]<<std::endl;
	    						flagB = 2;
	    						iter2 = j;
	    						count++;
	    					}
	    					else //all other cases
	    					{
	    						if(ibool)std::cout<<" dEdet already assigned to "<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" current detector is BD"<<i<<std::endl;
	    						if(ibool)std::cout<<" previous strip = "<<dBD[i].Fnum[iter]<<", current strip = "<<dBD[i].Fnum[j]<<std::endl;
	    						flagB = 3;
	    						count++;
	    					}
	    				}
	    			}
	    		}
	    		if(ibool)std::cout<<" Barc "<<dEdet<<", flagUpDn = "<<flagUpDn<<", flagB = "<<flagB<<std::endl;

	    		//TODO: can this be a function?
	    		//grab all barc data (up or down)
	    		if(flagB==1) //flagB = 1: one barc strip
	    		{
	    			switch(flagUpDn)
	    			{
	    				case -1:
	    					dEmult = count; dEstrip = dBD[dEdet].Fnum[iter];
	    					dEtime = dBD[dEdet].Ftime[iter]; dE = dBD[dEdet].Fenergy[iter];
	    					dEz = dBD[dEdet].z[iter]; dEphi = dBD[dEdet].phi[iter]; dErho = m_Brho;
	    					break;
	    				case 1:
	    					dEmult = count; dEstrip = dBU[dEdet].Fnum[iter];
	    					dEtime = dBU[dEdet].Ftime[iter]; dE = dBU[dEdet].Fenergy[iter];
	    					dEz = dBU[dEdet].z[iter]; dEphi = dBU[dEdet].phi[iter]; dErho = m_Brho;
	    					break;
	    			}
	    		}
	    		else if(flagB==2) //flagB = 2: two adjacent barc strips (summed energy & avg z,time)
	    		{
	    			switch(flagUpDn)
	    			{
	    				case -1:
	    					dEmult = count; dEstrip = dBD[dEdet].Fnum[iter];
	    					dEtime = 0.5*(dBD[dEdet].Ftime[iter]+dBD[dEdet].Ftime[iter2]); 
	    					dE = dBD[dEdet].Fenergy[iter]+dBD[dEdet].Fenergy[iter2];
	    					dEz = 0.5*(dBD[dEdet].z[iter]+dBD[dEdet].z[iter2]); 
	    					dEphi = dBD[dEdet].phi[iter]; dErho = m_Brho;
	    					if(ibool)std::cout<<"strip1 = "<<dBD[dEdet].Fnum[iter]<<", E = "<<dBD[dEdet].Fenergy[iter]<<std::endl;
	    					if(ibool)std::cout<<"strip2 = "<<dBD[dEdet].Fnum[iter2]<<", E = "<<dBD[dEdet].Fenergy[iter2]<<std::endl;
	    					if(ibool)std::cout<<" total energy = "<<dE<<std::endl;
	    					break;
	    				case 1:
	    					dEmult = count; dEstrip = dBU[dEdet].Fnum[iter];
	    					dEtime = 0.5*(dBU[dEdet].Ftime[iter]+dBU[dEdet].Ftime[iter2]); 
	    					dE = dBU[dEdet].Fenergy[iter]+dBU[dEdet].Fenergy[iter2];
	    					dEz = 0.5*(dBU[dEdet].z[iter]+dBU[dEdet].z[iter2]); 
	    					dEphi = dBU[dEdet].phi[iter]; dErho = m_Brho;
	    					break;
	    			}
	    		}
	    		else //flagB = 3: non-adjacent barc strips (???)
	    		{
	    			switch(flagUpDn)
	    			{
	    				case -1:
	    					dEmult = count; dEstrip = dBD[dEdet].Fnum[iter];
	    					dEtime = dBD[dEdet].Ftime[iter]; dE = dBD[dEdet].Fenergy[iter];
	    					dEz = dBD[dEdet].z[iter]; dEphi = dBD[dEdet].phi[iter]; dErho = m_Brho;
	    					break;
	    				case 1:
	    					dEmult = count; dEstrip = dBU[dEdet].Fnum[iter];
	    					dEtime = dBU[dEdet].Ftime[iter]; dE = dBU[dEdet].Fenergy[iter];
	    					dEz = dBU[dEdet].z[iter]; dEphi = dBU[dEdet].phi[iter]; dErho = m_Brho;
	    					break;
	    			}
	    		}

    			// calculate IntZ if dE & E hit in same direction
    			int upper = dEphi+45.;
    			int lower = dEphi-45.;
    			bool sameDir;// "same direction" criteria
    			if(upper>360.) sameDir = ((Ephi>=lower)&&(Ephi<=360.)) || ((Ephi>=0.)&&(Ephi<=upper-360.)); //handles when search range >360
    			else if(lower<0.) sameDir = ((Ephi>=0.)&&(Ephi<=upper)) || ((Ephi>=lower+360.)&&(Ephi<=360.)); //handles when search range <0
    			else sameDir = (Ephi>=lower)&&(Ephi<=upper); //default search range
    			if(sameDir) IntZ = Ez - Erho*( (Ez-dEz)/(Erho-dErho) );

    			//CHECK: why are some IntZ negative?
    			if(IntZ<-1.) std::cout<<"QQQz = "<<Ez<<", QQQrho = "<<Erho<<", barcZ = "<<", "<<dEz<<std::endl;

    			//if IntZ in active area, calculate IntTheta & IntPath (from interaction point to outer hit)
    			if(IntZ>50.)
    			{
    				IntTheta = std::atan(Erho/(Ez-IntZ))*180./M_PI; // convert to degrees
    				IntPath = (Ez-IntZ)/std::cos(IntTheta*M_PI/180.);
    			}
    			//later: do i need to calculate path lengths as well for energy loss calculations?
    			//later: calculate dEtheta (how? maybe a straggling correction from BarcMult?)

    			outTree->Fill();
	    	} //end of QQQ energy threshold loop
	    } //end of mult=1 loop
	    if(jentry%10000 == 0) std::cout<<"Entry "<<jentry<<" of "<<nevents<<", "<<100 * jentry/nevents<<"\% complete \r"<<std::flush;
	} //end of event loop

	outputfile->cd();
	std::cout << "Printing" << std::endl; outTree->Print();
	std::cout << "Writing" << std::endl; outTree->Write();

	std::cout<<"\n All done!"<<std::endl;

	inputfile->Close();
	outputfile->Close();
	delete inputfile;
	delete outputfile;
}