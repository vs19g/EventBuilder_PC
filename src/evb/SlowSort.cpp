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
	
	/*Constructor takes input of coincidence window size, and fills sabre channel map*/
	SlowSort::SlowSort() :
	  coincWindow(-1.0), eventFlag(false)
	{
	  event_stats = new TH2F("coinc_event_stats","coinc_events_stats;global channel;number of coincident hits;counts",144,0,144,20,0,20);
	}
	
	SlowSort::SlowSort(double windowSize) :
	  coincWindow(windowSize), eventFlag(false)
	{
	  event_stats = new TH2F("coinc_event_stats","coinc_events_stats;global channel;number of coincident hits;counts",144,0,144,20,0,20);
	}
	
	SlowSort::~SlowSort() {}
	
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
		else if (curHit.Timestamp < previousHitTime)
			return false;
		else if ((curHit.Timestamp - startTime) < coincWindow)
			hitList.push_back(curHit);
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
	
	std::vector<DPPChannel> SlowSort::GetEvent()
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
		event = hitList;
	}

}