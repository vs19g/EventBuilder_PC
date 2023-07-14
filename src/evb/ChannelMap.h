#ifndef CHANNEL_MAP_H
#define CHANNEL_MAP_H


namespace EventBuilder {

	enum DetType
	{
		FQQQ0Ring,
		FQQQ0Wedge,
		FQQQ1Ring,
		FQQQ1Wedge,
		FQQQ2Ring,
		FQQQ2Wedge,
		FQQQ3Ring,
		FQQQ3Wedge,
		Barrel0FrontUp,
		Barrel0FrontDn,
		Barrel0Back,  
		Barrel1FrontUp,
		Barrel1FrontDn,
		Barrel1Back,  
		Barrel2FrontUp,
		Barrel2FrontDn,
		Barrel2Back,  
		Barrel3FrontUp,
		Barrel3FrontDn,
		Barrel3Back,  
		Barrel4FrontUp,
		Barrel4FrontDn,
		Barrel4Back,  
		Barrel5FrontUp,
		Barrel5FrontDn,
		Barrel5Back,  
		Barrel6FrontUp,
		Barrel6FrontDn,
		Barrel6Back,  
		Barrel7FrontUp,
		Barrel7FrontDn,
		Barrel7Back,  
		Barrel8FrontUp,
		Barrel8FrontDn,
		Barrel8Back,  
		Barrel9FrontUp,
		Barrel9FrontDn,
		Barrel9Back,  
		Barrel10FrontUp,
		Barrel10FrontDn,
		Barrel10Back,  
		Barrel11FrontUp,
		Barrel11FrontDn,
		Barrel11Back,  
		PCAnode,
		PCCathode,
		None
	};


	struct Channel
	{
		DetType type;
		int local_channel;
	};

	class ChannelMap
	{
	public:
		using Container = std::unordered_map<int, Channel>;
		using Iterator = std::unordered_map<int, Channel>::iterator;

		ChannelMap(const std::string& filename);
		~ChannelMap();

		bool FillMap(const std::string& filename);
		inline Iterator FindChannel(int key) { return m_cmap.find(key); }
		inline Iterator End() { return m_cmap.end(); }
	        inline bool IsValid() { return m_isValid; }	

	private:
		Container m_cmap;
		bool m_isValid;
	};

}


#endif
