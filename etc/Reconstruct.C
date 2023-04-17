#include "Reconstruct.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath> // std::abs, M_PI, std::cos, std::atan (in radians)
#include <numeric> // std::accumulate
#include <unordered_map>
#include <algorithm> // std::find, std::any_of

/*
	Verify thresholds at top of Reconstruct function!

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

//useful functions for drawing a bunch of 1D or 2D histograms without initializing each one
void MyFill(std::unordered_map<std::string, TH1*>& map, const std::string& name, int bins, double min, double max, double value)
{
	auto iter = map.find(name);
	if(iter != map.end())
		iter->second->Fill(value);
	else
	{
		TH1F* histo = new TH1F(name.c_str(), name.c_str(), bins, min, max);
		histo->Fill(value);
		map[name] = histo;
	}
}
void MyFill(std::unordered_map<std::string, TH1*>& map, const std::string& name, int binsx, double minx, double maxx, int binsy, double miny, double maxy, double valuex, double valuey)
{
	auto iter = map.find(name);
	if(iter != map.end())
		iter->second->Fill(valuex, valuey);
	else
	{
		TH2F* histo = new TH2F(name.c_str(), name.c_str(), binsx, minx, maxx, binsy, miny, maxy);
		histo->Fill(valuex, valuey);
		map[name] = histo;
	}
}

void Reconstruct(int runNumber)
{
	//set thresholds here
	bool ibool = 0; //debug
	double eQmin = 50.; // QQQ energy threshold (units: channel)
	double eBmin = 0.; // barc energy threshold (units: channel)
	float phiRange = 45.; // +/- phi search range for inner & outer hits (units: degrees)
	float activeZ = 58.5; // active area begins at this Z (currently position of aperture)

	//file names
	std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/trees/run_"+std::to_string(runNumber)+"_18F_pruned.root";
	std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/Analysis/run_"+std::to_string(runNumber)+"_18F_reconstructed.root";

	std::cout<<"Reconstructing data from "<<input_filename<<std::endl;

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
	TTree *outTree = new TTree("tBarcQQQ","Barc+QQQ events for QQQmult=1-2");
	std::cout<<"Writing to "<<input_filename<<std::endl;

	//initialize variables for MyFill functions
	std::unordered_map<std::string, TH1*> histMap;
	std::string name;

	// ***** initialize variables to store outTree data *****
	int dEmult, dEstrip, EmultF, EmultB, EstripF, EstripB;
	int dEdet, Edet, flagB, flagQf, flagQb, flagUpDn;
	float dEz, dEphi, dErho, Ez, Ephi, Erho, IntZ, IntTheta, IntPath;
	double dEtime, dE, EtFront, EtBack, Ef, Eb, EroughF, EroughB; 
	ULong64_t entryNum;

	// ***** output tree branches *****
	outTree->Branch("EntryNo",&entryNum,"EntryNo/l"); // row of pruned tree
	outTree->Branch("flagQf",&flagQf,"flagQf/I"); // for QQQ fronts; see flag key in event loop
	outTree->Branch("flagQb",&flagQb,"flagQb/I"); // for QQQ backs; see flag key in event loop
	outTree->Branch("flagB",&flagB,"flagB/I"); // for Barcs; see flag key in event loop
	
	outTree->Branch("dEmult",&dEmult,"dEmult/I"); // detector mult above threshold
	outTree->Branch("dEt",&dEtime,"dEt/D"); // time
	outTree->Branch("dE",&dE,"dE/D"); // dE from inner
	outTree->Branch("dEz",&dEz,"dEz/F"); // z from inner
	outTree->Branch("dEphi",&dEphi,"dEphi/F"); // phi from inner
	outTree->Branch("dErho",&dErho,"dErho/F"); // rho from inner
	outTree->Branch("dEdet",&dEdet,"dEdet/I"); // detector inner
	outTree->Branch("dEstrip",&dEstrip,"dEstrip/I"); // strip number
	outTree->Branch("flagUpDn",&flagUpDn,"flagUpDn/I"); // up (+1) or downstream (-1) barc
	
	outTree->Branch("EmultF",&EmultF,"EmultF/I"); outTree->Branch("EmultB",&EmultB,"EmultB/I"); // detector mult above threshold
	outTree->Branch("EtF",&EtFront,"EtF/D"); outTree->Branch("EtB",&EtBack,"EtB/D"); // front & back times
	outTree->Branch("Ef",&Ef,"Ef/D"); outTree->Branch("Eb",&Eb,"Eb/D"); // E from outer
	outTree->Branch("EroughF",&EroughF,"EroughF/D"); outTree->Branch("EroughB",&EroughB,"EroughB/D"); // rough E calibration (same gain all channels)
	outTree->Branch("Ez",&Ez,"Ez/F"); // z from outer (z for QQQ is a constant, z for SX3s requires calibration first)
	outTree->Branch("Ephi",&Ephi,"Ephi/F"); // phi from outer
	outTree->Branch("Erho",&Erho,"Erho/F"); // rho from outer
	outTree->Branch("Edet",&Edet,"Edet/I"); // detector number
	outTree->Branch("EstripF",&EstripF,"EstripF/I"); outTree->Branch("EstripB",&EstripB,"EstripB/I"); // strip number

	outTree->Branch("IntZ",&IntZ,"IntZ/F"); // z for interaction point (x=y=0)
	outTree->Branch("IntTheta",&IntTheta,"IntTheta/F"); // angle from interaction point (x=y=0) to outer detector hit
	outTree->Branch("IntPath",&IntPath,"IntPath/F"); // path length from interaction point (x=y=0) to outer detector hit


	// ***** set branch addresses for Pruned Tree *****

	//initialize variables to store inTree data
	int QQQmult,BarcMult;
	dataE dQ[4];
	dataDE dBU[6];
	dataDE dBD[6];

	//link variables to branch data
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
	ULong64_t nevents = inTree->GetEntries(); //grab total number of entries from tree
	//ULong64_t nevents = 2000; //for testing only

	//initialize vectors to temporarily store detector data
	std::vector<double> energyVec, energyCalVec, timeVec; //energy, calibrated energy, time
	std::vector<int> stripVec; //local strip number
	std::vector<float> coordVec; //cylindrical coordinate
	int count; //number of hits above threshold (can be different from stripVec.size() for flag=3 events)

	for(ULong64_t jentry=0; jentry<nevents; jentry++) //event loop
	{
		// ******************************************************************************
		// **************************** PART 1: CALCULATIONS ****************************
		// ******************************************************************************

		//grab entry
	    inTree->GetEntry(jentry);

	    //reset default values
	    dEmult = 0; dEtime = -1; dE = -1.; dEz = -1.; dEphi = -1.; dErho = -1.; 
	    dEdet = -1; dEstrip = -1; flagUpDn = 0; flagQf = 0; flagQb = 0; flagB = 0;
	    EmultF = 0; EmultB = 0; EtFront = -1; EtBack = -1; Ef = -1.; Eb = -1.; 
	    EroughF = -1; EroughB = -1; Ez = -1.; Erho = -1.; Ephi = -1.; 
	    Edet = -1; EstripF = -1; EstripB = -1;
	    IntZ = -1.; IntTheta = -1.; IntPath = -1.;

	    //clear temporary vectors
	    //note: vector.clear() makes vector.size()=0
	    count = 0;
	    energyVec.clear(); energyCalVec.clear(); timeVec.clear(); stripVec.clear(); coordVec.clear();

	    /* EVENT FLAG KEY
	     * option 0: default flag = 0 (below threshold)
	     * option 1: one hit above threshold -> flag = 1
	     * option 2: adjacent hits above threshold -> sum energy, average physical coordinates -> flag = 2
	     * option 3: weird events (including non-adjacent hits or multiple detectors hit) -> ??? -> flag = 3 [BD4: strip 26&10, 29&13 (wire bond damage)]
	     */

	    //start with QQQ (front) mult=1or2 events only
	    if(QQQmult<=2)
	    {
	    	if(ibool)std::cout<<"\nEntry = "<<jentry<<std::endl;
	    	entryNum = jentry; //store row of pruned tree
	    	
	    	//find the QQQ front hit(s)
	    	for(int i=0; i<=3; i++)
	    	{
	    		for(int j=0; j<dQ[i].Fmult; j++)
	    		{
	    			if(dQ[i].Fenergy[j]>eQmin) //energy threshold
	    			{
	    				//if first match
		    			if( stripVec.size()==0 )
		    			{
		    				if(ibool)std::cout<<"\nEntry = "<<jentry<<" above threshold"<<std::endl;

		    				//set flag
		    				flagQf = 1;
		    				//store detector number
	    					Edet = i;
		    				//append temporary vectors
		    				energyVec.push_back(dQ[i].Fenergy[j]);
		    				energyCalVec.push_back(dQ[i].FroughCal[j]); 
		    				timeVec.push_back(dQ[i].Ftime[j]); 
		    				stripVec.push_back(dQ[i].Fnum[j]); 
		    				coordVec.push_back(dQ[i].rho[j]);
		    				//increment multiplicity
		    				count ++;
		    			}
		    			/* for QQQ fronts, detector number must match
		    			 * looking for cases with 2 adjacent strips
		    			 * proton can hit up to 2 strips in a QQQ (unlike barcs)
		    			 */
		    			else if( Edet==i && stripVec.size()==1 && std::abs(stripVec[0]-dQ[i].Fnum[j])==1 )
		    			{
		    				if(ibool)std::cout<<"Entry = "<<jentry<< std::endl;
		    				if(ibool)std::cout<<" OK: previous strip = "<<stripVec[0]<<", current strip = "<<dQ[i].Fnum[j]<<std::endl;
		    				if(ibool)std::cout<<" flag = 2"<<std::endl;
		    				//set flag
		    				flagQf = 2;
		    				//append temporary vectors
		    				energyVec.push_back(dQ[i].Fenergy[j]);
		    				energyCalVec.push_back(dQ[i].FroughCal[j]); 
		    				timeVec.push_back(dQ[i].Ftime[j]); 
		    				stripVec.push_back(dQ[i].Fnum[j]); 
		    				coordVec.push_back(dQ[i].rho[j]);
		    				//increment multiplicity
		    				count++;
		    			}
		    			/* all other cases flagged for further review
		    			 * includes events with >2 strips above threshold
		    			 * includes events with 2 non-adjacent strips above threshold
		    			 */
		    			else
		    			{
		    				if(ibool)std::cout<<"Entry = "<<jentry<< std::endl;
		    				if(ibool)std::cout<<" WEIRD: previous strip = "<<stripVec[0]<<", current strip = "<<dQ[i].Fnum[j]<<std::endl;
		    				if(ibool)std::cout<<" flag = 3"<<std::endl;
		    				//set flag
		    				flagQf = 3;
		    				//increment multiplicity
		    				count++;
		    			}
	    			}
	    		}
	    	} //end QQQ front loop

	    	//only proceed if energy threshold met (QQQ front hit required for reconstruction)
	    	if(Edet>-1) //note: this includes flagQf=3 events
	    	{
	    		if(ibool)std::cout<<" QQQ front hit exists"<<std::endl;
	    		if(ibool)std::cout<<" QQQ = "<<Edet<<std::endl;

	    		//store all QQQ front data, according to flag
	    		if(flagQf==1 || flagQf==2) //flagQf = 1 or 2: one ring or adjacent rings
	    		{
	    			//sum energy, average coordinates & time
	    			EmultF = count;
	    			EstripF = std::accumulate(stripVec.begin(),stripVec.end(),0.0) / stripVec.size();
	    			EtFront = std::accumulate(timeVec.begin(),timeVec.end(),0.0) / timeVec.size(); 
	    			Ef = std::accumulate(energyVec.begin(),energyVec.end(),0.0); 
	    			EroughF = std::accumulate(energyCalVec.begin(),energyCalVec.end(),0.0); 
	    			Erho = std::accumulate(coordVec.begin(),coordVec.end(),0.0) / coordVec.size();
	    			if(ibool)std::cout<<"QQQ front data"<<std::endl;
	    			if(ibool)std::cout<<" mult = "<<EmultF<<std::endl;
	    			if(ibool)std::cout<<" strip = "<<EstripF<<std::endl;
	    			if(ibool)std::cout<<" time = "<<EtFront<<std::endl;
	    			if(ibool)std::cout<<" energy = "<<Ef<<std::endl;
	    			if(ibool)std::cout<<" calEnergy = "<<EroughF<<std::endl;
	    			if(ibool)std::cout<<" coord = "<<Erho<<std::endl;
	    		}
	    		else //flagQf = 3: non-adjacent or too many rings (???)
	    		{
	    			//just store the first entry (largest energy ring) of Edet
	    			EmultF = count; EstripB = dQ[Edet].Fnum[0];
	    			EtFront = dQ[Edet].Ftime[0]; 
	    			Ef = dQ[Edet].Fenergy[0]; EroughF = dQ[Edet].FroughCal[0]; 
	    			Erho = dQ[Edet].rho[0];
	    		}

	    		//clear temporary vectors
	    		count = 0;
	    		energyVec.clear(); energyCalVec.clear(); timeVec.clear(); stripVec.clear(); coordVec.clear();

	    		//find the QQQ back hit(s)
	    		for(int j=0; j<dQ[Edet].Bmult; j++) //Edet already set, so don't need loop over all 4 QQQs
	    		{
	    			if(dQ[Edet].Benergy[j]>eQmin) //energy threshold
	    			{
	    				//if first match
	    				if( stripVec.size()==0 )
	    				{
	    					//set flag
	    					flagQb = 1;
	    					//append temporary vectors
	    					energyVec.push_back(dQ[Edet].Benergy[j]);
	    					energyCalVec.push_back(dQ[Edet].BroughCal[j]); 
	    					timeVec.push_back(dQ[Edet].Btime[j]); 
	    					stripVec.push_back(dQ[Edet].Bnum[j]); 
	    					coordVec.push_back(dQ[Edet].phi[j]);
	    					//increment multiplicity
	    					count ++;
	    				}
	    				/* for QQQ backs, detector number already set
	    				 * looking for cases with 2 adjacent strips
	    				 * proton can hit up to 2 strips in a QQQ (unlike barcs)
	    				 */
	    				else if( stripVec.size()==1 && std::abs(stripVec[0]-dQ[Edet].Bnum[j])==1 )
	    				{
	    					if(ibool)std::cout<<"Entry = "<<jentry<< std::endl;
	    					if(ibool)std::cout<<" OK: previous strip = "<<stripVec[0]<<", current strip = "<<dQ[Edet].Bnum[j]<<std::endl;
	    					if(ibool)std::cout<<" flag = 2"<<std::endl;
	    					//set flag
	    					flagQb = 2;
	    					//append temporary vectors
	    					energyVec.push_back(dQ[Edet].Benergy[j]);
	    					energyCalVec.push_back(dQ[Edet].BroughCal[j]); 
	    					timeVec.push_back(dQ[Edet].Btime[j]); 
	    					stripVec.push_back(dQ[Edet].Bnum[j]); 
	    					coordVec.push_back(dQ[Edet].phi[j]);
	    					//increment multiplicity
	    					count++;
	    				}
	    				/* all other cases flagged for further review
	    				 * includes events with >2 strips above threshold
	    				 * includes events with 2 non-adjacent strips above threshold
	    				 */
	    				else
	    				{
	    					if(ibool)std::cout<<"Entry = "<<jentry<< std::endl;
	    					if(ibool)std::cout<<" WEIRD: previous strip = "<<stripVec[0]<<", current strip = "<<dQ[Edet].Bnum[j]<<std::endl;
	    					if(ibool)std::cout<<" flag = 3"<<std::endl;
	    					//set flag
	    					flagQb = 3;
	    					//increment multiplicity
	    					count++;
	    				}
	    			}
	    		} //end QQQ back loop
	    		//store all QQQ back data, according to flag
	    		if(flagQb==0) //flagQb = 0: no wedge
	    		{
	    			//set phi as center of detector
	    			float phi = 225. - Edet*90.; //calculate phi in degrees
	    			Ephi = (phi < 0.) ? (phi + 360.) : (phi); //if phi < 0, then add 360; otherwise Ephi = phi
	    		}
	    		else if(flagQb==1 || flagQb==2) //flagQb = 1 or 2: one wedge or adjacent wedges
	    		{
	    			//sum energy, average coordinates & time
	    			EmultB = count;
	    			EstripB = std::accumulate(stripVec.begin(),stripVec.end(),0.0) / stripVec.size();
	    			EtBack = std::accumulate(timeVec.begin(),timeVec.end(),0.0) / timeVec.size(); 
	    			Eb = std::accumulate(energyVec.begin(),energyVec.end(),0.0); 
	    			EroughB = std::accumulate(energyCalVec.begin(),energyCalVec.end(),0.0); 
	    			Ephi = std::accumulate(coordVec.begin(),coordVec.end(),0.0) / coordVec.size();
	    			Ez = m_Qz;
	    			if(ibool)std::cout<<"QQQ back data"<<std::endl;
	    			if(ibool)std::cout<<" mult = "<<EmultB<<std::endl;
	    			if(ibool)std::cout<<" strip = "<<EstripB<<std::endl;
	    			if(ibool)std::cout<<" time = "<<EtBack<<std::endl;
	    			if(ibool)std::cout<<" energy = "<<Eb<<std::endl;
	    			if(ibool)std::cout<<" calEnergy = "<<EroughB<<std::endl;
	    			if(ibool)std::cout<<" coord = "<<Ephi<<std::endl;
	    			if(ibool)std::cout<<" z = "<<Ez<<std::endl;
	    		}
	    		else //flagQb = 3: non-adjacent or too many wedges (???)
	    		{
	    			//just store the first entry (largest energy wedge) of Edet
	    			EmultB = count; EstripB = dQ[Edet].Bnum[0];
	    			EtBack = dQ[Edet].Btime[0]; 
	    			Eb = dQ[Edet].Benergy[0]; EroughB = dQ[Edet].BroughCal[0]; 
	    			Ephi = dQ[Edet].phi[0]; Ez = m_Qz;
	    		}

	    		//TODO: list of dead wedges to "turn off"

	    		//clear temporary vectors
	    		count = 0;
	    		energyVec.clear(); energyCalVec.clear(); timeVec.clear(); stripVec.clear(); coordVec.clear();

	    		//look for barc (up or down) signal
	    		for(int i=0; i<=5; i++)
	    		{
	    			for(int j=0; j<dBU[i].Fmult; j++) //up loop
	    			{
	    				if(dBU[i].Fenergy[j]>eBmin) //energy threshold
	    				{
	    					//if first match
	    					if(dEdet<0)
	    					{
	    						//set flag
	    						flagB = 1;
	    						//store detector number
	    						dEdet = i;
	    						flagUpDn = 1; //store if inner hit is upstream (+1) barc
	    						//append temporary vectors
	    						energyVec.push_back(dBU[i].Fenergy[j]);
		    					timeVec.push_back(dBU[i].Ftime[j]); 
		    					stripVec.push_back(dBU[i].Fnum[j]); 
		    					coordVec.push_back(dBU[i].z[j]);
		    					//increment multiplicity
	    						count++;
	    					}
	    					/* detector number must match AND strip must be adjacent to *any* previous strip
	    					 * std::any_of() iterates over all collected strips in stripVec, returns true if lambda expression is ever true
	    					 * lambda expression returns true if absoluteValue(currentStrip - previousStrip)=1
	    					 * proton can hit >2 strips in a barc (unlike QQQs)
	    					 */
	    					else if( dEdet==i && std::any_of(stripVec.begin(), stripVec.end(), [&dBU,&i,&j](int k){return std::abs(k-dBU[i].Fnum[j])==1;}) )
	    					{
	    						if(ibool)std::cout<<"Entry = "<<jentry<<std::endl;
	    						if(ibool)std::cout<<" dEdet already assigned to BU"<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" previous strip =";
	    						if(ibool)for(auto& z : stripVec)std::cout<<" "<<z;
	    						if(ibool)std::cout<<", current strip = "<<dBU[i].Fnum[j]<<std::endl;
	    						//set flag
	    						flagB = 2;
	    						//append temporary vectors
	    						energyVec.push_back(dBU[i].Fenergy[j]);
		    					timeVec.push_back(dBU[i].Ftime[j]); 
		    					stripVec.push_back(dBU[i].Fnum[j]); 
		    					coordVec.push_back(dBU[i].z[j]);
		    					//increment multiplicity
	    						count++;
	    					}
	    					//all other cases flagged for further review
	    					else
	    					{
	    						if(ibool)std::cout<<" dEdet already assigned to "<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" current detector is BU"<<i<<std::endl;
								if(ibool)std::cout<<" previous strip =";
	    						if(ibool)for(auto& z : stripVec)std::cout<<" "<<z;
	    						if(ibool)std::cout<<", current strip = "<<dBU[i].Fnum[j]<<std::endl;	    						//set flag
	    						flagB = 3;
	    						//increment multiplicity
	    						count++;
	    					}
	    				}
	    			} // end upstream loop
	    			for(int j=0; j<dBD[i].Fmult; j++) //down loop
	    			{
	    				if(dBD[i].Fenergy[j]>eBmin) //energy threshold
	    				{
	    					//if first match
	    					if(dEdet<0)
	    					{
	    						//set flag
	    						flagB = 1;
	    						dEdet = i;
	    						flagUpDn = -1; //store if inner hit is downstream (-1) barc
	    						//append temporary vectors
	    						energyVec.push_back(dBD[i].Fenergy[j]);
		    					timeVec.push_back(dBD[i].Ftime[j]); 
		    					stripVec.push_back(dBD[i].Fnum[j]); 
		    					coordVec.push_back(dBD[i].z[j]);
	    						//increment multiplicity
	    						count++;
	    					}
	    					//detector number must match AND strip must be adjacent to *any* previous strip
	    					else if( flagUpDn==-1 && dEdet==i && std::any_of(stripVec.begin(), stripVec.end(), [&dBD,&i,&j](int k){return std::abs(k-dBD[i].Fnum[j])==1;}) )
	    					{
	    						if(ibool)std::cout<<"Entry = "<<jentry<<std::endl;
	    						if(ibool)std::cout<<" dEdet already assigned to BD"<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" previous strip =";
	    						if(ibool)for(auto& z : stripVec)std::cout<<" "<<z;
	    						if(ibool)std::cout<<", current strip = "<<dBD[i].Fnum[j]<<std::endl;	    						
	    						//set flag
	    						flagB = 2;
	    						//append temporary vectors
	    						energyVec.push_back(dBD[i].Fenergy[j]);
		    					timeVec.push_back(dBD[i].Ftime[j]); 
		    					stripVec.push_back(dBD[i].Fnum[j]); 
		    					coordVec.push_back(dBD[i].z[j]);
	    						//increment multiplicity
	    						count++;
	    					}
	    					//all other cases flagged for further review
	    					else
	    					{
	    						if(ibool)std::cout<<" dEdet already assigned to "<<dEdet<<std::endl;
	    						if(ibool)std::cout<<" current detector is BD"<<i<<std::endl;
	    						if(ibool)std::cout<<" previous strip =";
	    						if(ibool)for(auto& z : stripVec)std::cout<<" "<<z;
	    						if(ibool)std::cout<<", current strip = "<<dBD[i].Fnum[j]<<std::endl;	    						//set flag
	    						flagB = 3;
	    						//increment multiplicity
	    						count++;
	    					}
	    				}
	    			} //end downstream loop
	    		} //end barc loop
	    		if(ibool)std::cout<<"Barc "<<dEdet<<", flagUpDn = "<<flagUpDn<<", flagB = "<<flagB<<std::endl;
	    		if(flagB==1 || flagB==2) //flagB = 1 or 2: one strip or adjacent strips
	    		{
	    			//sum energy, average coordinates & time
	    			dEmult = count;
	    			dEstrip = std::accumulate(stripVec.begin(),stripVec.end(),0.0) / stripVec.size();
	    			dEtime = std::accumulate(timeVec.begin(),timeVec.end(),0.0) / timeVec.size(); 
	    			dE = std::accumulate(energyVec.begin(),energyVec.end(),0.0); 
	    			dEz = std::accumulate(coordVec.begin(),coordVec.end(),0.0) / coordVec.size();
	    			dEphi = (flagUpDn==1) ? dBU[dEdet].phi[0] : dBD[dEdet].phi[0]; //if flagUpDn = 1, get phi from dBU; otherwise, get phi from dBD
	    			dErho = m_Brho;
	    			if(ibool)std::cout<<"Barc data"<<std::endl;
	    			if(ibool)std::cout<<" mult = "<<dEmult<<std::endl;
	    			if(ibool)std::cout<<" strip = "<<dEstrip<<std::endl;
	    			if(ibool)std::cout<<" time = "<<dEtime<<std::endl;
	    			if(ibool)std::cout<<" energy = "<<dE<<std::endl;
	    			if(ibool)std::cout<<" z = "<<dEz<<std::endl;
	    			if(ibool)std::cout<<" coord = "<<dEphi<<std::endl;
	    			if(ibool)std::cout<<" rho = "<<dErho<<std::endl;
	    		}
	    		else //flagB = 3: non-adjacent barc strips (???)
	    		{
	    			//just store the first entry (largest energy strip) of dEdet
	    			switch(flagUpDn)
	    			{
	    				case -1:
	    					dEmult = count; dEstrip = dBD[dEdet].Fnum[0];
	    					dEtime = dBD[dEdet].Ftime[0]; dE = dBD[dEdet].Fenergy[0];
	    					dEz = dBD[dEdet].z[0]; dEphi = dBD[dEdet].phi[0]; dErho = m_Brho;
	    					break;
	    				case 1:
	    					dEmult = count; dEstrip = dBU[dEdet].Fnum[0];
	    					dEtime = dBU[dEdet].Ftime[0]; dE = dBU[dEdet].Fenergy[0];
	    					dEz = dBU[dEdet].z[0]; dEphi = dBU[dEdet].phi[0]; dErho = m_Brho;
	    					break;
	    			}
	    		}

	    		//only continue if dE hit above threshold
	    		if(dEdet>-1)
	    		{
	    			// calculate IntZ if dE & E hit in same direction
	    			float upper = dEphi+phiRange;
	    			float lower = dEphi-phiRange;
	    			bool sameDir; // "same direction" criteria
	    			if(upper>360.) sameDir = ((Ephi>=lower)&&(Ephi<=360.)) || ((Ephi>=0.)&&(Ephi<=upper-360.)); //handles when search range >360
	    			else if(lower<0.) sameDir = ((Ephi>=0.)&&(Ephi<=upper)) || ((Ephi>=lower+360.)&&(Ephi<=360.)); //handles when search range <0
	    			else sameDir = (Ephi>=lower)&&(Ephi<=upper); //default search range
	    			if(sameDir) IntZ = Ez - Erho*( (Ez-dEz)/(Erho-dErho) );

	    			//if IntZ in active area, calculate IntTheta & IntPath (from interaction point to outer hit)
	    			if( (IntZ>=activeZ) && (IntZ<m_Qz) )
	    			{
	    				IntTheta = std::atan(Erho/(Ez-IntZ))*180./M_PI; // convert to degrees
	    				IntPath = (Ez-IntZ)/std::cos(IntTheta*M_PI/180.);
	    				//TODO: calculate a straggling correction on path and theta
	    				//TODO: call function here that calculates energy loss in gas
	    				//TODO: sum final energy, gas energy loss, straggling energy loss and write to outTree
	    			}

	    			outTree->Fill();
	    		} //end of loop requiring barc hit
	    	} //end of loop requiring QQQ front hit
	    } //end of QQQmult=1or2 loop

	    // ******************************************************************************
	    // ***************************** PART 2: HISTOGRAMS *****************************
	    // ******************************************************************************

		// filter: all event flags = 1 or 2 (aka it's a good event)
		if( (flagB==1 || flagB==2) && (flagQb==1 || flagQb==2) && (flagQf==1 || flagQf==2) )
		{
			//fill histograms
			name = "hBarcID_vs_QQQID";
			MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

			name = "hDEphi_vs_Ephi";
			MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

			name = "hDE_vs_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

			name = "hTimePeak_QQQ_Barc";
			MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

			name = "hQQQrings";
			MyFill(histMap,name,16,0,16,EstripF);

			if(flagUpDn==-1) //barcDn plots
			{
				name = "hBarcDnID_vs_QQQID";
				MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

				name = "hDEphi_BarcDn_vs_Ephi";
				MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

				name = "hDE_BarcDn_vs_E";
				MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

				name = "hTimePeak_QQQ_BarcDn";
				MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

				name = "hQQQrings_BarcDn";
				MyFill(histMap,name,16,0,16,EstripF);
			}
			else if(flagUpDn==1) //barcUp plots
			{
				name = "hBarcUpID_vs_QQQID";
				MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

				name = "hDEphi_BarcUp_vs_Ephi";
				MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

				name = "hDE_BarcUp_vs_E";
				MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

				name = "hTimePeak_QQQ_BarcUp";
				MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

				name = "hQQQrings_BarcUp";
				MyFill(histMap,name,16,0,16,EstripF);
			}

			/*** apply cuts ***/
			if( std::abs(EtFront-dEtime)<=1000 ) //time cut +/- 1000ns
			{
				//fill histograms
				name = "hBarcID_vs_QQQID_timeCut";
				MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

				name = "hDEphi_vs_Ephi_timeCut";
				MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

				name = "hDE_vs_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

				name = "hTimePeak_QQQ_Barc_timeCut";
				MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

				name = "hQQQrings_timeCut";
				MyFill(histMap,name,16,0,16,EstripF);

				if(flagUpDn==-1) //barcDn plots with time cut
				{
					name = "hBarcDnID_vs_QQQID_timeCut";
					MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

					name = "hDEphi_BarcDn_vs_Ephi_timeCut";
					MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

					name = "hDE_BarcDn_vs_E_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

					name = "hTimePeak_QQQ_BarcDn_timeCut";
					MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

					name = "hQQQrings_BarcDn_timeCut";
					MyFill(histMap,name,16,0,16,EstripF);
				}
				else if(flagUpDn==1) //barcUp plots with time cut
				{
					name = "hBarcUpID_vs_QQQID_timeCut";
					MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

					name = "hDEphi_BarcUp_vs_Ephi_timeCut";
					MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

					name = "hDE_BarcUp_vs_E_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

					name = "hTimePeak_QQQ_BarcUp_timeCut";
					MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

					name = "hQQQrings_BarcUp_timeCut";
					MyFill(histMap,name,16,0,16,EstripF);
				}

				if( IntPath>-1 ) //active area cut (leave out non-physical interation points)
				{
					//fill histograms with time & acive area cuts
					name = "hBarcID_vs_QQQID_timeCut_activeCut";
					MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

					name = "hDEphi_vs_Ephi_timeCut_activeCut";
					MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

					name = "hDE_vs_E_timeCut_activeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

					name = "hTimePeak_QQQ_Barc_timeCut_activeCut";
					MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

					name = "hQQQrings_timeCut_activeCut";
					MyFill(histMap,name,16,0,16,EstripF);

					if(flagUpDn==-1) //barcDn plots with time & acive area cuts
					{
						name = "hBarcDnID_vs_QQQID_timeCut_activeCut";
						MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

						name = "hDEphi_BarcDn_vs_Ephi_timeCut_activeCut";
						MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

						name = "hDE_BarcDn_vs_E_timeCut_activeCut";
						MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

						name = "hTimePeak_QQQ_BarcDn_timeCut_activeCut";
						MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

						name = "hQQQrings_BarcDn_timeCut_activeCut";
						MyFill(histMap,name,16,0,16,EstripF);
					}
					else if(flagUpDn==1) //barcUp plots with time & acive area cuts
					{
						name = "hBarcUpID_vs_QQQID_timeCut_activeCut";
						MyFill(histMap,name,4,0,4,6,0,6,Edet,dEdet);

						name = "hDEphi_BarcUp_vs_Ephi_timeCut_activeCut";
						MyFill(histMap,name,64,0,360,6,0,360,Ephi,dEphi);

						name = "hDE_BarcUp_vs_E_timeCut_activeCut";
						MyFill(histMap,name,512,0,4096,512,0,4096,Ef,dE);

						name = "hTimePeak_QQQ_BarcUp_timeCut_activeCut";
						MyFill(histMap,name,128,-3072,3072,EtFront-dEtime);

						name = "hQQQrings_BarcUp_timeCut_activeCut";
						MyFill(histMap,name,16,0,16,EstripF);
					} 
				} //end of active area cut
			} //end of time cut
		} //end of good event filter

	    if(jentry%100 == 0) std::cout<<"Entry "<<jentry<<" of "<<nevents<<", "<<100 * jentry/nevents<<"\% complete \r"<<std::flush;
	} //end of event loop

	outputfile->cd();
	//std::cout << "Printing" << std::endl; outTree->Print();
	std::cout << "\nWriting" << std::endl; outTree->Write();
	std::cout << "Drawing" << std::endl;
	for(auto& iter : histMap)
		iter.second->Write();

	inputfile->Close();
	outputfile->Close();
	delete inputfile;
	delete outputfile;
}