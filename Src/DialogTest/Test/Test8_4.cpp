#include "stdafx.h"
#include "Test8_4.h"
#include "regex"
#include "iostream"
#include "algorithm"
void Test8_4()
{
	testRegex();
}
void testRegex()
{
	string date = "<person>"
		"< first>Jick</ first>\n"
		"< second>Sun</ second>\n"
		"</person>\n";


regex reg("<(.*)>(.*)</(\\1)>");
sregex_iterator pos(date.cbegin(), date.cend(), reg);
sregex_iterator end;
vector<vector<CString>>vecString;
for (; pos != end; ++pos)
	vecString.push_back({ stringToCString(pos->str()), stringToCString(pos->str(1)), stringToCString( pos->str(2)) });

}
CString stringToCString(string str)
{
	return CString(str.c_str());
}