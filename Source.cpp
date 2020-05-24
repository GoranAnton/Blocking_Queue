#include<iostream>
#include<fstream>
#include<thread>
#include<list>
#include<mutex>   
#include<iostream>
#include<memory>
#include<condition_variable>
#include<vector>
#include<string>

std::mutex m3, m4;

class Blocking_Queue
{
	std::list<std::string> msg_queue;
	std::condition_variable cv1, cv2;
	std::mutex m;
	int max;
public:
	Blocking_Queue(int n) :max(n) {}
	void push_back(std::string);
	std::string pop_front();
};


void Blocking_Queue::push_back(std::string s)
{
	std::unique_lock<std::mutex> ul1(m);
	cv1.wait(ul1, [&]() { return msg_queue.size() < max; });
	msg_queue.push_back(s);
	cv2.notify_all();
}


std::string Blocking_Queue::pop_front()
{
	std::unique_lock<std::mutex> ul2(m);
	cv2.wait(ul2, [&]() { return msg_queue.size() > 0; });
	std::string temp = *msg_queue.begin();
	msg_queue.pop_front();
	cv1.notify_all();
	return temp;
}

//---------------------------------------------------------------//

Blocking_Queue bq(100);

void Produce(std::string* niz, int n)
{
	for(int i=0; i<n; ++i)
	{
		bq.push_back(niz[i]);
	}
}

void Consume()
{
	while (true)
	{
		std::string f = bq.pop_front();
		std::ifstream in(f);
		std::string s;
		m4.lock();
		std::cout << "Id thread-a: " << std::this_thread::get_id() << std::endl;
		while (getline(in, s))
		{
			std::cout << s << std::endl;
		}
		m4.unlock();
		std::cout << std::endl;
		in.close();
	}
}

void read(std::string* niz, int n)
{
	std::thread Producer(Produce, niz, n);

	int num_consumers = 5;
	std::vector<std::thread> consumers;
	for (int i = 0; i < num_consumers; i++)
	{
		consumers.emplace_back(std::thread(Consume));
	}

	if (Producer.joinable())
	{
		Producer.join();
	}
	for (int i = 0; i < num_consumers; i++)
	{
		if (consumers[i].joinable())
		{
			consumers[i].join();
		}
	}
}

int main()
{
	std::string niz[] = {
	"C:\\Users\\goran.antonijevic\\OneDrive - ENDAVA\\Desktop\\jedan.txt",
	"C:\\Users\\goran.antonijevic\\OneDrive - ENDAVA\\Desktop\\dva.txt",
	"C:\\Users\\goran.antonijevic\\OneDrive - ENDAVA\\Desktop\\tri.txt",
	"C:\\Users\\goran.antonijevic\\OneDrive - ENDAVA\\Desktop\\cetiri.txt",
	"C:\\Users\\goran.antonijevic\\OneDrive - ENDAVA\\Desktop\\pet.txt"
	};
	read(niz, 5);
}

