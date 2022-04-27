#ifndef FLAGHANDLER_H
#define FLAGHANDLER_H

#include <map>

namespace EventBuilder {

	struct FlagCount
	{
		int64_t total_counts=0;
		int64_t dead_time=0;
		int64_t time_roll=0;
		int64_t time_reset=0;
		int64_t fake_event=0;
		int64_t mem_full=0;
		int64_t trig_lost=0;
		int64_t n_trig_lost=0;
		int64_t sat_in_gate=0;
		int64_t trig_1024=0;
		int64_t sat_input=0;
		int64_t n_trig_count=0;
		int64_t event_not_matched=0;
		int64_t fine_time=0;
		int64_t pile_up=0;
		int64_t pll_lock_loss=0;
		int64_t over_temp=0;
		int64_t adc_shutdown=0;
	};
	
	class FlagHandler
	{
	public:
		FlagHandler();
		FlagHandler(const std::string& filename);
		~FlagHandler();
		void CheckFlag(int board, int channel, int flag);
	
		const int DEAD_TIME = 0x00000001;
		const int TIME_ROLLOVER = 0x00000002;
		const int TIME_RESET = 0x00000004;
		const int FAKE_EVENT = 0x00000008;
		const int MEM_FULL = 0x00000010;
		const int TRIG_LOST = 0x00000020;
		const int N_TRIG_LOST = 0x00000040;
		const int SATURATING_IN_GATE = 0x00000080;
		const int TRIG_1024_COUNTED = 0x00000100;
		const int SATURATING_INPUT = 0x00000400;
		const int N_TRIG_COUNTED = 0x00000800;
		const int EVENT_NOT_MATCHED = 0x00001000;
		const int FINE_TIME  = 0x00004000;
		const int PILE_UP = 0x00008000;
		const int PLL_LOCK_LOSS = 0x00080000;
		const int OVER_TEMP = 0x00100000;
		const int ADC_SHUTDOWN = 0x00200000;
	
	private:
		std::ofstream log;
		std::map<int, FlagCount> event_count_map;
	
		void WriteLog();
	};

}
#endif
