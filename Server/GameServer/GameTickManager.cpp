#include "pch.h"
#include "GameTickManager.h"

GameTickManager GTickManager;

void GameTickManager::Init()
{
    past = GetTickCount64();
}

void GameTickManager::Update()
{
    // Delta Time
    past = now;
    now = GetTickCount64();

    uint64 a = now;
}

float GameTickManager::GetDeltaTime()
{
    float currentTick = now - past;
    float deltaTime = currentTick / 1000.f;
    return deltaTime;
}

string GameTickManager::GetCurrentTimestamp()
{
    // Time stamp
    const auto now = chrono::system_clock::now();
    time_t tm_now = chrono::system_clock::to_time_t(now);
    tm t_struct;
    localtime_s(&t_struct , &tm_now);

    string result = ::format("[{}:{}:{} - {}:{}:{}]", t_struct.tm_year + 1990, t_struct.tm_mon + 1, t_struct.tm_mday,
        t_struct.tm_hour, t_struct.tm_min, t_struct.tm_sec);

    return result;
}
