#include "stdafx.h"
#include "test7_4.h"
#include "algorithm"
//#include "functional"
#include "numeric"

void test7_4()
{
	testTemplate();
	testEnverimentVar();
	testAlgorithm();
}
void testTemplate()
{
	vector<int>vecInt = GetVec(23);
	auto vecD = GetVec(CString("A"), CString("Hello"), CString("Kitty"), CString("Help"));
	// 
	// 
	//Count<int, double> count1;
	
}
void testEnverimentVar()
{

	//CString pathStr, foldstr;
	//GetEnvironmentVariable("Path", pathStr.GetBuffer(),MAX_PATH);
	//GetCurrentDirectory(MAX_PATH, foldstr.GetBuffer());

	//foldstr.ReleaseBuffer();
	//pathStr.ReleaseBuffer();

	//CString str;
	//str = "hello";
}
//#pragma optimize( "", on )

void testAlgorithm()
{
	int intArr[]{13, 30, 10, 134, 340};
	using namespace std;
	transform(begin(intArr), end(intArr), begin(intArr), begin(intArr), multiplies<int>());
	int sum = accumulate(begin(intArr), end(intArr), 0);
	int b(0);
}
