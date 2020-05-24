#include<iostream>
#include<thread>
#include<list>
#include<mutex>   
#include<iostream>
#include<memory>
#include<condition_variable>
#include<vector>
#include<string>

std::mutex m3;

template<typename T>
class Blocking_Queue
{
	std::list<T> msg_queue;
	std::condition_variable cv1, cv2;
	std::mutex m;
	int max;
public:
	Blocking_Queue(int n) :max(n) {}
	void push_back(T t);
	T pop_front();
};

template<typename T>
void Blocking_Queue<T>::push_back(T t)
{
	std::unique_lock<std::mutex> ul1(m);
	cv1.wait(ul1, [&]() { return msg_queue.size() < max; });
	msg_queue.push_back(t);
	cv2.notify_all();
}

template<typename T>
T Blocking_Queue<T>::pop_front()
{
	std::unique_lock<std::mutex> ul2(m);
	cv2.wait(ul2, [&]() { return msg_queue.size() > 0; });
	T temp = *msg_queue.begin();
	msg_queue.pop_front();
	cv1.notify_all();
	return temp;
}

//---------------------------------------------------------------//

Blocking_Queue<std::string> bq(100);

std::string Generate()
{
	std::string i = "abc ";
	static int c = 1;
	return i+std::to_string(c++);
}

void Produce()
{
	while (true)
	{
		bq.push_back(Generate());
	}
}

void Consume()
{
	while (true)
	{
		std::unique_lock<std::mutex> ul2(m3);
		std::cout << bq.pop_front() << std::endl;
	}
}


int main()
{
	int num_producers = 4;
	int num_consumers = 5;
	std::vector<std::thread> producers;
	std::vector<std::thread> consumers;


	for (int i = 0; i < num_producers; i++)
	{
		producers.emplace_back(std::thread(Produce));
	}

	for (int i = 0; i < num_consumers; i++)
	{
		consumers.emplace_back(std::thread(Consume));
	}

	for (int i = 0; i < num_producers; i++)
	{
		if (producers[i].joinable())
		{
			producers[i].join();
		}
	}
	for (int i = 0; i < num_consumers; i++)
	{
		if (consumers[i].joinable())
		{
			consumers[i].join();
		}
	}
}

