#include "stdafx.h"
#include "test6-28.h"

#include "tuple"
using namespace  std;
void test6_28()
{
	testTuple();
}

void testTuple()
{
	auto A = make_tuple(10, 2.0);
	typedef tuple<int, float> tupeType;

	typedef tuple_element<1, tupeType>::type secondType;
	secondType B(5);
	int c = 0;
}
