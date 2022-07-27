#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "../src/evbdict/DataStructs.h"

#include <string>
#include <vector>
#include <unordered_map>


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

void ANASENPlotEdit()
{
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/mnt/data1/WorkingData/built/run_106.root";
	std::string output_filename = "/mnt/data1/WorkingData/histograms/run_106.root";

	TFile* inputfile = TFile::Open(input_filename.c_str(), "READ");
	TTree* tree = (TTree*) inputfile->Get("SortTree");

	CoincEvent* event = new CoincEvent();

	tree->SetBranchAddress("event", &event);

	TFile* outputfile = TFile::Open(output_filename.c_str(), "RECREATE");

	auto nevents = tree->GetEntries();
	std::cout<<"Total events: "<<nevents<<std::endl;
	std::string name;
	for(auto i=0; i<nevents; i++)
	{
		std::cout<<"\rEvent number "<<i<<" being processed"<<std::flush;
		tree->GetEntry(i);
		for(int j=0; j<12; j++)
		{
			for(auto& frontup : event->barrel[j].frontsUp)
			{
				name = "barrel"+std::to_string(j)+"_"+std::to_string(frontup.globalChannel)+"_frontUp";
				MyFill(histMap,name,4096,0,4096,frontup.energy);
			}
			for(auto& frontdown : event->barrel[j].frontsDown)
			{
				name = "barrel"+std::to_string(j)+"_"+std::to_string(frontdown.globalChannel)+"_frontDown";
				MyFill(histMap,name,4096,0,4096,frontdown.energy);
			}
			for(auto& back : event->barrel[j].backs)
			{
				name = "barrel"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
				MyFill(histMap,name,4096,0,4096,back.energy);
				
			    for(int k=0; k<6; k++)  //loop over barc detectors
			    {
			        for(auto& front : event->barcUp[k].fronts)  //loop over barcup fronts
			        {
			           name = "barcUp_dE_vs_SX3_E";
			           MyFill(histMap,name,4096,0,4096,4096,0,4096,back.energy,front.energy);  // plot qqq vs barcup
			        }
			        for(auto& front : event->barcDown[k].fronts)  //loop over barcdown fronts
			        {
			            name = "barcDown_dE_vs_SX3_E";
			            MyFill(histMap,name,4096,0,4096,4096,0,4096,back.energy,front.energy);  // plot qqq vs barcdown
			        }
		        }
			}
		}
		for(int j=0; j<4; j++)  //loop over the QQQ detectors
		{
			for(auto& ring : event->fqqq[j].rings)  //loop over the rings
			{
				name = "fqqq"+std::to_string(j)+"_"+std::to_string(ring.globalChannel)+"_ring";
				MyFill(histMap,name,4096,0,4096,ring.energy);
				
			    for(int k=0; k<6; k++)  //loop over barc detectors
		        {
			        for(auto& front : event->barcUp[k].fronts)  //loop over barcup fronts
			        {
				        name = "barcUp_dE_vs_qqq_E";
				        MyFill(histMap,name,4096,0,4096,4096,0,4096,ring.energy,front.energy);  // plot qqq vs barcup
				    }
				  
				    for(auto& front : event->barcDown[k].fronts)  //loop over barcdown fronts
			        {
				        name = "barcDown_dE_vs_qqq_E";
				        MyFill(histMap,name,4096,0,4096,4096,0,4096,ring.energy,front.energy);  // plot qqq vs barcdown
				    }
		        }
				for(auto& wedge : event->fqqq[j].wedges)
			    {
				    name = "fqqq"+std::to_string(j)+"_ring_wedge";
				    MyFill(histMap,name,4096,0,4096,4096,0,4096,ring.energy,wedge.energy);
			    }
		    }
			for(auto& wedge : event->fqqq[j].wedges)
			{
				name = "fqqq"+std::to_string(j)+"_"+std::to_string(wedge.globalChannel)+"_wedge";
				MyFill(histMap,name,4096,0,4096,wedge.energy);
			}
		}
		
		for(int j=0; j<6; j++)
		{
            name = "barcUp"+std::to_string(j)+"_multiplicity";
            MyFill(histMap, name, 10, 0.0, 10.0, event->barcUp[j].fronts.size());
            name = "barcDown"+std::to_string(j)+"_multiplicity";
            MyFill(histMap, name, 10, 0.0, 10.0, event->barcDown[j].fronts.size());
			for(auto& front : event->barcUp[j].fronts)
			{
				name = "barcUp"+std::to_string(j)+"_"+std::to_string(front.globalChannel)+"_front";
				MyFill(histMap,name,4096,0,4096,front.energy);
			}
			for(auto& back : event->barcUp[j].backs)
			{
				name = "barcUp"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
				MyFill(histMap,name,4096,0,4096,back.energy);
			}
			for(auto& front : event->barcDown[j].fronts)
			{
				name = "barcDown"+std::to_string(j)+"_"+std::to_string(front.globalChannel)+"_front";
				MyFill(histMap,name,4096,0,4096,front.energy);
			}
			for(auto& back : event->barcDown[j].backs)
			{
				name = "barcDown"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
				MyFill(histMap,name,4096,0,4096,back.energy);
			}
		}
	}
	std::cout<<std::endl;
	inputfile->Close();
	outputfile->cd();
	for(auto& iter : histMap)
		iter.second->Write();
	outputfile->Close();
}
