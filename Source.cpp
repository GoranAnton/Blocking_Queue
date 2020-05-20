#include<iostream>
#include<thread>
#include<list>
#include<mutex>
#include<ctime>
#include<iostream>
#include<chrono>
using namespace std::chrono;

class Blocking_Queue
{
	unsigned int num_producers;						 // number of consumers
	unsigned int num_consumers;						 // number if producers
	std::list<unsigned int> message_queue;			 // arrray of numbers
	std::thread* producers;							 // pointer to array of producer threads
	std::thread* consumers;							 // pointer to array of consumer threads
	std::mutex m1, m2;
public:
	Blocking_Queue(unsigned int p, unsigned int c);  // parametrized constructor
	~Blocking_Queue();								 // destructor
	void LoadToMessageQueue();						 // adding numbers at the end of message queue
	void PrintFromMessageQueue();					 // taking numbers from begining og message queue
	void start();									 // starting producer's and consumer's threads
};

Blocking_Queue::Blocking_Queue(unsigned int p, unsigned int c)
{
	producers = new std::thread[num_producers = p]();
	consumers = new std::thread[num_consumers = c]();
}

Blocking_Queue::~Blocking_Queue()
{
	delete[] producers;
	producers = nullptr;
	delete[] consumers;
	consumers = nullptr;
}

void Blocking_Queue::LoadToMessageQueue()
{
	std::lock_guard<std::mutex> lg1(m1);   // exception safety
	srand(time(0));
	message_queue.push_back(rand() % 10);
	std::this_thread::sleep_for(milliseconds(1000));
}

void Blocking_Queue::PrintFromMessageQueue()
{
	std::lock_guard<std::mutex> lg2(m2);     // exception safety
	if (message_queue.size() != 0)			
	{
		std::cout << *message_queue.begin() << std::endl;
		message_queue.pop_front();
		std::this_thread::sleep_for(milliseconds(1000));
	}
}

void Blocking_Queue::start()
{
	while (true)
	{
		for (unsigned int i = 0; i < num_producers; ++i)
		{
			(producers[i]) = std::thread(&Blocking_Queue::LoadToMessageQueue, this);
		}
		for (unsigned int i = 0; i < num_consumers; ++i)
		{
			(consumers[i]) = std::thread(&Blocking_Queue::PrintFromMessageQueue, this);
		}
		for (unsigned int i = 0; i < num_producers; ++i)
		{
			if (producers[i].joinable())
				producers[i].join();
		}
		for (unsigned int i = 0; i < num_consumers; ++i)
		{
			if (consumers[i].joinable())
				consumers[i].join();
		}
	}
}


int main()
{
	Blocking_Queue b(4, 5);
	b.start();
}