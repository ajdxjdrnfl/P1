#include "pch.h"
#include "MemoryPool.h"

Memory::Memory()
{
	int32 size = 1;
	int32 index = 1;

	for (size = 1; size <= MAX_ALLOC_SIZE; size *= 2)
	{
		MemoryPool* pool = new MemoryPool(size, 1024);
		_pools.push_back(pool);
		while (index <= size)
		{
			_poolsTable[index++] = pool;
		}
	}
}

Memory::~Memory()
{
	for (auto pool : _pools)
	{
		if (pool != nullptr)
			delete pool;
	}
	_pools.clear();
}

void* Memory::Allocate(size_t size)
{
	MemoryHeader* ptr = nullptr;
	const int32 totalSize = size + sizeof(MemoryHeader);

	if (totalSize > MAX_ALLOC_SIZE)
	{
		ptr = reinterpret_cast<MemoryHeader*>(::malloc(totalSize));
	}
	else
	{
		ptr = reinterpret_cast<MemoryHeader*>(_poolsTable[totalSize]->Allocate());

		if (ptr == nullptr)
		{
			ptr = reinterpret_cast<MemoryHeader*>(::malloc(totalSize));
		}
	}
	
	void* ptrToAllocate = ptr->AttachHeader(totalSize);
	return ptrToAllocate;

}

void Memory::Delocate(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32 size = header->GetSize();
	void* ptrToDelocate = reinterpret_cast<void*>(header);

	if (size > MAX_ALLOC_SIZE || !_poolsTable[size]->IsVerifiable(ptrToDelocate))
	{
		free(ptrToDelocate);
	}
	else
	{
		assert(_poolsTable[size]->Delocate(ptrToDelocate));
	}

}

MemoryPool::MemoryPool(uint32 blockSize, int32 blockCount) : _blockSize(blockSize), _maxBlockCount(blockCount)
{
	_header = static_cast<BYTE*>(malloc(_blockSize * _maxBlockCount));
	BYTE* p = _header;

	for (int32 i = 0; i < _maxBlockCount; i++)
	{
		*reinterpret_cast<int*>(p) = i + 1;
		p += _blockSize;
	}

	*reinterpret_cast<int*>(p) = -1;

	_blockCount = _maxBlockCount;
}

MemoryPool::~MemoryPool()
{
	free(_header);
}

void* MemoryPool::Allocate()
{

	BYTE* blockToAllocate = nullptr;

	int idToAllocate = _currentBlockID.load();
	int next;

	do
	{
		if (idToAllocate == -1)
		{
			return nullptr;
		}

		next = *reinterpret_cast<int*>(_header + _blockSize * idToAllocate);
	} while (!_currentBlockID.compare_exchange_strong(idToAllocate, next));

	if (idToAllocate == -1)
	{
		return nullptr;
	}

	return reinterpret_cast<void*>(_header + idToAllocate * _blockSize);
}

bool MemoryPool::Delocate(void* blockToDelocate)
{
	// 잘못된 메모리 할당
	if (!IsVerifiable(blockToDelocate))
	{
		return false;
	}

	int blockIndex = (static_cast<int>(reinterpret_cast<BYTE*>(blockToDelocate) - _header)) / _blockSize;
	int next;
	do
	{
		next = _currentBlockID.load();
	} while (!_currentBlockID.compare_exchange_strong(next, blockIndex));

	*reinterpret_cast<int*>(blockToDelocate) = next;

	return true;
}

bool MemoryPool::IsVerifiable(void* blockToVerify)
{
	if (blockToVerify < _header || blockToVerify >= _header + _maxBlockCount * _blockSize)
	{
		return false;
	}
	else
		return true;
}

