#pragma once
class GameTickManager
{
public:
	void		Init();
	void		Update();
	float		GetDeltaTime();
	
	string		GetCurrentTimestamp();
private:
	uint64 now;
	uint64 past;


};

extern GameTickManager GTickManager;
