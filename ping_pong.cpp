#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

void ping(mutex &m, bool &flag, condition_variable &cv1, condition_variable &cv2)
{
	for (int i = 0; i < 500000; i++)
	{
		unique_lock<mutex> locker(m);
		while(!flag)
			cv1.wait(locker);
		cout << "ping" << endl;
		flag = false;
		cv2.notify_one();
	}
}

void pong(mutex &m, bool &flag, condition_variable &cv1, condition_variable &cv2)
{
	for (int i = 0; i < 500000; i++)
	{
		unique_lock<mutex> locker(m);
		while(flag)
			cv2.wait(locker);
		cout << "pong" << endl;
		flag = true;
		cv1.notify_one();
	}
}

int main() {

	mutex m;
	bool flag = true;
	condition_variable cv1;
	condition_variable cv2;


	thread m1(ping, std::ref(m), std::ref(flag), std::ref(cv1), std::ref(cv2));
	thread m2(pong, std::ref(m), std::ref(flag), std::ref(cv1), std::ref(cv2));

	m1.join();
	m2.join();

	return 0;
}
