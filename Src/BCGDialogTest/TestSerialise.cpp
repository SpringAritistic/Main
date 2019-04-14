#include "stdafx.h"
#include "TestSerialise.h"
#include "fstream"

Person::Person(const CString& name, int age, SexType type) :m_name(name), m_age(age), m_sex(type)
{

}
void Person::serialize_members(member_rw_interface& mt)
{
	x_object_flag_serialization_guard tmp_obj_flag_guard(_T("Person"), mt);
	int version = 1;
	mt & x_wrapper_macro(version);
	if (mt.is_storing() || (!mt.is_storing() && version >= 1))
	{

		mt & x_wrapper_macro(m_name);
		mt & x_wrapper_macro(m_age);
		mt & x_wrapper_macro_enum(m_sex);
	}
}
void TestBin(vector<Person>& vec, bool isLoad)
{
	UINT type = isLoad ? 0 : CFile::modeCreate;

	CFile file("G:\\练习集\\序列化\\Person.text", type | CFile::modeReadWrite);
	CArchive ar(&file, (!isLoad ? CArchive::store : CArchive::load));
	bin_archive mt(ar);
	vector<Person>persons;
	if (!isLoad)
		persons = vec;
	mt & x_wrapper_macro(persons);
	if (isLoad)
		vec = persons;
	ar.Close();
}
void TestXml(vector<Person>& vec, bool isLoad)
{
	using namespace tinyxml2;
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pXml = NULL;
	CString pathName = "G:\\练习集\\序列化\\Person.xml";
	if (!isLoad)
	{
		XMLDeclaration* declaration = doc.NewDeclaration();
		doc.InsertFirstChild(declaration);
		tinyxml2::XMLElement& xml = *doc.NewElement(_T("hell"));
		doc.InsertEndChild(&xml);
		pXml = &xml;
	}
	else
	{
		if (doc.LoadFile(pathName) == XML_SUCCESS)
		{
			pXml = doc.RootElement();
		}
	}
	if (!pXml)
		return;
	//
	xml_archive mt(pXml, !isLoad);
	vector<Person>persons;
	if (!isLoad)
		persons = vec;
	mt & x_wrapper_macro(persons);
	if (isLoad)
		vec = persons;
	if (!isLoad)
	{
		doc.SaveFile(pathName);
	}

}
void TestJson(vector<Person>& vec, bool isLoad)
{
	Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         
	CString pathName = "G:\\练习集\\序列化\\Person.json";
	vector<Person>persons;

	if (!isLoad)
	{
		json_archive mt(&root, !isLoad);
		if (!isLoad)
			persons = vec;
		mt & x_wrapper_macro(persons);
		//Json::FastWriter writer;
		Json::StyledWriter writer;
		std::string strWrite = writer.write(root);



		std::ofstream ofs;
		ofs.open(pathName);



		ofs << strWrite;
		ofs.close();
	}
	else
	{
		Json::Reader reader;// 解析json用Json::Reader   
		std::ifstream stream;
		stream.open(pathName);
		if (reader.parse(stream, root))
		{
			json_archive mt(&root, !isLoad);
			vector<Person>persons;
			if (!isLoad)
				persons = vec;
			mt & x_wrapper_macro(persons);
			if (isLoad)
				vec = persons;
		}
		stream.close();
	}
}


void TestSerial()
{
	vector<Person>vecPerson;
	bool isLoad = false;
	if (!isLoad)
	{
		vecPerson.emplace_back("John", 13, Person::MAN);
		vecPerson.emplace_back("White", 20, Person::WOMAN);
		vecPerson.emplace_back("John1", 14, Person::MAN);
		vecPerson.emplace_back("John2", 15, Person::MAN);
		vecPerson.emplace_back("John3", 16, Person::MAN);
		vecPerson.emplace_back("John4", 17, Person::MAN);
		vecPerson.emplace_back("John5", 18, Person::MAN);
		vecPerson.emplace_back("John6", 19, Person::MAN);
		vecPerson.emplace_back("John7", 20, Person::MAN);
		vecPerson.emplace_back("John8", 21, Person::MAN);
		vecPerson.emplace_back("John9", 22, Person::MAN);
		vecPerson.emplace_back("John10", 23, Person::MAN);
		vecPerson.emplace_back("John11", 24, Person::MAN);
		vecPerson.emplace_back("John12", 25, Person::MAN);
		vecPerson.emplace_back("John26", 26, Person::MAN);
		vecPerson.emplace_back("John14", 27, Person::MAN);
		vecPerson.emplace_back("John15", 28, Person::MAN);
		vecPerson.emplace_back("John16", 29, Person::MAN);
		vecPerson.emplace_back("John17", 30, Person::MAN);
		vecPerson.emplace_back("John18", 31, Person::MAN);
		vecPerson.emplace_back("John19", 32, Person::MAN);
		vecPerson.emplace_back("John20", 33, Person::MAN);
		vecPerson.emplace_back("John21", 34, Person::MAN);
		vecPerson.emplace_back("John22", 35, Person::MAN);
		vecPerson.emplace_back("John23", 36, Person::MAN);
		vecPerson.emplace_back("John24", 37, Person::MAN);
		vecPerson.emplace_back("John25", 38, Person::MAN);
		vecPerson.emplace_back("John26", 39, Person::MAN);
		vecPerson.emplace_back("John27", 40, Person::MAN);
		vecPerson.emplace_back("John28", 41, Person::MAN);
		vecPerson.emplace_back("John29", 42, Person::MAN);
		vecPerson.emplace_back("John30", 43, Person::MAN);
		vecPerson.emplace_back("John31", 44, Person::MAN);
		vecPerson.emplace_back("John32", 45, Person::MAN);
		vecPerson.emplace_back("John33", 46, Person::MAN);
		vecPerson.emplace_back("John34", 47, Person::MAN);
		vecPerson.emplace_back("John35", 48, Person::MAN);
		vecPerson.emplace_back("John36", 49, Person::MAN);
		vecPerson.emplace_back("John37", 50, Person::MAN);
		vecPerson.emplace_back("John38", 51, Person::MAN);
		vecPerson.emplace_back("John39", 52, Person::MAN);
		vecPerson.emplace_back("John40", 53, Person::MAN);
		vecPerson.emplace_back("John41", 54, Person::MAN);
		vecPerson.emplace_back("John42", 55, Person::MAN);
		vecPerson.emplace_back("John43", 56, Person::MAN);
		vecPerson.emplace_back("John44", 57, Person::MAN);
		vecPerson.emplace_back("John45", 58, Person::MAN);
		vecPerson.emplace_back("John46", 59, Person::MAN);
		vecPerson.emplace_back("John47", 60, Person::MAN);
		vecPerson.emplace_back("John48", 61, Person::MAN);
		vecPerson.emplace_back("John49", 62, Person::MAN);
		vecPerson.emplace_back("John50", 63, Person::MAN);
		vecPerson.emplace_back("John51", 64, Person::MAN);
		vecPerson.emplace_back("John52", 65, Person::MAN);
		vecPerson.emplace_back("John53", 66, Person::MAN);
		vecPerson.emplace_back("John54", 67, Person::MAN);
		vecPerson.emplace_back("John55", 68, Person::MAN);
		vecPerson.emplace_back("John56", 69, Person::MAN);
		vecPerson.emplace_back("John57", 70, Person::MAN);
		vecPerson.emplace_back("John58", 71, Person::MAN);
		vecPerson.emplace_back("John59", 72, Person::MAN);
		vecPerson.emplace_back("John60", 73, Person::MAN);
		vecPerson.emplace_back("John61", 74, Person::MAN);
		vecPerson.emplace_back("John62", 75, Person::MAN);
		vecPerson.emplace_back("John63", 76, Person::MAN);
		vecPerson.emplace_back("John64", 77, Person::MAN);
		vecPerson.emplace_back("John65", 78, Person::MAN);
		vecPerson.emplace_back("John66", 79, Person::MAN);
		vecPerson.emplace_back("John67", 80, Person::MAN);
		vecPerson.emplace_back("John68", 81, Person::MAN);
		vecPerson.emplace_back("John69", 82, Person::MAN);
		vecPerson.emplace_back("John70", 83, Person::MAN);
		vecPerson.emplace_back("John71", 84, Person::MAN);
		vecPerson.emplace_back("John72", 85, Person::MAN);
		vecPerson.emplace_back("John73", 86, Person::MAN);
		vecPerson.emplace_back("John74", 87, Person::MAN);
		vecPerson.emplace_back("John75", 88, Person::MAN);
		vecPerson.emplace_back("John76", 89, Person::MAN);
		vecPerson.emplace_back("John77", 90, Person::MAN);
		vecPerson.emplace_back("John78", 91, Person::MAN);
		vecPerson.emplace_back("John79", 92, Person::MAN);
		vecPerson.emplace_back("John80", 93, Person::MAN);
		vecPerson.emplace_back("John81", 94, Person::MAN);
		vecPerson.emplace_back("John82", 95, Person::MAN);
		vecPerson.emplace_back("John83", 96, Person::MAN);
		vecPerson.emplace_back("John84", 97, Person::MAN);
		vecPerson.emplace_back("John85", 98, Person::MAN);
		vecPerson.emplace_back("John86", 99, Person::MAN);
		vecPerson.emplace_back("John87", 100, Person::MAN);
		vecPerson.emplace_back("John88", 101, Person::MAN);
		vecPerson.emplace_back("John89", 102, Person::MAN);
		vecPerson.emplace_back("John90", 103, Person::MAN);
		vecPerson.emplace_back("John91", 104, Person::MAN);
		vecPerson.emplace_back("John92", 105, Person::MAN);
		vecPerson.emplace_back("John93", 106, Person::MAN);
		vecPerson.emplace_back("John94", 107, Person::MAN);
		vecPerson.emplace_back("John95", 108, Person::MAN);
		vecPerson.emplace_back("John96", 109, Person::MAN);
		vecPerson.emplace_back("John97", 110, Person::MAN);
		vecPerson.emplace_back("John98", 111, Person::MAN);
		vecPerson.emplace_back("John99", 112, Person::MAN);
		vecPerson.emplace_back("John100", 113, Person::MAN);
		vecPerson.emplace_back("John101", 114, Person::MAN);
		vecPerson.emplace_back("John102", 115, Person::MAN);
		vecPerson.emplace_back("John103", 116, Person::MAN);
		vecPerson.emplace_back("John104", 117, Person::MAN);
		vecPerson.emplace_back("John105", 118, Person::MAN);
		vecPerson.emplace_back("John106", 119, Person::MAN);
		vecPerson.emplace_back("John107", 120, Person::MAN);
		vecPerson.emplace_back("John108", 121, Person::MAN);
		vecPerson.emplace_back("John109", 122, Person::MAN);
		vecPerson.emplace_back("John110", 123, Person::MAN);
		vecPerson.emplace_back("John111", 124, Person::MAN);
		vecPerson.emplace_back("John112", 125, Person::MAN);
		vecPerson.emplace_back("John1126", 126, Person::MAN);
		vecPerson.emplace_back("John114", 127, Person::MAN);
		vecPerson.emplace_back("John115", 128, Person::MAN);
		vecPerson.emplace_back("John116", 129, Person::MAN);
		vecPerson.emplace_back("John117", 130, Person::MAN);
		vecPerson.emplace_back("John118", 131, Person::MAN);
		vecPerson.emplace_back("John119", 132, Person::MAN);
		vecPerson.emplace_back("John120", 133, Person::MAN);
		vecPerson.emplace_back("John121", 134, Person::MAN);
		vecPerson.emplace_back("John122", 135, Person::MAN);
		vecPerson.emplace_back("John123", 136, Person::MAN);
		vecPerson.emplace_back("John124", 137, Person::MAN);
		vecPerson.emplace_back("John125", 138, Person::MAN);
		vecPerson.emplace_back("John126", 139, Person::MAN);
		vecPerson.emplace_back("John127", 140, Person::WOMAN);
		vecPerson.emplace_back("John128", 141, Person::WOMAN);
		vecPerson.emplace_back("John129", 142, Person::WOMAN);
		vecPerson.emplace_back("John1430", 143, Person::WOMAN);
		vecPerson.emplace_back("John1441", 144, Person::WOMAN);
		vecPerson.emplace_back("John1452", 145, Person::WOMAN);
		vecPerson.emplace_back("John1463", 146, Person::WOMAN);
		vecPerson.emplace_back("John1474", 147, Person::WOMAN);
		vecPerson.emplace_back("John1485", 148, Person::WOMAN);
		vecPerson.emplace_back("John1496", 149, Person::WOMAN);
		vecPerson.emplace_back("John1507", 150, Person::WOMAN);
		vecPerson.emplace_back("John1518", 151, Person::WOMAN);
		vecPerson.emplace_back("John1529", 152, Person::WOMAN);
		vecPerson.emplace_back("John140", 153, Person::WOMAN);
		vecPerson.emplace_back("John141", 154, Person::WOMAN);
		vecPerson.emplace_back("John142", 155, Person::WOMAN);
		vecPerson.emplace_back("John143", 156, Person::WOMAN);
		vecPerson.emplace_back("John144", 157, Person::WOMAN);
		vecPerson.emplace_back("John145", 158, Person::WOMAN);
		vecPerson.emplace_back("John146", 159, Person::WOMAN);
		vecPerson.emplace_back("John147", 160, Person::WOMAN);
		vecPerson.emplace_back("John148", 161, Person::WOMAN);
		vecPerson.emplace_back("John149", 162, Person::WOMAN);
		vecPerson.emplace_back("John150", 163, Person::WOMAN);
		vecPerson.emplace_back("John151", 164, Person::WOMAN);
		vecPerson.emplace_back("John152", 165, Person::WOMAN);
		vecPerson.emplace_back("John153", 166, Person::WOMAN);
		vecPerson.emplace_back("John154", 167, Person::WOMAN);
		vecPerson.emplace_back("John155", 168, Person::WOMAN);
		vecPerson.emplace_back("John156", 169, Person::WOMAN);
		vecPerson.emplace_back("John157", 170, Person::WOMAN);
		vecPerson.emplace_back("John158", 171, Person::WOMAN);
		vecPerson.emplace_back("John159", 172, Person::WOMAN);
		vecPerson.emplace_back("John160", 173, Person::WOMAN);
		vecPerson.emplace_back("John161", 174, Person::WOMAN);
		vecPerson.emplace_back("John162", 175, Person::WOMAN);
		vecPerson.emplace_back("John163", 176, Person::WOMAN);
		vecPerson.emplace_back("John164", 177, Person::WOMAN);
		vecPerson.emplace_back("John165", 178, Person::WOMAN);
		vecPerson.emplace_back("John166", 179, Person::WOMAN);
		vecPerson.emplace_back("John167", 180, Person::WOMAN);
		vecPerson.emplace_back("John168", 181, Person::WOMAN);
		vecPerson.emplace_back("John169", 182, Person::WOMAN);
		vecPerson.emplace_back("John170", 183, Person::WOMAN);
		vecPerson.emplace_back("John171", 184, Person::WOMAN);
		vecPerson.emplace_back("John172", 185, Person::WOMAN);
		vecPerson.emplace_back("John173", 186, Person::WOMAN);
		vecPerson.emplace_back("John174", 187, Person::WOMAN);
		vecPerson.emplace_back("John175", 188, Person::WOMAN);
		vecPerson.emplace_back("John176", 189, Person::WOMAN);
		vecPerson.emplace_back("John177", 190, Person::WOMAN);
		vecPerson.emplace_back("John178", 191, Person::WOMAN);
		vecPerson.emplace_back("John179", 192, Person::WOMAN);
		vecPerson.emplace_back("John180", 193, Person::WOMAN);
		vecPerson.emplace_back("John181", 194, Person::WOMAN);
		vecPerson.emplace_back("John182", 195, Person::WOMAN);
		vecPerson.emplace_back("John183", 196, Person::WOMAN);
		vecPerson.emplace_back("John184", 197, Person::WOMAN);
		vecPerson.emplace_back("John185", 198, Person::WOMAN);
		vecPerson.emplace_back("John186", 199, Person::WOMAN);
		vecPerson.emplace_back("John187", 200, Person::WOMAN);
		vecPerson.emplace_back("John188", 201, Person::WOMAN);
		vecPerson.emplace_back("John189", 202, Person::WOMAN);
		vecPerson.emplace_back("John190", 203, Person::WOMAN);
		vecPerson.emplace_back("John191", 204, Person::WOMAN);
		vecPerson.emplace_back("John192", 205, Person::WOMAN);
		vecPerson.emplace_back("John193", 206, Person::WOMAN);
		vecPerson.emplace_back("John194", 207, Person::WOMAN);
		vecPerson.emplace_back("John195", 208, Person::WOMAN);
		vecPerson.emplace_back("John196", 209, Person::WOMAN);
		vecPerson.emplace_back("John197", 210, Person::WOMAN);
		vecPerson.emplace_back("John198", 211, Person::WOMAN);
		vecPerson.emplace_back("John199", 212, Person::WOMAN);

	}

	//if (false)
		TestBin(vecPerson, isLoad);
	if (false)
		TestXml(vecPerson, isLoad);
	//if (false)
		TestJson(vecPerson, isLoad);



}