#pragma once

template <typename T>
class MPSCQueue
{
public:
	MPSCQueue()
	{
		_head = make_shared<Node>();
		_tail.store(_head.load());
	}
		
	~MPSCQueue()
	{
		
	}

	void Push(const T& data)
	{
		shared_ptr<Node> newNode = make_shared<Node>();
		newNode->data = data;
		
		shared_ptr<Node> prevHead = _head.exchange(newNode, ::memory_order_acq_rel);
		prevHead->next.store(newNode, ::memory_order_release);

		//std::cout << "Push " << newNode << " , prev : "  << prevHead<< std::endl;
	}

	bool Pop(T& output)
	{
		shared_ptr<Node> tail = _tail.load(::memory_order_relaxed);
		shared_ptr<Node> next = tail->next.load(::memory_order_acquire);

		if (next == nullptr)
		{
			return false;
		}

		//std::cout << "Pop " << tail << std::endl;
		output = next->data;
		_tail.store(next, ::memory_order_release);
		return true;
	}

	void Clear()
	{
		T dummy;
		while (Pop(dummy));
	}

private:
	struct Node
	{
		T data;
		::atomic<shared_ptr<Node>> next;
	};

	::atomic<shared_ptr<Node>> _head;
	::atomic<shared_ptr<Node>> _tail;
};

