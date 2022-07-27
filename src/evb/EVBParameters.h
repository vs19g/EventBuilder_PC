#ifndef EVB_PARAMETERS_H
#define EVB_PARAMETERS_H

namespace EventBuilder {

	struct EVBParameters
	{
		std::string workspaceDir = "";
		std::string channelMapFile = "";
		std::string timeShiftFile = "";
		std::string scalerFile = "";

		int runMin = 0;
		int runMax = 0;

		double slowCoincidenceWindow = 3.0e6;
        	size_t bufferSize = 2000;
	};
}

#endif
