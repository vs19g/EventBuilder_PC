/*
	EVBApp.h
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#ifndef EVBAPP_H
#define EVBAPP_H

#include "EVBParameters.h"
#include "EVBWorkspace.h"
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
	
		void SetParameters(const EVBParameters& params);
		inline const EVBParameters& GetParameters() { return m_params; }

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
		double m_progressFraction;
		EVBParameters m_params;
		std::shared_ptr<EVBWorkspace> m_workspace;
	
		ProgressCallbackFunc m_progressCallback;
	
	};

}
#endif
