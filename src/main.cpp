#include "EventBuilder.h"
#include "EVBApp.h"
#include "Stopwatch.h"

int main(int argc, char** argv) {

	EventBuilder::Logger::Init();
	if(argc != 3)
	{
		EVB_ERROR("Incorrect number of command line arguments!");
		EVB_ERROR("Need to specify type of operation (buildSlow, buildFast, etc.) and input file.");
		return 1;
	}

	std::string filename = argv[2];
	std::string operation = argv[1];


	/* DEFAULT Operation Types:
		convert (convert binary archive to root data)
		convertSlow (convert binary arhcive to event slow data)
		merge (combine root files)
	*/

	EventBuilder::EVBApp theBuilder;
	if(!theBuilder.ReadConfigFile(filename))
	{
		return 1;
	}
	EventBuilder::Stopwatch timer;
	timer.Start();
	if(operation == "convert")
	{
		theBuilder.Convert2RawRoot();
	}
	else if(operation == "merge")
	{
		theBuilder.MergeROOTFiles();
	}
	else if (operation == "convertSlow")
	{
		theBuilder.Convert2SortedRoot();
	}
	else
	{
		EVB_ERROR("Unidentified type of operation! Check your first argument.");
		return 1;
	}
	timer.Stop();
	EVB_INFO("Elapsed time (ms): {0}", timer.GetElapsedMilliseconds());

	return 0;
}