#include<iostream>
#include<thread>
#include<list>
#include<mutex>   
#include<iostream>
#include<memory>
#include<condition_variable>
#include<vector>
#include<string>
#include"json.hpp"
using json = nlohmann::json;

std::mutex m3;

class Blocking_Queue
{
	std::list<json> msg_queue;
	std::condition_variable cv1, cv2;
	std::mutex m;
	int max;
public:
	Blocking_Queue(int n) :max(n) {}
	void push_back(json j);
	json pop_front();
};

void Blocking_Queue::push_back(json j)
{
	std::unique_lock<std::mutex> ul1(m);
	cv1.wait(ul1, [&]() { return msg_queue.size() < max; });
	msg_queue.push_back(j);
	cv2.notify_all();
}

json Blocking_Queue::pop_front()
{
	std::unique_lock<std::mutex> ul2(m);
	cv2.wait(ul2, [&]() { return msg_queue.size() > 0; });
	json temp = *msg_queue.begin();
	msg_queue.pop_front();
	cv1.notify_all();
	return temp;
}

//---------------------------------------------------------------//

Blocking_Queue bq(100);


void Produce()
{
	while (true)
	{
		bq.push_back(json{ {"a",rand()%10},{"b",rand()%10} });
	}
}

void Consume()
{
	while (true)
	{
		std::string s = bq.pop_front().dump();
		auto parsed = json::parse(s.c_str());
		int a = parsed["a"];
		int b = parsed["b"];
		std::cout << a + b << std::endl;
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

