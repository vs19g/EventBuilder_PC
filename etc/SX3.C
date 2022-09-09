#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "../src/evbdict/DataStructs.h"
#include "../src/evb/ChannelMap.cpp"

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


void SX3(int runNumber)
{
	std::unordered_map<std::string, TH1*> histMap;
	EventBuilder::ChannelMap chanMap("ANASEN_TRIUMFAug_run21+_ChannelMap.txt");
	std::string input_filename = "/media/tandem/Moria/WorkingData/built/run_"+std::to_string(runNumber)+".root";
	std::string output_filename = "/media/tandem/Moria/WorkingData/histograms/run_"+std::to_string(runNumber)+"_SX3.root";

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
	int sx3UpDownMatch[8] = {1, 0, 3, 2, 5, 4, 7, 6};
	for(auto i=0; i<nevents; i++){
		count++;
		if(count == flush_val){
			flush_count++;
			count = 0;
			std::cout<<"\rPercent of data processed: "<<flush_count*percent*100<<"%"<<std::flush;
		}

		tree->GetEntry(i);
		//	std::cout << "Gordon sucks because he's leaving me " << i << " days early" << std::endl;
		// for each of the 12 SX3s, need a plot of frontUp v frontDown for each of the four strips

		for(int j=0; j<12; j++){
			for(auto& frontup : event->barrel[j].frontsUp){
				for(auto& frontdown : event->barrel[j].frontsDown){
					auto iterUp = chanMap.FindChannel(frontup.globalChannel);
					auto iterDown = chanMap.FindChannel(frontdown.globalChannel);
					int partner = sx3UpDownMatch[iterUp->second.local_channel];
					if(iterDown->second.local_channel == partner){
					   name = "barrel"+std::to_string(j)+"_"+std::to_string(frontup.globalChannel)+"_"+std::to_string(frontdown.globalChannel)+"_upvdown";
					   MyFill(histMap,name,4096,0,4096,4096,0,4096,frontup.energy,frontdown.energy);		
					}
				}
			}
			auto backmax=0;
			for(auto& back : event->barrel[j].backs){
			   name = "barrel"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
			   MyFill(histMap,name,4096,0,4096,back.energy);

			   if(back.energy > backmax) backmax = back.energy;
			}
			for(auto& frontup : event->barrel[j].frontsUp){
			    for(auto& frontdown : event->barrel[j].frontsDown){
			      auto iterUp = chanMap.FindChannel(frontup.globalChannel);
		       	      auto iterDown = chanMap.FindChannel(frontdown.globalChannel);
			      int partner = sx3UpDownMatch[iterUp->second.local_channel];
			      if(iterDown->second.local_channel == partner){
					   name = "barrel"+std::to_string(j)+"_"+std::to_string(frontup.globalChannel)+"_"+std::to_string(frontdown.globalChannel)+"_upvdown/back";
					   MyFill(histMap,name,4096,0,4096,4096,0,4096,frontup.energy/backmax,frontdown.energy/backmax);		
			      }
			    }
			}
		}
	}

	std::cout << "hello, I'm outside the loop" << std::endl;

	inputfile->Close();
	outputfile->cd();
	for(auto& iter : histMap)
		iter.second->Write();
	outputfile->Close();

	delete inputfile;
	delete outputfile;
}


