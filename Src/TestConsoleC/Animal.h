#ifndef _ANIMAL_H_
#define  _ANIMAL_H_
#include "afx.h"
#include <boost/serialization/serialization.hpp>
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/export.hpp"
#include "atlstr.h"
#include <vector>
using namespace std;
class Animal :public CObject
{
public:
	Animal();
	Animal(const Animal& any);
	Animal(string name);	//
	friend class ::boost::serialization::access;
	DECLARE_SERIAL(Animal);
	virtual void Serialize(CArchive& ar);
	
	template < class Archive>
	void serialize (Archive & ar,const unsigned int version)
	{
		
		ar & m_name;
	}

public:
	virtual void show();
	string m_name;
};

class Fish :public Animal
{
public:
	Fish();
	Fish(string name, int age);
	void show();
	int m_age;
	DECLARE_SERIAL(Fish);

public:
	friend class ::boost::serialization::access;
	virtual void Serialize(CArchive& ar);
	template < class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
	ar & ::boost::serialization::base_object<Animal>(*this);
		ar & m_age;
	}
};
class Chicken :public Animal
{
public:
	Chicken();
	Chicken(string name, int sex);
	void show();
	int m_sex;
public:
public:
	friend class ::boost::serialization::access;
	DECLARE_SERIAL(Chicken);
	virtual void Serialize(CArchive& ar);
	template < class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & ::boost::serialization::base_object<Animal>(*this);// 
		ar & m_sex;
	}
};
class AnimalManager
{
public:
	~AnimalManager();

	template<class T> void 
		Add(const T& obj)
	{
		m_vec.push_back(new T(obj));
	}

	//void Add(const Animal& obj);
	vector<Animal* > m_vec;
};


void TestDerive();

#endif
