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
#include <TH2.h>

namespace EventBuilder {

	class SlowSort
	{
	public:
		SlowSort();
		SlowSort(double windowSize);
		~SlowSort();
		inline void SetWindowSize(double window) { coincWindow = window; }
		bool AddHitToEvent(CompassHit& mhit);
		std::vector<DPPChannel> GetEvent();
		inline TH2F* GetEventStats() { return event_stats; }
		void FlushHitsToEvent(); //For use with *last* hit list
		inline bool IsEventReady() { return eventFlag; }
	
	private:
		void InitVariableMaps();
		void Reset();
		void StartEvent();
		void ProcessEvent();
	
		double coincWindow;
		std::vector<DPPChannel> hitList;
		std::vector<DPPChannel> event;
		bool eventFlag;
		DPPChannel hit;
	
		double startTime, previousHitTime;
	
		TH2F* event_stats;
	
	};

}

#endif
