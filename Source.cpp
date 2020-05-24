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

class Blocking_Queue
{
	std::list<std::function<void(int,int)>> msg_queue;
	std::condition_variable cv1, cv2;
	std::mutex m;
	int max;
public:
	Blocking_Queue(int n) :max(n) {}
	void push_back(std::function<void(int,int)>);
	std::function<void(int,int)> pop_front();
};


void Blocking_Queue::push_back(std::function<void(int,int)> fun)
{
	std::unique_lock<std::mutex> ul1(m);
	cv1.wait(ul1, [&]() { return msg_queue.size() < max; });
	msg_queue.push_back(fun);
	cv2.notify_all();
}


std::function<void(int,int)> Blocking_Queue::pop_front()
{
	std::unique_lock<std::mutex> ul2(m);
	cv2.wait(ul2, [&]() { return msg_queue.size() > 0; });
	std::function<void(int,int)> temp = *msg_queue.begin();
	msg_queue.pop_front();
	cv1.notify_all();
	return temp;
}

//---------------------------------------------------------------//

Blocking_Queue bq(100);

void Produce(std::function<void(int, int)> f)
{
	while (true)
	{
		bq.push_back(f);
	}
}

void Consume()
{
	while (true)
	{
		std::function<void(int,int)> f= bq.pop_front();
		f(rand()%10,rand()%10);
	}
}

void zbir(int a, int b)		{ std::cout << "Zbir " << a << " i " << b << " iznosi " << a + b << std::endl; }
void razlika(int a, int b)	{ std::cout << "Razlika " << a << " i " << b << " iznosi " << a - b << std::endl;}
void proizvod(int a, int b)	{ std::cout << "Proizvod " << a << " i " << b << " iznosi " << a * b << std::endl;}
void kolicnik(int a, int b)	{ std::cout << "Kolicnik " << a << " i " << b+1 << " iznosi " << a / (b+1) << std::endl;}
std::function<void(int, int)> operacija[] = { zbir ,razlika, proizvod,kolicnik };


int main()
{
	int num_producers = 4;
	int num_consumers = 5;
	std::vector<std::thread> producers;
	std::vector<std::thread> consumers;


	for (int i = 0; i < num_producers; i++)
	{
		producers.emplace_back(std::thread(Produce, operacija[rand()%4]));   // biramo operaciju
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

