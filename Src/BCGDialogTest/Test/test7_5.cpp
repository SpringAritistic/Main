#include "stdafx.h"
#include "test7_5.h"
#include "vector"
#include "functional"
using namespace  std;
void test7_5()
{
	testWrapper();
}
vector<reference_wrapper<CString>>GetNames()
{
	vector<reference_wrapper<CString>>vecNames;
	CString* wb = new CString("王博");  //不能删除指针，否则会出错，因为是 使用了引用，一般对象会在此函数结束时被析构，new到堆中本地删除操作
	CString*lzh = new CString("李争华");//不能删除指针，否则会出错，因为是 使用了引用，一般对象会在此函数结束时被析构，new到堆中本地删除操作
	CString*cjd = new CString("曹景德");//不能删除指针，否则会出错，因为是 使用了引用，一般对象会在此函数结束时被析构，new到堆中本地删除操作
	vecNames.push_back(*wb);
	vecNames.push_back(*lzh);
	vecNames.push_back(*cjd);
	vector<int>vecInt{ (int)&wb, (int)&lzh, (int)&cjd };		//指针变量自己的 栈地址
	vector<int>vecInt1{ *(int*)wb, *(int*)lzh, *(int*)cjd };	//指针所指内存的 堆地址的前4个字节当做int处理
	vector<int>vecInt2{ (int)wb, (int)lzh, (int)cjd };			//指针所指内存的 堆地址
	return vecNames;
}

void testWrapper()
{
	vector<reference_wrapper<CString>> names = GetNames();
	//names[0].get() = CString("王鹏");
	names[0].get() = "王鹏";


}