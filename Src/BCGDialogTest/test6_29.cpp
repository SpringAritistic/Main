#include "stdafx.h"
#include "test6_29.h"
#include "utility"
#include "vector"
using namespace  std;
void test6_29()
{
	testTrait();
	CString str("Kitty");
	int strId = *(int*)str.GetBuffer();
	CString teststr1 = testMove(std::cref(str));
	CString teststr2 = testMove(str);
	CString& teststr = testMove(move(str));
	CString teststr3 = testMove(CString("China"));
	strId = *(int*)&teststr3;
	str = "welcome";

	testPair();
	testChange();

	testGetID();
	testID();
}

void testTrait()
{
	CString str = foo(0);
	str = foo("Hello");
	char* p = new char(10);
	str = foo(p);
}
CString&& testMove(CString&& str)
{
	int strId = *(int*)&str;

	str = "Hello";
	return move(str);
}
CString testMove(CString& str)
{
	int strId = *(int*)&str;

	str = "Hello1";
	return str;
}
CString testMove(const CString& str)
{
	return "str";
}
void testPair()
{
	int num(10);
	CString index("hell0");
	auto myPair = make_pair(ref(index), ref(num));
	myPair.first = "welcome";
	myPair.second = 20;
	//
	int num1(23);
	CString index1("Kitty");
	auto myPair1 = make_pair(move(num1), move(index1));
	myPair1.first++;
}
void testChange()
{
	person A;
	//clerk B = (clerk)A;
	clerk B = *(clerk*)&A;
	B.m_name = "White";
	B.m_index = 0;
}
IntId& GetId(int value, vector<IntId*>&vecId)
{
	IntId* id = new IntId(value);
	vecId.push_back(id);
	return *id;
}

int& GetId(int value, int& id)
{
	int* pInt = new int(value);
	id = int(pInt);
	return *pInt;
}
void testGetID()
{
	int num1, num2;
	int id1 = GetId(20,num1);
	int& id2 = GetId(20, num2);

	vector<int>vecInt{ int(&id1), int(&id2) };
}
void testID()
{
	vector<IntId*>vecId;
	{
		IntId id1 = GetId(10, vecId);
		id1 = 200;
	}
	IntId& id2 = GetId(100, vecId);
	for (auto&item : vecId)
		delete item;
	vecId.clear();
}
