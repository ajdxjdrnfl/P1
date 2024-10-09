#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

// �ٸ� JobQueue������ �����ϸ� �ȵ�

void JobQueue::Push(JobRef job, bool pushOnly)
{

	_jobs.Push(job); // WRITE_LOCK
	const int32 prevCount = _jobCount.fetch_add(1, ::memory_order_release);

	// ù��° Job�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
		// �̹� �������� JobQueue�� ������ ����
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		// 
		else if(LCurrentJobQueue != this)
		{
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue�� �ѱ��
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

// 1) �ϰ��� ��~�� ������?
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
