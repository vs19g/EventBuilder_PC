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

#include "EVBParameters.h"
#include "EVBWorkspace.h"
#include "CompassFile.h"
#include "DataStructs.h"
#include "ShiftMap.h"
#include "ProgressCallback.h"
#include <TParameter.h>

namespace EventBuilder {

	class CompassRun
	{
	public:
		CompassRun(const EVBParameters& params, const std::shared_ptr<EVBWorkspace>& workspace);
		~CompassRun();
		inline void SetRunNumber(int n) { m_runNum = n; };
		void Convert2RawRoot(const std::string& name);
		void Convert2SortedRoot(const std::string& name);

		inline void SetProgressCallbackFunc(ProgressCallbackFunc& func) { m_progressCallback = func; }
		inline void SetProgressFraction(double frac) { m_progressFraction = frac; }
	
	private:
		bool GetBinaryFiles();
		bool GetHitsFromFiles();
		void SetScalers();
		void ReadScalerData(const std::string& filename);
	
		EVBParameters m_params;
		std::shared_ptr<EVBWorkspace> m_workspace;
		std::vector<CompassFile> m_datafiles;
		unsigned int m_startIndex; //this is the file we start looking at; increases as we finish files.
		ShiftMap m_smap;
		std::unordered_map<std::string, TParameter<int64_t>> m_scaler_map; //maps scaler files to the TParameter to be saved
	
		//Potential branch variables
		CompassHit m_hit;
	
		//what run is this
		int m_runNum;
		uint64_t m_totalHits;
	
		//Scaler switch
		bool m_scaler_flag;

		ProgressCallbackFunc m_progressCallback;
		double m_progressFraction;
	
	};

}
#endif
