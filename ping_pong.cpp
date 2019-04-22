#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

void ping(mutex &m, bool &flag, condition_variable &cv)
{
	for (int i = 0; i < 500000; i++)
	{
		unique_lock<mutex> locker(m);
		while(!flag)
			cv.wait(locker);
		cout << "ping" << endl;
		flag = false;
		cv.notify_one();
	}
}

void pong(mutex &m, bool &flag, condition_variable &cv)
{
	for (int i = 0; i < 500000; i++)
	{
		unique_lock<mutex> locker(m);
		while(flag)
			cv.wait(locker);
		cout << "pong" << endl;
		flag = true;
		cv.notify_one();
	}
}

int main() {

	mutex m;
	bool flag = true;
	condition_variable cv;

	thread m1(ping, std::ref(m), std::ref(flag), std::ref(cv));
	thread m2(pong, std::ref(m), std::ref(flag), std::ref(cv));

	m1.join();
	m2.join();

	return 0;
}
