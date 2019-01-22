#include "stdafx.h"
#include <cmath>
#include "compare_tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


long double compare_tools::m_precision = 1e-8;


compare_tools::compare_tools()
{
}


compare_tools::~compare_tools()
{
}

//////////////////////////////////////////////////////////////////////////
// 其它的所有比较函数都只能依赖于相等和小于来实现
bool compare_tools::is_equal(long double first, long double second, long double precision)
{
	return (abs(first - second) <= precision);
}

bool compare_tools::is_less(long double first, long double second, long double precision)
{
	return (!is_equal(first, second, precision) && first < second);
}

//////////////////////////////////////////////////////////////////////////

bool compare_tools::is_not_equal(long double first, long double second, long double precision)
{
	return !is_equal(first, second, precision);
}

bool compare_tools::is_great(long double first, long double second, long double precision)
{
	return !is_equal(first, second, precision) && !is_less(first, second, precision);
}

bool compare_tools::is_great_equal(long double first, long double second, long double precision)
{
	return !is_less(first, second, precision);
}

bool compare_tools::is_less_equal(long double first, long double second, long double precision)
{
	return is_equal(first, second, precision) || is_less(first, second, precision);
}

long double compare_tools::get_precision()
{
	return m_precision;
}

void compare_tools::set_precision(long double precision)
{
	m_precision = precision;
}

void compare_tools::restore_default_precision()
{
	m_precision = 1e-8; // 保持与初始化的值相同
}
