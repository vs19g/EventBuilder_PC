/*
	EVBApp.h
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#ifndef EVBAPP_H
#define EVBAPP_H

#include "RunCollector.h"
#include "ProgressCallback.h"

namespace EventBuilder {
	
	class EVBApp
	{
	public:

		EVBApp();
		~EVBApp();
	
		bool ReadConfigFile(const std::string& filename);
		void WriteConfigFile(const std::string& filename);
	
		void MergeROOTFiles();
		void Convert2SortedRoot();
		void Convert2RawRoot();
	
		void SetRunRange(int rmin, int rmax);
		void SetWorkDirectory(const std::string& dir);
		void SetChannelMap(const std::string& name);
		void SetBoardShiftFile(const std::string& name);
		void SetSlowCoincidenceWindow(double window);
		void SetScalerFile(const std::string& fullpath);
		void SetBufferSize(uint64_t bufsize);
	
		inline int GetRunMin() const { return m_rmin; }
		inline int GetRunMax() const { return m_rmax; }
		inline std::string GetWorkDirectory() const { return m_workspace; }
		inline double GetSlowCoincidenceWindow() const { return m_SlowWindow; }
		inline std::string GetBoardShiftFile() const { return m_shiftfile; }
		inline std::string GetScalerFile() const { return m_scalerfile; }

		void DefaultProgressCallback(int64_t curVal, int64_t totalVal);
		inline void SetProgressCallbackFunc(const ProgressCallbackFunc& function) { m_progressCallback = function; }
		inline void SetProgressFraction(double frac) { m_progressFraction = frac; }
	
		enum Operation 
		{
			Convert,
			ConvertSlow,
			Merge
		};
	
	private:
	
		int m_rmin, m_rmax;
		double m_progressFraction;
	
		std::string m_workspace;
		std::string m_shiftfile;
		std::string m_scalerfile;
	
		uint64_t m_buffersize;
		double m_SlowWindow;
	
		RunCollector grabber;

		ProgressCallbackFunc m_progressCallback;
	
	};

}
#endif
