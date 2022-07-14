/*
	CompassRun.cpp
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020

	Updated to also handle scaler data. -- GWM Oct. 2020
*/
#include "CompassRun.h"
#include "SlowSort.h"
#include "FlagHandler.h"

namespace EventBuilder {
	
	CompassRun::CompassRun(const EVBParameters& params, const std::shared_ptr<EVBWorkspace>& workspace) :
		m_params(params), m_workspace(workspace), m_runNum(0), m_scaler_flag(false)
	{
		m_smap.SetFile(m_params.timeShiftFile);
	}
	
	CompassRun::~CompassRun() {}
	
	
	/*Load em into a map*/
	void CompassRun::SetScalers()
	{
		std::ifstream input(m_params.scalerFile);
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
			filename = m_workspace->GetTempDir()+filename+"_run_"+std::to_string(m_runNum)+".BIN";
			m_scaler_map[filename] = TParameter<int64_t>(varname.c_str(), init);
		}
		input.close();
	}
	
	bool CompassRun::GetBinaryFiles()
	{
		auto files = m_workspace->GetTempFiles();

		m_datafiles.clear(); //so that the CompassRun can be reused
		m_datafiles.reserve(files.size()); //To avoid move semantics use reserve. Moving std::ifstream is dangerous
		bool scalerd;
		m_totalHits = 0; //reset total run size
	
		for(auto& entry : files)
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
			m_datafiles.emplace_back(entry, m_params.bufferSize);
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
		files. Once a file has gone EOF, we no longer need it. If this is the first file in the list, we can just skip
		that index all together. In this way, the loop can go from N times to N-1 times.
	*/
	bool CompassRun::GetHitsFromFiles()
	{
	
		CompassFile* earliestHit = nullptr;
		for(unsigned int i=m_startIndex; i<m_datafiles.size(); i++)
		{
			if(m_datafiles[i].CheckHitHasBeenUsed())
				m_datafiles[i].GetNextHit();
	
			if(m_datafiles[i].IsEOF())
			{
				if(i == m_startIndex)
					m_startIndex++;
				continue;
			}
			else if(i == m_startIndex)
				earliestHit = &m_datafiles[i];
			else if(m_datafiles[i].GetCurrentHit().timestamp < earliestHit->GetCurrentHit().timestamp)
				earliestHit = &m_datafiles[i];
		}
	
		if(earliestHit == nullptr)
			return false; //Make sure that there actually was a hit
		m_hit = earliestHit->GetCurrentHit();
		earliestHit->SetHitHasBeenUsed();
		return true;
	}
	
	void CompassRun::Convert2RawRoot(const std::string& name)
	{
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("Data", "Data");
	
		outtree->Branch("Board", &m_hit.board);
		outtree->Branch("Channel", &m_hit.channel);
		outtree->Branch("Energy", &m_hit.energy);
		outtree->Branch("EnergyShort", &m_hit.energyShort);
		outtree->Branch("EnergyCal", &m_hit.energyCalibrated);
		outtree->Branch("Timestamp", &m_hit.timestamp);
		outtree->Branch("Flags", &m_hit.flags);
		outtree->Branch("Ns", &m_hit.Ns);
		outtree->Branch("Samples", &m_hit.samples);
	
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
	
		m_startIndex = 0; //Reset the startIndex

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
	
	void CompassRun::Convert2SortedRoot(const std::string& name) {
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("SortTree", "SortTree");
	
		std::vector<DPPChannel> event;
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
	
	
		m_startIndex = 0;
		SlowSort coincidizer(m_params.slowCoincidenceWindow);
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
				coincidizer.AddHitToEvent(m_hit);
	
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