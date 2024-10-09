#pragma once
#include "GameTickManager.h"

class LogUtils
{
public:
	template<typename T>
	static void LogPacket(const char* funcName, T& pkt)
	{
		uint64 packetSize = pkt.ByteSizeLong();
		string metainfo = pkt.DebugString();
		
		string timestamp = GTickManager.GetCurrentTimestamp();
		//cout << timestamp << "\"" << funcName << "\"" << "packetsize : " << packetSize << endl << metainfo << endl;

	}

	// TODO : 로그 저장기능

};

