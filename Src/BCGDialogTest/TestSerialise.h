#include "HMGlobalTool/HMGlobalToolMacro.h"
#include "HMGlobalTool/interface/serialize_interface.h"


class Person
{
public:
	enum SexType{ MAN = 0, WOMAN = 1, NONE };
public:
	Person(const CString& name="",int age=0,SexType type=NONE);

public:
	virtual void serialize_members(member_rw_interface& mt);

protected:

private:
	CString m_name;
	int m_age;
	SexType m_sex;

};
void TestBin(vector<Person>& vec, bool isLoad);
void TestJson(vector<Person>& vec, bool isLoad);
void TestXml(vector<Person>& vec, bool isLoad);

void TestSerial();