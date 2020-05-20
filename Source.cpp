#include<iostream>
#include<thread>
#include<list>
#include<mutex>
#include<ctime>
#include<iostream>
#include<memory>
#include<chrono>
using namespace std::chrono;

class Blocking_Queue
{
	std::list<unsigned int> message_queue;	
	std::mutex m1, m2;
public:
	void LoadToMessageQueue()
	{
		std::lock_guard<std::mutex> lg1(m1);   // exception safety
		srand(time(0));
		message_queue.push_back(rand() % 10);
		std::this_thread::sleep_for(milliseconds(1000));
	}
	void PrintFromMessageQueue()
	{
		std::lock_guard<std::mutex> lg2(m2);     // exception safety
		if (message_queue.size() != 0)
		{
			std::cout << *message_queue.begin() << std::endl;
			message_queue.pop_front();
			std::this_thread::sleep_for(milliseconds(1000));
		}
	}					
};


void demo(int p, int c)
{
	std::unique_ptr<std::thread[]> producers = std::make_unique<std::thread[]>(p);
	std::unique_ptr<std::thread[]> consumers = std::make_unique<std::thread[]>(c);
	Blocking_Queue bq;
	while (true)
	{
		for (int i = 0; i < p; i++)
		{
			producers[i] = std::thread(&Blocking_Queue::LoadToMessageQueue, &bq);
		}
		for (int i = 0; i < c; i++)
		{
			consumers[i] = std::thread(&Blocking_Queue::PrintFromMessageQueue, &bq);
		}
		for (unsigned int i = 0; i < p; ++i)
		{
			if (producers[i].joinable())
				producers[i].join();
		}
		for (unsigned int i = 0; i < c; ++i)
		{
			if (consumers[i].joinable())
				consumers[i].join();
		}
	}
}

int main()
{
	demo(5, 4);
}