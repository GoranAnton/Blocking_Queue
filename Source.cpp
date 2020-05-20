#include<iostream>
#include<thread>
#include<list>
#include<mutex>   
#include<ctime>
#include<iostream>
#include<memory>
#include<vector>
#include<chrono>
using namespace std::chrono;

std::mutex m1, m2;


class Blocking_Queue
{
	std::list<std::pair<std::thread::id,int>> msg_queue;  // PORED SAMOG BROJA CUVAM I ID PRODUCER-a JE GENERISAO TAJ BROJ
public:
	void LoadToMessageQueue(std::thread::id this_id)  
	{
		std::lock_guard<std::mutex> lg1(m1);
		srand(time(0));
		msg_queue.push_back(std::make_pair(this_id, rand() % 10));
		std::this_thread::sleep_for(milliseconds(1000));
	}
	void PrintFromMessageQueue(std::thread::id this_id)  // PORED SAMOG BROJA CUVAM I ID CONSUMER-a JE ISPISAO TAJ BROJ
	{
		std::lock_guard<std::mutex> lg2(m2);
		if (msg_queue.size() != 0)
		{											
			std::cout << "(" << msg_queue.begin()->first << "," << this_id << ") => " << msg_queue.begin()->second << std::endl; // (ID PRODUCER-a, ID CONSUMER-a) => BROJ
			msg_queue.pop_front();
			std::this_thread::sleep_for(milliseconds(1000));
		}
	}
};

Blocking_Queue bq;

void Produce()
{
	while (true)
	{
		bq.LoadToMessageQueue(std::this_thread::get_id());
		std::this_thread::sleep_for(milliseconds(500));
	}
}

void Consume()
{
	while (true)
	{
		bq.PrintFromMessageQueue(std::this_thread::get_id());
		std::this_thread::sleep_for(milliseconds(500));
	}
}


int main()
{
	int num_producers = 5;
	int num_consumers = 4;
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