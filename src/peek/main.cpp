#include "EventBuilder.h"
#include "evb/CompassFile.h"
#include "evb/Logger.h"
#include <iostream>

std::ostream& operator<<(std::ostream& stream, const EventBuilder::CompassHit& hit)
{
    stream << "------------------------------\n";
    stream << "Board: " << hit.board << "\n";
    stream << "Channel: " << hit.channel << "\n";
    stream << "Timestamp: " << hit.timestamp << "\n";
    stream << "Energy: " << hit.energy << "\n";
    stream << "EnergyShort: " << hit.energyShort << "\n";
    stream << "EnergyCalibrated: " << hit.energyCalibrated << "\n";
    stream << "------------------------------\n";
    return stream;
}

void Peek(const std::string& filename, const std::string& option, int hitNumber)
{
    EventBuilder::CompassFile file(filename);
    if(!file.IsOpen())
    {
        EVB_ERROR("Unable to open file {0}, check that it exists");
        return;
    }
    else if(file.IsEOF())
    {
        EVB_ERROR("The file {0} does not contain any valid CoMPASS data... Cannot peek.", filename);
        return;
    }

    if(option.empty())
    {
        file.GetNextHit();
        EVB_INFO("First hit of file {0}", filename);
        EVB_INFO("\n{0}", file.GetCurrentHit());
    }
    else if(option == "--count")
    {
        EVB_INFO("Number of hits in file {0}: {1}", filename, file.GetNumberOfHits());
    }
    else if(option == "--hit" && hitNumber != -1)
    {
        for(int i=0; i<=hitNumber; i++)
            file.GetNextHit();
        EVB_INFO("Hit number {0} of file {1}", hitNumber, filename);
        EVB_INFO("{0}", file.GetCurrentHit());
    }
    return;
}

int main(int argc, char** argv)
{
    EventBuilder::Logger::Init();
    std::string filename;
    std::string option;
    int number = -1;
    if(argc == 2)
    {
        option = "";
        filename = argv[1];
    }
    else if (argc == 3)
    {
        option = argv[1];
        filename = argv[2];
    }
    else if (argc == 4)
    {
        option = argv[1];
        number = std::stoi(argv[2]);
        filename = argv[3];
    }
    else
    {
        EVB_TRACE("Peek must be called with at minimum the name of a CoMPASS .BIN file");
        EVB_TRACE("Can use --count to count the number of events in the file.");
        EVB_TRACE("Can use --hit <hit_number> to specify a specific hit (by count from 0) to look at.");
        return 1;
    }

    Peek(filename, option, number);

}
