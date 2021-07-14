/*
	GWMEventBuilder.h
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#ifndef GWMEVENTBUILDER_H
#define GWMEVENTBUILDER_H

#include "RunCollector.h"
#include <TGProgressBar.h>

class GWMEventBuilder {
public:
	GWMEventBuilder();
	~GWMEventBuilder();

	bool ReadConfigFile(const std::string& filename);
	void WriteConfigFile(const std::string& filename);

	void MergeROOTFiles();
	void Convert2SortedRoot();
	void Convert2RawRoot();

	inline void SetAnalysisType(int type) { m_analysisType = type;};
	inline int GetAnalysisType() { return m_analysisType; };

	inline void SetRunRange(int rmin, int rmax) { m_rmin = rmin; m_rmax = rmax; };
	inline void SetWorkDirectory(const std::string& fullpath) { m_workspace = fullpath; };
	inline void SetBoardShiftFile(const std::string& name) { m_shiftfile = name; };
	inline void SetScalerFile(const std::string& name) { m_scalerfile = name; };
	inline void SetSlowCoincidenceWindow(double window) { m_SlowWindow = window; };
	inline void SetBufferSize(unsigned int size) { m_buffersize = size; };

	inline int GetRunMin() {return m_rmin;};
	inline int GetRunMax() {return m_rmax;};
	inline std::string GetWorkDirectory() {return m_workspace;};
	inline double GetSlowCoincidenceWindow() { return m_SlowWindow; };
	inline std::string GetBoardShiftFile() { return m_shiftfile; };
	inline std::string GetScalerFile() { return m_scalerfile; };
	inline unsigned int GetBufferSize() { return m_buffersize; };

	inline void AttachProgressBar(TGProgressBar* pb) { m_pb = pb; };

	enum BuildType {
		CONVERT,
		CONVERT_S,
		MERGE
	};

private:

	int m_rmin, m_rmax;

	std::string m_workspace;
	std::string m_shiftfile;
	std::string m_scalerfile;

	double m_SlowWindow;

	int m_analysisType;
	unsigned int m_buffersize;

	RunCollector grabber;

	TGProgressBar* m_pb;

};

#endif
