/*
	CompassFile.h
	Wrapper class around a shared pointer to an ifstream. Here the shared pointer is used
	to overcome limitations of the ifstream class, namely that it is written such that ifstream
	cannot be modified by move semantics. Contains all information needed to parse a single binary
	CompassFile. Currently has a class wide defined buffer size; may want to make this user input
	in the future.

	Written by G.W. McCann Oct. 2020
*/
#ifndef COMPASSFILE_H
#define COMPASSFILE_H

#include "CompassHit.h"
#include "ShiftMap.h"
#include <memory>

namespace EventBuilder {

	class CompassFile
	{
	public:
		CompassFile();
		CompassFile(const std::string& filename);
		CompassFile(const std::string& filename, uint64_t bsize);
		~CompassFile();
		void Open(const std::string& filename);
		void Close();
		bool GetNextHit();

		inline bool IsOpen() { return m_file->is_open(); }
		inline CompassHit GetCurrentHit() const { return m_currentHit; }
		inline std::string GetName() { return  m_filename; }
		inline bool CheckHitHasBeenUsed() { return m_hitUsedFlag; } //query to find out if we've used the current hit
		inline void SetHitHasBeenUsed() { m_hitUsedFlag = true; } //flip the flag to indicate the current hit has been used
		inline bool IsEOF() { return m_eofFlag; }; //see if we've read all available data
		inline bool* GetUsedFlagPtr() { return &m_hitUsedFlag; }
		inline void AttachShiftMap(ShiftMap* map) { m_smap = map; }
		inline uint64_t GetSize() { return m_size; }
		inline uint64_t GetNumberOfHits() { return m_nHits; }
	
	
	private:
		unsigned int GetHitSize();
		void ParseNextHit();
		void GetNextBuffer();
	
		using Buffer = std::vector<char>;
	
		using FilePointer = std::shared_ptr<std::ifstream>; //to make this class copy/movable
	
		std::string m_filename;
		Buffer m_hitBuffer;
		char* m_bufferIter;
		char* m_bufferEnd;
		ShiftMap* m_smap; //NOT owned by CompassFile. DO NOT delete
		bool m_hitUsedFlag;
		uint64_t m_bufsizeHits = 200000; //size of the buffer in hits
		uint32_t m_hitsize = 24; //size of a CompassHit in bytes (without alignment padding), sans waveform samples
		uint64_t m_buffersize;
		CompassHit m_currentHit;
		FilePointer m_file;
		bool m_eofFlag;
		uint64_t m_size; //size of the file in bytes
		uint64_t m_nHits; //number of hits in the file (m_size/24)
	
	
	};

}

#endif
