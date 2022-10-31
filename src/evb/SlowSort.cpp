/*SlowSort.cpp
 *Class designed to first time-order raw data, and then based on a given coincidence window
 *sort the raw data into coincidence structures. Utilizes dictionary elements DPPChannel and
 *CoincEvent. Based on work by S. Balak, K. Macon, and E. Good from LSU. 
 *
 *Gordon M. Oct. 2019
 *
 *Refurbished and updated Jan 2020 GWM
 */
#include "SlowSort.h"

namespace EventBuilder {

	static bool SortEnergy(const DetectorHit& i, const DetectorHit& j)
	{
		return i.energy > j.energy;
	}
	
	/*Constructor takes input of coincidence window size, and fills sabre channel map*/
	SlowSort::SlowSort(double windowSize, const std::string& filename) :
	  coincWindow(windowSize), eventFlag(false), m_chanMap(filename)
	{
	  event_stats = new TH2F("coinc_event_stats","coinc_events_stats;global channel;number of coincident hits;counts",144,0,144,20,0,20);
	  InitVariableMaps();
	}
	
	SlowSort::~SlowSort() {}

	void SlowSort::InitVariableMaps()
	{
		//QQQ memory
		m_varMap[DetType::FQQQ0Ring] = &event.fqqq[0].rings;
		m_varMap[DetType::FQQQ0Wedge] = &event.fqqq[0].wedges;
		m_varMap[DetType::FQQQ1Ring] = &event.fqqq[1].rings;
		m_varMap[DetType::FQQQ1Wedge] = &event.fqqq[1].wedges;
		m_varMap[DetType::FQQQ2Ring] = &event.fqqq[2].rings;
		m_varMap[DetType::FQQQ2Wedge] = &event.fqqq[2].wedges;
		m_varMap[DetType::FQQQ3Ring] = &event.fqqq[3].rings;
		m_varMap[DetType::FQQQ3Wedge] = &event.fqqq[3].wedges;
		
		//Barrel memory
		m_varMap[DetType::Barrel0FrontUp] = &event.barrel[0].frontsUp;
		m_varMap[DetType::Barrel0FrontDn] = &event.barrel[0].frontsDown;
		m_varMap[DetType::Barrel0Back] = &event.barrel[0].backs;
		m_varMap[DetType::Barrel1FrontUp] = &event.barrel[1].frontsUp;
		m_varMap[DetType::Barrel1FrontDn] = &event.barrel[1].frontsDown;
		m_varMap[DetType::Barrel1Back] = &event.barrel[1].backs;
		m_varMap[DetType::Barrel2FrontUp] = &event.barrel[2].frontsUp;
		m_varMap[DetType::Barrel2FrontDn] = &event.barrel[2].frontsDown;
		m_varMap[DetType::Barrel2Back] = &event.barrel[2].backs;
		m_varMap[DetType::Barrel3FrontUp] = &event.barrel[3].frontsUp;
		m_varMap[DetType::Barrel3FrontDn] = &event.barrel[3].frontsDown;
		m_varMap[DetType::Barrel3Back] = &event.barrel[3].backs;
		m_varMap[DetType::Barrel4FrontUp] = &event.barrel[4].frontsUp;
		m_varMap[DetType::Barrel4FrontDn] = &event.barrel[4].frontsDown;
		m_varMap[DetType::Barrel4Back] = &event.barrel[4].backs;
		m_varMap[DetType::Barrel5FrontUp] = &event.barrel[5].frontsUp;
		m_varMap[DetType::Barrel5FrontDn] = &event.barrel[5].frontsDown;
		m_varMap[DetType::Barrel5Back] = &event.barrel[5].backs;
		m_varMap[DetType::Barrel6FrontUp] = &event.barrel[6].frontsUp;
		m_varMap[DetType::Barrel6FrontDn] = &event.barrel[6].frontsDown;
		m_varMap[DetType::Barrel6Back] = &event.barrel[6].backs;		
		m_varMap[DetType::Barrel7FrontUp] = &event.barrel[7].frontsUp;
		m_varMap[DetType::Barrel7FrontDn] = &event.barrel[7].frontsDown;
		m_varMap[DetType::Barrel7Back] = &event.barrel[7].backs;		
		m_varMap[DetType::Barrel8FrontUp] = &event.barrel[8].frontsUp;
		m_varMap[DetType::Barrel8FrontDn] = &event.barrel[8].frontsDown;
		m_varMap[DetType::Barrel8Back] = &event.barrel[8].backs;		
		m_varMap[DetType::Barrel9FrontUp] = &event.barrel[9].frontsUp;
		m_varMap[DetType::Barrel9FrontDn] = &event.barrel[9].frontsDown;
		m_varMap[DetType::Barrel9Back] = &event.barrel[9].backs;		
		m_varMap[DetType::Barrel10FrontUp] = &event.barrel[10].frontsUp;
		m_varMap[DetType::Barrel10FrontDn] = &event.barrel[10].frontsDown;
		m_varMap[DetType::Barrel10Back] = &event.barrel[10].backs;		
		m_varMap[DetType::Barrel11FrontUp] = &event.barrel[11].frontsUp;
		m_varMap[DetType::Barrel11FrontDn] = &event.barrel[11].frontsDown;
		m_varMap[DetType::Barrel11Back] = &event.barrel[11].backs;		

		//Barcelona memory
		m_varMap[DetType::BarcUpstream0Front] = &event.barcUp[0].fronts;
		m_varMap[DetType::BarcUpstream0Back] = &event.barcUp[0].backs;
		m_varMap[DetType::BarcUpstream1Front] = &event.barcUp[1].fronts;
		m_varMap[DetType::BarcUpstream1Back] = &event.barcUp[1].backs;
		m_varMap[DetType::BarcUpstream2Front] = &event.barcUp[2].fronts;
		m_varMap[DetType::BarcUpstream2Back] = &event.barcUp[2].backs;
		m_varMap[DetType::BarcUpstream3Front] = &event.barcUp[3].fronts;
		m_varMap[DetType::BarcUpstream3Back] = &event.barcUp[3].backs;
		m_varMap[DetType::BarcUpstream4Front] = &event.barcUp[4].fronts;
		m_varMap[DetType::BarcUpstream4Back] = &event.barcUp[4].backs;
		m_varMap[DetType::BarcUpstream5Front] = &event.barcUp[5].fronts;
		m_varMap[DetType::BarcUpstream5Back] = &event.barcUp[5].backs;
		m_varMap[DetType::BarcDownstream0Front] = &event.barcDown[0].fronts;
		m_varMap[DetType::BarcDownstream0Back] = &event.barcDown[0].backs;
		m_varMap[DetType::BarcDownstream1Front] = &event.barcDown[1].fronts;
		m_varMap[DetType::BarcDownstream1Back] = &event.barcDown[1].backs;
		m_varMap[DetType::BarcDownstream2Front] = &event.barcDown[2].fronts;
		m_varMap[DetType::BarcDownstream2Back] = &event.barcDown[2].backs;
		m_varMap[DetType::BarcDownstream3Front] = &event.barcDown[3].fronts;
		m_varMap[DetType::BarcDownstream3Back] = &event.barcDown[3].backs;
		m_varMap[DetType::BarcDownstream4Front] = &event.barcDown[4].fronts;
		m_varMap[DetType::BarcDownstream4Back] = &event.barcDown[4].backs;
		m_varMap[DetType::BarcDownstream5Front] = &event.barcDown[5].fronts;
		m_varMap[DetType::BarcDownstream5Back] = &event.barcDown[5].backs;
	}
	
	bool SlowSort::AddHitToEvent(CompassHit& mhit)
	{
		DPPChannel curHit;
		curHit.Timestamp = mhit.timestamp/1.0e3; //convert to ns for easier drawing
		curHit.Energy = mhit.energy;
		curHit.EnergyShort = mhit.energyShort;
		curHit.EnergyCal = mhit.energyCalibrated;
		curHit.Channel = mhit.channel;
		curHit.Board = mhit.board;
		curHit.Flags = mhit.flags;
		curHit.Samples = mhit.samples;
	
		if(hitList.empty())
		{
			startTime = curHit.Timestamp;
			hitList.push_back(curHit);
		}
		else if ((curHit.Timestamp - startTime) < coincWindow)
		{
			hitList.push_back(curHit);
		}
		else
		{
			ProcessEvent();
			hitList.clear();
			startTime = curHit.Timestamp;
			hitList.push_back(curHit);
			eventFlag = true;
		}
	
		return true;
	}
	
	void SlowSort::FlushHitsToEvent()
	{
		if(hitList.empty())
		{
		  eventFlag = false;
		  return;
		}
	
		ProcessEvent();
		hitList.clear();
		eventFlag = true;
	}
	
	CoincEvent SlowSort::GetEvent()
	{
		eventFlag = false;
		return event;
	}
	
	/*Function called when a start of a coincidence event is detected*/
	void SlowSort::StartEvent()
	{
		if(hitList.size() != 0)
		{
			EVB_WARN("Attempting to initalize hitList when not cleared!! Check processing order.");
		}
		startTime = hit.Timestamp;
		hitList.push_back(hit);
	}
	
	/*Function called when an event outside the coincidence window is detected
	 *Process all of the hits in the list, and write them to the sorted tree
	 */
	void SlowSort::ProcessEvent()
	{
		event = blank;
		DetectorHit dhit;
		for(auto& curHit : hitList)
		{
			dhit.globalChannel = curHit.Channel + curHit.Board * 64; //global channel = board*64channels/perboard + channel_in_board 
			dhit.timestamp = curHit.Timestamp;
			dhit.energy = curHit.Energy;

			auto channel_info = m_chanMap.FindChannel(dhit.globalChannel);
			if(channel_info == m_chanMap.End())
			{
				EVB_WARN("At SlowSort::ProcessEvent() -- Data Assignment Error! Global channel {0} not assigned in ChannelMap!", dhit.globalChannel);
				continue;
			}
			else
			{
				auto iter = m_varMap.find(channel_info->second.type);
				if(iter == m_varMap.end())
				{
					EVB_WARN("At SlowSort::ProcessEvent() -- Data Assignment Error! Global channel {0} does not have bound memory in VarMap!",
						  dhit.globalChannel);
				}
				else
					iter->second->push_back(dhit);
			}
		}

		//first hit (for each detector type) is largest energy
		for(int i=0; i<4; i++)
		{
			std::sort(event.fqqq[i].rings.begin(), event.fqqq[i].rings.end(), SortEnergy);
			std::sort(event.fqqq[i].wedges.begin(), event.fqqq[i].wedges.end(), SortEnergy);
		}

		for(int i=0; i<12; i++)
		{
			std::sort(event.barrel[i].frontsUp.begin(), event.barrel[i].frontsUp.end(), SortEnergy);
			std::sort(event.barrel[i].frontsDown.begin(), event.barrel[i].frontsDown.end(), SortEnergy);
			std::sort(event.barrel[i].backs.begin(), event.barrel[i].backs.end(), SortEnergy);
		}

		for(int i=0; i<6; i++)
		{
			std::sort(event.barcUp[i].fronts.begin(), event.barcUp[i].fronts.end(), SortEnergy);
			std::sort(event.barcDown[i].fronts.begin(), event.barcDown[i].fronts.end(), SortEnergy);
		}
	}

}
