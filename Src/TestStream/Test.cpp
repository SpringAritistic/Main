#include "stdafx.h"
#include "Test.h"
#include "testIgnore.h"
#include "iostream"
#include "iomanip"
#include "string"
#include "sstream"
#include "fstream"
//
#include "locale"
#include "chrono"
#include "ctime"
//

using namespace std;

// custom manipulator with per-stream static data:
std::ostream& Counter(std::ostream& os) {
	const static int index = os.xalloc();
	return os << ++os.iword(index);
}

void test()
{
	IgnoreLines ig(2);
	int b;
	cout << "将跳过以下2行:\n";
	cin >> ig;
	cout << "请输入一个整数：";
	cin >> b;
	cout << "--------------------------------------" << endl;
	//cout.setf(ios::showbase|ios::showpos|ios::uppercase|ios::scientific|ios::fixed);
	string str1("sa");
	cout << setw(20) << setfill('_') << left << internal << dec << b << setw(20) << setfill('*') << uppercase << internal << str1 << endl;
	//
	int x;
	float y;
	string str("30.05");
	istringstream is(str);
	is >> x >> y;
	cout << x << ";" << y << endl;
	//

	std::cout << Counter << ": first line\n";
	std::cout << Counter << ": second line\n";
	std::cout << Counter << ": third line\n";
	// cerr has its own count
	std::cerr << Counter << ": error line\n";
	std::clog << Counter << ": log line\n";
	//testFlush
	testFlush(); 
	cout << "After" << endl;
	testTime();
	system("pause");
}
void testFlush()
{
	ofstream file("Hello.txt");
	auto buf= cout.rdbuf(file.rdbuf());

	file << "hello" << endl;
	cout << "kitty" << endl;
	cout.rdbuf(buf);
	cout << "Welcome" << endl;
	//locale a;
	//string str((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());
	//cout << str << endl;
}
void testTime()
{
	using namespace chrono;
	auto now = system_clock::now();
	time_t t = system_clock::to_time_t(now);
	tm* nowTM=new tm;
	localtime_s(nowTM,&t);

	locale loc;
	const time_put<char>&tpC = use_facet <time_put<char >> (loc);
	stringstream myStream;
	string formatStr = "%c";
	tpC.put(myStream, cout, ' ', nowTM, formatStr.c_str(), formatStr.c_str() + formatStr.size());

	string myStr=myStream.str();
	//myStream >> myStr;
	cout << myStr << endl;
}