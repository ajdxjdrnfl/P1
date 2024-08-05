#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "CorePch.h"

#include <filesystem>
#include <fstream>
#include <chrono>

#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "Struct.pb.h"
#include "ServerPacketHandler.h"
#include "Utils.h"
#include "LogUtils.h"
#include "Types.h"
#include "Enums.h"
#include "Defines.h"
#include "GameSessionManager.h"


USING_SHARED_PTR(GameSession);
USING_SHARED_PTR(Room);
USING_SHARED_PTR(GameObject);
USING_SHARED_PTR(Player);
USING_SHARED_PTR(Enemy);
USING_SHARED_PTR(SkillActor);
USING_SHARED_PTR(Boss);
USING_SHARED_PTR(Structure);