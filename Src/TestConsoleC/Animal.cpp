#include "stdafx.h"
#include <iostream>
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/serialization/vector.hpp"
#include <fstream>
#include "Animal.h"

IMPLEMENT_SERIAL(Animal, CObject, 1)
IMPLEMENT_SERIAL(Fish, Animal, 1)
IMPLEMENT_SERIAL(Chicken, Animal, 1)

BOOST_CLASS_EXPORT(Fish);
BOOST_CLASS_EXPORT(Chicken);

void print(string str)
{
	cout << str << endl;
}
Animal::Animal() :m_name(__FUNCTION__)
{

}
Animal::Animal(const Animal& any)
{
	m_name = any.m_name;
}

void Animal::Serialize(CArchive& ar)
{
	CString str;
	if (ar.IsLoading())
	{
		ar >> str;
		m_name = str;
		//ar >> m_name;
	}
	else
	{
		str = m_name.c_str();
		ar << str;
	}
}
Animal::Animal(string name) : m_name(name)
{

}


void Animal::show()
{
	print(__FUNCTION__);
}
Fish::Fish() :Fish(__FUNCTION__, 0)
{

}
Fish::Fish(string name, int age) :Animal(name), m_age(age)
{

}
void Fish::show()
{
	print(__FUNCTION__);
}
void Chicken::show()
{
	print(__FUNCTION__);
}
Chicken::Chicken() :Chicken(__FUNCTION__,0)
{

}
Chicken::Chicken(string name, int sex) :Animal(name), m_sex(sex)
{

}
void Fish::Serialize(CArchive& ar)
{
	Animal::Serialize(ar);
	if (ar.IsLoading())
	{
		ar >> m_age;
	}
	else
	{
		ar << m_age;
	}
}
void Chicken::Serialize(CArchive& ar)
{
	Animal::Serialize(ar);
	if (ar.IsLoading())
	{
		ar >> m_sex;
	}
	else
	{
		ar << m_sex;
	}
}
AnimalManager::~AnimalManager()
{
	for (auto& item : m_vec)
		delete item;

}

//void  AnimalManager::Add(const Animal& obj)
//{
//	ofstream file("F:\\清风细雨\\C++练习\\工程—练习\\测试项目\\测试文本.txt");
//	if (file.is_open())
//	{
//		boost::archive::text_oarchive oa(file);
//		oa & obj;
//		file.close();
//	}
//	Animal * newObj=NULL;
//	//输入
//	ifstream inFile("F:\\清风细雨\\C++练习\\工程—练习\\测试项目\\测试文本.txt");
//	if (inFile.is_open())
//	{
//		boost::archive::text_iarchive ia(inFile);
//		ia & newObj;
//		inFile.close();
//	}
//	if (newObj)
//		m_vec.push_back(newObj);
//}

void TestDerive()
{
	vector<Animal*> vec{ 
		new Fish("王博", 22),
		new Fish("Sam", 23),
		new Chicken("John", 24),
		new Fish("White", 25),
		new Chicken("Black", 26),
	};

	using namespace  std;
	//ofstream file("测试文本.txt");
	ofstream file("F:\\清风细雨\\C++练习\\工程—练习\\测试项目\\测试文本.txt");
	if (file.is_open())
	{
		boost::archive::text_oarchive oa(file);
		oa & vec;
		file.close();
	}
	vector<Animal*>ReadVec;

	//输入
	ifstream inFile("F:\\清风细雨\\C++练习\\工程—练习\\测试项目\\测试文本.txt");
	if (inFile.is_open())
	{
		boost::archive::text_iarchive ia(inFile);
		ia & ReadVec;
		inFile.close();
	}
	AnimalManager manager;
	for (auto& item : ReadVec)
	{
		manager.Add(*item);
	}
	
	

	cout << "hell0" << endl;
}