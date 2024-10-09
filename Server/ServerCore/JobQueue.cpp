#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

// 다른 JobQueue에서는 관여하면 안됨

void JobQueue::Push(JobRef job, bool pushOnly)
{

	_jobs.Push(job); // WRITE_LOCK
	const int32 prevCount = _jobCount.fetch_add(1, ::memory_order_release);

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		// 
		else if(LCurrentJobQueue != this)
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

// 1) 일감이 너~무 몰리면?
void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		JobRef job;

		if (_jobs.Pop(job) == false) 
		{
			LCurrentJobQueue = nullptr;
			OnFinishedExecute();
			GGlobalQueue->Push(shared_from_this());
			return;
		}

		job->Execute();

		if (_jobCount.fetch_sub(1, ::memory_order_acquire) == 1)
		{
			LCurrentJobQueue = nullptr;
			OnFinishedExecute();
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			OnFinishedExecute();
			GGlobalQueue->Push(shared_from_this());
			break;
		}
		/*vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			OnFinishedExecute();
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			OnFinishedExecute();
			GGlobalQueue->Push(shared_from_this());
			break;
		}*/			
	}


	
}
