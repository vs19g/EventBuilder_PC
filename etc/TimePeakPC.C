/*
  Generates histograms for events with PC & SX3Back hits with option to gate on timing peak.
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
R__LOAD_LIBRARY(../lib/libEVBDict.so);

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

//plots SX3Back vs PCC, SX3Back vs. pc, SX3Back vs. PC, SX3Back_i vs. PC with no timing peak cuts
void Run_TimePeak(int runNumber)
{	
	//set input & output files
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/home/vigneshsitaraman/PCTesting/WorkingData/built/run_"+std::to_string(runNumber)+".root";
	std::string output_filename = "/home/vigneshsitaraman/PCTesting/WorkingData/histograms/run_"+std::to_string(runNumber)+"_coinc.root";

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
	std::string strSX3b;

	double percent = 0.05;
	uint64_t flush_val = nevents * percent;
	uint64_t flush_count = 0;
	uint64_t count = 0;

	double tPCA, tPCC, tPC, tSX3b;
	double ePCA, ePCC, ePC, eSX3b;

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
		tPCA = -1; tPCC = -1; tSX3b = -1;
		ePCA = -1; ePCC = -1; eSX3b = -1;

		for(int j=0; j<12; j++)
		{
			if(!event->barrel[j].backs.empty()) //SX3Back backs
			{
				DetectorHit& back = event->barrel[j].backs[0]; //get SX3Back_j back with biggest energy
				if(eSX3b<back.energy)
				{
					eSX3b = back.energy; //store biggest overall energy
					tSX3b = back.timestamp/1000; //store time from biggest overall energy
					strSX3b = std::to_string(j); // store SX3Back_j
				}
			}
		}
			if(!event->pc[0].anodes.empty()) //upstream PC
			{
				DetectorHit& anode = event->pc[0].anodes[0]; //get pc_k strip with biggest energy
				if(ePCA<anode.energy)
				{
					ePCA = anode.energy; //store biggest overall energy
					tPCA = anode.timestamp/1000; //store time from biggest overall energy
				}
			}
			if(!event->pc[0].cathodes.empty()) //downstream PC
			{
				DetectorHit& cathode = event->pc[0].cathodes[0]; //get PCC_k strip with biggest energy
				if(ePCC<cathode.energy)
				{
					ePCC = cathode.energy; //store biggest overall energy
					tPCC = cathode.timestamp/1000; //store time from biggest overall energy
				}
			}
		

		if( eSX3b>-1 && ePCA>-1 && ePCA==std::max(ePCC,ePCA) ) //upstream PC signal bigger
		{
			//fill summary histograms
			name = "PC_SX3Back_timePeak";
			MyFill(histMap,name,512,-100,100,tSX3b-tPCA);
			name = "PC_dE_vs_SX3Back_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);
			name = "PC_dE_vs_SX3Back" + strSX3b + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);

			//fill pc histograms
			name = "pc_SX3Back_timePeak";
			MyFill(histMap,name,512,-100,100,tSX3b-tPCA);
			name = "pc_dE_vs_SX3Back_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);
		}
		else if( eSX3b>-1 && ePCC>-1 && ePCC==std::max(ePCC,ePCA) ) //downstream PC signal bigger
		{
			//fill summary histograms
			name = "PC_SX3Back_timePeak";
			MyFill(histMap,name,512,-100,100,tSX3b-tPCC);
			name = "PC_dE_vs_SX3Back_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);
			name = "PC_dE_vs_SX3Back" + strSX3b + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);

			//fill PCDn histograms
			name = "pcc_SX3Back_timePeak";
			MyFill(histMap,name,512,-100,100,tSX3b-tPCC);
			name = "pcc_dE_vs_SX3Back_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);
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

	double tPCA, tPCC, tSX3b; //time
	double ePCA, ePCC, eSX3b; //energy
	int iPCA, iPCC, iSX3b; //ID

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
		tPCA = -1; tPCC = -1; tSX3b = -1;
		ePCA = -1; ePCC = -1; eSX3b = -1;

		for(int j=0; j<4; j++)
		{
			if(!event->barrel[j].backs.empty()) //sx3 backs
			{
				DetectorHit& back = event->barrel[j].backs[0]; //get sx3_j back with biggest energy
				if(eSX3b<back.energy)
				{
					eSX3b = back.energy; //store biggest overall energy
					tSX3b = back.timestamp; //store time from biggest overall energy
					iSX3b = j; // store sx3_j
				}
			}
		}
		for(int k=0; k<6; k++)
		{
			if(!event->pc[0].anodes.empty()) //upstream PC
			{
				DetectorHit& anode = event->pc[0].anodes[k]; //get pc_k strip with biggest energy
				if(ePCA<anode.energy)
				{
					ePCA = anode.energy; //store biggest overall energy
					tPCA = anode.timestamp; //store time from biggest overall energy
					iPCA = anode.globalChannel; // store pca_k
				}
			}
			if(!event->pc[0].cathodes.empty()) //downstream PC
			{
				DetectorHit& cathode = event->pc[0].cathodes[k]; //get pcc_k strip with biggest energy
				if(ePCC<cathode.energy)
				{
					ePCC = cathode.energy; //store biggest overall energy
					tPCC = cathode.timestamp; //store time from biggest overall energy
					iPCC = cathode.globalChannel; // store pcc_k
				}
			}
		}

		if( eSX3b>-1 && ePCA>-1 && ePCA==std::max(ePCC,ePCA) ) //upstream PC signal bigger
		{
			//fill summary histograms
			name = "anode_sx3_timePeak";
			MyFill(histMap,name,128,-3072,3072,tSX3b-tPCA);
			name = "anode_dE_vs_sx3_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);
			name = "anode_dE_vs_sx3" + std::to_string(iSX3b) + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);

			//fill pc histograms
			name = "pc_sx3_timePeak";
			MyFill(histMap,name,128,-3072,3072,tSX3b-tPCA);
			name = "pc_dE_vs_sx3_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);
			MyFill(histMap, "pcID_vs_sx3ID", 4, 0, 4, 6, 0, 6, iSX3b, iPCA);

			if( (tSX3b-tPCA)>tMin && (tSX3b-tPCA)<tMax ) //time cut
			{
				//fill summary histograms_timecut
				name = "anode_sx3_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tSX3b-tPCA);
				name = "anode_dE_vs_sx3_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);
				name = "anode_dE_vs_sx3" + std::to_string(iSX3b) + "_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);

				//fill pc histograms_timecut
				name = "pc_sx3_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tSX3b-tPCA);
				name = "pc_dE_vs_sx3_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCA);
				MyFill(histMap, "pcID_vs_sx3ID_timeCut", 4, 0, 4, 6, 0, 6, iSX3b, iPCA);
			}
		}
		else if( eSX3b>-1 && ePCC>-1 && ePCC==std::max(ePCC,ePCA) ) //downstream cathode signal bigger
		{
			//fill summary histograms
			name = "cathode_sx3_timePeak";
			MyFill(histMap,name,128,-3072,3072,tSX3b-tPCC);
			name = "cathode_dE_vs_sx3_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);
			name = "cathode_dE_vs_sx3" + std::to_string(iSX3b) + "_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);

			//fill cathodeDn histograms
			name = "cathode_SX3Back_timePeak";
			MyFill(histMap,name,128,-3072,3072,tSX3b-tPCC);
			name = "cathode_dE_vs_SX3Back_E";
			MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);
			MyFill(histMap, "pccID_vs_SX3BackID", 4, 0, 4, 6, 0, 6, iSX3b, iPCC);

			if( (tSX3b-tPCC)>tMin && (tSX3b-tPCC)<tMax )
			{
				//fill summary histograms_timecut
				name = "cathode_SX3Back_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tSX3b-tPCC);
				name = "cathode_dE_vs_SX3Back_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);
				name = "cathode_dE_vs_SX3Back" + std::to_string(iSX3b) + "_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);

				//fill cathodeDn histograms_timecut
				name = "cathode_SX3Back_timePeak_timeCut";
				MyFill(histMap,name,128,-3072,3072,tSX3b-tPCC);
				name = "cathode_dE_vs_SX3Back_E_timeCut";
				MyFill(histMap,name,512,0,4096,512,0,4096,eSX3b,ePCC);
				MyFill(histMap, "pccID_vs_SX3BackID_timeCut", 4, 0, 4, 6, 0, 6, iSX3b, iPCC);
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

void TimePeakPC(int run)
{
	Run_TimePeak(run);
	// Run_TimePeakCuts();
}