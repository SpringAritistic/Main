#include "stdafx.h"
#include <algorithm>
#include <string>
#include "../tools/compare_tools.h"
#include "../tools/string_tools.h"

#include "db_adapter_interface.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


db_field::db_field(type tp/* = type::tp_integer*/)
{
	m_value_type = tp;
	m_value = nullptr;
}

db_field::db_field(const db_field& src)
{
	m_value_type = type::tp_integer;
	m_value = nullptr;
	*this = src;
}

db_field::db_field(db_field&& src)
{
	m_value_type = type::tp_integer;
	m_value = nullptr;
	*this = std::move(src);
}

db_field::~db_field()
{
	destory_value();
}

db_field::type db_field::get_type() const
{
	return m_value_type;
}

void db_field::get_value(bool& val) const
{
	unsigned long long tmp = get_value<unsigned long long>();
	val = (0ull != tmp);
}

void db_field::get_value(std::pair<size_t, char*>& val) const
{
	if (type::tp_buffer == m_value_type)
	{
		val = *((pair<size_t, char*>*) m_value);
		return;
	}

	val = make_pair(0, nullptr);
}

void db_field::get_value(CString& val) const
{
	if (type::tp_buffer == m_value_type)
	{
		if (!m_value)
		{
			val = _T("");
			return;
		}

		pair<size_t, char*>& val_buffer = *((pair<size_t, char*>*) m_value);
		if (!val_buffer.second || 0 == val_buffer.first)
		{
			val = _T("");
			return;
		}

		string buffer_text(val_buffer.first, '\0');
		for (size_t i = 0; i < val_buffer.first; ++i)
			buffer_text[i] = val_buffer.second[i];

		val = string_tools::utf8_to_CString(buffer_text);
		return;
	}

	if (type::tp_integer == m_value_type)
	{
		unsigned long long tmp = get_value<unsigned long long>();
		val = string_tools::num_to_string(tmp);
		return;
	}

	if (type::tp_real == m_value_type)
	{
		long double tmp = get_value<long double>();
		val = string_tools::num_to_string(tmp, 8);
		return;
	}

	if (!m_value)
	{
		val = _T("");
		return;
	}

	val = *((CString*) m_value);
}

void db_field::get_value(long double& val) const
{
	if (type::tp_buffer == m_value_type)
	{
		if (!m_value)
		{
			val = 0.0;
			return;
		}

		pair<size_t, char*>& val_buffer = *((pair<size_t, char*>*) m_value);
		if (!val_buffer.second || 0 == val_buffer.first)
		{
			val = 0.0;
			return;
		}

		int ld_size = sizeof(long double);
		vector<char> num_buf(ld_size);
		for (int i = min((int)val_buffer.first, ld_size); i >= 0; --i)
			num_buf[i] = val_buffer.second[i];

		val = *(reinterpret_cast<long double*>(&num_buf[0]));
		return;
	}

	if (type::tp_string == m_value_type)
	{
		CString tmp = get_value<CString>();
		TCHAR* res = nullptr;
		val = _tcstold(tmp, &res);
		return;
	}

	if (type::tp_integer == m_value_type)
	{
		unsigned long long tmp = get_value<unsigned long long>();
		val = (long double)tmp;
		return;
	}

	if (!m_value)
	{
		val = 0.0;
		return;
	}

	val = *((long double*) m_value);
}

void db_field::get_value(unsigned long long& val) const
{
	if (type::tp_buffer == m_value_type)
	{
		val = 0ull;
		return;

		if (!m_value)
		{
			val = 0ull;
			return;
		}

		pair<size_t, char*>& val_buffer = *((pair<size_t, char*>*) m_value);
		if (!val_buffer.second || 0 == val_buffer.first)
		{
			val = 0ull;
			return;
		}

		int ld_size = sizeof(unsigned long long);
		vector<char> num_buf(ld_size);
		for (int i = min((int)val_buffer.first, ld_size); i >= 0; --i)
			num_buf[i] = val_buffer.second[i];

		val = *(reinterpret_cast<unsigned long long*>(&num_buf[0]));
		return;
	}

	if (type::tp_string == m_value_type)
	{
		CString tmp = get_value<CString>();
		TCHAR* res = nullptr;
		val = _ttoll(tmp);
		return;
	}

	if (type::tp_real == m_value_type)
	{
		long double tmp = get_value<long double>();
		val = (unsigned long long)tmp;
		return;
	}

	if (!m_value)
	{
		val = 0ull;
		return;
	}

	val = *((unsigned long long*) m_value);
}

void db_field::get_value(double& val) const
{
	val = (double) (get_value<long double>());
}

void db_field::get_value(float& val) const
{
	val = (float) (get_value<long double>());
}

void db_field::get_value(long long& val) const
{
	val = (long long) (get_value<unsigned long long>());
}

void db_field::get_value(unsigned long& val) const
{
	val = (unsigned long) (get_value<unsigned long long>());
}

void db_field::get_value(long& val) const
{
	val = (long) (get_value<unsigned long long>());
}

void db_field::get_value(unsigned int& val) const
{
	val = (unsigned int) (get_value<unsigned long long>());
}

void db_field::get_value(int& val) const
{
	val = (int) (get_value<unsigned long long>());
}

void db_field::get_value(unsigned short& val) const
{
	val = (unsigned short) (get_value<unsigned long long>());
}

void db_field::get_value(short& val) const
{
	val = (short) (get_value<unsigned long long>());
}

void db_field::get_value(wchar_t& val) const
{
	val = (wchar_t) (get_value<unsigned long long>());
}

void db_field::get_value(unsigned char& val) const
{
	val = (unsigned char) (get_value<unsigned long long>());
}

void db_field::get_value(char& val) const
{
	val = (char) (get_value<unsigned long long>());
}

void db_field::set_value(bool val)
{
	set_value((unsigned long long)(val ? 1 : 0));
}

void db_field::set_value(char val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(unsigned char val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(wchar_t val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(short val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(unsigned short val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(int val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(unsigned int val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(long val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(unsigned long val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(long long val)
{
	set_value((unsigned long long)val);
}

void db_field::set_value(unsigned long long val)
{
	destory_value();
	m_value_type = type::tp_integer;
	unsigned long long* buffer = new unsigned long long();
	*buffer = val;
	m_value = buffer;
}

void db_field::set_value(float val)
{
	set_value((long double)val);
}

void db_field::set_value(double val)
{
	set_value((long double) val);
}

void db_field::set_value(long double val)
{
	destory_value();
	m_value_type = type::tp_real;
	long double* buffer = new long double();
	*buffer = val;
	m_value = buffer;
}

void db_field::set_value(const CString& val)
{
	destory_value();
	m_value_type = type::tp_string;
	CString* buffer = new CString();
	*buffer = val;
	m_value = buffer;
}

void db_field::set_value(const std::pair<size_t, const char*>& val, bool clone_buffer/* = true*/)
{
	destory_value();
	m_value_type = type::tp_buffer;
	pair<size_t, char*>* buffer = new pair<size_t, char*>();
	buffer->first = 0;
	buffer->second = nullptr;

	if (val.first > 0 && val.second)
	{
		buffer->first = val.first;
		if (clone_buffer)
		{
			buffer->second = new char[val.first];
			memcpy_s(buffer->second, buffer->first, val.second, val.first);
		}
		else
		{
			buffer->second = (char*)val.second;
		}
	}

	m_value = buffer;
}

void db_field::destory_value()
{
	if (type::tp_integer == m_value_type)
	{
		delete ((unsigned long long*)m_value);
		m_value = nullptr;
	}
	else if (type::tp_real == m_value_type)
	{
		delete ((long double*)m_value);
		m_value = nullptr;
	}
	else if (type::tp_string == m_value_type)
	{
		delete ((CString*)m_value);
		m_value = nullptr;
	}
	else if (type::tp_buffer == m_value_type)
	{
		pair<int, char*>* buffer = (pair<int, char*>*)m_value;
		delete[] buffer->second;
		m_value = nullptr;
	}
	else
	{
		m_value_type = type::tp_integer;
		m_value = nullptr;
	}
}

bool db_field::operator!=(const db_field& src) const
{
	return !(*this == src);
}

bool db_field::operator==(const db_field& src) const
{
	if (m_value_type != src.m_value_type) return false;

	if (nullptr == m_value && nullptr == src.m_value) return true;
	if (nullptr == m_value && nullptr != src.m_value) return false;
	if (nullptr != m_value && nullptr == src.m_value) return false;

	if (type::tp_integer == m_value_type)
	{
		return *((unsigned long long*)m_value) == *((unsigned long long*)src.m_value);
	}
	else if (type::tp_real == m_value_type)
	{
		// 精度为-12次方
		return compare_tools::is_equal(*((long double*)m_value), *((long double*)src.m_value), 1.0e-12);
	}
	else if (type::tp_string == m_value_type)
	{
		return *((CString*)m_value) == *((CString*)src.m_value);
	}
	else if (type::tp_buffer == m_value_type)
	{
		pair<int, char*>* buffer = (pair<int, char*>*)m_value;
		pair<int, char*>* buffer_src = (pair<int, char*>*)src.m_value;
		if (!buffer->first != buffer_src->first) return false;
		if (0 == buffer->first && 0 == buffer_src->first) return true;
		if (buffer->first < 0 || buffer_src->first < 0) return false; // 内部数据错误返回false

		for (int i = 0; i < buffer->first; ++i)
			if (buffer->second[i] != buffer_src->second[i]) return false;

		return true;
	}
	else
	{
		return false; // 内部数据错误返回false
	}
}

db_field& db_field::operator=(db_field&& src)
{
	if (this == &src) return *this;

	destory_value();
	m_value_type = src.m_value_type;
	m_value = src.m_value;
	src.m_value = nullptr;	

	return *this;
}

db_field& db_field::operator=(const db_field& src)
{
	if (this == &src) return *this;

	destory_value();

	if (type::tp_integer == src.m_value_type)
	{
		m_value_type = src.m_value_type;
		unsigned long long* buffer = new unsigned long long();
		if (src.m_value)
			*buffer = *((unsigned long long*)src.m_value);
		else
			*buffer = 0ull;

		m_value = buffer;
	}
	else if (type::tp_real == src.m_value_type)
	{
		m_value_type = src.m_value_type;
		long double* buffer = new long double();
		if (src.m_value)
			*buffer = *((long double*)src.m_value);
		else
			*buffer = 0.0;

		m_value = buffer;
	}
	else if (type::tp_string == src.m_value_type)
	{
		m_value_type = src.m_value_type;
		CString* buffer = new CString();
		if (src.m_value)
			*buffer = *((CString*)src.m_value);

		m_value = buffer;
	}
	else if (type::tp_buffer == src.m_value_type)
	{
		pair<int, char*>* buffer = (pair<int, char*>*)src.m_value;

		m_value_type = src.m_value_type;
		pair<int, char*>* new_buffer = new pair<int, char*>();
		
		if (buffer)
		{
			new_buffer->first = buffer->first;
			new_buffer->second = nullptr;

			if (buffer->first > 0)
			{
				new_buffer->second = new char[buffer->first];
				memcpy_s(new_buffer->second, new_buffer->first, buffer->second, buffer->first);
			}
		}
		else
		{
			new_buffer->first = 0;
			new_buffer->second = nullptr;
		}

		m_value = new_buffer;
	}
	else
	{
		m_value_type = type::tp_integer;
		unsigned long long* buffer = new unsigned long long();
		buffer = 0ull;
		m_value = buffer;
	}

	return *this;
}


db_record::db_record()
{

}

db_record::~db_record()
{

}

db_field* db_record::field(const CString& name)
{
	auto it = find_if(m_row.begin(), m_row.end(), [&](pair<CString, db_field>& cur)
	{
		return 0 == name.Compare(cur.first);
	});

	if (it != m_row.end())
		return &(it->second);
	else
		return nullptr;
}


db_field* db_record::field(unsigned int index)
{
	if (m_row.size() <= index)
		return nullptr;
	else
		return &m_row[index].second;
}

//////////////////////////////////////////////////////////////////////////


db_adapter_interface::db_adapter_interface()
{

}

db_adapter_interface::~db_adapter_interface()
{

}

bool db_adapter_interface::has_query_result(const CString& sql)
{
	return get_query_result_count(sql) > 0ull;
}

bool db_adapter_interface::is_select_sql(const CString& sql)
{
	CString sel_string = _T("select");
	CString tmp_header = sql.Left(sel_string.GetLength());
	return (0 == sel_string.CompareNoCase(tmp_header));
}


//////////////////////////////////////////////////////////////////////////


class temp_db_adapter : public db_adapter_interface
{
public:
	temp_db_adapter() {}
	~temp_db_adapter() {}

public:
	virtual bool test_connection() const
	{
		return false;
	}

	virtual bool has_query_result(const CString& sql)
	{
		return false;
	}

	virtual unsigned long long get_query_result_count(const CString& sql)
	{
		return 0ull;
	}

	virtual std::pair<bool, CString> get_query_result(std::vector<db_record>& result, const CString& sql)
	{
		result.clear();
		return make_pair(true, _T(""));
	}

	virtual std::pair<bool, CString> run_as_a_batch(std::vector<const CString>& all_sql)
	{
		return make_pair(false, _T("没有可用的数据库操作实例"));
	}

	virtual std::pair<bool, CString> get_batch_result(std::vector<const CString>& all_sql, std::vector<db_record>& result)
	{
		return make_pair(false, _T("没有可用的数据库操作实例"));
	}

	virtual std::pair<bool, CString> run_alone(const CString& all_sql)
	{
		return make_pair(false, _T("没有可用的数据库操作实例"));
	}
};


//////////////////////////////////////////////////////////////////////////


static db_adapter_interface* global_adapter = nullptr;


void regist_database_adapter(db_adapter_interface* adapter)
{
	global_adapter = adapter;
}

db_adapter_interface* get_database_adapter()
{
	// 注意：请保证此接口及调用的功能均能正确支持并发访问
	//////////////////////////////////////////////////////////////////////////

	static unique_ptr<temp_db_adapter> tmp_adapter = make_unique<temp_db_adapter>();

	if (!global_adapter)
		return tmp_adapter.get(); // 虽然返回的静态对象，但内部实现没有数据需要同步，因此也是支持并发的
	else
		return global_adapter;
}
