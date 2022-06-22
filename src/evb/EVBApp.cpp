/*
	EVBApp.cpp
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#include "EventBuilder.h"
#include <cstdlib>
#include "EVBApp.h"
#include "RunCollector.h"
#include "CompassRun.h"
#include "SlowSort.h"

namespace EventBuilder {
	
	EVBApp::EVBApp() :
		m_rmin(0), m_rmax(0),
		m_progressFraction(0.1), m_workspace("none"), m_shiftfile("none"),
		m_scalerfile("none"), m_buffersize(200000), m_SlowWindow(0)
	{
		SetProgressCallbackFunc(BIND_PROGRESS_CALLBACK_FUNCTION(EVBApp::DefaultProgressCallback));
	}
	
	EVBApp::~EVBApp() 
	{
	}
	
	void EVBApp::DefaultProgressCallback(long curVal, long totalVal)
	{
		double fraction = ((double)curVal)/totalVal;
		EVB_INFO("Percent of run built: {0}", fraction*100);
	}

	bool EVBApp::ReadConfigFile(const std::string& fullpath) 
	{
		EVB_INFO("Reading in EVB configuration from file {0}...", fullpath);
		std::ifstream input(fullpath);
		if(!input.is_open()) 
		{
			EVB_WARN("Read of EVB config failed, unable to open input file!");
			return false;
		}
		std::string junk;
	
		std::getline(input, junk);
		input>>junk>>m_workspace;
		input>>junk;
		std::getline(input, junk);
		std::getline(input, junk);
		input>>junk>>m_scalerfile;
		input>>junk;
		std::getline(input, junk);
		std::getline(input, junk);
		input>>junk>>m_shiftfile;
		input>>junk>>m_SlowWindow;
		input>>junk;
		std::getline(input, junk);
		std::getline(input, junk);
		input>>junk>>m_rmin;
		input>>junk>>m_rmax;
		input>>junk>>m_buffersize;
	
		input.close();
	
		EVB_INFO("Successfully loaded EVB config.");
	
		return true;
	}
	
	void EVBApp::WriteConfigFile(const std::string& fullpath) 
	{
	
		EVB_INFO("Writing EVB config to file {0}...",fullpath);
		std::ofstream output(fullpath);
		if(!output.is_open()) 
		{
			EVB_WARN("Failed to write to config to file {0}, unable to open file!", fullpath);
			return;
		}
	
		output<<"-------Data Location----------"<<std::endl;
		output<<"WorkspaceDirectory: "<<m_workspace<<std::endl;
		output<<"-------------------------------"<<std::endl;
		output<<"------Experimental Inputs------"<<std::endl;
		output<<"ScalerFile: "<<m_scalerfile<<std::endl;
		output<<"-------------------------------"<<std::endl;
		output<<"-------Timing Information------"<<std::endl;
		output<<"BoardOffsetFile: "<<m_shiftfile<<std::endl;
		output<<"SlowCoincidenceWindow(ps): "<<m_SlowWindow<<std::endl;
		output<<"-------------------------------"<<std::endl;
		output<<"--------Run Information--------"<<std::endl;
		output<<"MinRun: "<<m_rmin<<std::endl;
		output<<"MaxRun: "<<m_rmax<<std::endl;
		output<<"BufferSize: "<<m_buffersize<<std::endl;
		output<<"-------------------------------"<<std::endl;
	
		output.close();
	
		EVB_INFO("Successfully wrote config to file.");
	
	}
	
	void EVBApp::Convert2RawRoot() 
	{
		std::string rawroot_dir = m_workspace+"/raw_root/";
		std::string unpack_dir = m_workspace+"/temp_binary/";
		std::string binary_dir = m_workspace+"/raw_binary/";
		EVB_INFO("Converting binary archives to ROOT files over run range [{0}, {1}]",m_rmin,m_rmax);
	
		grabber.SetSearchParams(binary_dir, "", ".tar.gz",0,1000);
	
		std::string rawfile, binfile;
		std::string unpack_command, wipe_command;
	
		CompassRun converter(unpack_dir, m_buffersize);
		converter.SetShiftMap(m_shiftfile);
		converter.SetScalerInput(m_scalerfile);
		converter.SetProgressCallbackFunc(m_progressCallback);
		converter.SetProgressFraction(m_progressFraction);

		int result;
	
		EVB_INFO("Beginning conversion...");
		for(int i=m_rmin; i<=m_rmax; i++) 
		{
			binfile = grabber.GrabFile(i);
			if(binfile == "") 
				continue;
			converter.SetRunNumber(i);
			EVB_INFO("Converting file {0}...", binfile);
			rawfile = rawroot_dir + "compass_run_"+ std::to_string(i) + ".root";
			unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
			wipe_command = "rm -r "+unpack_dir+"*.BIN";
	
			result = system(unpack_command.c_str());
			converter.Convert2RawRoot(rawfile);
			result = system(wipe_command.c_str());
	
		}
		EVB_INFO("Conversion complete.");
	}
	
	void EVBApp::MergeROOTFiles() 
	{
		std::string merge_file = m_workspace+"/merged/run_"+std::to_string(m_rmin)+"_"+std::to_string(m_rmax)+".root";
		std::string file_dir = m_workspace+"/analyzed/";
		EVB_INFO("Merging ROOT files into single file for runs in range [{0}, {1}]", m_rmin, m_rmax);
		EVB_INFO("Merged file will be named {0}", merge_file);
		std::string prefix = "";
		std::string suffix = ".root";
		grabber.SetSearchParams(file_dir, prefix, suffix,m_rmin,m_rmax);
		EVB_INFO("Starting merge...");
		if(!grabber.Merge_TChain(merge_file)) 
		{
			EVB_ERROR("Unable to find files for merge at EVBApp::MergeROOTFiles()!");
			return;
		}
		EVB_INFO("Finished.");
	}
	
	void EVBApp::Convert2SortedRoot() 
	{
		std::string sortroot_dir = m_workspace+"/sorted/";
		std::string unpack_dir = m_workspace+"/temp_binary/";
		std::string binary_dir = m_workspace+"/raw_binary/";
		EVB_INFO("Converting binary archives to event built ROOT files over run range [{0}, {1}]",m_rmin,m_rmax);
	
		grabber.SetSearchParams(binary_dir,"",".tar.gz",m_rmin,m_rmax);
	
		std::string sortfile, binfile;
		std::string unpack_command, wipe_command;
	
		CompassRun converter(unpack_dir, m_buffersize);
		converter.SetShiftMap(m_shiftfile);
		converter.SetScalerInput(m_scalerfile);
		converter.SetProgressCallbackFunc(m_progressCallback);
		converter.SetProgressFraction(m_progressFraction);
	
		int result;
		EVB_INFO("Beginning conversion...");
	
		int count=0;
		for(int i=m_rmin; i<= m_rmax; i++) 
		{
			binfile = grabber.GrabFile(i);
			if(binfile == "") 
				continue;
			converter.SetRunNumber(i);
			EVB_INFO("Converting file {0}...",binfile);
	
			sortfile = sortroot_dir +"run_"+std::to_string(i)+ ".root";
			unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
			wipe_command = "rm -r "+unpack_dir+"*.BIN";
	
			result = system(unpack_command.c_str());
			converter.Convert2SortedRoot(sortfile, m_SlowWindow);
			result = system(wipe_command.c_str());
			count++;
		}
		if(count==0)
			EVB_WARN("Conversion failed, no archives were found!");
		else
			EVB_INFO("Conversion complete.");
	}
	
	void EVBApp::SetRunRange(int rmin, int rmax) { EVB_TRACE("Min Run, max run set to [{0}, {1}]", rmin, rmax); m_rmin = rmin; m_rmax = rmax; }
	void EVBApp::SetWorkDirectory(const std::string& fullpath) { EVB_TRACE("Workspace set to {0}", fullpath); m_workspace = fullpath; }
	void EVBApp::SetBoardShiftFile(const std::string& name) { EVB_TRACE("Shift file set to {0}", name); m_shiftfile = name; }
	void EVBApp::SetSlowCoincidenceWindow(double window) { EVB_TRACE("Slow Coincidence Window set to {0}",window); m_SlowWindow = window; }
	void EVBApp::SetScalerFile(const std::string& fullpath) { EVB_TRACE("Scaler file set to {0}", fullpath); m_scalerfile = fullpath; }
	void EVBApp::SetBufferSize(uint64_t bufsize) { EVB_TRACE("Buffer size set to {0} hits", bufsize); m_buffersize = bufsize; }

}