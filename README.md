# ANASEN Event Builder
This is a program designed for event building data from ANASEN with CAEN digitizers run using CoMPASS software. It reads in raw binary data and converts it into time ordered events and saves the data to a ROOT file. 

## Installation
To install, first pull the repository and all submodules using `git clone --recursive https://github.com/fsu-lsu-anasen/EventBuilder_Skeleton.git`. The repository uses the CMake build system. To install simply run the following commands (for UNIX)
```
mkdir build
cd build
cmake ..
make
```

## How To Use
1) Launch the EventBuilder from the EventBuilder_Skeleton directory
```
./bin/EventBuilderGui
```

2) At the completion of a CoMPASS run, copy the raw binary files from the CoMPASS project to the event builder workspace using the `archivist` shell script
```
./bin/archivist <runNumber>
```

3) Choose settings in the EventBuilder GUI and click "Run"

4) Generate generic plots during an experiment using the ROOT macro in the EventBuilder_Skeleton/etc directory. (Check the full paths at the top of `ANASENPlotEdit.C`.)

```
cd etc
root -l "ANASENPlotEdit.C(firstRun,lastRun)"
```

4) You can load detector geometry, perform preliminary analysis, and generate basic histograms from EventBuilder_Skeleton/etc directory. (Check the full paths at the top of `Fangorn.C`, `PruneTree.C`, and `Reconstruct.C`.)
```
cd etc
./processOneRun.sh <runNumber>
```

5) Alternatively, a range of runs can be processed (note: any missing built files will be skipped)
```
cd etc
./processAllRuns.sh <firstRun> <lastRun>
```

## Building Events
The event building operation is the bulk of the analysis process. As files are being converted to ROOT from the raw CoMPASS binary, events are built using information given by the user. In particular the code asks the user to specify a workspace (a top level directory which contains the following sub directories: raw_binary, temp_binary, raw_root, sorted), a shift file, a scaler file, a coincidence window, and the size of the file buffer in number of hits.

The buffer size is used to specify the size of the buffer in number of hits (each file has its own buffer of the specified size). The file evaluates the size of a hit by checking the first event and determining how many waveform samples are present. NOTE: The assumption is that a base event (no waveform samples) is 24 bytes (i.e. board, channel, timestamp, long, short, flags, Nsamples). If you run with the calibrated enrgy option in CoMPASS this appends an extra word in the data and will cause a crash if it is run through the event builder without modification!

### Types of Event Building
1. SlowSort: Full event building. Data is read from the binary files, time ordered, and then grouped into events based on the coincidence window.
2. Convert: Takes raw binary data and converts it into time-ordered root data.
 
#### Slow Sorting
The first stage is slow sorting the shifted data by timestamp and orgainizing detector hits into large data events. Events are structures which contain all detector hits that occur within a given coincidence window with physical detector variables. This event data is then written to an output file.

The ANASEN event builder sorts hits within each detector type by largest energy. For example: if a coincidence window includes multiple ring hits in QQQ3, the first hit (i.e. fqqq[3].rings[0]) will contain the ring with largest energy.

All of the user input is handled through an input file in the program directory named `input.txt`. This file is preformated; all the user needs to do is update the names and values. Everything from input and output directories, to shifts and coincidence windows should be specified in this file. Note that directories should be explicit fullpaths.

#### Merging
The program is capable of merging several root files together using either `hadd` or the ROOT TChain class. Currently, only the TChain version is implemented in the API, however if you want the other method, it does exist in the RunCollector class.

### Scaler Support
Currently the pipeline supports declaring individual digitizer channels as scalers. These channels will be used a pure counting measures. To make a channel a scaler, put the CoMPASS formated name of the channel and board (check the given etc/ScalerFile.txt for an example) in a text file along with a parameter name for the scaler to be saved as. These files are then processed outside of the event building loop, which can greatly increase the computational speed. Future versions will include scaler rates as well.

## The `etc` Directory
This directory contains some essentials, like the channel map, scaler file, and shift map. Reference the existing examples for how to format these files for ANASEN experiments.

It also contains ROOT macros for preliminary analysis.
- `ANASENPlotEdit.C`: best for generating plots during an experiment. By default, it processes a range of runs and plots the largest energy hit of each detector type.

- `Fangorn.C`: converts built data to a verbose tree more convenient for analysis. Best when you're confident the channel map and detector geometry are correct.

- `PruneTree.C`: prunes the Fangorn.C tree ouput using set criteria (i.e. events with 1+ inner and 1+ outer detector hit)

- `Reconstruct.C`: calculates the interaction point (IntZ), the path traveled from IntZ to an outer detector (IntPath), and theta (IntTheta). It also generates basic histograms.

- `processOneRun.sh`: a shell script to Fangorn, Prune, and Reconstruct a single run

- `processAllRuns.sh`: a shell script to process a range of runs. Any missing built run files are automatically skipped.

## System Requirements
- Requires ROOT version which supports CMake dictionary generation
- Requires CMake > 3.16
- This version is for data from CAEN CoMPASS > 2.0. Data from older CoMPASS versions are not compatible.