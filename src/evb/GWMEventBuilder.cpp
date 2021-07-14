/*
	GWMEventBuilder.cpp
	Class which represents the API of the event building environment. Wraps together the core concepts
	of the event builder, from conversion to plotting. Even intended to be able to archive data.
	Currently under development.

	Written by G.W. McCann Oct. 2020
*/
#include "EventBuilder.h"
#include <cstdlib>
#include "GWMEventBuilder.h"
#include "RunCollector.h"
#include "CompassRun.h"

GWMEventBuilder::GWMEventBuilder() :
	m_rmin(0), m_rmax(0), m_workspace("none"), m_shiftfile("none"), m_SlowWindow(0), m_buffersize(200000), m_pb(nullptr)
{
}

GWMEventBuilder::~GWMEventBuilder() 
{
}

bool GWMEventBuilder::ReadConfigFile(const std::string& fullpath) {
	std::cout<<"Reading in configuration from file: "<<fullpath<<std::endl;
	std::ifstream input(fullpath);
	if(!input.is_open()) {
		std::cout<<"Read failed! Unable to open input file!"<<std::endl;
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

	std::cout<<"Completed."<<std::endl;

	return true;
}

void GWMEventBuilder::WriteConfigFile(const std::string& fullpath) {

	std::cout<<"Writing out configuration to file: "<<fullpath<<std::endl;
	std::ofstream output(fullpath);
	if(!output.is_open()) {
		std::cout<<"Write failed! Unable to open output file!"<<std::endl;
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

	std::cout<<"Completed."<<std::endl;

}

void GWMEventBuilder::Convert2RawRoot() {
	std::string rawroot_dir = m_workspace+"/raw_root/";
	std::string unpack_dir = m_workspace+"/temp_binary/";
	std::string binary_dir = m_workspace+"/raw_binary/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Converting Binary file Archive to ROOT file"<<std::endl;
	std::cout<<"Binary Archive Directory: "<<binary_dir<<std::endl;
	std::cout<<"Temporary Unpack Directory: "<<unpack_dir<<std::endl;
	std::cout<<"Timestamp Shift File: "<<m_shiftfile<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	std::cout<<"Buffer Size in hits: "<<m_buffersize<<std::endl;
	

	grabber.SetSearchParams(binary_dir, "", ".tar.gz",0,1000);

	std::cout<<"Workspace Directory: "<<m_workspace<<std::endl;

	std::string rawfile, binfile;
	std::string unpack_command, wipe_command;

	CompassRun converter(unpack_dir, m_buffersize);
	converter.SetShiftMap(m_shiftfile);
	converter.SetScalerInput(m_scalerfile);
	if(m_pb) converter.AttachProgressBar(m_pb);

	std::cout<<"Beginning conversion..."<<std::endl;

	for(int i=m_rmin; i<=m_rmax; i++) {
		binfile = grabber.GrabFile(i);
		if(binfile == "") continue;
		converter.SetRunNumber(i);
		std::cout<<"Converting file: "<<binfile<<std::endl;

		rawfile = rawroot_dir + "compass_run_"+ to_string(i) + ".root";
		unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
		wipe_command = "rm -r "+unpack_dir+"*.bin";

		system(unpack_command.c_str());
		converter.Convert2RawRoot(rawfile);
		system(wipe_command.c_str());

	}
	std::cout<<std::endl<<"Conversion complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;

}

void GWMEventBuilder::MergeROOTFiles() {
	std::string merge_file = m_workspace+"/merged/run_"+to_string(m_rmin)+"_"+to_string(m_rmax)+".root";
	std::string file_dir = m_workspace+"/analyzed/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Merging ROOT files into single ROOT file"<<std::endl;
	std::cout<<"Workspace directory: "<<m_workspace<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	std::cout<<"Output file: "<<merge_file<<std::endl;
	std::string prefix = "";
	std::string suffix = ".root";
	grabber.SetSearchParams(file_dir, prefix, suffix,m_rmin,m_rmax);
	std::cout<<"Beginning the merge...";
	if(!grabber.Merge_TChain(merge_file)) {
		std::cout<<"Unable to find files at MergeROOTFiles"<<std::endl;
		return;
	}
	std::cout<<" Complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}

void GWMEventBuilder::Convert2SortedRoot() {
	std::string sortroot_dir = m_workspace+"/sorted/";
	std::string unpack_dir = m_workspace+"/temp_binary/";
	std::string binary_dir = m_workspace+"/raw_binary/";
	std::cout<<"-------------GWM Event Builder-------------"<<std::endl;
	std::cout<<"Converting Binary file Archive to ROOT file"<<std::endl;
	std::cout<<"Binary Archive Directory: "<<binary_dir<<std::endl;
	std::cout<<"Temporary Unpack Directory: "<<unpack_dir<<std::endl;
	std::cout<<"Timestamp Shift File: "<<m_shiftfile<<std::endl;
	std::cout<<"Slow Coincidence Window(ps): "<<m_SlowWindow<<std::endl;
	std::cout<<"Min Run: "<<m_rmin<<" Max Run: "<<m_rmax<<std::endl;
	std::cout<<"Buffer Size in hits: "<<m_buffersize<<std::endl;

	grabber.SetSearchParams(binary_dir,"",".tar.gz",m_rmin,m_rmax);

	std::cout<<"Workspace Directory: "<<m_workspace<<std::endl;

	std::string sortfile, binfile;
	std::string unpack_command, wipe_command;

	CompassRun converter(unpack_dir, m_buffersize);
	converter.SetShiftMap(m_shiftfile);
	converter.SetScalerInput(m_scalerfile);
	if(m_pb) converter.AttachProgressBar(m_pb);

	std::cout<<"Beginning conversion..."<<std::endl;

	for(int i=m_rmin; i<= m_rmax; i++) {
		binfile = grabber.GrabFile(i);
		if(binfile == "") continue;
		converter.SetRunNumber(i);
		std::cout<<"Converting file: "<<binfile<<std::endl;

		sortfile = sortroot_dir +"run_"+to_string(i)+ ".root";
		unpack_command = "tar -xzf "+binfile+" --directory "+unpack_dir;
		wipe_command = "rm -r "+unpack_dir+"*.bin";

		system(unpack_command.c_str());
		converter.Convert2SortedRoot(sortfile, m_SlowWindow);
		system(wipe_command.c_str());

	}
	std::cout<<std::endl<<"Conversion complete."<<std::endl;
	std::cout<<"-------------------------------------------"<<std::endl;
}
