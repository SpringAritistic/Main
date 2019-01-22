#ifndef _TEST6_29_
#define  _TEST6_29_
#include "type_traits"
void test6_29();
void testTrait();
template <class T>
CString foo(const T&val)
{
	CString str;
	using namespace  std;
	if (is_pointer<T>::value)
		str = "调用的是指针";
	else
		str = "调用的是对象";
	return str;
}
CString&& testMove(CString&& str);
CString testMove(CString& str);
CString testMove(const CString& str);
//
struct IntId
{
	IntId(int val){ set(val); }
	IntId():id(nullptr){ }
	void set(int val)
	{
		if (id != nullptr)
			id = new int;
		*id = val;
	}
	int * id;
	void operator =(int val){ *id = val; }
};
IntId& GetId(int value, vector< IntId*>&vecId);
int& GetId(int value,int& id);
void testGetID();

void testPair();
class person
{
public:
	//person(){}
	person(CString name="Jonh", int age=20,bool isMan=true,CString school="小学") :m_name(name), m_age(age),m_isMan(isMan),m_school(school){}
	CString m_name;
	int m_age;

	CString m_school;
	bool m_isMan;
};
class clerk
{
public:
	CString m_name;
	int m_age;
	char m_index;

};
//两种类型的强制转换，必须保证类里面的 成员变量顺序一致，否则就会解析错误
void testChange();
void testID();

#endif