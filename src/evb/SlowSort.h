/*SlowSort.h
 *Class designed to first time-order raw data, and then based on a given coincidence window
 *sort the raw data into coincidence structures. Utilizes dictionary elements DPPChannel and
 *CoincEvent. Based on work by S. Balak, K. Macon, and E. Good from LSU. 
 *
 *Gordon M. Oct. 2019
 *
 *Refurbished and updated Jan 2020 GWM
 */
#ifndef SLOW_SORT_H
#define SLOW_SORT_H

#include "CompassHit.h"
#include "evbdict/DataStructs.h"
#include "ChannelMap.h"
#include <TH2.h>

namespace EventBuilder {

	class SlowSort
	{
	public:
		SlowSort(double windowSize, const std::string& filename);
		~SlowSort();
		inline void SetWindowSize(double window) { coincWindow = window; }
		bool AddHitToEvent(CompassHit& mhit);
		CoincEvent GetEvent();
		inline TH2F* GetEventStats() { return event_stats; }
		void FlushHitsToEvent(); //For use with *last* hit list
		inline bool IsEventReady() { return eventFlag; }
	
	private:
		void InitVariableMaps();
		void StartEvent();
		void ProcessEvent();
	
		double coincWindow;
		std::vector<DPPChannel> hitList;
		CoincEvent event, blank;
		bool eventFlag;
		DPPChannel hit;
	
		ChannelMap m_chanMap;
		std::unordered_map<DetType, std::vector<DetectorHit>*> m_varMap;

		double startTime, previousHitTime;
	
		TH2F* event_stats;
	
	};

}

#endif
