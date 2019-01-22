#include "stdafx.h"

#include <tchar.h>
#include <cfloat>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <string>


#include "scope_assemblage.h"


using namespace std;

//////////////////////////////////////////////////////////////////////////

// 同时支持两种字符集
typedef std::basic_string<TCHAR> tstring;

//////////////////////////////////////////////////////////////////////////

// 内部用到的浮点数比较工具
class compare_double
{
private:
	compare_double();
	~compare_double();

public:
	static bool is_equal(double first, double decond);
	static bool is_not_equal(double first, double decond);
	static bool is_great(double first, double decond);
	static bool is_great_equal(double first, double decond);
	static bool is_less(double first, double decond);
	static bool is_less_equal(double first, double decond);

	static double get_precision();
	static void set_precision(double dPrec);
	static void restore_default_precision();

private:
	static double m_precision;
};

double compare_double::m_precision = 1e-8; // 初始化为10的-8次方

compare_double::compare_double()
{
}

compare_double::~compare_double()
{
}

// 其它的所有比较函数都只能依赖于相等和小于来实现
bool compare_double::is_equal(double first, double decond)
{
	return (abs(first - decond) <= m_precision);
}

bool compare_double::is_less(double first, double decond)
{
	return (!is_equal(first, decond) && first < decond);
}

bool compare_double::is_not_equal(double first, double decond)
{
	return !is_equal(first, decond);
}

bool compare_double::is_great(double first, double decond)
{
	return !is_equal(first, decond) && !is_less(first, decond);
}

bool compare_double::is_great_equal(double first, double decond)
{
	return !is_less(first, decond);
}

bool compare_double::is_less_equal(double first, double decond)
{
	return is_equal(first, decond) || is_less(first, decond);
}

double compare_double::get_precision()
{
	return m_precision;
}

void compare_double::set_precision(double dPrec)
{
	m_precision = dPrec;
}

void compare_double::restore_default_precision()
{
	m_precision = 1e-8; // 保持与初始化的值相同
}

//////////////////////////////////////////////////////////////////////////

// 一个内部用到的字符串处理工具
class private_string_tool
{
private:
	private_string_tool();
	~private_string_tool();

public:
	// 判断字符串str是否以strBegin开头
	static bool is_begin_with(const tstring& str, const tstring& pre);

	// 把字符串按指定的字符串中的任意字符来拆分并返回结果，默认分隔符为空格，分隔符可能是临时变量，不使用引用传递
	static void split_string(std::vector<tstring>& result, const tstring& text, const tstring split = _T(" "));

	// 将字符串中的中文逗号、句号、分号、左右小括号、左右中括号、全角空格全部换成英文半角字符
	static void replace_CHS_to_EN_in_scope_string(tstring& str);

	// 把浮点数转为字符串，默认只保留6位小数，结束的0会删除
	static tstring convert_to_tstring(double val);

	// 删除首尾两端的空白字符（空格、制表符、回车）
	static void trim_blank(tstring& str);

	// 字符串转换
	static std::string wstring_to_string(const std::wstring& str);
	static std::wstring string_to_wstring(const std::string& str);
	static std::string tstring_to_string(const tstring& str);
	static std::wstring tstring_to_wstring(const tstring& str);
	static tstring wstring_to_tstring(const std::wstring& str);
	static tstring string_to_tstring(const std::string& str);
};

private_string_tool::private_string_tool()
{
}

private_string_tool::~private_string_tool()
{
}

bool private_string_tool::is_begin_with(const tstring& str, const tstring& pre)
{
	if (str.empty() && pre.empty()) return true;
	if (str.empty() || pre.empty()) return false;
	if (str.size() < pre.size()) return false;

	tstring strSub = str.substr(0, pre.size());
	return (strSub == pre);
}

void private_string_tool::split_string(std::vector<tstring>& result, const tstring& text, const tstring split /*= _T(" ")*/)
{
	result.clear();
	if (_T("") == text) return;

	tstring tmp;
	for (const TCHAR& x : text)
	{
		if (tstring::npos == split.find_first_of(x))
		{
			tmp += x;
			continue;
		}

		// 第一个字符为分隔符认为之前有一个空串，在这也插入
		result.push_back(tmp);
		tmp = _T("");
	}

	// 把残留的是最后一个字符串加到结果中（有可能是个空串）
	// 最后一个字符为分隔符认为后面有一个空串，也会原样插入一个空串
	result.push_back(tmp);
}

std::wstring private_string_tool::string_to_wstring(const std::string& str)
{
	// 没有使用系统的API MultiByteToWideChar，这样移植性较好。
	// 字符串中的中文不会乱码，因为已经切换区域代码。

	std::string s(str.c_str());

	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "chs");

	size_t szDist = 0;
	if (0 != mbstowcs_s(&szDist, nullptr, 0, s.c_str(), s.length())) return L""; // 返回的长度已经算上结束符了
	if (szDist <= 0) return L"";

	wchar_t* pwstr = new wchar_t[szDist];
	memset(pwstr, 0, szDist);
	mbstowcs_s(&szDist, pwstr, szDist, s.c_str(), s.length());
	pwstr[szDist - 1] = '\0';
	wstring result(pwstr);
	safe_delete_array(pwstr);

	setlocale(LC_ALL, curLocale.c_str());

	return result;
}

std::string private_string_tool::wstring_to_string(const std::wstring& str)
{
	// 没有使用系统的API WideCharToMultiByte，这样移植性较好。
	// 字符串中的中文不会乱码，因为已经切换区域代码。

	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "chs");

	size_t szDist = 0;
	wcstombs_s(&szDist, nullptr, 0, str.c_str(), 0);
	if (szDist <= 0) return "";
	szDist += sizeof(char); // 长度再加一个结束符(应该可以不要，懒得查MSDN了，多一个稳妥点)

	char* pstr = new char[szDist];
	memset(pstr, 0, szDist);
	size_t szDistRes = 0;
	wcstombs_s(&szDistRes, pstr, szDist, str.c_str(), szDist);
	pstr[szDist - 1] = '\0'; // 最后一个字节为结束符
	string result(pstr);
	safe_delete_array(pstr);

	setlocale(LC_ALL, curLocale.c_str());

	return result;
}

std::string private_string_tool::tstring_to_string(const tstring& str)
{
	string result;

#	ifdef _UNICODE
	result = wstring_to_string(str);
#	else
	result = str;
#	endif

	return result;
}

std::wstring private_string_tool::tstring_to_wstring(const tstring& str)
{
	wstring result;

#	ifdef _UNICODE
	result = str;
#	else
	result = string_to_wstring(str);
#	endif

	return result;
}

tstring private_string_tool::wstring_to_tstring(const std::wstring& str)
{
	tstring result;

#	ifdef _UNICODE
	result = str;
#	else
	result = wstring_to_string(str);
#	endif

	return result;

}

tstring private_string_tool::string_to_tstring(const std::string& str)
{
	tstring result;

#	ifdef _UNICODE
	result = string_to_wstring(str);
#	else
	result = str;
#	endif

	return result;
}

void private_string_tool::replace_CHS_to_EN_in_scope_string(tstring& str)
{
	// 中文字符在双字节模式下占两个字符，所以先转为宽字符再替换，否则要加很多字符识别和组合的逻辑
	wstring str_des = tstring_to_wstring(str);

	for (auto& x : str_des)
	{
		if (L'　' == x) { x = L' '; continue; }
		if (L'，' == x) { x = L','; continue; }
		if (L'；' == x) { x = L';'; continue; }
		if (L'。' == x) { x = L'.'; continue; }
		if (L'［' == x) { x = L'['; continue; }
		if (L'］' == x) { x = L']'; continue; }
		if (L'（' == x) { x = L'('; continue; }
		if (L'）' == x) { x = L')'; continue; }
		if (L'【' == x) { x = L'['; continue; }
		if (L'】' == x) { x = L']'; continue; }
		if (L'０' == x) { x = L'0'; continue; }
		if (L'１' == x) { x = L'1'; continue; }
		if (L'２' == x) { x = L'2'; continue; }
		if (L'３' == x) { x = L'3'; continue; }
		if (L'４' == x) { x = L'4'; continue; }
		if (L'５' == x) { x = L'5'; continue; }
		if (L'６' == x) { x = L'6'; continue; }
		if (L'７' == x) { x = L'7'; continue; }
		if (L'８' == x) { x = L'8'; continue; }
		if (L'９' == x) { x = L'9'; continue; }
	}

	str = wstring_to_tstring(str_des);
}

tstring private_string_tool::convert_to_tstring(double val)
{
	TCHAR buffer[50] = _T("");
	_stprintf_s(buffer, _T("%lf"), val); // 默认6位小数
	buffer[49] = _T('\0'); // 为防止格式化错误时数据没有结束符，在末尾强制设置一个

	tstring str_val(buffer);
	size_t dot_pos = str_val.find_first_of(_T('.'));
	
	if (str_val.empty()) return _T("0");
	if (tstring::npos == dot_pos) return str_val;

	size_t zero_start_pos = str_val.find_last_not_of(_T(".0"), dot_pos); // 末尾的0全部去掉，如果只剩下一个小数点也去掉
	if (string::npos == zero_start_pos) return str_val;

	str_val.erase(zero_start_pos + 1, str_val.length() - zero_start_pos);

	return str_val;
}

void private_string_tool::trim_blank(tstring& str)
{
	if (str.empty()) return;

	size_t st = str.find_first_not_of(_T(" \t\n\r"));
	if (tstring::npos != st) str = str.substr(st);

	size_t en = str.find_last_not_of(_T(" \t\n\r"));
	if (tstring::npos != en) str = str.substr(0, en + 1);
}

//////////////////////////////////////////////////////////////////////////
bool operator<(double value, const number& num);
bool operator<(const number& num, double value);
bool operator>(double value, const number& num);
bool operator>(const number& num, double value);
bool operator>=(double value, const number& num);
bool operator>=(const number& num, double value);
bool operator<=(double value, const number& num);
bool operator<=(const number& num, double value);
bool operator!=(double value, const number& num);
bool operator!=(const number& num, double value);


const number operator+(const number& num, double value);
const number operator+(double value, const number& num);
const number operator+(const number& num1, const number& num2);
const number operator-(const number& num, double value);
const number operator-(double value, const number& num);
const number operator-(const number& num1, const number& num2);
const number operator*(const number& num, double value);
const number operator*(double value, const number& num);
const number operator*(const number& num1, const number& num2);
const number operator/(const number& num, double value);
const number operator/(double value, const number& num);
const number operator/(const number& num1, const number& num2);

//////////////////////////////////////////////////////////////////////////

number::number(double value /*= 0.0*/)
{
	set_value(value);
}

number::number(bool is_infinitely_great_or_small)
{
	set_value(is_infinitely_great_or_small);
}

number::number( const number& num )
{
	*this = num;
}

number::number( short value )
{
	set_value(value);
}

number::number( int value )
{
	set_value(value);
}

number::number( long value )
{
	set_value(value);
}

number::number( unsigned short value )
{
	set_value(value);
}

number::number( unsigned int value )
{
	set_value(value);
}

number::number( unsigned long value )
{
	set_value(value);
}

number::number( float value )
{
	set_value(value);
}

number::number()
{
	set_value(0.0);
}

void number::set_value(double value)
{
	m_type = 0;
	m_value = value;
}

void number::set_value(bool is_infinitely_great_or_small)
{
	if (is_infinitely_great_or_small)
		set_infinitely_p();
	else
		set_infinitely_n();
}

void number::set_value( const number& num )
{
	if (num.is_infinite_n())
	{
		m_value = 0.0;
		m_type = -1;
	}
	else if (num.is_infinite_p())
	{
		m_value = 0.0;
		m_type = 1;
	}
	else
	{
		m_value = num.get_value();
		m_type = 0;
	}
}

void number::set_value( short value )
{
	set_value((double)value);
}

void number::set_value( int value )
{
	set_value((double)value);
}

void number::set_value( long value )
{
	set_value((double)value);
}

void number::set_value( unsigned short value )
{
	set_value((double)value);
}

void number::set_value( unsigned int value )
{
	set_value((double)value);
}

void number::set_value( unsigned long value )
{
	set_value((double)value);
}

void number::set_value( float value )
{
	set_value((double)value);
}

void number::set_infinitely_p()
{
	m_value = 0.0;
	m_type = 1;
}

void number::set_infinitely_n()
{
	m_value = 0.0;
	m_type = -1;
}

bool number::is_infinite() const
{
	return is_infinite_n() || is_infinite_p();
}

bool number::is_value() const
{
	return 0 == m_type;
}

bool number::is_infinite_p() const
{
	return 1 == m_type;
}

bool number::is_infinite_n() const
{
	return -1 == m_type;
}

double number::get_value() const
{
	if (is_infinite_p())
		return DBL_MAX;
	else if (is_infinite_n())
		return -DBL_MAX; // 注意不是DBL_MIN;
	else
		return m_value;
}

bool operator<(double value, const number& num)
{
	return number(value) < num;
}

bool operator<(const number& num, double value)
{
	return num < number(value);
}

bool operator>(double value, const number& num)
{
	return number(value) > num;
}

bool operator>(const number& num, double value)
{
	return num > number(value);
}

bool operator>=(double value, const number& num)
{
	return number(value) >= num;
}

bool operator>=(const number& num, double value)
{
	return num >= number(value);
}

bool operator<=(double value, const number& num)
{
	return number(value) <= num;
}

bool operator<=(const number& num, double value)
{
	return num <= number(value);
}

bool operator!=(double value, const number& num)
{
	return number(value) != num;
}

bool operator!=(const number& num, double value)
{
	return num != number(value);
}

bool number::operator<(const number& num) const
{
	if (is_infinite_n() && num.is_infinite_n())
		return false;
	else if (is_infinite_n() && num.is_infinite_p())
		return true;
	else if (is_infinite_n() && num.is_value())
		return true;
	else if (is_infinite_p() && num.is_infinite_n())
		return false;
	else if (is_infinite_p() && num.is_infinite_p())
		return false;
	else if (is_infinite_p() && num.is_value())
		return false;
	else if (is_value() && num.is_infinite_n())
		return false;
	else if (is_value() && num.is_infinite_p())
		return true;
	else if (is_value() && num.is_value())
		return compare_double::is_less(m_value, num.get_value());
	else
		return false;
}

bool number::operator==(const number& num) const
{
	if (is_infinite_n() && num.is_infinite_n())
		return true;
	else if (is_infinite_n() && num.is_infinite_p())
		return false;
	else if (is_infinite_n() &&  num.is_value())
		return false;
	else if (is_infinite_p() && num.is_infinite_n())
		return false;
	else if (is_infinite_p() && num.is_infinite_p())
		return true;
	else if (is_infinite_p() && num.is_value())
		return false;
	else if (is_value() && num.is_infinite_n())
		return false;
	else if (is_value() && num.is_infinite_p())
		return false;
	else if (is_value() && num.is_value())
		return compare_double::is_equal(m_value, num.get_value());
	else
		return false;
}

bool number::operator>(const number& num) const
{
	return !(*this == num || *this < num);
}

bool number::operator>=(const number& num) const
{
	return !(*this < num);
}

bool number::operator<=(const number& num) const
{
	return (*this < num || *this == num);
}

bool number::operator!=(const number& num) const
{
	return !(*this == num);
}

bool number::operator!() const
{
	return (*this == number(0.0)); // 为0返回true，否则返回false
}

number& number::operator=(double value)
{
	set_value(value);

	return *this;
}

number& number::operator=(const number& num)
{
	set_value(num);

	return *this;
}

number::operator double() const
{
	return ( is_infinite() ? 0.0 : m_value);
}

number::~number()
{

}

std::string number::to_string_a() const
{
	return private_string_tool::wstring_to_string(to_string_w());
}

std::wstring number::to_string_w() const
{
	tstring text;
	if (is_infinite())
		text = _T("");
	else
		text = private_string_tool::convert_to_tstring(get_value());

	return private_string_tool::tstring_to_wstring(text);
}

const number operator+(const number& num, double value)
{
	return operator+(num, number(value));
}

const number operator+(double value, const number& num)
{
	return operator+(number(value), num);
}

const number operator+( const number& num1, const number& num2 )
{
	number numResutl;

	if (num1.is_infinite_p())
	{
		if (num2.is_infinite_n())
			numResutl.set_value(0.0);
		else
			numResutl.set_value(true);
	}
	else if (num1.is_infinite_n())
	{
		if (num2.is_infinite_p())
			numResutl.set_value(0.0);
		else
			numResutl.set_value(false);
	}
	else
	{
		if (num2.is_infinite_p())
			numResutl.set_value(true);
		else if (num2.is_infinite_n())
			numResutl.set_value(false);
		else
			numResutl.set_value(num1.get_value() + num2.get_value());
	}

	return numResutl;
}

const number operator-(const number& num, double value)
{
	return operator-(num, number(value));
}

const number operator-(double value, const number& num)
{
	return operator-(number(value), num);
}

const number operator-( const number& num1, const number& num2 )
{
	number numResutl;

	if (num1.is_infinite_p())
	{
		if (num2.is_infinite_p())
			numResutl.set_value(0.0);
		else
			numResutl.set_value(true);
	}
	else if (num1.is_infinite_n())
	{
		if (num2.is_infinite_n())
			numResutl.set_value(0.0);
		else
			numResutl.set_value(false);
	}
	else
	{
		if (num2.is_infinite_p())
			numResutl.set_value(false); // 减正无穷变成负无穷
		else if (num2.is_infinite_n())
			numResutl.set_value(true); // 减负无穷变成正无穷
		else
			numResutl.set_value(num1.get_value() - num2.get_value());
	}

	return numResutl;
}

const number operator*(const number& num, double value)
{
	return operator*(num, number(value));
}

const number operator*(double value, const number& num)
{
	return operator*(number(value), num);
}

const number operator*( const number& num1, const number& num2 )
{
	number num_result;

	if (num1.is_infinite_p())
	{
		if (num2.is_infinite_n())
			num_result.set_value(false); // 正无穷与负无穷乘积为负无穷
		else if (num2.is_infinite_p())
			num_result.set_value(true);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else if (compare_double::is_less(num2.get_value(), 0.0))
			num_result.set_value(false); // 正无穷与负数乘积为负无穷
		else
			num_result.set_value(true);
	}
	else if (num1.is_infinite_n())
	{
		if (num2.is_infinite_n())
			num_result.set_value(true); // 负无穷与负无穷乘积为正无穷
		else if (num2.is_infinite_p())
			num_result.set_value(false);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else if (compare_double::is_less(num2.get_value(), 0.0))
			num_result.set_value(true); // 负无穷与负数乘积为正无穷
		else
			num_result.set_value(false);
	}
	else if (compare_double::is_equal(num1.get_value(), 0.0))
	{
		num_result.set_value(0.0);
	}
	else if (compare_double::is_less(num1.get_value(), 0.0))
	{
		if (num2.is_infinite_n())
			num_result.set_value(true); // 负无穷与负数乘积为正无穷
		else if (num2.is_infinite_p())
			num_result.set_value(false);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else
			num_result.set_value(num1.get_value() * num2.get_value());
	}
	else
	{
		if (num2.is_infinite_n())
			num_result.set_value(false);
		else if (num2.is_infinite_p())
			num_result.set_value(true);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else
			num_result.set_value(num1.get_value() * num2.get_value());
	}

	return num_result;
}

const number operator/(const number& num, double value)
{
	return operator/(num, number(value));
}

const number operator/(double value, const number& num)
{
	return operator/(number(value), num);
}

const number operator/( const number& num1, const number& num2 )
{
	number num_result;

	if (num1.is_infinite_p())
	{
		if (num2.is_infinite_n())
			num_result.set_value(-1.0);
		else if (num2.is_infinite_p())
			num_result.set_value(1.0);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else if (compare_double::is_less(num2.get_value(), 0.0))
			num_result.set_value(false);
		else
			num_result.set_value(true);
	}
	else if (num1.is_infinite_n())
	{
		if (num2.is_infinite_n())
			num_result.set_value(1.0);
		else if (num2.is_infinite_p())
			num_result.set_value(-1.0);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else if (compare_double::is_less(num2.get_value(), 0.0))
			num_result.set_value(true);
		else
			num_result.set_value(false);
	}
	else if (compare_double::is_equal(num1.get_value(), 0.0))
	{
		num_result.set_value(0.0);
	}
	else if (compare_double::is_less(num1.get_value(), 0.0))
	{
		if (num2.is_infinite_n())
			num_result.set_value(0.0); // 负无穷与负数乘积为正无穷
		else if (num2.is_infinite_p())
			num_result.set_value(-0.0);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else
			num_result.set_value(num1.get_value() / num2.get_value());
	}
	else
	{
		if (num2.is_infinite_n())
			num_result.set_value(-0.0);
		else if (num2.is_infinite_p())
			num_result.set_value(0.0);
		else if (compare_double::is_equal(num2.get_value(), 0.0))
			num_result.set_value(0.0);
		else
			num_result.set_value(num1.get_value() / num2.get_value());
	}

	return num_result;
}

//////////////////////////////////////////////////////////////////////////

scope_value::scope_value()
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(L"");
}

scope_value::scope_value( number val )
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(val);
}

scope_value::scope_value(const std::string& left, number l_val, number r_val, const std::string& right)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(left, l_val, r_val, right);
}

scope_value::scope_value(const std::string& text)
{
	set_value(text);

	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false,为防止其它函数修改,放在最后
	m_old_ui_string = private_string_tool::string_to_wstring(text);
}

scope_value::scope_value( double value )
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(number(value));
}

scope_value::scope_value(bool is_open_left, const number& l_val, const number& r_val, bool is_open_right)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(is_open_left, l_val, r_val, is_open_right);
}

scope_value::scope_value(const std::wstring& text)
{
	set_value(text);

	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false,为防止其它函数修改,放在最后
	m_old_ui_string = text;
}

scope_value::scope_value(const std::wstring& left, number l_val, number r_val, const std::wstring& right)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(left, l_val, r_val, right);
}

scope_value::scope_value(const char* text)
{
	string test_string(text);
	set_value(test_string);

	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false,为防止其它函数修改,放在最后
	m_old_ui_string = private_string_tool::string_to_wstring(test_string);
}

scope_value::scope_value(const wchar_t* text)
{
	wstring test_string(text);
	set_value(test_string);

	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false,为防止其它函数修改,放在最后
	m_old_ui_string = test_string;
}

scope_value::scope_value(const char* left, number l_val, number r_val, const char* right)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(string(left), l_val, r_val, string(right));
}

scope_value::scope_value(const wchar_t* left, number l_val, number r_val, const wchar_t* right)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(wstring(left), l_val, r_val, wstring(right));
}

scope_value::scope_value(short value)
{
	set_value(value);
}

scope_value::scope_value(int value)
{
	set_value(value);
}

scope_value::scope_value(long value)
{
	set_value(value);
}

scope_value::scope_value(unsigned short value)
{
	set_value(value);
}

scope_value::scope_value(unsigned int value)
{
	set_value(value);
}

scope_value::scope_value(unsigned long value)
{
	set_value(value);
}

scope_value::scope_value(float value)
{
	set_value(value);
}

scope_value::scope_value(bool is_infinitely_great_or_small)
{
	set_value(is_infinitely_great_or_small);
}

scope_value::~scope_value()
{
}

void scope_value::set_value( const number& val )
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	m_left = val;
	m_right = val;
	m_is_single_value = true;
	m_is_left_open = false;
	m_is_right_open = false;
}

void scope_value::set_value(const std::string& left, number l_val, number r_val, const std::string& right)
{
	set_value(private_string_tool::string_to_wstring(left), l_val, r_val, private_string_tool::string_to_wstring(right));
}

void scope_value::set_value(const std::wstring& left, number l_val, number r_val, const std::wstring& right)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	tstring new_left = private_string_tool::wstring_to_tstring(left);
	tstring new_right = private_string_tool::wstring_to_tstring(right);
	private_string_tool::replace_CHS_to_EN_in_scope_string(new_left);
	private_string_tool::replace_CHS_to_EN_in_scope_string(new_right);

	if (l_val == r_val)
	{
		set_value(l_val); // 设置为单个值
	}
	else
	{
		set_value(new_left, l_val);
		set_value(r_val, new_right);
		m_is_single_value = false;
	}
}

void scope_value::set_value(const std::string& left, const number& l_val)
{
	set_value(private_string_tool::string_to_wstring(left), l_val);
}

void scope_value::set_value(const std::wstring& left, const number& l_val)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	tstring new_left = private_string_tool::wstring_to_tstring(left);
	private_string_tool::replace_CHS_to_EN_in_scope_string(new_left);

	m_left = l_val;
	if (new_left.empty())
		set_value(true, l_val);
	else
		set_value(_T('[') != new_left.front(), l_val);
}

void scope_value::set_value(const number& r_val, const std::string& right)
{
	set_value(r_val, private_string_tool::string_to_wstring(right));
}

void scope_value::set_value(const number& r_val, const std::wstring& right)
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	tstring new_right = private_string_tool::wstring_to_tstring(right);
	private_string_tool::replace_CHS_to_EN_in_scope_string(new_right);

	m_right = r_val;
	if (new_right.empty())
		set_value(r_val, true);
	else
		set_value(r_val, _T(']') != new_right.front());
}

void scope_value::set_value( double value )
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	set_value(number(value));
}

void scope_value::set_value( bool is_open_left, const number& l_val, const number& r_val, bool is_open_right )
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	if (l_val == r_val)
	{
		set_value(l_val); // 设置为单个值
	}
	else
	{
		set_value(is_open_left, l_val);
		set_value(r_val, is_open_right);
		m_is_single_value = false;
	}
}

void scope_value::set_value( bool is_open_left, const number& l_val )
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	m_is_left_open = is_open_left;
	m_left = l_val;
}

void scope_value::set_value( const number& r_val, bool is_open_right )
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	m_is_right_open = is_open_right;
	m_right = r_val;
}

void scope_value::set_value(const std::string& strText)
{
	set_value(private_string_tool::string_to_wstring(strText));
}

void scope_value::set_value(const std::wstring& strText)
{
	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false,为防止其它函数修改,放在最后
	m_old_ui_string = strText;

	tstring text_copy = private_string_tool::wstring_to_tstring(strText);
	private_string_tool::replace_CHS_to_EN_in_scope_string(text_copy);
	private_string_tool::trim_blank(text_copy);

	size_t split_pos = text_copy.find_first_of(_T(" ,")); // 空格和逗号都可以作为左、右区间的分隔符
	if (tstring::npos == split_pos)
	{
		// 没有找到时分两种情况，为空表示负，则作为定值处理
		if (text_copy.empty())
		{
			set_value(_T("("), number(false), number(true), _T(")"));
		}
		else
		{
			if (_T("+") == text_copy) set_value(number(true));
			else if (_T("-") == text_copy) set_value(number(false));
			else set_value(number(_tstof(text_copy.c_str())));
		}
	}
	else
	{
		// 拆分左、右区间
		tstring left = text_copy.substr(0, split_pos);
		tstring right = text_copy.substr(split_pos + 1);

		private_string_tool::trim_blank(left);
		private_string_tool::trim_blank(right);

		// 去括号并处理区间开、闭的情况
		tstring left_kh = _T("(");
		tstring right_kh = _T(")");

		if (!left.empty())
		{
			if (_T('[') == left.front() || _T('(') == left.front()
				|| _T(')') == left.front() || _T(']') == left.front())
			{
				if (_T('[') == left.front() || _T(']') == left.front()) left_kh = _T('[');
				left.erase(0, 1);
				private_string_tool::trim_blank(left);
			}
		}

		if (!right.empty())
		{
			if (_T('[') == right.back() || _T('(') == right.back()
				|| _T(')') == right.back() || _T(']') == right.back())
			{
				if (_T('[') == right.back() || _T(']') == right.back()) right_kh = _T(']');
				right.erase(right.length() - 1, 1);
				private_string_tool::trim_blank(right);
			}
		}

		// 生成左、右区间值
		number numLeft;
		number numRight;

		if (left.empty() || _T("-") == left)
			numLeft.set_value(number(false));
		else
			numLeft.set_value(number(_tstof(left.c_str())));

		if (right.empty() || _T("+") == right)
			numRight.set_value(number(true));
		else
			numRight.set_value(number(_tstof(right.c_str())));

		// 生成区间
		set_value(left_kh, numLeft, numRight, right_kh);
	}
}

void scope_value::set_value(const char* text)
{
	set_value(string(text));
}

void scope_value::set_value(const wchar_t* text)
{
	set_value(wstring(text));
}

void scope_value::set_value(const number& r_val, const char* right)
{
	set_value(r_val, string(right));
}

void scope_value::set_value(const number& r_val, const wchar_t* right)
{
	set_value(r_val, wstring(right));
}

void scope_value::set_value(const char* left, const number& l_val)
{
	set_value(string(left), l_val);
}

void scope_value::set_value(const wchar_t* left, const number& l_val)
{
	set_value(wstring(left), l_val);
}

void scope_value::set_value(const char* left, number l_val, number r_val, const char* right)
{
	set_value(string(left), l_val, r_val, string(right));
}

void scope_value::set_value(const wchar_t* left, number l_val, number r_val, const wchar_t* right)
{
	set_value(wstring(left), l_val, r_val, wstring(right));
}

void scope_value::set_value(short value)
{
	set_value(number(value));
}

void scope_value::set_value(int value)
{
	set_value(number(value));
}

void scope_value::set_value(long value)
{
	set_value(number(value));
}

void scope_value::set_value(unsigned short value)
{
	set_value(number(value));
}

void scope_value::set_value(unsigned int value)
{
	set_value(number(value));
}

void scope_value::set_value(unsigned long value)
{
	set_value(number(value));
}

void scope_value::set_value(float value)
{
	set_value(number(value));
}

void scope_value::set_value(bool is_infinitely_great_or_small)
{
	set_value(number(is_infinitely_great_or_small));
}

void scope_value::get_left_value(number& l_val, bool& is_open) const
{
	l_val = m_left;
	is_open = m_is_left_open;
}

const number scope_value::get_left_value() const
{
	bool is_open = false;

	number l_val;
	get_left_value(l_val, is_open);

	return l_val;
}

void scope_value::get_right_value( number& r_val, bool& is_open ) const
{
	r_val = m_right;
	is_open = m_is_right_open;
}

const number scope_value::get_right_value() const
{
	bool is_open = false;

	number r_val;
	get_right_value(r_val, is_open);

	return r_val;
}

bool scope_value::is_open( bool is_left /*= true*/ ) const
{
	if (is_left)
	{
		if (m_left.is_infinite())
			return true;
		else
			return m_is_left_open;
	}
	else
	{
		if (m_right.is_infinite())
			return true;
		else
			return m_is_right_open;
	}
}

bool scope_value::is_closed( bool is_left /*= true*/ ) const
{
	return !is_open(is_left);
}

bool scope_value::is_in_scope(const number& val ) const
{
	if (m_is_single_value) return (m_left == val);

	bool is_left = false;
	if (m_is_left_open)
		is_left = (val > m_left);
	else
		is_left = (val >= m_left);

	bool bRight = false;
	if (m_is_right_open)
		bRight = (val < m_right);
	else
		bRight = (val <= m_right);

	return is_left && bRight;	
}

bool scope_value::is_in_scope( const scope_value& scope ) const
{
	if (&scope == this) return true;

	if (is_single_value())
	{
		if (scope.is_single_value())
		{
			return (get_single_value() == scope.get_single_value());
		}
		else
		{
			bool is_open_left = false;
			bool is_open_right = false;
			number numLeft;
			number numRight;

			scope.get_left_value(numLeft, is_open_left);
			scope.get_right_value(numRight, is_open_right);

			// 为区间时要看区间是否两端值相同且均为闭区间,是则返回true,否则返回false(因为区间不可能包含在数值内)
			return (numLeft == numRight && false == is_open_left && false == is_open_right);
		}
	}
	else
	{
		if (scope.is_single_value())
		{
			return is_in_scope(scope.get_single_value());
		}
		else
		{
			// 两个都是区间
			number left;
			bool is_open_left = false;
			number right;
			bool is_open_right = false;

			scope.get_left_value(left, is_open_left);
			scope.get_right_value(right, is_open_right);

			// IsInScope只能判断具体的数值,相当是传一个闭区间的一个端点,当把一个开区间的端点传给这个
			// 函数,并且两个区间的端点值相同,但均为开区间,此时就会计算错误,所以要提前把参数所在的区间
			// 为开区间的情况处理掉才行
			
			bool check_left = false;
			bool check_right = false;

			if (left == get_left_value() && true == is_open_left) // 相等时开区间肯定会被包含在内,不用两边都判断是否为开区间
				check_left = true;
			else
				check_left = is_in_scope(left);

			if (right == get_right_value() && true == is_open_right) // 相等时开区间肯定会被包含在内,不用两边都判断是否为开区间
				check_right = true;
			else
				check_right = is_in_scope(right);

			return (check_left && check_right);
		}
	}

	// warning C4702: unreachable code，上面每条路径都有返回值了，为屏蔽警告特注释掉，以后维护上面的代码时要小心不要漏了某条路径的返回值
	// return false;
}

int scope_value::get_scope_status() const
{
	if (m_is_single_value)
		return -1;
	else if (m_left < m_right)
		return 1;
	else
		return 0;
}

std::string scope_value::to_string_a() const
{
	return private_string_tool::wstring_to_string(to_string_w());
}

std::wstring scope_value::to_string_w() const
{
	wstring text;

	if (m_is_old_ui_string)
	{
		text = m_old_ui_string;
	}
	else
	{
		if (m_is_single_value)
		{
			text = m_left.to_string_w();
		}
		else
		{
			text += (m_is_left_open ? L"(" : L"[");
			text += m_left.to_string_w() + L"," + m_right.to_string_w();
			text += (m_is_right_open ? L")" : L"]");
		}
	}

	return text;
}

bool scope_value::is_single_value() const
{
	return m_is_single_value;
}

void scope_value::get_single_value( number& val ) const
{
	val = m_left;
}

const number scope_value::get_single_value() const
{
	number num;
	get_single_value(num);
	return num;
}

bool scope_value::operator==( const scope_value& scope ) const
{
	bool bR = false;

	if (m_is_single_value)
	{
		bR = ( scope.is_single_value() && scope.get_left_value() == m_left );
	}
	else
	{
		bR = ( false == scope.is_single_value()
			&& scope.get_left_value() == m_left
			&& scope.get_right_value() == m_right
			&& scope.is_open(true) == m_is_left_open
			&& scope.is_open(false) == m_is_right_open);
	}

	return bR;
}

bool scope_value::operator!=( const scope_value& assem ) const
{
	return !(*this == assem);
}

void scope_value::set_left_value( bool is_open_left, const number& l_val )
{
	set_value(is_open_left, l_val);
}

void scope_value::set_left_value(const std::string& left, const number& l_val)
{
	set_value(left, l_val);
}

void scope_value::set_left_value(const std::wstring& left, const number& l_val)
{
	set_value(left, l_val);
}

void scope_value::set_left_value(const char* left, const number& l_val)
{
	set_left_value(string(left), l_val);
}

void scope_value::set_left_value(const wchar_t* left, const number& l_val)
{
	set_left_value(wstring(left), l_val);
}

void scope_value::set_right_value(const number& r_val, bool is_open_right)
{
	set_value(r_val, is_open_right);
}

void scope_value::set_right_value(const number& r_val, const std::string& right)
{
	set_value(r_val, right);
}

void scope_value::set_right_value(const number& r_val, const std::wstring& right)
{
	set_value(r_val, right);
}

void scope_value::set_right_value(const number& r_val, const char* right)
{
	set_right_value(r_val, string(right));
}

void scope_value::set_right_value(const number& r_val, const wchar_t* right)
{
	set_right_value(r_val, wstring(right));
}

bool scope_value::is_old_ui_string() const
{
	return m_is_old_ui_string;
}

std::wstring scope_value::get_old_ui_string() const
{
	return m_old_ui_string;
}

bool scope_value::split_scope( const number& val, bool is_close, scope_value& out_left, scope_value& out_right ) const
{
	// 单值时特殊处理
	if (is_single_value())
	{
		number num = get_single_value();
		if ( num != val) return false;

		if (false == is_close) return false;

		out_left.set_value(num);
		out_right.set_value(num);

		return true;
	}

	// 按区间处理

	if (0 == get_scope_status()) return false; // 区间错误
	if (false == is_in_scope(val)) return false; // 不在区间内

	scope_value left = *this;
	left.set_right_value(val, !is_close);
	
	// 检查是否是[6, 6]这种特殊的区间，是则置为单值
	bool is_open_left = false;
	bool is_open_right = false;
	number numLeft;
	number numRight;

	left.get_left_value(numLeft, is_open_left);
	left.get_right_value(numRight, is_open_right);

	if (numLeft == numRight)
	{
		if (false == is_open_left && false == is_open_right)
			left.set_value(numLeft);
		else
			return false;
	}

	// 最后检查区间构成是否合理
	if (0 == left.get_scope_status()) return false;


	// 同样的方式处理第二个区间
	scope_value right = *this;
	right.set_right_value(val, !is_close);

	right.get_left_value(numLeft, is_open_left);
	right.get_right_value(numRight, is_open_right);

	if (numLeft == numRight)
	{
		if (false == is_open_left && false == is_open_right)
			right.set_value(numLeft);
		else
			return false;
	}

	// 生成的区间无法满足数学要求
	if (0 == right.get_scope_status()) return false;

	
	// 正常执行完成
	out_left = left;
	out_right = right;

	return true;
}

bool scope_value::merge_scope_value( const number& val )
{
	return merge_scope_value(scope_value(val));
}

bool scope_value::merge_scope_value( const scope_value& val )
{
	scope_value item_copy = *this; // 保留一个副本

	if (0 == val.get_scope_status()) return false;

	if (is_single_value() && val.is_single_value())
	{
		if (*this == val) return true; // 相等不用合并
	}
	else if (is_single_value() && false == val.is_single_value())
	{
		if (val.is_in_scope(*this))
		{
			*this = val;
			return true; // 已包含，不用合并
		}

		// 检测是否能连续成一个区间,此时将开区间改为半区间

		number num;
		bool is_open = false;
		val.get_left_value(num, is_open);

		if (true == is_open && num == get_single_value())
		{
			*this = val;
			set_left_value(false, num);
			return true;
		}

		val.get_right_value(num, is_open);
		if (true == is_open && num == get_single_value())
		{
			*this = val;
			set_right_value(num, false);
			return true;
		}
	}
	else if (false == is_single_value() && val.is_single_value())
	{
		if (is_in_scope(val)) return true; // 已包含，不用合并

		// 检测是否能连续成一个区间,此时将开区间改为半区间

		number num;
		bool is_open = false;
		get_left_value(num, is_open);

		if (true == is_open && num == val.get_single_value())
		{
			set_left_value(false, num);
			return true;
		}

		get_right_value(num, is_open);
		if (true == is_open && num == val.get_single_value())
		{
			set_right_value(num, false);
			return true;
		}
	}
	else
	{
		if (*this == val) return true; // 不用合并
		
		// 错误的区间数据已经在前面检测并删除了
		bool is_open_left_01 = false;
		bool is_open_left_02 = false;
		bool is_open_right_01 = false;
		bool is_open_right_02 = false;

		number left01;
		number left02;
		number right01;
		number right02;

		get_left_value(left01, is_open_left_01);
		get_right_value(right01, is_open_right_01);
		val.get_left_value(left02, is_open_left_02);
		val.get_right_value(right02, is_open_right_02);

		// 判断两个区间是否有公共部分
		bool is_ok = false;

		// 不有只判断个区间，两个都要互相判断，以防一个区间值为开另一个为闭时不会漏掉，这样就能判断
		// 出两区间是否有公共部分或者没有公共部分但能相连起来的情况
		if (is_in_scope(left02) || is_in_scope(right02) || val.is_in_scope(left01) || val.is_in_scope(right01))
			is_ok = true;

		if (false == is_ok) return false; // 没有公共部分或者没有能相连的端点不能合并

		// 求取合并后的两端点值及开闭情况
		bool has_result_left = false;
		number numResultLeft;

		if (left01 < left02)
		{
			has_result_left = is_open_left_01;
			numResultLeft = left01;			
		}
		else if (left01 > left02)
		{
			has_result_left = is_open_left_02;
			numResultLeft = left02;			
		}
		else
		{
			has_result_left = (false == is_open_left_01 || false == is_open_left_02 ? false : true); // 两值一样但可能开闭情况不一样，优先取闭区间
			numResultLeft = left01; // 两值一样任取一值
		}


		bool has_result_right = false;
		number numResultRight;

		if (right01 < right02)
		{
			has_result_right = is_open_right_02;
			numResultRight = right02;			
		}
		else if (right01 > right02)
		{
			has_result_right = is_open_right_01;
			numResultRight = right01;			
		}
		else
		{
			has_result_right = (false == is_open_right_01 || false == is_open_right_02 ? false : true); // 两值一样但可能开闭情况不一样，优先取闭区间
			numResultRight = is_open_right_01; // 两值一样任取一值
		}

		set_value(has_result_left, numResultLeft, numResultRight, has_result_right);
		
		return true; // 合并成功
	}

	// 无法合并，保持原值不变
	*this = item_copy; // 恢复原值

	return false;
}

assemblage_value::assemblage_value()
{
	set_null();

	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

assemblage_value::assemblage_value( const assemblage_value& assem )
{
	*this = assem;
}

assemblage_value::assemblage_value(const std::string& text)
{
	set_value(text);

	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false,为防止其它函数修改,放在最后
	m_old_ui_string = private_string_tool::string_to_wstring(text);
}

assemblage_value::assemblage_value(const number& val)
{
	set_value(val);
	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

assemblage_value::assemblage_value(const std::wstring& text)
{
	set_value(text);

	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false,为防止其它函数修改,放在最后
	m_old_ui_string = text;
}

assemblage_value::assemblage_value(double value)
{
	set_value(value);
	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

assemblage_value::assemblage_value(const char* text)
{
	set_value(text);
	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

assemblage_value::assemblage_value(const scope_value& val)
{
	set_value(val);
	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

assemblage_value::assemblage_value(const wchar_t* text)
{
	set_value(text);
	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

assemblage_value::assemblage_value(short value)
{
	set_value(value);
}

assemblage_value::assemblage_value(int value)
{
	set_value(value);
}

assemblage_value::assemblage_value(long value)
{
	set_value(value);
}

assemblage_value::assemblage_value(unsigned short value)
{
	set_value(value);
}

assemblage_value::assemblage_value(unsigned int value)
{
	set_value(value);
}

assemblage_value::assemblage_value(unsigned long value)
{
	set_value(value);
}

assemblage_value::assemblage_value(float value)
{
	set_value(value);
}

assemblage_value::assemblage_value(bool is_infinitely_great_or_small)
{
	set_value(is_infinitely_great_or_small);
}

void assemblage_value::set_value(const std::string& text)
{
	set_value(private_string_tool::string_to_wstring(text));
}

void assemblage_value::set_value(const std::wstring& text)
{
	m_is_old_ui_string = true; // 每次Initialize()及用字符串构件时就把它记为true,一旦改动就变为false
	m_old_ui_string = text;

	m_all_scope.clear();
	
	tstring assem_no_const = private_string_tool::wstring_to_tstring(text);
	private_string_tool::replace_CHS_to_EN_in_scope_string(assem_no_const);
	private_string_tool::trim_blank(assem_no_const);

	// 忽略末尾最后一个分号或逗号（多个分号或逗号时时仅删除一个，其余的按分隔符解析）
	if (!assem_no_const.empty())
	{
		if (_T(';') == assem_no_const.back() || _T(',') == assem_no_const.back())
		{
			assem_no_const.erase(assem_no_const.length() - 1, 1);
			private_string_tool::trim_blank(assem_no_const); // 去除分号后仍要再除一次空白字符,以防分号前有空格
		}
	}

	if (assem_no_const.empty())
	{
		m_all_scope.push_back(scope_value(_T(""))); // 空表示处理成负无穷到正无穷
	}
	else
	{
		vector<tstring> all_assem;
		private_string_tool::split_string(all_assem, assem_no_const, _T(";"));

		for (tstring item_copy : all_assem) // 使用对象，不是引用，目的是内部要对两端的空白进行修剪
		{
			private_string_tool::trim_blank(item_copy);
			m_all_scope.push_back(scope_value(private_string_tool::tstring_to_wstring(item_copy)));
		}
	}

	arrange();
}

void assemblage_value::set_value(const char* text)
{
	set_value(string(text));
}

void assemblage_value::set_value(const wchar_t* text)
{
	set_value(wstring(text));
}

const std::string assemblage_value::to_string_a() const
{
	return private_string_tool::wstring_to_string(to_string_w());
}

const std::wstring assemblage_value::to_string_w() const
{
	wstring strResult;

	if (m_is_old_ui_string)
	{
		strResult = m_old_ui_string;
	}
	else
	{
		for (size_t i = 0; i < m_all_scope.size(); ++i)
		{
			strResult += m_all_scope[i].to_string_w();
			if (i < m_all_scope.size() - 1)
				strResult += L";";
		}
	}

	return strResult;
}

void assemblage_value::set_null()
{
	m_is_old_ui_string = false;
	m_old_ui_string = L"";
	m_all_scope.clear();
}

bool assemblage_value::is_null() const
{
	// 区间不可能存在空区间，空区间表示负无穷到正无穷，所以只要有元素就
	// 表示有非空区间，只有集合才存在空集合的说法
	return m_all_scope.empty();
}

bool assemblage_value::operator==( const assemblage_value& assem ) const
{
	return is_subset(assem) && assem.is_subset(*this);
}

bool assemblage_value::operator!=( const assemblage_value& assem ) const
{
	return !(*this == assem);
}

assemblage_value::~assemblage_value()
{
}

void assemblage_value::set_value( const number& val )
{
	set_null();
	union_set(val);

	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

void assemblage_value::set_value( double value )
{
	set_value(number(value));
}

void assemblage_value::set_value(short value)
{
	set_value(number(value));
}

void assemblage_value::set_value(int value)
{
	set_value(number(value));
}

void assemblage_value::set_value(long value)
{
	set_value(number(value));
}

void assemblage_value::set_value(unsigned short value)
{
	set_value(number(value));
}

void assemblage_value::set_value(unsigned int value)
{
	set_value(number(value));
}

void assemblage_value::set_value(unsigned long value)
{
	set_value(number(value));
}

void assemblage_value::set_value(float value)
{
	set_value(number(value));
}

void assemblage_value::set_value(bool is_infinitely_great_or_small)
{
	set_value(number(is_infinitely_great_or_small));
}

void assemblage_value::set_value( const scope_value& val )
{
	set_null();
	union_set(val); // 内部会调用Arrange()

	m_is_old_ui_string = false;
	m_old_ui_string = L"";
}

void assemblage_value::set_value( const assemblage_value& value )
{
	*this = value;
	arrange();
}

void assemblage_value::set_value( const std::vector<scope_value>& scope_items )
{
	if (scope_items.empty()) set_null();

	m_all_scope = scope_items;
	m_is_old_ui_string = false;
	m_old_ui_string = L"";
	arrange();
}

assemblage_value& assemblage_value::intersection(const number& num)
{
	for (size_t i = 0; i < m_all_scope.size(); /* Nothing */)
	{
		if (m_all_scope[i].is_in_scope(num))
		{
			m_all_scope.clear();
			m_all_scope.push_back(scope_value(num));
			break; // 只留下一个就可以了
		}
		else
		{
			// 没有交集则删除了
			m_all_scope.erase(m_all_scope.begin() + i);
		}
	}

	m_is_old_ui_string = false;
	m_old_ui_string = L"";
	
	return *this;
}

// 返回两个区间的公共区间，如果没有公共区间，则返回false，否则返回true
bool GetIntersectionOfTwoScope(const scope_value& first, const scope_value& second, scope_value& result)
{
	if (0 == first.get_scope_status() || 0 == second.get_scope_status()) return false;

	// 为单元的数值时要特殊处理,因为开闭情况对单值无效,反而会影响判断结果
	// 为区间时,不论任何情况,都按左端取较大,右端取较小的原则
	
	// 处理单值情况
	if (first.is_single_value())
	{
		if (second.is_in_scope(first.get_single_value()))
		{
			result.set_value(first.get_single_value());
			return true;
		}
		else
		{
			return false;
		}
	}

	if (second.is_single_value())
	{
		if (first.is_in_scope(second.get_single_value()))
		{
			result.set_value(second.get_single_value());
			return true;
		}
		else
		{
			return false;
		}
	}

	// 处理区间情况

	number left01;
	bool is_left_open_01 = false;
	first.get_left_value(left01, is_left_open_01);

	number right01;
	bool is_right_open_01 = false;
	first.get_right_value(right01, is_right_open_01);

	number left02;
	bool is_left_open_02 = false;
	second.get_left_value(left02, is_left_open_02);

	number right02;
	bool is_right_open_02 = false;
	second.get_right_value(right02, is_right_open_02);

	number left_result;
	bool has_left_open_result = false;

	number rightResult;
	bool has_right_open_result = false;

	
	// 左区间
	if (left01 < left02)
	{
		left_result = left02;
		has_left_open_result = is_left_open_02;
	}
	else if (left01 > left02)
	{
		left_result = left01;
		has_left_open_result = is_left_open_01;
	}
	else
	{
		left_result = left01;

		if (is_left_open_01 == is_left_open_02)
			has_left_open_result = is_left_open_01;
		else
			has_left_open_result = true; // 求交集要优先按开区间处理（不包含端点）(求并集则优先按闭区间处理)
	}

	// 右区间
	if (right01 < right02)
	{
		rightResult = right01;
		has_right_open_result = is_right_open_01;
	}
	else if (right01 > right02)
	{
		rightResult = right02;
		has_right_open_result = is_right_open_02;
	}
	else
	{
		rightResult = right01;

		if (is_right_open_01 == is_right_open_02)
			has_right_open_result = is_right_open_01;
		else
			has_right_open_result = true; // 求交集要优先按开区间处理（不包含端点）(求并集则优先按闭区间处理
	}

	scope_value sc(has_left_open_result, left_result, rightResult, has_right_open_result);
	if (0 == sc.get_scope_status()) return false;

	// 因为前面仅按左取大,右取小的原则来生成区间, 但有可能本身两个区间就没有交集,那结果就错了,所以
	// 此处还要验证对结果进行验证，正确后再赋值，以防非法数据生成错误结构的区间
	if (false == first.is_in_scope(sc) || false == second.is_in_scope(sc)) return false;

	result = sc;

	return true;
}

assemblage_value& assemblage_value::intersection(const scope_value& scope)
{
	// 全部的交集都找到了再整理，不要找到一处就返回，因为可能每个区间的交集的大小不一样
	for (size_t i = 0; i < m_all_scope.size(); /* Nothing */)
	{
		scope_value tmp;
		if (GetIntersectionOfTwoScope(m_all_scope[i], scope, tmp))
		{
			m_all_scope[i] = tmp; // 有次则改为交集
			++i;
		}
		else
		{
			// 没有交集则删除了
			m_all_scope.erase(m_all_scope.begin() + i);
		}
	}

	arrange();

	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	return *this;
}

assemblage_value& assemblage_value::intersection(const assemblage_value& scope)
{
	if (this == &scope) return *this;

	vector<scope_value> vecResult;
	for (size_t i = 0; i < m_all_scope.size(); ++i)
	{
		// 不能在循环中调用参数为区间的求交集的重载版本,因为它内部会删除元素,会造成此处越界
		for (size_t j = 0; j < scope.m_all_scope.size(); ++j)
		{
			scope_value tmp;
			if (GetIntersectionOfTwoScope(m_all_scope[i], scope.m_all_scope[j], tmp))
				vecResult.push_back(tmp);
		}
	}

	m_all_scope = vecResult;

	arrange();

	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	return *this;
}

assemblage_value& assemblage_value::union_set(const number& num)
{
	scope_value sc(num);
	m_all_scope.push_back(sc);

	arrange();
	
	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	return *this;
}

assemblage_value& assemblage_value::union_set(const scope_value& scope)
{
	m_all_scope.push_back(scope);

	arrange();

	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	return *this;
}

assemblage_value& assemblage_value::union_set(const assemblage_value& scope)
{
	if (this == &scope) return *this;

	for (size_t i = 0; i < scope.m_all_scope.size(); ++i)
		m_all_scope.push_back(scope.m_all_scope[i]);

	arrange();

	m_is_old_ui_string = false;
	m_old_ui_string = L"";

	return *this;
}

bool assemblage_value::has_intersection(const number& num) const
{
	assemblage_value tmp_assem = *this;
	tmp_assem.intersection(num);

	return (false == tmp_assem.is_null());
}

bool assemblage_value::has_intersection(const scope_value& scope) const
{
	assemblage_value tmp_assem = *this;
	tmp_assem.intersection(scope);

	return (false == tmp_assem.is_null());
}

bool assemblage_value::has_intersection(const assemblage_value& assem) const
{
	assemblage_value tmp_assem = *this;
	tmp_assem.intersection(assem);

	return (false == tmp_assem.is_null());
}

bool assemblage_value::is_subset(const assemblage_value& assem) const
{
	if (this == &assem) return true;

	assemblage_value tmp_assem_01 = *this;
	assemblage_value tmp_assem_02 = assem;

	// 确保内部的所有区间没有重叠、没有间断（能连在一起的区间必须合并，而不是说所有区间都没有间断）
	tmp_assem_01.arrange();
	tmp_assem_02.arrange();

	for (size_t i = 0; i < tmp_assem_01.m_all_scope.size(); ++i)
	{
		bool bIn = false;
		for (size_t j = 0; j < tmp_assem_02.m_all_scope.size(); ++j)
		{
			if (tmp_assem_02.m_all_scope[i].is_in_scope(tmp_assem_01.m_all_scope[j])) // 02包含01
			{
				bIn = true;
				break;
			}
		}

		if (false == bIn) return false;
	}

	return true;
}

bool assemblage_value::is_superset(const assemblage_value& assem) const
{
	if (this == &assem) return true;

	return (assem.is_subset(*this)); // 不得写成false == IsSubset(assem); 否则可能是子集，也可能不是子集，但有交集
}

void assemblage_value::arrange()
{
	// 调整一次之后并不能知道是否已经完成，因为有可能第一次调整时两个区间没有关系，均被
	// 保留下来，但一次整理后原先没有关系的两个区间又有关系了，所以必须要不断的整理，直
	// 到没有任何区间发生过合并为止。
	
	bool be_arranged = false; // 执行过程中有元素发生过变化则返回false，没有元素发生变化说明已经整理好
	
	while(false == be_arranged)
	{
		be_arranged = true; // 先认为本次操作已经整理好，后面有变动再改为false

		for (size_t i = 0; i < m_all_scope.size(); /* Nothing */)
		{
			scope_value& itemNew = m_all_scope[i];

			// 错误的区间移除
			if (0 == itemNew.get_scope_status())
			{
				m_all_scope.erase(m_all_scope.begin() + i);
				be_arranged = false;
				continue;
			}

			for (size_t j = i + 1; j < m_all_scope.size(); /* Nothing */)
			{
				if (true == itemNew.merge_scope_value(m_all_scope[j]))
				{
					m_all_scope.erase(m_all_scope.begin() + j);
					be_arranged = false;
					continue;
				}

				++j;
			}

			++i;
		}

	} // end of while

	// 不要动原始字符串，整理区间只是优化结构，并不改变区间的范围，所以不要改原始字符串
	//m_is_old_ui_string = false;
	//m_old_ui_string = L"";

}

assemblage_value& assemblage_value::operator=(const assemblage_value& assem)
{
	if (this == &assem) return *this;

	m_is_old_ui_string = assem.m_is_old_ui_string;
	m_old_ui_string = assem.m_old_ui_string;
	m_all_scope = assem.m_all_scope;

	return *this;
}

void assemblage_value::to_scope_items( std::vector<scope_value>& scope_items ) const
{
	scope_items.clear();
	scope_items = m_all_scope;
}

const std::vector<scope_value> assemblage_value::to_scope_items() const
{
	vector<scope_value> scope_items = m_all_scope;
	return scope_items;
}

bool assemblage_value::is_in_assemblage( const number& num ) const
{
	return has_intersection(num);
}

bool assemblage_value::is_in_assemblage( const scope_value& scope ) const
{
	return has_intersection(scope);
}

bool assemblage_value::is_in_assemblage( const assemblage_value& scope ) const
{
	return has_intersection(scope);
}


//////////////////////////////////////////////////////////////////////////

// 测试代码,暂留
//class test_scope
//{
//public:
//	test_scope()
//	{
//		// 检测使用数字构造有没有二义性
//		number nm_1(1);
//		number nm_2(1.0);
//
//		scope_value sc_1(1);
//		scope_value sc_2(1.0);
//
//		assemblage_value as_1(1);
//		assemblage_value as_2(1.0);
//
//		assemblage_value sa;
//		sa.union_set(scope_value(_T("(０,5)")));
//		sa.union_set(number(2));						// sa为(0, 5)
//		sa.union_set(number(0));						// sa为[0, 5)
//		sa.union_set(number(5));						// sa为[0, 5]
//		sa.union_set(scope_value(_T("[0, 3)")));		// sa为[0, 5]
//		sa.union_set(scope_value(_T("(-10, 0)")));		// sa为(-10, 5]
//		sa.union_set(scope_value(_T("[-20, -10)")));	// sa为[-20, 10) (-10, 5]
//		sa.union_set(scope_value(_T("(5, 10)")));		// sa为[-20, 10) (-10, 10)
//		sa.union_set(scope_value(_T("[5, 20)")));		// sa为[-20, 10) (-10, 20)
//		sa.union_set(scope_value(_T("(20, 30)")));		// sa为[-20, 10) (-10, 20) (20, 30)
//
//		assemblage_value saNew = sa;
//		sa.union_set(saNew);							// sa为[-20, 10) (-10, 20) (20, 30)
//		sa.intersection(scope_value(_T("(5, 15)")));	// sa为(5, 15)
//
//	}
//};
//
//
//test_scope k;
