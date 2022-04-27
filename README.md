# Event Builder Skeleton
This is a program designed to be a launching point for event building data from CAEN digitizers run using CoMPASS software. It reads in raw binary data and converts it into time ordered events and saves the data to a ROOT file. This code is intended to provide a start point; further changes would need to be made to make a code that provides some preliminary analysis of the data. 

This code can in princple work with any setup that uses CAEN digitizers, as it requires no knowledge of the physical setup other than a coincidence window. It can also be used to build waveform data (THIS HAS NOT BEEN FULLY TESTED).

## Installation
To install, first pull the repository and all submodules using `git clone --recursive https://github.com/sesps/EventBuilder_Skeleton.git`. The repository uses the [*premake*](https://premake.github.io/) build system. Follow the link to download the premake5 release and then run `premake5 <type>`, where in type you should specify the type of project, from the top level directory of the repository. Then build the project for your enviroment. As an example, the typical process for a Linux machine would look like:
- `premake5 gmake2`
- `make -j 4`

Currently the repository only supports Linux and Mac environments due to limiatations with building ROOT CERN dictionaries in Windows. 

## EventBuilder vs. EventBuilderGui
There are two programs provided. They are `EventBuilder` and `EventBuilderGui`. The second is a full GUI version of the event builder. The GUI supports all conversion methods and the plotting tool. The first is a commandline version.

### Building Events
The event building operation is the bulk of the analysis process. As files are being converted to ROOT from the raw CoMPASS binary, events are built using information given by the user. In particular the code asks the user to specify a workspace (a top level directory which contains the following sub directories: raw_binary, temp_binary, raw_root, sorted), a shift file, a scaler file, a coincidence window, and the size of the file buffer in number of hits.

The buffer size is used to specify the size of the buffer in number of hits (each file has its own buffer of the specified size). The file evaluates the size of a hit by checking the first event and determining how many waveform samples are present. NOTE: The assumption is that a base event (no waveform samples) is 24 bytes (i.e. board, channel, timestamp, long, short, flags, Nsamples). If you run with the calibrated enrgy option in CoMPASS this appends an extra word in the data and will cause a crash if it is run through the event builder without modification!

#### Types of Event Building
1. SlowSort: Full event building. Data is read from the binary files, time ordered, and then grouped into events based on the coincidence window.
2. Convert: Takes raw binary data and converts it into time-ordered root data.
 
#### Slow Sorting
The first stage is slow sorting the shifted data by timestamp and orgainizing detector hits into 
large data events. Events are structures which contain all detector hits that occur within a given coincidence window with physical detector variables. This event data is then written to an output file.

All of the user input is handled through an input file in the program directory named 
`input.txt`. This file is preformated; all the user needs to do is update the names and
values. Everything from input and output directories, to shifts and coincidence windows should
be specified in this file. Note that directorires should be explicit fullpaths.

### Merging
The program is capable of merging several root files together using either `hadd` or the ROOT TChain class. Currently, only the TChain version is implemented in the API, however if you want the other method, it does exist in the RunCollector class.

### Scaler Support
Currently the pipeline supports declaring individual digitizer channels as scalers. These channels will be used a pure counting measures. To make a channel a scaler, put the CoMPASS formated name of the channel and board (check the given etc/ScalerFile.txt for an example) in a text file along with a parameter name for the scaler to be saved as. These files are then processed outside of the event building loop, which can greatly increase the computational speed. Future versions will include scaler rates as well.

## System Requirements
Only tested with `ROOT` 6.14, mileage may vary
Uses C++11 standards
Only compatible with MacOSX and Linux