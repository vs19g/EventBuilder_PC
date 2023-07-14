#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "../src/evbdict/DataStructs.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>


R__LOAD_LIBRARY(../lib/libEVBDict.so);

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

void TestEventTime(CoincEvent* event)
{
	std::vector<double> timeStamps;
	for(int i=0; i<12; i++)
	{
		for(auto& front : event->barrel[i].frontsUp)
			timeStamps.push_back(front.timestamp);
		for(auto& front : event->barrel[i].frontsDown)
			timeStamps.push_back(front.timestamp);
		for(auto& back : event->barrel[i].backs)
			timeStamps.push_back(back.timestamp);
	}

	for(int i=0; i<4; i++)
	{
		for(auto& ring : event->fqqq[i].rings)
			timeStamps.push_back(ring.timestamp);
		for(auto& wedge : event->fqqq[i].wedges)
			timeStamps.push_back(wedge.timestamp);
	}

	
	for(auto& anode : event->pc[0].anodes)
		timeStamps.push_back(anode.timestamp);
	for(auto& cathode : event->pc[0].cathodes)
		timeStamps.push_back(cathode.timestamp);
	

	std::sort(timeStamps.begin(), timeStamps.end());

	double first = timeStamps[0];
	double last = timeStamps[timeStamps.size() - 1];
	std::cout<<"First: "<<first<<" Last: "<<last<<" Difference: "<<last-first<<std::endl;
}

void Run_ANASENPlotEdit_AllCombos(int runNumber)
{
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/home/vigneshsitaraman/PCTesting/WorkingData/built/run_"+std::to_string(runNumber)+".root";
	std::string output_filename = "/home/vigneshsitaraman/PCTesting/WorkingData/histograms/run_"+std::to_string(runNumber)+"_edit.root";

	std::cout<<"Processing data in "<<input_filename<<std::endl;
	std::cout<<"Writing histograms to "<<output_filename<<std::endl;

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

	std::string summary_hist_name = "SummaryHistogram";
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

		//TestEventTime(event);
		for(int j=0; j<12; j++)
		{
			for(auto& frontup : event->barrel[j].frontsUp)
			{
				name = "barrel"+std::to_string(j)+"_"+std::to_string(frontup.globalChannel)+"_frontUp";
				MyFill(histMap,name,4096,0,4096,frontup.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, frontup.globalChannel, frontup.energy);
				
			}
			for(auto& frontdown : event->barrel[j].frontsDown)
			{
				name = "barrel"+std::to_string(j)+"_"+std::to_string(frontdown.globalChannel)+"_frontDown";
				MyFill(histMap,name,4096,0,4096,frontdown.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, frontdown.globalChannel, frontdown.energy);
			}
			for(auto& back : event->barrel[j].backs)
			{
				name = "barrel"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
				MyFill(histMap,name,4096,0,4096,back.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, back.globalChannel, back.energy);
		    }

			for(auto& anode : event->pc[0].anodes){
			    for(auto& cathode : event->pc[0].cathodes){  
			    	{
					name = "PC Anode vs PC Cathode";
					MyFill(histMap, name, 512,0,4096,512,0,4096, anode.energy, cathode.energy);
					}
				}
			}

		}

	// for(auto& anode : event->pc[0].anodes){
	// 		    for(auto& cathode : event->pc[0].cathodes){  
	// 		    {
	// 				name = "pc_"+std::to_string(anode.globalChannel)+"_anode"+"_"+"pc"+std::to_string(cathode.globalChannel)+"_cathode";
	// 				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, anode.globalChannel, cathode.globalChannel);
	// 			}
	// 			}
	// 		    }	
		
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


void ANASENPlotEditPC(int runMin, int runMax)
{
	for(int i=runMin; i<=runMax; i++)
	Run_ANASENPlotEdit_AllCombos(i);
}
