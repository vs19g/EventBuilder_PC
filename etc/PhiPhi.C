/*
  Use for (slightly) more sophisticated E-dE and timing plots than TimePeak.C. This takes 
  geometry into account when matching hits between barc & QQQ detectors.
*/

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
//path to DataStructs.h from EventBuilder
#include "../src/evbdict/DataStructs.h"
#include "../src/evb/ChannelMap.h"
#include "../src/evb/ChannelMap.cpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

//path to shared library from EventBuilder (.dylib or .so)
R__LOAD_LIBRARY(../lib/libEVBDict.dylib);

//shorthand for creating/filling 1D histograms
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

//shorthand for creating/filling 2D histograms
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

//now includes time cut
//looking at QQQ wedge number and Barc number (like ID vs. ID plot)
//first pass: hard code loop over barcs in range of QQQ
void Run_PhiPhi()
{	
	double tMin = -1200; //gate on -1.2us to 1us
	double tMax = 1000;

	//set input & output files
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/merged/runs_210-220_good18F.root";
	std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/Analysis/histo_210-220_good18F.root";

	//set channel map
	EventBuilder::ChannelMap m_chanMap("ANASEN_TRIUMFAug_run21+_ChannelMap.txt");

	std::cout<<"Processing data in "<<input_filename<<std::endl;
	std::cout<<"Writing histograms to "<<output_filename<<std::endl;
	std::cout<<"Time cut: "<<tMin<<"ns to "<<tMax<<"ns"<<std::endl;

	//test input file exists, not open
	TFile* inputfile = TFile::Open(input_filename.c_str(), "READ");
	if(inputfile == nullptr || !inputfile->IsOpen())
	{
		std::cout<<"Could not find file "<<input_filename<<". Skipping."<<std::endl;
		delete inputfile;
		return;
	}
	TTree* tree = (TTree*) inputfile->Get("SortTree");
	if(tree == nullptr)
	{
		std::cout<<"No tree named SortTree found inside file "<<input_filename<<". Skipping."<<std::endl;
		inputfile->Close();
		delete inputfile;
		return;
	}

	CoincEvent* event = new CoincEvent();

	tree->SetBranchAddress("event", &event);

	//recreates & opens output file
	TFile* outputfile = TFile::Open(output_filename.c_str(), "RECREATE");
	if(outputfile == nullptr)
	{
		std::cout<<"Could not find file "<<output_filename<<". Skipping."<<std::endl;
		inputfile->Close();
		delete inputfile;
		return;
	}

	uint64_t nevents = tree->GetEntries();
	std::cout<<"Total events: "<<nevents<<std::endl;
	std::string name;

	double percent = 0.05;
	uint64_t flush_val = nevents * percent;
	uint64_t flush_count = 0;
	uint64_t count = 0;

	DetectorHit hBarcUp, hBarcDn, hQQQ; //detector hit
	int iBarcUp, iBarcDn, iQQQ; //detector ID
	int nWedge; //local wedge number = iQQQ*16 + local channel
	double phi, pQQQ; //phi in degrees

	//loop over all events in input file
	for(auto i=0; i<nevents; i++)
	{
		count++;
		if(count == flush_val)
		{
			flush_count++;
			count = 0;
			std::cout<<"\rPercent of data processed: "<<flush_count*percent*100<<"%"<<std::flush;
		}

		tree->GetEntry(i);

		//initialize stuff
		hBarcUp.globalChannel = -1; hBarcDn.globalChannel = -1; hQQQ.globalChannel = -1;
		hBarcUp.energy = 50; hBarcDn.energy = 50; hQQQ.energy = 50; //set threshold to cut out noise
		hBarcUp.timestamp = -1; hBarcDn.timestamp = -1; hQQQ.timestamp = -1;
		iBarcUp = -1; iBarcDn = -1; iQQQ = -1;

		/*
		let's think this through:
		1) start with biggest QQQ wedge hit (only continue if hit exists)
		2) get local channel
		3) convert local channel to phi
		4) pick barcs to search through (still only looking at biggest signal)
		[do i want to expand so i'm looking at all strips within time cut? or does it make
		 more sense to wait until i have theta,z information?]
		5) loop over barcUp & barcDn for biggest hit
		6) if both detector hits exist (index>-1), fill histogram
		*/

		for(int j=0; j<4; j++)
		{
			if(!event->fqqq[j].wedges.empty()) //QQQ backs
			{
				DetectorHit& wedge = event->fqqq[j].wedges[0]; //get QQQ_j wedge with biggest energy
				if(hQQQ.energy<wedge.energy)
				{
					hQQQ = wedge; //store detector hit
					iQQQ = j; // store detector ID
				}
			}
		}

		//look up local QQQ channel
		auto iter = m_chanMap.FindChannel(hQQQ.globalChannel);

		switch(iQQQ)
		{
			case 0:
				//QQQ wedge number = iQQQ*16 + local channel
				nWedge = iQQQ*16 + (15-iter->second.local_channel); //necessary for QQQ0&2
				break;
			case 1:
				nWedge = iQQQ*16 + iter->second.local_channel;
				break;
			case 2:
				iQQQ = 3; //correction for QQQ2&3 swapped cables in good18F
				nWedge = iQQQ*16 + iter->second.local_channel;
				break;
			case 3:
				iQQQ = 2; //correction for QQQ2&3 swapped cables in good18F
				nWedge = iQQQ*16 + (15-iter->second.local_channel); //necessary for QQQ0&2
				break;
		}

		if(iQQQ>-1) //test QQQ hit exsits
		{
			/*//get phi for that wedge (degrees)
			//first pass: assume no dead space
			phi = 270. - nWedge*360./64.;
			pQQQ = (phi < 0.) ? (phi + 360.) : (phi); //if phi < 0, then add 360*/

			//first pass: hard code which barc to search for each QQQ
			if(iQQQ==0)
			{
				for(int k=0; k<3; k++)
				{
					if(!event->barcUp[k].fronts.empty()) //upstream barcelona
					{
						DetectorHit& front = event->barcUp[k].fronts[0]; //get barcUp_k strip with biggest energy
						if(hBarcUp.energy<front.energy)
						{
							hBarcUp = front; //store detector hit
							iBarcUp = k; // store detector ID
						}
					}
					if(!event->barcDown[k].fronts.empty()) //downstream barcelona
					{
						DetectorHit& front = event->barcDown[k].fronts[0]; //get barcDn_k strip with biggest energy
						if(hBarcDn.energy<front.energy)
						{
							hBarcDn = front; //store detector hit
							iBarcDn = k; // store detector ID
						}
					}
				}
			}
			else if(iQQQ==1)
			{
				for(int k=1; k<4; k++)
				{
					if(!event->barcUp[k].fronts.empty()) //upstream barcelona
					{
						DetectorHit& front = event->barcUp[k].fronts[0]; //get barcUp_k strip with biggest energy
						if(hBarcUp.energy<front.energy)
						{
							hBarcUp = front; //store detector hit
							iBarcUp = k; // store detector ID
						}
					}
					if(!event->barcDown[k].fronts.empty()) //downstream barcelona
					{
						DetectorHit& front = event->barcDown[k].fronts[0]; //get barcDn_k strip with biggest energy
						if(hBarcDn.energy<front.energy)
						{
							hBarcDn = front; //store detector hit
							iBarcDn = k; // store detector ID
						}
					}
				}
			}
			else if(iQQQ==2)
			{
				for(int k=3; k<6; k++)
				{
					if(!event->barcUp[k].fronts.empty()) //upstream barcelona
					{
						DetectorHit& front = event->barcUp[k].fronts[0]; //get barcUp_k strip with biggest energy
						if(hBarcUp.energy<front.energy)
						{
							hBarcUp = front; //store detector hit
							iBarcUp = k; // store detector ID
						}
					}
					if(!event->barcDown[k].fronts.empty()) //downstream barcelona
					{
						DetectorHit& front = event->barcDown[k].fronts[0]; //get barcDn_k strip with biggest energy
						if(hBarcDn.energy<front.energy)
						{
							hBarcDn = front; //store detector hit
							iBarcDn = k; // store detector ID
						}
					}
				}
			}
			else if(iQQQ==3)
			{
				for(int k=4; k<7; k++)
				{
					if(!event->barcUp[k%6].fronts.empty()) //upstream barcelona
					{
						DetectorHit& front = event->barcUp[k%6].fronts[0]; //get barcUp_k strip with biggest energy
						if(hBarcUp.energy<front.energy)
						{
							hBarcUp = front; //store detector hit
							iBarcUp = k%6; // store detector ID
						}
					}
					if(!event->barcDown[k%6].fronts.empty()) //downstream barcelona
					{
						DetectorHit& front = event->barcDown[k%6].fronts[0]; //get barcDn_k strip with biggest energy
						if(hBarcDn.energy<front.energy)
						{
							hBarcDn = front; //store detector hit
							iBarcDn = k%6; // store detector ID
						}
					}
				}
			}

			//still inside loop that requires QQQ hit
			if( iBarcUp>-1 && hBarcUp.energy==std::max(hBarcDn.energy,hBarcUp.energy) ) //upstream barc signal bigger
			{
				//fill summary histograms
				name = "barc_qqq_timePeak_phi";
				MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcUp.timestamp);
				name = "barc_dE_vs_qqq_E_phi";
				MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);
				name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E_phi";
				MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);

				//fill barcUp histograms
				name = "barcUp_qqq_timePeak_phi";
				MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcUp.timestamp);
				//name = "barcUp" + std::to_string(iBarcUp) + "_dE_vs_qqq_E_phi";
				//MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);
				name = "barcUp_dE_vs_qqq_E_phi";
				MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);
				MyFill(histMap, "barcUpID_vs_qqqWedgeID_phi", 64, 0, 64, 6, 0, 6, nWedge, iBarcUp);

				if( (hQQQ.timestamp-hBarcUp.timestamp)>tMin && (hQQQ.timestamp-hBarcUp.timestamp)<tMax ) //time cut
				{
					//fill summary histograms_timecut
					name = "barc_qqq_timePeak_phi_timeCut";
					MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcUp.timestamp);
					name = "barc_dE_vs_qqq_E_phi_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);
					name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E_phi_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);

					//fill barcUp histograms_timecut
					name = "barcUp_qqq_timePeak_phi_timeCut";
					MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcUp.timestamp);
					//name = "barcUp" + std::to_string(iBarcUp) + "_dE_vs_qqq_E_phi_timeCut";
					//MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);
					name = "barcUp_dE_vs_qqq_E_phi_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcUp.energy);
					MyFill(histMap, "barcUpID_vs_qqqWedgeID_phi_timeCut", 64, 0, 64, 6, 0, 6, nWedge, iBarcUp);
				}
			}
			else if( iBarcDn>-1 && hBarcDn.energy==std::max(hBarcDn.energy,hBarcUp.energy) ) //downstream barc signal bigger
			{
				//fill summary histograms
				name = "barc_qqq_timePeak_phi";
				MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcDn.timestamp);
				name = "barc_dE_vs_qqq_E_phi";
				MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);
				name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E_phi";
				MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);

				//fill barcDn histograms
				name = "barcDn_qqq_timePeak_phi";
				MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcDn.timestamp);
				//name = "barcDn" + std::to_string(iBarcDn) + "_dE_vs_qqq_E_phi";
				//MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);
				name = "barcDn_dE_vs_qqq_E_phi";
				MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);
				MyFill(histMap, "barcDownID_vs_qqqWedgeID_phi", 64, 0, 64, 6, 0, 6, nWedge, iBarcDn);

				if( (hQQQ.timestamp-hBarcDn.timestamp)>tMin && (hQQQ.timestamp-hBarcDn.timestamp)<tMax )
				{
					//fill summary histograms_timecut
					name = "barc_qqq_timePeak_phi_timeCut";
					MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcDn.timestamp);
					name = "barc_dE_vs_qqq_E_phi_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);
					name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E_phi_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);

					//fill barcDn histograms_timecut
					name = "barcDn_qqq_timePeak_phi_timeCut";
					MyFill(histMap,name,128,-3072,3072,hQQQ.timestamp-hBarcDn.timestamp);
					//name = "barcDn" + std::to_string(iBarcDn) + "_dE_vs_qqq_E_phi_timeCut";
					//MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);
					name = "barcDn_dE_vs_qqq_E_phi_timeCut";
					MyFill(histMap,name,512,0,4096,512,0,4096,hQQQ.energy,hBarcDn.energy);
					MyFill(histMap, "barcDownID_vs_qqqWedgeID_phi_timeCut", 64, 0, 64, 6, 0, 6, nWedge, iBarcDn);
				}
			}
		}
	}

	std::cout<<std::endl;
	inputfile->Close();
	outputfile->cd();
	for(auto& iter : histMap)
		iter.second->Write();
	outputfile->Close();

	delete inputfile;
	delete outputfile;
}