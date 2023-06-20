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

	for(int i=0; i<6; i++)
	{
		for(auto& front : event->barcDown[i].fronts)
			timeStamps.push_back(front.timestamp);
		for(auto& front : event->barcUp[i].fronts)
			timeStamps.push_back(front.timestamp);
	}

	std::sort(timeStamps.begin(), timeStamps.end());

	double first = timeStamps[0];
	double last = timeStamps[timeStamps.size() - 1];
	std::cout<<"First: "<<first<<" Last: "<<last<<" Difference: "<<last-first<<std::endl;
}

void Run_ANASENPlotEdit_AllCombos(int runNumber)
{
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/media/tandem/Moria/WorkingData/built/run_"+std::to_string(runNumber)+".root";
	std::string output_filename = "/media/tandem/Moria/WorkingData/histograms/allCombos/run_"+std::to_string(runNumber)+"_allCombos.root";

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
				
			    for(int k=0; k<6; k++)  //loop over barc detectors
			    {
			        for(auto& front : event->barcUp[k].fronts)  //loop over barcup fronts
			        {
			           name = "barcUp_dE_vs_SX3_E";
			           MyFill(histMap,name,512,0,4096,512,0,4096,back.energy,front.energy);  // plot qqq vs barcup
			        }
			        for(auto& front : event->barcDown[k].fronts)  //loop over barcdown fronts
			        {
			            name = "barcDown_dE_vs_SX3_E";
			            MyFill(histMap,name,512,0,4096,512,0,4096,back.energy,front.energy);  // plot qqq vs barcdown
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
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, ring.globalChannel, ring.energy);
				
			    for(int k=0; k<6; k++)  //loop over barc detectors
		        {
			        for(auto& front : event->barcUp[k].fronts)  //loop over barcup fronts
			        {
				        name = "barcUp_dE_vs_qqq_E";
				        MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);  // plot qqq vs barcup
						name = "barcUp_dE_vs_qqq" + std::to_string(j) + "_E";
						MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);
						MyFill(histMap, "barcUpID_vs_qqqID", 4, 0, 4, 6, 0, 6, j, k);
				    }
				  
				    for(auto& front : event->barcDown[k].fronts)  //loop over barcdown fronts
			        {
				        name = "barcDown_dE_vs_qqq_E";
				        MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);  // plot qqq vs barcdown
						name = "barcDown_dE_vs_qqq" + std::to_string(j) + "_E";
						MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);
						MyFill(histMap, "barcDownID_vs_qqqID", 4, 0, 4, 6, 0, 6,  j, k);
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
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, wedge.globalChannel, wedge.energy);
			}
		}
		
		for(int j=1; j<4; j++)  //loop over the QQQ detectors
		{
			for(auto& ring : event->fqqq[j].rings)  //loop over the rings
			{
				 for(int k=0; k<6; k++)  //loop over barc detectors
		        {
			        for(auto& front : event->barcUp[k].fronts)  //loop over barcup fronts
			        {
				        name = "barcUp_dE_vs_qqq123_E";
				        MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);  // plot qqq vs barcup
				    }
				  
				    for(auto& front : event->barcDown[k].fronts)  //loop over barcdown fronts
			        {
				        name = "barcDown_dE_vs_qqq123_E";
				        MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);  // plot qqq vs barcdown
				    }
		        }
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
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, front.globalChannel, front.energy);
			}
			for(auto& back : event->barcUp[j].backs)
			{
				name = "barcUp"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
				MyFill(histMap,name,4096,0,4096,back.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, back.globalChannel, back.energy);
			}
			for(auto& front : event->barcDown[j].fronts)
			{
				name = "barcDown"+std::to_string(j)+"_"+std::to_string(front.globalChannel)+"_front";
				MyFill(histMap,name,4096,0,4096,front.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, front.globalChannel, front.energy);
			}
			for(auto& back : event->barcDown[j].backs)
			{
				name = "barcDown"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
				MyFill(histMap,name,4096,0,4096,back.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, back.globalChannel, back.energy);
			}
		}

		/*
		for(int j=0; j<6; j++)
		{
			std::vector<DetectorHit>& barcDown1 = event->barcDown[j].fronts;
			std::vector<DetectorHit>& barcUp1 = event->barcUp[j].fronts;
			for(int k=0; k<6; k++)
			{
				std::vector<DetectorHit>& barcDown2 = event->barcDown[k].fronts;
				std::vector<DetectorHit>& barcUp2 = event->barcUp[k].fronts;
				for(auto& hit1 : barcDown1)
				{
					for(auto& hit2 : barcDown2)
					{
						MyFill(histMap, "barcDownID_vs_barcDownID", 6, 0, 6, 6, 0, 6, j, k);
						name = "barcDown"+std::to_string(j)+"barcDown"+std::to_string(k)+"_time";
						MyFill(histMap, name, 1000, -5000.0, 5000.0, hit1.timestamp - hit2.timestamp);
					}
				}
				for(auto& hit1 : barcUp1)
				{
					for(auto& hit2 : barcUp2)
					{
						MyFill(histMap, "barcUpID_vs_barcUpID", 6, 0, 6, 6, 0, 6, j, k);
						name = "barcUp"+std::to_string(j)+"barcUp"+std::to_string(k)+"_time";
						MyFill(histMap, name, 1000, -5000.0, 5000.0, hit1.timestamp - hit2.timestamp);
					}
				}
				for(auto& hit1 : barcDown1)
				{
					for(auto& hit2 : barcUp2)
					{
						MyFill(histMap, "barcUpID_vs_barcDownID", 6, 0, 6, 6, 0, 6, j, k);
						name = "barcDown"+std::to_string(j)+"barcUp"+std::to_string(k)+"_time";
						MyFill(histMap, name, 1000, -5000.0, 5000.0, hit1.timestamp - hit2.timestamp);
					}
				}
			}
		}
		*/

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

void Run_ANASENPlotEdit_LargestEnergy(int runNumber)
{
	std::unordered_map<std::string, TH1*> histMap;
	std::string input_filename = "/media/tandem/Moria/WorkingData/built/run_"+std::to_string(runNumber)+".root";
	std::string output_filename = "/media/tandem/Moria/WorkingData/histograms/run_"+std::to_string(runNumber)+".root";

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
			if(!event->barrel[j].frontsUp.empty())
			{
				DetectorHit& frontup = event->barrel[j].frontsUp[0];
				//name = "barrel"+std::to_string(j)+"_"+std::to_string(frontup.globalChannel)+"_frontUp";
				//MyFill(histMap,name,4096,0,4096,frontup.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, frontup.globalChannel, frontup.energy);
				
			}
			if(!event->barrel[j].frontsDown.empty())
			{
				DetectorHit& frontdown = event->barrel[j].frontsDown[0];
				//name = "barrel"+std::to_string(j)+"_"+std::to_string(frontdown.globalChannel)+"_frontDown";
				//MyFill(histMap,name,4096,0,4096,frontdown.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, frontdown.globalChannel, frontdown.energy);
			}
			if(!event->barrel[j].backs.empty())
			{
				DetectorHit& back = event->barrel[j].backs[0];
				//name = "barrel"+std::to_string(j)+"_"+std::to_string(back.globalChannel)+"_back";
				//MyFill(histMap,name,4096,0,4096,back.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, back.globalChannel, back.energy);
				
			    for(int k=0; k<6; k++)  //loop over barc detectors
			    {
			        if(!event->barcUp[k].fronts.empty())  //loop over barcup fronts
			        {
						DetectorHit& front = event->barcUp[k].fronts[0];
			    		name = "barcUp_dE_vs_SX3_E";
			        	MyFill(histMap,name,512,0,4096,512,0,4096,back.energy,front.energy);  // plot qqq vs barcup
			        }
			        if(!event->barcDown[k].fronts.empty())  //loop over barcdown fronts
			        {
						DetectorHit& front = event->barcDown[k].fronts[0];
			            name = "barcDown_dE_vs_SX3_E";
			            MyFill(histMap,name,512,0,4096,512,0,4096,back.energy,front.energy);  // plot qqq vs barcdown
			        }
		        }
			}
		}
		for(int j=0; j<4; j++)  //loop over the QQQ detectors
		{
			if(!event->fqqq[j].rings.empty())  //loop over the rings
			{
				DetectorHit& ring = event->fqqq[j].rings[0];
				//name = "fqqq"+std::to_string(j)+"_"+std::to_string(ring.globalChannel)+"_ring";
				//MyFill(histMap,name,4096,0,4096,ring.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, ring.globalChannel, ring.energy);
				
			    for(int k=0; k<6; k++)  //loop over barc detectors
		        {
			        if(!event->barcUp[k].fronts.empty())  //loop over barcup fronts
			        {
						DetectorHit& front = event->barcUp[k].fronts[0];
				        name = "barcUp_dE_vs_qqq_E";
				        MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);  // plot qqq vs barcup
						name = "barcUp_dE_vs_qqq" + std::to_string(j) + "_E";
						MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);
						MyFill(histMap, "barcUpID_vs_qqqID", 4, 0, 4, 6, 0, 6, j, k);
				    }
				  
			        if(!event->barcDown[k].fronts.empty())  //loop over barcup fronts
			        {
						DetectorHit& front = event->barcDown[k].fronts[0];
				        name = "barcDown_dE_vs_qqq_E";
				        MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);  // plot qqq vs barcdown
						name = "barcDown_dE_vs_qqq" + std::to_string(j) + "_E";
						MyFill(histMap,name,512,0,4096,512,0,4096,ring.energy,front.energy);
						MyFill(histMap, "barcDownID_vs_qqqID", 4, 0, 4, 6, 0, 6,  j, k);
				    }
		        }
				if(!event->fqqq[j].wedges.empty())
			    {
					DetectorHit& wedge = event->fqqq[j].wedges[0];
				    name = "fqqq"+std::to_string(j)+"_ring_wedge";
				    MyFill(histMap,name,4096,0,4096,4096,0,4096,ring.energy,wedge.energy);
			    }
		    }
			if(!event->fqqq[j].wedges.empty())
			{
				DetectorHit& wedge = event->fqqq[j].wedges[0];
				//name = "fqqq"+std::to_string(j)+"_"+std::to_string(wedge.globalChannel)+"_wedge";
				//MyFill(histMap,name,4096,0,4096,wedge.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, wedge.globalChannel, wedge.energy);
			}
		}
		
		for(int j=0; j<6; j++)
		{
            name = "barcUp"+std::to_string(j)+"_multiplicity";
            MyFill(histMap, name, 10, 0.0, 10.0, event->barcUp[j].fronts.size());
            name = "barcDown"+std::to_string(j)+"_multiplicity";
            MyFill(histMap, name, 10, 0.0, 10.0, event->barcDown[j].fronts.size());
			if(!event->barcUp[j].fronts.empty())
			{
				DetectorHit& front = event->barcUp[j].fronts[0];
				//name = "barcUp"+std::to_string(j)+"_"+std::to_string(front.globalChannel)+"_front";
				//MyFill(histMap,name,4096,0,4096,front.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, front.globalChannel, front.energy);
			}
			if(!event->barcDown[j].fronts.empty())
			{
				DetectorHit& front = event->barcDown[j].fronts[0];
				//name = "barcDown"+std::to_string(j)+"_"+std::to_string(front.globalChannel)+"_front";
				//MyFill(histMap,name,4096,0,4096,front.energy);
				MyFill(histMap, summary_hist_name, 640, 0, 640, 512, 0.0, 4096.0, front.globalChannel, front.energy);
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

void ANASENPlotEdit(int runMin, int runMax)
{
	/*
	for(int i=runMin; i<=runMax; i++)
		Run_ANASENPlotEdit_AllCombos(i);
	*/
	for(int i=runMin; i<=runMax; i++)
		Run_ANASENPlotEdit_LargestEnergy(i);
}
