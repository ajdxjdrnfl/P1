#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"
#include "MPSCQueue.h"

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallbackType&& callback)
	{
		Push(make_shared<Job>(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Params, typename... Args>
	void DoAsync(Ret(T::*memFunc)(Params...), Args&&... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(make_shared<Job>(owner, memFunc, ::forward<Args>(args)...));
	}

	void DoTimer(uint64 tickAfter, CallbackType&& callback)
	{
		JobRef job = make_shared<Job>(std::move(callback));
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Params, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Params...), Args&&... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		JobRef job = make_shared<Job>(owner, memFunc, std::forward<Args>(args)...);
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	void					ClearJobs() { _jobs.Clear(); }

public:
	void					Push(JobRef job, bool pushOnly = false);
	void					Execute();

private:
	virtual void OnFinishedExecute() { }

protected:
	//LockQueue<JobRef>		_jobs;
	MPSCQueue<JobRef>		_jobs;
	
	atomic<int32>			_jobCount = 0;
};

