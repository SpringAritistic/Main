#include "stdafx.h"
#include "../tools/tinyxml2.h"
#include "../tools/xml_tools.h"
#include "../tools/json.h"
#include "../tools/base64_tools.h"

#include "serialize_interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace tinyxml2;


serialize_interface::serialize_interface()
{

}

serialize_interface::~serialize_interface()
{

}

void serialize_interface::serialize_members(member_rw_interface& mt)
{
}

void member_rw_interface::read(const CString& name, char*& buffer, long& byte_count)
{
	read_impl(name, buffer, byte_count);
}

void member_rw_interface::read(const CString& name, const serialize_flag& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, bool& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, char& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, unsigned char& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, wchar_t& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, short& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, unsigned short& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, int& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, unsigned int& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, long& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, unsigned long& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, long long& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, unsigned long long& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, float& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, double& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, long double& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, CString& data)
{
	read_impl(name, data);
}

void member_rw_interface::read(const CString& name, std::string& data)
{
	CString tmp_data; read_impl(name, tmp_data); data = string_tools::CString_to_string(tmp_data);
}

void member_rw_interface::read(const CString& name, std::wstring& data)
{
	CString tmp_data; read_impl(name, tmp_data); data = string_tools::CString_to_wstring(tmp_data);
}

void member_rw_interface::write(const CString& name, char* buffer, long byte_count)
{
	write_impl(name, buffer, byte_count);
}

void member_rw_interface::write(const CString& name, const serialize_flag& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const bool& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const char& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const unsigned char& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const wchar_t& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const short& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const unsigned short& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const int& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const unsigned int& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const long& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const unsigned long& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const long long& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const unsigned long long& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const float& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const double& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const long double& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const CString& data)
{
	write_impl(name, data);
}

void member_rw_interface::write(const CString& name, const std::string& data)
{
	write_impl(name, string_tools::string_to_CString(data));
}

void member_rw_interface::write(const CString& name, const std::wstring& data)
{
	write_impl(name, string_tools::wstring_to_CString(data));
}

void member_rw_interface::write(const CString& name, const char* data)
{
	write_impl(name, string_tools::string_to_CString(std::string(data)));
}

void member_rw_interface::write(const CString& name, const wchar_t* data)
{
	write_impl(name, string_tools::wstring_to_CString(std::wstring(data)));
}

void member_rw_interface::read_impl(const CString& name, std::vector<bool>& val)
{
	read_impl(name, serialize_flag::array_bg);

	int count = 0;
	read_impl(_T("count"), count); // name没有意义，只用于保持原型统一

	val.resize(count);
	for (int i = 0; i < count; ++i)
	{
		bool tmp = false;
		read_impl(_T("item_") + string_tools::num_to_string(i), tmp); // tinyxml2解析时不支持节点名为数字开头,不允许带点号
		val[i] = tmp;
	}

	read_impl(name, serialize_flag::array_ed);
}

void member_rw_interface::write_impl(const CString& name, const std::vector<bool>& val)
{
	write_impl(name, serialize_flag::array_bg);

	write_impl(_T("count"), (int)(val.size())); // name没有意义，只用于保持原型统一
	for (size_t i = 0; i < val.size(); ++i)
		write_impl(_T("item_") + string_tools::num_to_string(i), (const bool&)val[i]); // tinyxml2解析时不支持节点名为数字开头,不允许带点号

	write_impl(name, serialize_flag::array_ed);
}


//////////////////////////////////////////////////////////////////////////


member_rw_interface::member_rw_interface()
{

}

member_rw_interface::member_rw_interface(const member_rw_interface& src)
{

}

member_rw_interface::~member_rw_interface()
{

}

//////////////////////////////////////////////////////////////////////////

bin_archive::bin_archive(CArchive& ar) : m_ar(ar)
{

}

bin_archive::~bin_archive()
{

}

bool bin_archive::is_storing()
{
	return (TRUE == m_ar.IsStoring());
}

void bin_archive::read_impl(const CString& name, const serialize_flag& data)
{
	UNUSED_ALWAYS(name);

	// write_impl写了值，则读取的时候也得配对读取，读取后丢弃不用即可
	int tmp = 0;
	m_ar >> tmp;
}

void bin_archive::read_impl(const CString& name, bool& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, char& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, unsigned char& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, wchar_t& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, short& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, unsigned short& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, int& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, unsigned int& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, long& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, unsigned long& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, long long& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, unsigned long long& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, float& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, double& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, long double& data)
{
	UNUSED_ALWAYS(name);

	// 微软编译器为了兼容以前的DOS程序，限定了long double与double一样宽为64bit，与gcc,clang的128bit并不一样
	double tmp = 0.0;
	m_ar >> tmp;
	data = tmp;
}

void bin_archive::read_impl(const CString& name, CString& data)
{
	UNUSED_ALWAYS(name);
	m_ar >> data;
}

void bin_archive::read_impl(const CString& name, char*& buffer, long& byte_count)
{
	UNUSED_ALWAYS(name);
	m_ar >> byte_count;
	if (byte_count <= 0)
	{
		buffer = nullptr;
		return;
	}

	buffer = new char[byte_count];
	memset(buffer, 0, byte_count);
	m_ar.Read(buffer, byte_count);
}

void bin_archive::write_impl(const CString& name, const serialize_flag& data)
{
	UNUSED_ALWAYS(name);
	m_ar << (int)data;
}

void bin_archive::write_impl(const CString& name, const bool& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const char& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const unsigned char& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const wchar_t& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const short& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const unsigned short& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const int& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const unsigned int& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const long& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const unsigned long& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const long long& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const unsigned long long& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const float& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const double& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, const long double& data)
{
	UNUSED_ALWAYS(name);

	// 微软编译器为了兼容以前的DOS程序，限定了long double与double一样宽为64bit，与gcc,clang的128bit并不一样
	double tmp = (double)data;
	m_ar << tmp;
}

void bin_archive::write_impl(const CString& name, const CString& data)
{
	UNUSED_ALWAYS(name);
	m_ar << data;
}

void bin_archive::write_impl(const CString& name, char* buffer, long byte_count)
{
	UNUSED_ALWAYS(name);
	m_ar << byte_count;
	if (byte_count > 0) m_ar.Write((void*)buffer, byte_count);
}


//////////////////////////////////////////////////////////////////////////


json_archive::json_archive(Json::Value* json, bool is_storing/* = false*/)
	: m_json(json), m_is_storing(is_storing), m_json_array_reading_index(0)
{
	clear_parent_nodes_cache();
}

json_archive::~json_archive()
{

}

bool json_archive::is_storing()
{
	return m_is_storing;
}

void json_archive::read_impl(const CString& name, const serialize_flag& data)
{
	if (serialize_flag::sub_obj_bg == data)
	{
		if (m_json->isArray())
		{
			Json::Value* tmp_json = m_json;
			m_json = &((*m_json)[m_json_array_reading_index++]);
			m_parent_nodes.push(make_pair(tmp_json, m_json_array_reading_index));
			//
		}
		//else
		{
			// 没有相应的成员会崩溃
			if (!m_json->isMember(string_tools::CString_to_string(name))) return;

			m_parent_nodes.push(make_pair(m_json, 0));
			m_json_array_reading_index = 0;
			m_json = &((*m_json)[name]);
		}
	}
	else if (serialize_flag::sub_obj_ed == data)
	{
		if (!m_parent_nodes.empty())
		{
			int count =2;
			for (int i = 0; i < count; ++i)
			{
				pair<Json::Value *, int> top_item = m_parent_nodes.top();
				m_json = top_item.first;
				m_json_array_reading_index = top_item.second;
				m_parent_nodes.pop();
			}
		}
	}
	else if (serialize_flag::array_bg == data)
	{
		if (m_json->isArray())
		{
			Json::Value* tmp_json = m_json;
			m_json = &((*m_json)[m_json_array_reading_index++]);
			m_parent_nodes.push(make_pair(tmp_json, m_json_array_reading_index));
		}
		else
		{
			// 没有相应的成员会崩溃
			if (!m_json->isMember(string_tools::CString_to_string(name))) return;

			m_parent_nodes.push(make_pair(m_json, 0));
			m_json_array_reading_index = 0;
			m_json = &((*m_json)[name]);
		}
	}
	else if (serialize_flag::array_ed == data)
	{
		if (!m_parent_nodes.empty())
		{
			pair<Json::Value *, int> top_item = m_parent_nodes.top();
			m_json = top_item.first;
			m_json_array_reading_index = top_item.second;
			m_parent_nodes.pop();
		}
	}
	else
	{
		// nothing;
	}
}

void json_archive::read_impl(const CString& name, bool& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asBool();
	else
		data = (*m_json)[name].asBool();
}

void json_archive::read_impl(const CString& name, char& data)
{
	if (m_json->isArray())
		data = (char)(*m_json)[m_json_array_reading_index++].asInt();
	else
		data = (char)(*m_json)[name].asInt();
}

void json_archive::read_impl(const CString& name, unsigned char& data)
{
	if (m_json->isArray())
		data = (unsigned char)(*m_json)[m_json_array_reading_index++].asUInt();
	else
		data = (unsigned char)(*m_json)[name].asUInt();
}

void json_archive::read_impl(const CString& name, wchar_t& data)
{
	if (m_json->isArray())
		data = (wchar_t)(*m_json)[m_json_array_reading_index++].asInt();
	else
		data = (wchar_t)(*m_json)[name].asUInt();
}

void json_archive::read_impl(const CString& name, short& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asInt();
	else
		data = (*m_json)[name].asInt();
}

void json_archive::read_impl(const CString& name, unsigned short& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asInt();
	else
		data = (*m_json)[name].asUInt();
}

void json_archive::read_impl(const CString& name, int& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asInt();
	else
		data = (*m_json)[name].asInt();
}

void json_archive::read_impl(const CString& name, unsigned int& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asInt();
	else
		data = (*m_json)[name].asUInt();
}

void json_archive::read_impl(const CString& name, long& data)
{
	if (m_json->isArray())
		data = (long)(*m_json)[m_json_array_reading_index++].asInt64();
	else
		data = (long)(*m_json)[name].asInt64();
}

void json_archive::read_impl(const CString& name, unsigned long& data)
{
	if (m_json->isArray())
		data = (unsigned long)(*m_json)[m_json_array_reading_index++].asUInt64();
	else
		data = (unsigned long)(*m_json)[name].asUInt64();
}

void json_archive::read_impl(const CString& name, long long& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asInt64();
	else
		data = (*m_json)[name].asInt64();
}

void json_archive::read_impl(const CString& name, unsigned long long& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asUInt64();
	else
		data = (*m_json)[name].asUInt64();
}

void json_archive::read_impl(const CString& name, float& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asFloat();
	else
		data = (*m_json)[name].asFloat();
}

void json_archive::read_impl(const CString& name, double& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asDouble();
	else
		data = (*m_json)[name].asDouble();
}

void json_archive::read_impl(const CString& name, long double& data)
{
	if (m_json->isArray())
		data = (*m_json)[m_json_array_reading_index++].asDouble();
	else
		data = (*m_json)[name].asDouble();
}

void json_archive::read_impl(const CString& name, CString& data)
{
	string str_utf_8;
	if (m_json->isArray())
		str_utf_8 = (*m_json)[m_json_array_reading_index++].asCString();
	else
		str_utf_8 = (*m_json)[string_tools::CString_to_string(name).c_str()].asCString();

	data = string_tools::utf8_to_CString(str_utf_8);
}

void json_archive::read_impl(const CString& name, char*& buffer, long& byte_count)
{
	safe_delete_array(buffer);
	byte_count = 0;

	CString buffer_text;
	read_impl(name, buffer_text);

	vector<char> output_buffer;
	base64_tools::decode_string_to_buffer(buffer_text, output_buffer);
	if (output_buffer.empty()) return;

	byte_count = output_buffer.size();
	buffer = new char[output_buffer.size()];
	for (unsigned int i = 0; i < output_buffer.size(); ++i)
		buffer[i] = output_buffer[i];
}

void json_archive::write_impl(const CString& name, const serialize_flag& data)
{
	if (name.IsEmpty()) return;

	if (serialize_flag::sub_obj_bg == data)
	{
		Json::Value new_item(Json::objectValue);
		if (m_json->isArray())
		{
			// 数据中放对象时也要记住前一个数组对象
			Json::Value& tmp_json = m_json->append(new_item);
			m_parent_nodes.push(make_pair(m_json, 0)); // 写入时不需要索引，直接给0
			m_json = &tmp_json;
		}
		//else
		{
			Json::Value new_item(Json::objectValue);

			m_parent_nodes.push(make_pair(m_json, 0)); // 写入时不需要索引，直接给0
			(*m_json)[name] = new_item;
			m_json = &((*m_json)[name]);
		}
	}
	else if (serialize_flag::sub_obj_ed == data)
	{
		// 数组时也压入元素了，因此也要弹出
		if (!m_parent_nodes.empty()/* && !m_json->isArray()*/)
		{
			int count = 2;
			for (int i = 0; i < count;++i)
			{
				pair<Json::Value *, int> top_item = m_parent_nodes.top();
				m_json = top_item.first;
				m_json_array_reading_index = top_item.second;
				m_parent_nodes.pop();
			}


		}
	}
	else if (serialize_flag::array_bg == data)
	{
		Json::Value new_item(Json::arrayValue);
		if (m_json->isArray())
		{
			// 数据中放对象时也要记住前一个数组对象
			Json::Value& tmp_json = m_json->append(new_item);
			m_parent_nodes.push(make_pair(m_json, 0)); // 写入时不需要索引，直接给0
			m_json = &tmp_json;
			m_json_array_reading_index = 0;
		}
		else
		{
			m_parent_nodes.push(make_pair(m_json, 0)); // 写入时不需要索引，直接给0
			(*m_json)[name] = new_item;
			m_json = &((*m_json)[name]);
		}
	}
	else if (serialize_flag::array_ed == data)
	{
		// 数组时也压入元素了，因此也要弹出
		if (!m_parent_nodes.empty()/* && !m_json->isArray()*/)
		{
			pair<Json::Value *, int> top_item = m_parent_nodes.top();
			m_json = top_item.first;
			m_json_array_reading_index = top_item.second;
			m_parent_nodes.pop();
		}
	}
	else
	{
		// nothing;
	}
}

void json_archive::write_impl(const CString& name, const bool& data)
{
	Json::Value new_item(data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const char& data)
{
	Json::Value new_item((int)data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const unsigned char& data)
{
	Json::Value new_item((unsigned int)data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const wchar_t& data)
{
	Json::Value new_item((int)data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const short& data)
{
	Json::Value new_item((int)data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const unsigned short& data)
{
	Json::Value new_item((unsigned int)data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const int& data)
{
	Json::Value new_item(data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const unsigned int& data)
{
	Json::Value new_item(data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const long& data)
{
	Json::Value new_item((long long)data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const unsigned long& data)
{
	Json::Value new_item((unsigned long long) data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const long long& data)
{
	Json::Value new_item(data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const unsigned long long& data)
{
	Json::Value new_item(data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const float& data)
{
	Json::Value new_item(data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const double& data)
{
	Json::Value new_item(data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const long double& data)
{
	Json::Value new_item((double)data);
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, const CString& data)
{
	// json内容必须为utf-8编码，因此要转换，但name一般约定为英文字符，因此不转码也没事
	Json::Value new_item(string_tools::CString_to_utf8(data).c_str());
	if (m_json->isArray())
		m_json->append(new_item);
	else
		(*m_json)[string_tools::CString_to_string(name).c_str()] = new_item;
}

void json_archive::write_impl(const CString& name, char* buffer, long byte_count)
{
	CString buffer_text;
	base64_tools::encode_buffer_to_string(buffer, byte_count, buffer_text);
	write(name, buffer_text);
}

void json_archive::clear_parent_nodes_cache()
{
	m_json_array_reading_index = 0;
	while (!m_parent_nodes.empty()) m_parent_nodes.pop();
}


//////////////////////////////////////////////////////////////////////////


xml_archive::xml_archive(tinyxml2::XMLElement* xml, bool is_storing/* = false*/)
	: m_xml(xml), m_is_storing(is_storing)
{

}

xml_archive::~xml_archive()
{

}

bool xml_archive::is_storing()
{
	return m_is_storing;
}

void xml_archive::read_impl(const CString& name, const serialize_flag& data)
{
	if (serialize_flag::sub_obj_bg == data)
	{
		tinyxml2::XMLElement* node_xml = nullptr;
		if (m_is_array_reading_mode && name == m_last_child_name)
		{
			if (!m_last_child_xml)
				node_xml = m_xml->FirstChildElement(name);
			else
				node_xml = m_last_child_xml->NextSiblingElement(name);
		}
		else
		{
			node_xml = m_xml->FirstChildElement(name);
		}

		if (!node_xml) return;

		// 本次要处理的对象确定后再登记到栈中
		m_last_child_xml = node_xml;
		m_last_child_name = name;
		if (m_is_array_reading_mode)
			m_parent_nodes.push(make_tuple(m_xml, true, m_last_child_xml, m_last_child_name));
		else
			m_parent_nodes.push(make_tuple(m_xml, false, nullptr, _T("")));

		// 登记后再设置为要操作的对象，以便后面操作
		m_is_array_reading_mode = false;
		m_xml = node_xml; // 注意，这里与读取数据时不一样，读取数据不用将node_xml赋给m_xml
	}
	else if (serialize_flag::sub_obj_ed == data)
	{
		auto parent = m_parent_nodes.top();
		m_xml = std::get<0>(parent);
		m_is_array_reading_mode = std::get<1>(parent);
		m_last_child_xml = std::get<2>(parent);
		m_last_child_name = std::get<3>(parent);
		m_parent_nodes.pop();
	}
	else if (serialize_flag::array_bg == data)
	{
		tinyxml2::XMLElement* node_xml = nullptr;
		if (m_is_array_reading_mode && name == m_last_child_name)
		{
			if (!m_last_child_xml)
				node_xml = m_xml->FirstChildElement(name);
			else
				node_xml = m_last_child_xml->NextSiblingElement(name);
		}
		else
		{
			node_xml = m_xml->FirstChildElement(name);
		}

		if (!node_xml) return;

		// 本次要处理的对象确定后再登记到栈中
		m_last_child_xml = node_xml;
		m_last_child_name = name;
		if (m_is_array_reading_mode)
			m_parent_nodes.push(make_tuple(m_xml, true, m_last_child_xml, m_last_child_name));
		else
			m_parent_nodes.push(make_tuple(m_xml, false, nullptr, _T("")));

		// 登记后再设置为要操作的对象，以便后面操作
		m_is_array_reading_mode = true;
		m_xml = node_xml; // 注意，这里与读取数据时不一样，读取数据不用将node_xml赋给m_xml
	}
	else if (serialize_flag::array_ed == data)
	{
		auto parent = m_parent_nodes.top();
		m_xml = std::get<0>(parent);
		m_is_array_reading_mode = std::get<1>(parent);
		m_last_child_xml = std::get<2>(parent);
		m_last_child_name = std::get<3>(parent);
		m_parent_nodes.pop();
	}
	else
	{
		// nothing;
	}
}

void xml_archive::read_impl(const CString& name, bool& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (0 == data_text.CompareNoCase(_T("true")) ? true : false);
}

void xml_archive::read_impl(const CString& name, char& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (char)(data_text.GetLength() > 0 ? data_text[0] : '\0');
}

void xml_archive::read_impl(const CString& name, unsigned char& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (unsigned char)(data_text.GetLength() > 0 ? data_text[0] : '\0');
}

void xml_archive::read_impl(const CString& name, wchar_t& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (wchar_t)(data_text.GetLength() > 0 ? data_text[0] : L'\0');
}

void xml_archive::read_impl(const CString& name, short& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (short)_ttoi(data_text);
}

void xml_archive::read_impl(const CString& name, unsigned short& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (unsigned short)_ttoi(data_text);
}

void xml_archive::read_impl(const CString& name, int& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = _ttoi(data_text);
}

void xml_archive::read_impl(const CString& name, unsigned int& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (unsigned int)_tcstoul(data_text, nullptr, 10);
}

void xml_archive::read_impl(const CString& name, long& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = _ttol(data_text);
}

void xml_archive::read_impl(const CString& name, unsigned long& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = _tcstoul(data_text, nullptr, 10);
}

void xml_archive::read_impl(const CString& name, long long& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = _ttoll(data_text);
}

void xml_archive::read_impl(const CString& name, unsigned long long& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = _tcstoull(data_text, nullptr, 10);
}

void xml_archive::read_impl(const CString& name, float& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = (float)_ttof(data_text);
}

void xml_archive::read_impl(const CString& name, double& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = _ttof(data_text);
}

void xml_archive::read_impl(const CString& name, long double& data)
{
	CString data_text;
	read_impl(name, data_text);
	data = _tcstold(data_text, nullptr);
}

void xml_archive::read_impl(const CString& name, CString& data)
{
	data = _T("");
	if (name.IsEmpty()) return;

	// 数组的内部元素读取过程中不会夹杂非数组对象的读取，因此逻辑可以简单一点
	tinyxml2::XMLElement* node_xml = nullptr;
	if (m_is_array_reading_mode && name == m_last_child_name)
	{
		if (!m_last_child_xml)
			node_xml = m_xml->FirstChildElement(name);
		else
			node_xml = m_last_child_xml->NextSiblingElement(name);

		m_last_child_xml = node_xml;
		m_last_child_name = name;
	}
	else if (m_is_array_reading_mode && name != m_last_child_name)
	{
		// 数组数据可能会放一些总个数，版本号之类的附加数据，这时虽然处于数组模式，但不是逐一往下读的，不用找兄弟节点
		node_xml = m_xml->FirstChildElement(name);
		m_last_child_xml = node_xml;
		m_last_child_name = name;
	}
	else
	{
		node_xml = m_xml->FirstChildElement(name);
		m_last_child_xml = nullptr;
		m_last_child_name = _T("");
	}

	if (!node_xml) return;

	const char* text_buffer = node_xml->GetText();
	string text = (text_buffer ? text_buffer : "");
	// safe_delete(text_buffer); // 不能删除，xml内部会处理

	xml_tree_encoding encoding = get_xml_tree_encoding(node_xml);
	if (xml_tree_encoding::utf_8 == encoding)
		data = string_tools::utf8_to_CString(text);
	else
		data = string_tools::string_to_CString(text);
}

void xml_archive::read_impl(const CString& name, char*& buffer, long& byte_count)
{
	safe_delete_array(buffer);
	byte_count = 0;

	CString buffer_text;
	read_impl(name, buffer_text);

	vector<char> output_buffer;
	base64_tools::decode_string_to_buffer(buffer_text, output_buffer);
	if (output_buffer.empty()) return;

	byte_count = output_buffer.size();
	buffer = new char[output_buffer.size()];
	for (unsigned int i = 0; i < output_buffer.size(); ++i)
		buffer[i] = output_buffer[i];
}

void xml_archive::write_impl(const CString& name, const serialize_flag& data)
{
	if (!m_xml) return;

	tinyxml2::XMLDocument* doc = m_xml->GetDocument();
	if (!doc) return;

	XMLNode* parent_node = m_xml->Parent();
	XMLElement* parent_xml = parent_node ? parent_node->ToElement() : nullptr;

	if (serialize_flag::sub_obj_bg == data)
	{
		XMLElement* new_node = doc->NewElement(string_tools::CString_to_string(name).c_str());
		if (!new_node) return;

		m_xml->InsertEndChild(new_node);
		m_xml = new_node;
	}
	else if (serialize_flag::sub_obj_ed == data)
	{
		if (parent_xml) m_xml = parent_xml;
	}
	else if (serialize_flag::array_bg == data)
	{
		// 创建数组本身
		XMLElement* new_node = doc->NewElement(string_tools::CString_to_string(name).c_str());
		if (!new_node) return;

		m_xml->InsertEndChild(new_node);
		m_xml = new_node;
	}
	else if (serialize_flag::array_ed == data)
	{
		if (parent_xml) m_xml = parent_xml;
	}
	else
	{
		// nothing;
	}
}

void xml_archive::write_impl(const CString& name, const bool& data)
{
	CString data_text = (data ? _T("true") : _T("false"));
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const char& data)
{
	CString data_text;
	data_text += (TCHAR)data;
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const unsigned char& data)
{
	CString data_text;
	data_text += (TCHAR)data;
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const wchar_t& data)
{
	CString data_text;
	data_text += (TCHAR)data;
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const short& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const unsigned short& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const int& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const unsigned int& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const long& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const unsigned long& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const long long& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const unsigned long long& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const float& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const double& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const long double& data)
{
	CString data_text = string_tools::num_to_string(data);
	write_impl(name, data_text);
}

void xml_archive::write_impl(const CString& name, const CString& data)
{
	string name_str;
	string data_str;
	xml_tree_encoding encoding = get_xml_tree_encoding(m_xml);
	if (xml_tree_encoding::utf_8 == encoding)
	{
		name_str = string_tools::CString_to_utf8(name);
		data_str = string_tools::CString_to_utf8(data);
	}
	else if (xml_tree_encoding::gb_2312 == encoding)
	{
		name_str = string_tools::CString_to_string(name);
		data_str = string_tools::CString_to_string(data);
	}
	else
	{
		name_str = string_tools::CString_to_string(name);
		data_str = string_tools::CString_to_string(data);
	}

	// xml格式为了与json匹配，且适应大数据对象，所有数据均写到新节点的value中，而不是当前节点的attribute中
	// xml当序列化数组的时候，子节点的节点名均相同，此时按同名的兄弟节点添加到末尾，否则按子节点添加
	// m_parent_xml->SetAttribute(string_tools::CString_to_string(name).c_str(), data_str.c_str());
	tinyxml2::XMLDocument* doc = (m_xml ? m_xml->GetDocument() : nullptr);
	if (!doc) return;

	tinyxml2::XMLElement* new_node = doc->NewElement(name_str.c_str());
	if (!new_node) return;

	new_node->InsertEndChild(doc->NewText(data_str.c_str()));
	m_xml->InsertEndChild(new_node);
}

void xml_archive::write_impl(const CString& name, char* buffer, long byte_count)
{
	CString buffer_text;
	base64_tools::encode_buffer_to_string(buffer, byte_count, buffer_text);
	write(name, buffer_text);
}

x_object_flag_serialization_guard::x_object_flag_serialization_guard(CString name, member_rw_interface& mt)
{
	m_name = name;
	m_mt = &mt;

	if (m_mt->is_storing())
		m_mt->write(m_name, serialize_flag::sub_obj_bg);
	else
		m_mt->read(m_name, serialize_flag::sub_obj_bg);
}

x_object_flag_serialization_guard::~x_object_flag_serialization_guard()
{
	if (m_mt->is_storing())
		m_mt->write(m_name, serialize_flag::sub_obj_ed);
	else
		m_mt->read(m_name, serialize_flag::sub_obj_ed);
}

x_array_flag_serialization_guard::x_array_flag_serialization_guard(CString name, member_rw_interface& mt)
{
	m_name = name;
	m_mt = &mt;

	if (m_mt->is_storing())
		m_mt->write(m_name, serialize_flag::array_bg);
	else
		m_mt->read(m_name, serialize_flag::array_bg);
}

x_array_flag_serialization_guard::~x_array_flag_serialization_guard()
{
	if (m_mt->is_storing())
		m_mt->write(m_name, serialize_flag::array_ed);
	else
		m_mt->read(m_name, serialize_flag::array_ed);
}

x_data_wrapper_buffer::x_data_wrapper_buffer(CString name, char*& buffer, long& byte_count)
	: m_name(name), m_buffer(buffer), m_byte_count(byte_count)
{

}

CString x_data_wrapper_buffer::get_name() const
{
	return m_name;
}

char*& x_data_wrapper_buffer::get_buffer() const
{
	return m_buffer;
}

long& x_data_wrapper_buffer::get_byte_count() const
{
	return m_byte_count;
}
