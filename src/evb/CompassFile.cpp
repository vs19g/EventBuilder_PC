/*
	CompassFile.cpp
	Wrapper class around a shared pointer to an ifstream. Here the shared pointer is used
	to overcome limitations of the ifstream class, namely that it is written such that ifstream
	cannot be modified by move semantics. Contains all information needed to parse a single binary
	CompassFile. Currently has a class wide defined buffer size; may want to make this user input
	in the future.

	Written by G.W. McCann Oct. 2020
*/
#include "EventBuilder.h"
#include "CompassFile.h"

namespace EventBuilder {

	CompassFile::CompassFile() :
		m_filename(""), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_smap(nullptr), m_hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()),
		m_eofFlag(false)
	{
		m_buffersize = m_bufsizeHits*m_hitsize;
		m_hitBuffer.resize(m_buffersize);
	}
	
	CompassFile::CompassFile(const std::string& filename) :
		m_filename(""), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_smap(nullptr), m_hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()),
		m_eofFlag(false)
	{
		m_buffersize = m_bufsizeHits*m_hitsize;
		m_hitBuffer.resize(m_buffersize);
		Open(filename);
	}
	
	CompassFile::CompassFile(const std::string& filename, uint64_t bsize) :
		m_filename(""), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_smap(nullptr), m_hitUsedFlag(true),
		m_bufsizeHits(bsize), m_file(std::make_shared<std::ifstream>()), m_eofFlag(false)
	{
		m_buffersize = m_bufsizeHits*m_hitsize;
		m_hitBuffer.resize(m_buffersize);
		Open(filename);
	}
	
	CompassFile::~CompassFile()
	{
		Close();
	}
	
	void CompassFile::Open(const std::string& filename)
	{
		m_eofFlag = false;
		m_hitUsedFlag = true;
		m_filename = filename;
		m_file->open(m_filename, std::ios::binary | std::ios::in);
	
		m_file->seekg(0, std::ios_base::end);
		m_size = m_file->tellg();
		if(m_size <= 24)
		{
			m_eofFlag = true;
			m_nHits = 0;
		}
		else
		{
			m_file->seekg(0, std::ios_base::beg);
			m_hitsize = GetHitSize();
			m_buffersize = m_hitsize*m_bufsizeHits;
			m_hitBuffer.resize(m_buffersize);
			m_nHits = m_size/m_hitsize;
		}
	}
	
	void CompassFile::Close()
	{
		if(IsOpen())
			m_file->close();
	}
	
	unsigned int CompassFile::GetHitSize()
	{
		if(!IsOpen())
		{
			std::cerr<<"Unable to get hit size due to file not being open!"<<std::endl;
			return 0;
		}
	
		char* firstHit = new char[24]; //A compass hit by default has 24 bytes (at least in our setup)
	
		m_file->read(firstHit, 24);
		
		uint32_t nsamples = *((uint32_t*) &firstHit[20]);
	
		m_file->seekg(0, std::ios_base::beg);
	
		delete[] firstHit;
	
		return 24 + nsamples*4; 
	
	}
	
	/*
		GetNextHit() is the function which... gets the next hit
		Has to check if the buffer needs refilled/filled for the first time
		Upon pulling a hit, sets the UsedFlag to false, letting the next level know
		that the hit should be free game.
	
		If the file cannot be opened, signals as though file is EOF
	*/
	bool CompassFile::GetNextHit()
	{
		if(!IsOpen())
			return true;
	
		if((m_bufferIter == nullptr || m_bufferIter == m_bufferEnd) && !IsEOF())
			GetNextBuffer();
	
		if(!IsEOF())
		{
			ParseNextHit();
			m_hitUsedFlag = false;
		}
	
		return m_eofFlag;
	}
	
	/*
		GetNextBuffer() ... self-explanatory name
		Note tht this is where the EOF flag is set. The EOF is only singaled
		after the LAST buffer is completely read (i.e literally no more data). ifstream sets its eof 
		bit upon pulling the last buffer, but this class waits until that entire
		last buffer is read to singal EOF (the true end of file). 
	*/
	void CompassFile::GetNextBuffer()
	{
	
		if(m_file->eof())
		{
			m_eofFlag = true;
			return;
		}
	
		m_file->read(m_hitBuffer.data(), m_hitBuffer.size());
	
		m_bufferIter = m_hitBuffer.data();
		m_bufferEnd = m_bufferIter + m_file->gcount(); //one past the last datum
	
	}
	
	void CompassFile::ParseNextHit()
	{
	
		m_currentHit.board = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.channel = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.timestamp = *((uint64_t*)m_bufferIter);
		m_bufferIter += 8;
		m_currentHit.lgate = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.sgate = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.flags = *((uint32_t*)m_bufferIter);
		m_bufferIter += 4;
		m_currentHit.Ns = *((uint32_t*)m_bufferIter);
		m_bufferIter += 4;
	
		if(m_smap != nullptr)
		{ //memory safety
			int gchan = m_currentHit.channel + m_currentHit.board*16;
			m_currentHit.timestamp += m_smap->GetShift(gchan);
		}
	
		/*
		if(m_currentHit.Ns*2 + 24 != hitsize) {
			std::cerr<<"WARNING! Hit size does not match current value of samples... Try to proceed, but expect seg fault."<<std::endl;
			return;
		}
		*/
		for(unsigned int i=0; i<m_currentHit.Ns; i++)
		{
			m_currentHit.samples.push_back(*((uint16_t*)m_bufferIter));
			m_bufferIter += 2;
		}
	}

}