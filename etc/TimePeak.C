/*
  Generates histograms for events with Barcelona & QQQ hits with option to gate on timing peak.
  Picks hit with largest overall energy for each detector type, with no regard for geometry.
  Great for first look at the data and picking a sensible time gate.
*/

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
//path to DataStructs.h from EventBuilder
#include "../src/evbdict/DataStructs.h"

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

//plots qqq vs barcDn, qqq vs. barcUp, qqq vs. barc, qqq_i vs. barc with no timing peak cuts
void Run_TimePeak()
{	
	//set input & output files
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/merged/runs_210-220_good18F.root";
	std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/Analysis/timePeak_210-220_good18F.root";

	std::cout<<"Processing data in "<<input_filename<<std::endl;
	std::cout<<"Writing histograms to "<<output_filename<<std::endl;
	

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
	std::string strQQQ;

	double percent = 0.05;
	uint64_t flush_val = nevents * percent;
	uint64_t flush_count = 0;
	uint64_t count = 0;

	double tBarcUp, tBarcDn, tBarc, tQQQ;
	double eBarcUp, eBarcDn, eBarc, eQQQ;

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
		tBarcUp = -1; tBarcDn = -1; tQQQ = -1;
		eBarcUp = -1; eBarcDn = -1; eQQQ = -1;

		for(int j=0; j<4; j++)
		{
			if(!event->fqqq[j].wedges.empty()) //QQQ backs
			{
				DetectorHit& wedge = event->fqqq[j].wedges[0]; //get QQQ_j wedge with biggest energy
				if(eQQQ<wedge.energy)
				{
					eQQQ = wedge.energy; //store biggest overall energy
					tQQQ = wedge.timestamp; //store time from biggest overall energy
					strQQQ = std::to_string(j); // store QQQ_j
				}
			}
		}
		for(int k=0; k<6; k++)
		{
			if(!event->barcUp[k].fronts.empty()) //upstream barcelona
			{
				DetectorHit& front = event->barcUp[k].fronts[0]; //get barcUp_k strip with biggest energy
				if(eBarcUp<front.energy)
				{
					eBarcUp = front.energy; //store biggest overall energy
					tBarcUp = front.timestamp; //store time from biggest overall energy
				}
			}
			if(!event->barcDown[k].fronts.empty()) //downstream barcelona
			{
				DetectorHit& front = event->barcDown[k].fronts[0]; //get barcDn_k strip with biggest energy
				if(eBarcDn<front.energy)
				{
					eBarcDn = front.energy; //store biggest overall energy
					tBarcDn = front.timestamp; //store time from biggest overall energy
				}
			}
		}

		if( eQQQ>-1 && eBarcUp>-1 && eBarcUp==std::max(eBarcDn,eBarcUp) ) //upstream barc signal bigger
		{
			//fill summary histograms
			name = "barc_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcUp);
			name = "barc_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);
			name = "barc_dE_vs_qqq" + strQQQ + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);

			//fill barcUp histograms
			name = "barcUp_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcUp);
			name = "barcUp_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);
		}
		else if( eQQQ>-1 && eBarcDn>-1 && eBarcDn==std::max(eBarcDn,eBarcUp) ) //downstream barc signal bigger
		{
			//fill summary histograms
			name = "barc_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcDn);
			name = "barc_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);
			name = "barc_dE_vs_qqq" + strQQQ + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);

			//fill barcDn histograms
			name = "barcDn_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcDn);
			name = "barcDn_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);
		}
	}

	std::cout<<std::endl;
	inputfile->Close();
	outputfile->cd();
	//gStyle->SetPalette(kSunset); //predefined color palette
	for(auto& iter : histMap)
		iter.second->Write();
	outputfile->Close();

	delete inputfile;
	delete outputfile;
}

//plots all histograms with AND without timing peak cuts
void Run_TimePeakCuts()
{	
	double tMin = -1200; //gate on -1.2us to 1us
	double tMax = 1000;

	//set input & output files
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/WorkingData/merged/runs_303-307_good18F.root";
	std::string output_filename = "/Users/theefizzicist/Documents/Projects/18Ne-a-p/Analysis/timePeak_303-307_good18F.root";

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

	double tBarcUp, tBarcDn, tQQQ; //time
	double eBarcUp, eBarcDn, eQQQ; //energy
	int iBarcUp, iBarcDn, iQQQ; //ID

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
		tBarcUp = -1; tBarcDn = -1; tQQQ = -1;
		eBarcUp = -1; eBarcDn = -1; eQQQ = -1;

		for(int j=0; j<4; j++)
		{
			if(!event->fqqq[j].wedges.empty()) //QQQ backs
			{
				DetectorHit& wedge = event->fqqq[j].wedges[0]; //get QQQ_j wedge with biggest energy
				if(eQQQ<wedge.energy)
				{
					eQQQ = wedge.energy; //store biggest overall energy
					tQQQ = wedge.timestamp; //store time from biggest overall energy
					iQQQ = j; // store QQQ_j
				}
			}
		}
		for(int k=0; k<6; k++)
		{
			if(!event->barcUp[k].fronts.empty()) //upstream barcelona
			{
				DetectorHit& front = event->barcUp[k].fronts[0]; //get barcUp_k strip with biggest energy
				if(eBarcUp<front.energy)
				{
					eBarcUp = front.energy; //store biggest overall energy
					tBarcUp = front.timestamp; //store time from biggest overall energy
					iBarcUp = k; // store barcUp_k
				}
			}
			if(!event->barcDown[k].fronts.empty()) //downstream barcelona
			{
				DetectorHit& front = event->barcDown[k].fronts[0]; //get barcDn_k strip with biggest energy
				if(eBarcDn<front.energy)
				{
					eBarcDn = front.energy; //store biggest overall energy
					tBarcDn = front.timestamp; //store time from biggest overall energy
					iBarcDn = k; // store barcDn_k
				}
			}
		}

		if( eQQQ>-1 && eBarcUp>-1 && eBarcUp==std::max(eBarcDn,eBarcUp) ) //upstream barc signal bigger
		{
			//fill summary histograms
			name = "barc_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcUp);
			name = "barc_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);
			name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);

			//fill barcUp histograms
			name = "barcUp_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcUp);
			name = "barcUp_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);
			MyFill(histMap, "barcUpID_vs_qqqID", 4, 0, 4, 6, 0, 6, iQQQ, iBarcUp);

			if( (tQQQ-tBarcUp)>tMin && (tQQQ-tBarcUp)<tMax ) //time cut
			{
				//fill summary histograms_timecut
				name = "barc_qqq_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcUp);
				name = "barc_dE_vs_qqq_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);
				name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);

				//fill barcUp histograms_timecut
				name = "barcUp_qqq_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcUp);
				name = "barcUp_dE_vs_qqq_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcUp);
				MyFill(histMap, "barcUpID_vs_qqqID_timeCut", 4, 0, 4, 6, 0, 6, iQQQ, iBarcUp);
			}
		}
		else if( eQQQ>-1 && eBarcDn>-1 && eBarcDn==std::max(eBarcDn,eBarcUp) ) //downstream barc signal bigger
		{
			//fill summary histograms
			name = "barc_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcDn);
			name = "barc_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);
			name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);

			//fill barcDn histograms
			name = "barcDn_qqq_timePeak";
			MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcDn);
			name = "barcDn_dE_vs_qqq_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);
			MyFill(histMap, "barcDownID_vs_qqqID", 4, 0, 4, 6, 0, 6, iQQQ, iBarcDn);

			if( (tQQQ-tBarcDn)>tMin && (tQQQ-tBarcDn)<tMax )
			{
				//fill summary histograms_timecut
				name = "barc_qqq_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcDn);
				name = "barc_dE_vs_qqq_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);
				name = "barc_dE_vs_qqq" + std::to_string(iQQQ) + "_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);

				//fill barcDn histograms_timecut
				name = "barcDn_qqq_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tQQQ-tBarcDn);
				name = "barcDn_dE_vs_qqq_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eQQQ,eBarcDn);
				MyFill(histMap, "barcDownID_vs_qqqID_timeCut", 4, 0, 4, 6, 0, 6, iQQQ, iBarcDn);
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

void TimePeak()
{
	//Run_TimePeak();
	Run_TimePeakCuts();
}