/*
	CompassRun.cpp
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020

	Updated to also handle scaler data. -- GWM Oct. 2020
*/
#include "EventBuilder.h"
#include "CompassRun.h"
#include "RunCollector.h"
#include "SlowSort.h"
#include "FlagHandler.h"

namespace EventBuilder {

	CompassRun::CompassRun() :
		directory(""), m_scalerinput(""), runNum(0), m_buffersize(200000), m_scaler_flag(false)
	{
	
	}
	
	CompassRun::CompassRun(const std::string& dir, size_t bsize) :
		directory(dir), m_scalerinput(""), runNum(0), m_buffersize(bsize), m_scaler_flag(false)
	{
	
	}
	
	CompassRun::~CompassRun() {}
	
	
	/*Load em into a map*/
	void CompassRun::SetScalers()
	{
		std::ifstream input(m_scalerinput);
		if(!input.is_open()) return;
	
		m_scaler_flag = true;
		std::string junk, filename, varname;
		int64_t init = 0;
		std::getline(input, junk);
		std::getline(input, junk);
		m_scaler_map.clear();
		while(input>>filename)
		{
			input>>varname;
			filename = directory+filename+"_run_"+std::to_string(runNum)+".bin";
			m_scaler_map[filename] = TParameter<int64_t>(varname.c_str(), init);
		}
		input.close();
	}
	
	bool CompassRun::GetBinaryFiles()
	{
		std::string prefix = "";
		std::string suffix = ".bin"; //binaries
		RunCollector grabber(directory, prefix, suffix);
		grabber.GrabAllFiles();
	
		m_datafiles.clear(); //so that the CompassRun can be reused
		m_datafiles.reserve(grabber.GetFileList().size());
		bool scalerd;
		m_totalHits = 0; //reset total run size
	
		for(auto& entry : grabber.GetFileList())
		{
			//Handle scaler files, if they exist
			if(m_scaler_flag)
			{
				scalerd = false;
				for(auto& scaler_pair : m_scaler_map)
				{
					if(entry == scaler_pair.first)
					{
						ReadScalerData(entry);
						scalerd = true;
						break;
					}
				}
				if(scalerd)
					continue;
			}
			m_datafiles.emplace_back(entry, m_buffersize);
			m_datafiles[m_datafiles.size()-1].AttachShiftMap(&m_smap);
			//Any time we have a file that fails to be found, we terminate the whole process
			if(!m_datafiles[m_datafiles.size() - 1].IsOpen())
				return false;

			m_totalHits += m_datafiles[m_datafiles.size()-1].GetNumberOfHits();
		}
	
		return true;
	}
	
	/*
		Pure counting of scalers. Potential upgrade path to something like
		average count rate etc. 
	*/
	void CompassRun::ReadScalerData(const std::string& filename)
	{
		if(!m_scaler_flag)
			return;
	
		int64_t count = 0;
		CompassFile file(filename);
		auto& this_param = m_scaler_map[file.GetName()];
		while(true)
		{
			file.GetNextHit();
			if(file.IsEOF())
				break;
			count++;
		}
		this_param.SetVal(count);
	}
	
	/*
		GetHitsFromFiles() is the function which actually retrieves and sorts the data from the individual
		files. There are several tricks which allow this to happen. First is that, after sorting, it is impossible
		to determine which file the data originally came from (short of parsing the name of the file against board/channel).
		However, we need to let the file know that we want it to pull the next hit. To do this, a pointer to the UsedFlag of the file
		is retrieved along with the data. This flag is flipped so that on the next hit cycle a new hit is pulled. Second is the use
		of a rolling start index. Once a file has gone EOF, we no longer need it. If this is the first file in the list, we can just skip
		that index all together. In this way, the loop can go from N times to N-1 times.
	*/
	bool CompassRun::GetHitsFromFiles()
	{
	
		std::pair<CompassHit, bool*> earliestHit = std::make_pair(CompassHit(), nullptr);
		for(unsigned int i=startIndex; i<m_datafiles.size(); i++)
		{
			if(m_datafiles[i].CheckHitHasBeenUsed())
				m_datafiles[i].GetNextHit();
	
			if(m_datafiles[i].IsEOF())
			{
				if(i == startIndex)
					startIndex++;
				continue;
			}
			else if(i == startIndex)
			{
				earliestHit = std::make_pair(m_datafiles[i].GetCurrentHit(), m_datafiles[i].GetUsedFlagPtr());
			} 
			else if(m_datafiles[i].GetCurrentHit().timestamp < earliestHit.first.timestamp)
			{
				earliestHit = std::make_pair(m_datafiles[i].GetCurrentHit(), m_datafiles[i].GetUsedFlagPtr());
			}
		}
	
		if(earliestHit.second == nullptr)
			return false; //Make sure that there actually was a hit
		hit = earliestHit.first;
		*earliestHit.second = true;
		return true;
	}
	
	void CompassRun::Convert2RawRoot(const std::string& name)
	{
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("Data", "Data");
	
		outtree->Branch("Board", &hit.board);
		outtree->Branch("Channel", &hit.channel);
		outtree->Branch("Energy", &hit.lgate);
		outtree->Branch("EnergyShort", &hit.sgate);
		outtree->Branch("Timestamp", &hit.timestamp);
		outtree->Branch("Flags", &hit.flags);
	
		if(!m_smap.IsSet())
		{
			std::cerr<<"Bad shift map at CompassRun::Convert()."<<std::endl;
			std::cerr<<"Shifts will be locked to 0"<<std::endl;
		}
	
		SetScalers();
	
		if(!GetBinaryFiles())
		{
			std::cerr<<"Unable to open a file!"<<std::endl;
			return;
		}
	
		startIndex = 0; //Reset the startIndex

		unsigned int count = 0, flush = m_totalHits*m_progressFraction, flush_count = 0;
		if(flush == 0)
			flush = 1;

		while(true)
		{
			count++;
			if(count == flush)
			{ //Progress Log
				count = 0;
				flush_count++;
				m_progressCallback(flush*flush_count, m_totalHits);
			}
	
			if(!GetHitsFromFiles())
				break;
			outtree->Fill();
		}
	
		output->cd();
		outtree->Write(outtree->GetName(), TObject::kOverwrite);
		for(auto& entry : m_scaler_map)
			entry.second.Write();
		output->Close();
	}
	
	void CompassRun::Convert2SortedRoot(const std::string& name, double window) {
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("SortTree", "SortTree");
	
		outtree->Branch("event", &event);
	
		if(!m_smap.IsSet())
		{
			std::cerr<<"Bad shift map at CompassRun::Convert()."<<std::endl;
			std::cerr<<"Shifts will be locked to 0"<<std::endl;
		}
	
		SetScalers();
	
		if(!GetBinaryFiles()) {
			std::cerr<<"Unable to open a file!"<<std::endl;
			return;
		}
	
	
		startIndex = 0;
		SlowSort coincidizer(window);
		bool killFlag = false;

		uint64_t count = 0, flush = m_totalHits*0.01, flush_count = 0;
		if(flush == 0)
			flush = 1;

		while(true)
		{
			count++;
			if(count == flush)
			{
				count = 0;
				flush_count++;
				m_progressCallback(flush*flush_count, m_totalHits);
			}
	
			if(!GetHitsFromFiles())
			{
				coincidizer.FlushHitsToEvent();
				killFlag = true;
			}
			else
				coincidizer.AddHitToEvent(hit);
	
			if(coincidizer.IsEventReady())
			{
				event = coincidizer.GetEvent();
				outtree->Fill();
				if(killFlag)
					break;
			}
		}
	
		output->cd();
		outtree->Write(outtree->GetName(), TObject::kOverwrite);
		for(auto& entry : m_scaler_map)
			entry.second.Write();
		coincidizer.GetEventStats()->Write();
		output->Close();
	}

}