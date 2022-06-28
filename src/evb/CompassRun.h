/*
	CompassRun.h
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020
*/
#ifndef COMPASSRUN_H
#define COMPASSRUN_H

#include "CompassFile.h"
#include "DataStructs.h"
#include "RunCollector.h"
#include "ShiftMap.h"
#include "ProgressCallback.h"
#include <TParameter.h>

namespace EventBuilder {

	class CompassRun
	{
	public:
		CompassRun();
		CompassRun(const std::string& dir, size_t bsize);
		~CompassRun();
		inline void SetDirectory(const std::string& dir) { directory = dir; }
		inline void SetScalerInput(const std::string& filename) { m_scalerinput = filename; }
		inline void SetRunNumber(int n) { runNum = n; };
		inline void SetShiftMap(const std::string& filename) { m_smap.SetFile(filename); }
		void Convert2RawRoot(const std::string& name);
		void Convert2SortedRoot(const std::string& name, double window);

		inline void SetProgressCallbackFunc(ProgressCallbackFunc& func) { m_progressCallback = func; }
		inline void SetProgressFraction(double frac) { m_progressFraction = frac; }
	
	private:
		bool GetBinaryFiles();
		bool GetHitsFromFiles();
		void SetScalers();
		void ReadScalerData(const std::string& filename);
	
		std::string directory, m_scalerinput;
		std::vector<CompassFile> m_datafiles;
		unsigned int startIndex; //this is the file we start looking at; increases as we finish files.
		ShiftMap m_smap;
		std::unordered_map<std::string, TParameter<int64_t>> m_scaler_map; //maps scaler files to the TParameter to be saved
	
		//Potential branch variables
		CompassHit hit;
		std::vector<DPPChannel> event;
	
		//what run is this
		int runNum;
		uint64_t m_totalHits;
		uint64_t m_buffersize;
	
		//Scaler switch
		bool m_scaler_flag;

		ProgressCallbackFunc m_progressCallback;
		double m_progressFraction;
	
	};

}
#endif
