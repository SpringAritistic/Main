//#include "stdafx.h"
#include "testAsync.h"
#include "chrono"
#include "future"
#include "thread"
#include "iostream"
#include "exception"
#include "stdexcept"
//
#include "mutex"
using namespace std;
//
mutex printMutex;

int queryNumber()
{
	cout << "Read Number:\t";
	int num;
	cin >> num;
	if (!cin)
		throw runtime_error("no number read");
	return num;
}
void doSomething(char c, shared_future<int>& f)
{
	lock_guard<mutex> l(printMutex);
	//cout << this_thread::get_id() << ":";
	try
	{
		int num = f.get();
		for (int i = 0; i < num; i++)
		{
			this_thread::sleep_for(chrono::milliseconds(1));
			cout.put(c).flush();
		}
		cout << endl;
	}
	catch (const exception& e)
	{
		cerr << "EXCEPTION in thread " << this_thread::get_id() << ": " << e.what() << endl;
	}

}


void testSharedFature()
{
	try{

		shared_future<int> f = async(queryNumber);

		auto f1 = async(launch::async, doSomething, '.', ref(f));
		auto f2 = async(launch::async, doSomething, '+',  ref(f));
		auto f3 = async(launch::async, doSomething, '-',  ref(f));


		f1.get();
		f2.get();
		f3.get();
	}
	catch (const exception& e)
	{
		cout << "\nEXCEPTION: " << e.what() << endl;
	}
	cout << "\nDone" << endl;
}