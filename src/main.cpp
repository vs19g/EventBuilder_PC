#include "EventBuilder.h"
#include "GWMEventBuilder.h"
#include "Stopwatch.h"

int main(int argc, char** argv) {
	if(argc != 3) {
		std::cerr<<"Incorrect number of command line arguments!"<<std::endl;
		std::cerr<<"Need to specify type of operation (buildSlow, buildFast, etc.) and input file."<<std::endl;
		return 1;
	}

	std::string filename = argv[2];
	std::string operation = argv[1];


	/* DEFAULT Operation Types:
		convert (convert binary archive to root data)
		convertSlow (convert binary arhcive to event slow data)
		merge (combine root files)
	*/

	GWMEventBuilder theBuilder;
	if(!theBuilder.ReadConfigFile(filename)) {
		return 1;
	}
	Stopwatch timer;
	timer.Start();
	if(operation == "convert") {
		theBuilder.SetAnalysisType(GWMEventBuilder::CONVERT);
		theBuilder.Convert2RawRoot();
	} else if(operation == "merge") {
		theBuilder.SetAnalysisType(GWMEventBuilder::MERGE);
		theBuilder.MergeROOTFiles();
	} else if (operation == "convertSlow"){
		theBuilder.SetAnalysisType(GWMEventBuilder::CONVERT_S);
		theBuilder.Convert2SortedRoot();
	} else {
		std::cerr<<"Unidentified type of operation! Check your first argument."<<std::endl;
		return 1;
	}
	timer.Stop();
	std::cout<<"Elapsed time (ms): "<<timer.GetElapsedMilliseconds()<<std::endl;

	return 0;
}