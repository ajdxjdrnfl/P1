#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
				Room();
	virtual		~Room();

	void		Init();
	void		Update();

public:
	bool		HandleEnterGame(GameSessionRef session);
	bool		HandleMove(Protocol::C_MOVE pkt);
	
private:
	void		EnterGame(PlayerRef player);

public:
	void		Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

public:
	RoomRef		GetRoomRef();


protected:
	unordered_map<uint64, PlayerRef> _players;
	

};

