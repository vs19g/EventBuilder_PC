/*
	EVBApp.cpp
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#include "EVBApp.h"
#include "CompassRun.h"
#include "SlowSort.h"

namespace EventBuilder {
	
	EVBApp::EVBApp() :
		m_workspace(nullptr), m_progressFraction(0.1)
	{
		if(EnforceDictonaryLinked()) //Avoid symbol stripping
		{
			EVB_INFO("Dictionary loaded"); 
		}
		SetProgressCallbackFunc(BIND_PROGRESS_CALLBACK_FUNCTION(EVBApp::DefaultProgressCallback));
	}
	
	EVBApp::~EVBApp() 
	{
	}
	
	void EVBApp::DefaultProgressCallback(int64_t curVal, int64_t totalVal)
	{
		double fraction = ((double)curVal)/totalVal;
		EVB_INFO("Percent of run built: {0}", fraction*100);
	}

	void EVBApp::SetParameters(const EVBParameters& params)
	{
		if(m_params.workspaceDir != params.workspaceDir)
		{
			m_workspace.reset(new EVBWorkspace(params.workspaceDir));
			if(!m_workspace->IsValid())
				EVB_ERROR("Unable to process new parameters due to bad workspace");
		}

		m_params = params;
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
		input>>junk>>m_params.workspaceDir;
		input>>junk;
		std::getline(input, junk);
		std::getline(input, junk);
		input>>junk>>m_params.scalerFile;
		input>>junk;
		std::getline(input, junk);
		std::getline(input, junk);
		input>>junk>>m_params.timeShiftFile;
		input>>junk>>m_params.slowCoincidenceWindow;
		input>>junk;
		std::getline(input, junk);
		std::getline(input, junk);
		input>>junk>>m_params.runMin;
		input>>junk>>m_params.runMax;
		input>>junk>>m_params.bufferSize;
	
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
		output<<"WorkspaceDirectory: "<<m_params.workspaceDir<<std::endl;
		output<<"-------------------------------"<<std::endl;
		output<<"------Experimental Inputs------"<<std::endl;
		output<<"ScalerFile: "<<m_params.scalerFile<<std::endl;
		output<<"-------------------------------"<<std::endl;
		output<<"-------Timing Information------"<<std::endl;
		output<<"BoardOffsetFile: "<<m_params.timeShiftFile<<std::endl;
		output<<"SlowCoincidenceWindow(ps): "<<m_params.slowCoincidenceWindow<<std::endl;
		output<<"-------------------------------"<<std::endl;
		output<<"--------Run Information--------"<<std::endl;
		output<<"MinRun: "<<m_params.runMin<<std::endl;
		output<<"MaxRun: "<<m_params.runMax<<std::endl;
		output<<"BufferSize: "<<m_params.bufferSize<<std::endl;
		output<<"-------------------------------"<<std::endl;
	
		output.close();
	
		EVB_INFO("Successfully wrote config to file.");
	
	}
	
	void EVBApp::Convert2RawRoot() 
	{
		if(m_workspace == nullptr || !m_workspace->IsValid())
		{
			EVB_ERROR("Unable to perform operation due to bad workspace.");
			return;
		}

		std::string rawroot_dir = m_workspace->GetSortedDir();
		EVB_INFO("Converting binary archives to ROOT files over run range [{0}, {1}]", m_params.runMin, m_params.runMax);
	
		std::string rawfile;
	
		CompassRun converter(m_params, m_workspace);
		converter.SetProgressCallbackFunc(m_progressCallback);
		converter.SetProgressFraction(m_progressFraction);

		int count = 0;
	
		EVB_INFO("Beginning conversion...");
		for(int i=m_params.runMin; i<=m_params.runMax; i++) 
		{
			rawfile = rawroot_dir + "compass_run_"+ std::to_string(i) + ".root";
			EVB_INFO("Converting file {0}...", rawfile);
			m_workspace->ClearTempDirectory();
			if(m_workspace->UnpackBinaryRunToTemp(i))
			{
				converter.SetRunNumber(i);
				converter.Convert2RawRoot(rawfile);
				++count;
			}	
			m_workspace->ClearTempDirectory();
		}
		if(count != 0)
			EVB_INFO("Conversion complete.");
		else
			EVB_WARN("Nothing converted, no files found in the range [{0}, {1}]", m_params.runMin, m_params.runMax);
	}
	
	void EVBApp::MergeROOTFiles() 
	{
		if(m_workspace == nullptr || !m_workspace->IsValid())
		{
			EVB_ERROR("Unable to perform operation due to bad workspace.");
			return;
		}

		std::string merge_file = m_workspace->GetMergedDir() + "run_" + std::to_string(m_params.runMin)+"_"+std::to_string(m_params.runMax)+".root";
		EVB_INFO("Merging ROOT files into single file for runs in range [{0}, {1}]", m_params.runMin, m_params.runMax);
		EVB_INFO("Merged file will be named {0}", merge_file);
		EVB_INFO("Starting merge...");
		if(!m_workspace->MergeBuiltFiles(merge_file, m_params.runMin, m_params.runMax)) 
		{
			EVB_ERROR("Unable to find files for merge at EVBApp::MergeROOTFiles()!");
			return;
		}
		EVB_INFO("Finished.");
	}
	
	void EVBApp::Convert2SortedRoot() 
	{
		if(m_workspace == nullptr || !m_workspace->IsValid())
		{
			EVB_ERROR("Unable to perform operation due to bad workspace.");
			return;
		}

		std::string sortroot_dir = m_workspace->GetBuiltDir();
		EVB_INFO("Converting binary archives to event built ROOT files over run range [{0}, {1}]", m_params.runMin, m_params.runMax);
	
		std::string sortfile;
	
		CompassRun converter(m_params, m_workspace);
		converter.SetProgressCallbackFunc(m_progressCallback);
		converter.SetProgressFraction(m_progressFraction);
	
		int result;
		EVB_INFO("Beginning conversion...");
	
		int count=0;
		for(int i=m_params.runMin; i<= m_params.runMax; i++) 
		{
	
			sortfile = sortroot_dir +"run_"+std::to_string(i)+ ".root";
			EVB_INFO("Converting file {0}...", sortfile);

			m_workspace->ClearTempDirectory();
			if(m_workspace->UnpackBinaryRunToTemp(i))
			{
				converter.SetRunNumber(i);
				converter.Convert2SortedRoot(sortfile);
				count++;
			}	
			m_workspace->ClearTempDirectory();
		}
		if(count==0)
			EVB_WARN("Conversion failed, no archives were found!");
		else
			EVB_INFO("Conversion complete.");
	}
	
}
