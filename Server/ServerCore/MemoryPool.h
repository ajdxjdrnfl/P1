#pragma once

class Memory
{
	enum
	{
		MAX_ALLOC_SIZE = 512,
	};

public:
	Memory();
	~Memory();

	void* Allocate(size_t size);
	void Delocate(void* ptr);

private:
	class MemoryPool* _poolsTable[MAX_ALLOC_SIZE + 1];
	vector<class MemoryPool*> _pools;
};

class MemoryHeader
{
public:
	MemoryHeader(int32 size) : _size(size) { }

	void* AttachHeader(int32 size)
	{
		_size = size;
		return this + 1;
	}
	static MemoryHeader* DetachHeader(void* ptr)
	{
		return reinterpret_cast<MemoryHeader*>(ptr) - 1;
	}

	int32 GetSize() { return _size; }

private:
	int32 _size = 0;
};

class MemoryPool
{
public:
	MemoryPool(uint32 blockSize, int32 blockCount);
	~MemoryPool();

	void* Allocate();
	bool Delocate(void* blockToDelocate);

public:
	bool IsVerifiable(void* blockToVerify);

private:
	BYTE* _header = nullptr;
	atomic<int32>			_blockCount = 0; // 남아있는 블록의 수
	atomic<int32>			_currentBlockID = 0;
	int32					_maxBlockCount;
	uint32					_blockSize;
};