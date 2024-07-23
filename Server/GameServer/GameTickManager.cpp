#include "pch.h"
#include "GameTickManager.h"

GameTickManager GTickManager;

void GameTickManager::Init()
{
    past = GetTickCount64();
}

void GameTickManager::Update()
{
    past = now;
    now = GetTickCount64();
}

float GameTickManager::GetDeltaTime()
{
    float currentTick = now - past;
    float deltaTime = currentTick / 1000.f;
    return deltaTime;
}
