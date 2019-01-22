#pragma once

#ifndef __HMGLOBALTOOL_INTERFACE_SERIALIZE_INTERFACE_H__
#define __HMGLOBALTOOL_INTERFACE_SERIALIZE_INTERFACE_H__



#include <array>
#include <vector>
#include <list>
#include <stack>
#include <map>

#include "../tools/string_tools.h"
#include "../tools/xml_tools.h"
#include "../tools/json.h"
//#include "../macro.h"


class member_rw_interface;


//////////////////////////////////////////////////////////////////////////


// 需要支持序列化的数据结构均需从此接口派生
// 没有特殊情况（如数据中心是注册方式工作的，不用写序列化宏），派生类都要写序列化宏，否则无法正确反序列化
class HM_GlobalTool_EXT serialize_interface
{
public:
	serialize_interface();
	virtual ~serialize_interface();

public:
	virtual void serialize_members(member_rw_interface& mt);
};


//////////////////////////////////////////////////////////////////////////


// 序列化数据标记（务必明确给定数值，以及以后扩展及调整顺序导致序列化失败，序列化时强转为int型，注意不要超出整数范围）
enum class serialize_flag
{
	sub_obj_bg = 0,		// 子节点开始
	sub_obj_ed = 1,		// 子节点结束
	array_bg = 2,		// 数组开始
	array_ed = 3		// 数组结束
};


//////////////////////////////////////////////////////////////////////////


// 将两个参数包装成一个参数，以便于&符号重载使用的时候直接可以给传两个参数就构造成一个对象，使用者不用关系对象的组装
// 注意，由于本对象要同时适用于数据读取和写入，因此，传入的数据对象不可以是右值（包括字面值），必须为左
// 值，如果确实有需要则请直接使用序列化读写接口
template <typename T>
class x_data_wrapper
{
public:
	// 禁止隐式转换防止与序列化其它接口造成二义性
	// 为了配合x_wrapper_macro使用，防止对象过早析构导致内部存的临时对象指针无效，必须用引用，而name则
	// 可能为自动生成，本身就可能是临时对象，因此不能存为引用，否则也会有一样的问题
	explicit x_data_wrapper(CString name, T& data)
	{
		m_name = name;
		m_data = &data;
	}

	~x_data_wrapper() = default;

public:
	CString get_name() const { return m_name; }
	T* get_data() const { return m_data; }

private:
	CString m_name;
	T* m_data = nullptr;
};


// 参见宏x_wrapper_macro_named_enum
template <typename T>
class x_data_wrapper_enum
{
public:
	explicit x_data_wrapper_enum(CString name, T& data)
	{
		m_name = name;
		m_data = &data;
	}

public:
	CString get_name() const { return m_name; }
	T* get_data() const { return m_data; }

private:
	CString m_name;
	T* m_data = nullptr;
};

// 参见宏x_wrapper_macro_named_buffer
class HM_GlobalTool_EXT x_data_wrapper_buffer
{
public:
	// 同时用于读写，长度不支持字面值，写的时候要正确设置好byte_count，读的时候，值由内部赋上buffer读取的长度
	explicit x_data_wrapper_buffer(CString name, char*& buffer, long& byte_count);

public:
	CString get_name() const;
	char*& get_buffer() const;
	long& get_byte_count() const;

private:
	CString m_name;
	char*& m_buffer;
	long& m_byte_count;
};


// 对象构造时写入serialize_flag::sub_obj_bg标记，析构时写入serialize_flag::sub_obj_ed标记
// 一般用于类的序列化函数开始处，参数为序列化对象时使用的标识名（同一作用域不能相同），第二个参数为member_rw_interface对象mt
class HM_GlobalTool_EXT x_object_flag_serialization_guard
{
public:
	explicit x_object_flag_serialization_guard(CString name, member_rw_interface& mt);
	~x_object_flag_serialization_guard();
	x_object_flag_serialization_guard(const x_object_flag_serialization_guard&) = delete;
	void operator=(const x_object_flag_serialization_guard&) const = delete;

private:
	CString m_name;
	member_rw_interface* m_mt = nullptr;
	bool   is_storing;
};


// 对象构造时写入serialize_flag::array_bg标记，析构时写入serialize_flag::array_ed标记
// 参数为序列化对象时使用的标识名（同一作用域不能相同），第二个参数为member_rw_interface对象mt
class HM_GlobalTool_EXT x_array_flag_serialization_guard
{
public:
	explicit x_array_flag_serialization_guard(CString name, member_rw_interface& mt);
	~x_array_flag_serialization_guard();
	x_array_flag_serialization_guard(const x_array_flag_serialization_guard&) = delete;
	void operator=(const x_array_flag_serialization_guard&) const = delete;

private:
	CString m_name;
	member_rw_interface* m_mt = nullptr;
};



// 提供便利性的宏，简化常规数据的读写以及读写代码保持一致，强烈建议使用此方法实现业务层的序列化，而
// 不是使用read,write接口！！！示例如下：
// mt & x_wrapper_macro(a) & x_data_wrapper<int>("name", b); // 使用&可以连续序列化
// pair<int, CString> test_pair;
// mt.read(x_wrapper_macro_named(_T("pair"), test_pair)); // 也可能使用读写接口
// 注意，由于以下两个宏要同时适用于数据读取和写入，因此，传入的数据对象不可以是右值（包括字面值），必须为左
// 值，如果确实有需要则请直接使用序列化读写接口
#define x_wrapper_macro_named(n, x) x_data_wrapper<decltype(x)>((n),(x))
#define x_wrapper_macro(x) x_wrapper_macro_named(#x, x)
#define x_wrapper_macro_named_enum(n, x) x_data_wrapper_enum<decltype(x)>((n),(x))
#define x_wrapper_macro_enum(x) x_wrapper_macro_named_enum(#x, x)
#define x_wrapper_macro_named_buffer(n, x, c) x_data_wrapper_buffer((n),(x),(c))
#define x_wrapper_macro_buffer(x, c) x_wrapper_macro_named_buffer(#x,x,c)


//////////////////////////////////////////////////////////////////////////


// 成员读写的接口，不同的存取方式需派生，如使用json存储要从此接口派生，同理，二进制存储或mfc的CArchive以
// 及xml格式等均要派生于此接口
// 注意，此接口不局限于以文件形式操作，保存为内存对象或将内存对象转储为文件均可使用此接口
// 强烈建议使用前面提供的简易包装宏实现业务层的序列化，而不是使用read,write接口！！！
// 示例如下：
// mt & x_wrapper_macro(123.456) & x_data_wrapper<int>("name", 001); // 使用&可以连续序列化
// pair<int, CString> test_pair;
// mt.read(x_wrapper_macro_named(_T("pair"), test_pair)); // 也可能使用读写接口
class HM_GlobalTool_EXT member_rw_interface
{
public:
	virtual bool is_storing() = 0;
	inline bool is_loading() { return !is_storing(); } // 此接口不需要定义为virtual的

	// 重载&运算符，以便利用boost库序列化的代码减少移植代价，以及序列化代码的批量生成
	// 用法主要有两种，示例如下：
	// mt & x_wrapper_macro(aaaaa) & x_wrapper_macro(123.456) & x_data_wrapper<int>("name", 001);
	template <typename T>
	member_rw_interface& operator&(x_data_wrapper<T>& data)
	{
		if (is_storing())
			write(data);
		else
			read(data);

		return *this;
	}

	template <typename T>
	member_rw_interface& operator&(x_data_wrapper_enum<T>& data)
	{
		if (is_storing())
		{
			write(data.get_name(), (long long) (*data.get_data())); // 先取值再转为类型，不要先转类型再取值
		}
		else
		{
			long long tmp = 0;
			read(data.get_name(), tmp);
			*data.get_data() = (std::remove_reference<T>::type)tmp;
		}

		return *this;
	}

	member_rw_interface& operator&(x_data_wrapper_buffer& data)
	{
		if (is_storing())
			write(data.get_name(), data.get_buffer(), data.get_byte_count());
		else
			read(data.get_name(), data.get_buffer(), data.get_byte_count());

		return *this;
	}

public:
	//////////////////////////////////////////////////////////////////////////
	// 常规函数版本

	// 由于json格式的限制，读取时必须指定节点名，xml和二进制方式则可以不用，但为了统一，强制要求传名称
	// json的限制之一是大部分库写入的节点内部使用map存储，最终序列化到文件会变成一个有序顺序，以及它不能便利的向上回溯等限制

	void read(const CString& name, const serialize_flag& data); // 标志不会修改，因此为const
	void read(const CString& name, bool& data);
	void read(const CString& name, char& data);
	void read(const CString& name, unsigned char& data);
	void read(const CString& name, wchar_t& data);
	void read(const CString& name, short& data);
	void read(const CString& name, unsigned short& data);
	void read(const CString& name, int& data);
	void read(const CString& name, unsigned int& data);
	void read(const CString& name, long& data);
	void read(const CString& name, unsigned long& data);
	void read(const CString& name, long long& data);
	void read(const CString& name, unsigned long long& data);
	void read(const CString& name, float& data);
	void read(const CString& name, double& data);
	void read(const CString& name, long double& data);
	void read(const CString& name, CString& data);
	void read(const CString& name, std::string& data);
	void read(const CString& name, std::wstring& data);
	
	// buffer由内部使用new分配并由外部使用的人负责管理（连续内存），length返回解码后的buffer字节数
	void read(const CString& name, char*& buffer, long& byte_count);

	void write(const CString& name, const serialize_flag& data);
	void write(const CString& name, const bool& data);
	void write(const CString& name, const char& data);
	void write(const CString& name, const unsigned char& data);
	void write(const CString& name, const wchar_t& data);
	void write(const CString& name, const short& data);
	void write(const CString& name, const unsigned short& data);
	void write(const CString& name, const int& data);
	void write(const CString& name, const unsigned int& data);
	void write(const CString& name, const long& data);
	void write(const CString& name, const unsigned long& data);
	void write(const CString& name, const long long& data);
	void write(const CString& name, const unsigned long long& data);
	void write(const CString& name, const float& data);
	void write(const CString& name, const double& data);
	void write(const CString& name, const long double& data);
	void write(const CString& name, const CString& data);
	void write(const CString& name, const std::string& data);
	void write(const CString& name, const std::wstring& data);

	// 写入时要支持字符串字面值，读取时则不用，通通转为CString进行写入操作
	void write(const CString& name, const char* data);
	void write(const CString& name, const wchar_t* data);
	void write(const CString& name, char* buffer, long byte_count); // buffer由外部分配好


	//////////////////////////////////////////////////////////////////////////
	// 泛化版本
	// 对于定义类或其它未在本接口中枚举出的类要支持读写则需要从serialize_interface接口派生并实现其接口

	template <typename T>
	inline void read(const CString& name, T& data)
	{
		read_impl(name, data);
	}

	template <typename T>
	inline void write(const CString& name, const T& data)
	{
		write_impl(name, data);
	}

	template <typename T>
	inline void read(x_data_wrapper<T>& data)
	{
		if (!data.get_name().IsEmpty() && data.get_data())
			read(data.get_name(), *data.get_data());
	}

	template <typename T>
	inline void write(x_data_wrapper<T>& data)
	{
		if (!data.get_name().IsEmpty() && data.get_data())
			write(data.get_name(), *data.get_data());
	}


protected:
	// 主要用于json\xml等格式的序列化，值用于内部标记，不会把数据存入标记，为扩大泛型接口匹配范围，用const引用
	virtual inline void read_impl(const CString& name, const serialize_flag& data) = 0;

	virtual inline void read_impl(const CString& name, bool& data) = 0;
	virtual inline void read_impl(const CString& name, char& data) = 0;
	virtual inline void read_impl(const CString& name, unsigned char& data) = 0;
	virtual inline void read_impl(const CString& name, wchar_t& data) = 0;
	virtual inline void read_impl(const CString& name, short& data) = 0;
	virtual inline void read_impl(const CString& name, unsigned short& data) = 0;
	virtual inline void read_impl(const CString& name, int& data) = 0;
	virtual inline void read_impl(const CString& name, unsigned int& data) = 0;
	virtual inline void read_impl(const CString& name, long& data) = 0;
	virtual inline void read_impl(const CString& name, unsigned long& data) = 0;
	virtual inline void read_impl(const CString& name, long long& data) = 0;
	virtual inline void read_impl(const CString& name, unsigned long long& data) = 0;
	virtual inline void read_impl(const CString& name, float& data) = 0;
	virtual inline void read_impl(const CString& name, double& data) = 0;
	virtual inline void read_impl(const CString& name, long double& data) = 0;
	virtual inline void read_impl(const CString& name, CString& data) = 0;

	// buffer由内部使用new分配并由外部使用的人负责管理（连续内存），length返回解码后的buffer字节数
	virtual inline void read_impl(const CString& name, char*& buffer, long& byte_count) = 0;


	// 主要用于json\xml等格式的序列化，值用于内部标记，对外没有使用价值，为扩大泛型接口匹配范围，用const引用
	virtual inline void write_impl(const CString& name, const serialize_flag& data) = 0;

	virtual inline void write_impl(const CString& name, const bool& data) = 0;
	virtual inline void write_impl(const CString& name, const char& data) = 0;
	virtual inline void write_impl(const CString& name, const unsigned char& data) = 0;
	virtual inline void write_impl(const CString& name, const wchar_t& data) = 0;
	virtual inline void write_impl(const CString& name, const short& data) = 0;
	virtual inline void write_impl(const CString& name, const unsigned short& data) = 0;
	virtual inline void write_impl(const CString& name, const int& data) = 0;
	virtual inline void write_impl(const CString& name, const unsigned int& data) = 0;
	virtual inline void write_impl(const CString& name, const long& data) = 0;
	virtual inline void write_impl(const CString& name, const unsigned long& data) = 0;
	virtual inline void write_impl(const CString& name, const long long& data) = 0;
	virtual inline void write_impl(const CString& name, const unsigned long long& data) = 0;
	virtual inline void write_impl(const CString& name, const float& data) = 0;
	virtual inline void write_impl(const CString& name, const double& data) = 0;
	virtual inline void write_impl(const CString& name, const long double& data) = 0;
	virtual inline void write_impl(const CString& name, const CString& data) = 0;
	virtual inline void write_impl(const CString& name, char* buffer, long byte_count) = 0; // buffer由外部分配好

protected:
	//////////////////////////////////////////////////////////////////////////
	// 对于已经有serialize_members函数的也可以直接序列化
	
	template <typename T>
	inline void read_impl(const CString& name, T& data) // name没有意义，只用于保持原型统一
	{
		data.serialize_members(*this);
	}

	template <typename T>
	inline void write_impl(const CString& name, const T& data) // name没有意义，只用于保持原型统一
	{
		const_cast<T&>(data).serialize_members(*this); // 成员函数不是const的，不能通过const对象调用
	}

	//////////////////////////////////////////////////////////////////////////
	// 对数组进行重载(如果元素是指针，则不打算支持，否则会要求元素从CObject派生，否则指针对象序列化由用户自己实现
	// 反序列化时对象是通过Runtime创建的，如果对象不是new的，而是其它对象的引用，则不支持，综合来看，指针的序列化
	// 功能基本上起不到太多实用效果，所以不提供)
	template<typename T, size_t N>
	inline void read_impl(const CString& name, std::array<T, N>& val) 
	{
		read_impl(name, serialize_flag::array_bg);

		int count = 0;
		read_impl(_T("count"), count); // name没有意义，只用于保持原型统一

		//val.clear();
		for (int i = 0; i < count && i < (int) val.size(); ++i)
			read_impl(_T("item_") + string_tools::num_to_string(i), val[i]); // tinyxml2解析时不支持节点名为数字开头,不允许带点号

		read_impl(name, serialize_flag::array_ed);
	}

	template<typename T, size_t N>
	inline void write_impl(const CString& name, const std::array<T, N>& val)
	{
		write_impl(name, serialize_flag::array_bg);

		write_impl(_T("count"), (int) (val.size())); // name没有意义，只用于保持原型统一
		for (size_t i = 0; i < val.size(); ++i)
			write_impl(_T("item_") + string_tools::num_to_string(i), val[i]); // tinyxml2解析时不支持节点名为数字开头,不允许带点号

		write_impl(name, serialize_flag::array_ed);
	}

	// vector<bool>和其它的情况不一样，会有压缩存储，因此要使用特化版本
	void read_impl(const CString& name, std::vector<bool>& val);
	void write_impl(const CString& name, const std::vector<bool>& val);

	template<typename T>
	inline void read_impl(const CString& name, std::vector<T, std::allocator<T>>& val)
	{
		read_impl(name, serialize_flag::array_bg);

		int count = 0;
		read_impl(_T("count"), count); // name没有意义，只用于保持原型统一

		val.resize(count);
		for (int i = 0; i < count; ++i)
			read_impl(_T("item_") + string_tools::num_to_string(i), val[i]); // tinyxml2解析时不支持节点名为数字开头,不允许带点号

		read_impl(name, serialize_flag::array_ed);
	}

	template<typename T>
	inline void write_impl(const CString& name, const std::vector<T, std::allocator<T>>& val)
	{
		write_impl(name, serialize_flag::array_bg);

		write_impl(_T("count"), (int) (val.size())); // name没有意义，只用于保持原型统一
		for (size_t i = 0; i < val.size(); ++i)
			write_impl(_T("item_") + string_tools::num_to_string(i), val[i]); // tinyxml2解析时不支持节点名为数字开头,不允许带点号

		write_impl(name, serialize_flag::array_ed);
	}

	template<typename T>
	inline void read_impl(const CString& name, std::list<T, std::allocator<T>>& val)
	{
		read_impl(name, serialize_flag::array_bg);

		int count = 0;
		read_impl(_T("count"), count); // name没有意义，只用于保持原型统一

		val.clear();
		for (int i = 0; i < count; ++i)
		{
			T item;
			read_impl(_T("item_") + string_tools::num_to_string(i), item); // tinyxml2解析时不支持节点名为数字开头,不允许带点号
			val.push_back(item);
		}

		read_impl(name, serialize_flag::array_ed);
	}

	template<typename T>
	inline void write_impl(const CString& name, const std::list<T, std::allocator<T>>& val) // name没有意义，只用于保持原型统一
	{
		write_impl(name, serialize_flag::array_bg);

		write_impl(_T("count"), (int) (val.size()));
		size_t index = 0;
		for (const auto& x : val)
			write_impl(_T("item_") + string_tools::num_to_string(index++), x); // tinyxml2解析时不支持节点名为数字开头,不允许带点号

		write_impl(name, serialize_flag::array_ed);
	}

	template<typename T1, typename T2>
	inline void read_impl(const CString& name, std::pair<T1, T2>& val) // name没有意义，只用于保持原型统一
	{
		// tinyxml2解析时不支持节点名为数字开头,不允许带点号
		T1 key;
		read_impl(name + _T("_key"), key);
		
		T2 value;
		read_impl(name + _T("_value"), value);

		val = std::make_pair(key, value);
	}

	template<typename T1, typename T2>
	inline void write_impl(const CString& name, const std::pair<T1, T2>& val) // name没有意义，只用于保持原型统一
	{
		// tinyxml2解析时不支持节点名为数字开头,不允许带点号
		write_impl(name + _T("_key"), val.first);
		write_impl(name + _T("_value"), val.second);
	}

	template<typename TK, typename TV>
	inline void read_impl(const CString& name,
		std::map<TK, TV, std::less<TK>, std::allocator<std::pair<const TK, TV>>>& val) // name没有意义，只用于保持原型统一
	{
		read_impl(name, serialize_flag::array_bg);

		int count = 0;
		read_impl(_T("count"), count);

		val.clear();
		for (int i = 0; i < count; ++i)
		{
			std::pair<TK, TV> item;
			read_impl(name + _T("_item_") + string_tools::num_to_string(i), item); // tinyxml2解析时不支持节点名为数字开头,不允许带点号
			val.insert(item);
		}

		read_impl(name, serialize_flag::array_ed);
	}

	template<typename TK, typename TV>
	inline void write_impl(const CString& name,
		const std::map<TK, TV, std::less<TK>, std::allocator<std::pair<const TK, TV>>>& val) // name没有意义，只用于保持原型统一
	{
		write_impl(name, serialize_flag::array_bg);

		write_impl(_T("count"), (int) (val.size()));
		size_t index = 0;
		for (const auto& x : val)
			write_impl(name + _T("_item_") + string_tools::num_to_string(index++), x); // tinyxml2解析时不支持节点名为数字开头,不允许带点号

		write_impl(name, serialize_flag::array_ed);
	}

protected:
	member_rw_interface(); // 只能由框架或派生类来构造这个对象
	member_rw_interface(const member_rw_interface& src);
	virtual ~member_rw_interface();

private:
	friend class serialize_interface;
};


//////////////////////////////////////////////////////////////////////////


class HM_GlobalTool_EXT bin_archive : public member_rw_interface
{
public:
	bin_archive(CArchive& ar);
	virtual ~bin_archive();

public:
	virtual bool is_storing();

protected:
	virtual inline void read_impl(const CString& name, const serialize_flag& data);
	virtual inline void read_impl(const CString& name, bool& data);
	virtual inline void read_impl(const CString& name, char& data);
	virtual inline void read_impl(const CString& name, unsigned char& data);
	virtual inline void read_impl(const CString& name, wchar_t& data);
	virtual inline void read_impl(const CString& name, short& data);
	virtual inline void read_impl(const CString& name, unsigned short& data);
	virtual inline void read_impl(const CString& name, int& data);
	virtual inline void read_impl(const CString& name, unsigned int& data);
	virtual inline void read_impl(const CString& name, long& data);
	virtual inline void read_impl(const CString& name, unsigned long& data);
	virtual inline void read_impl(const CString& name, long long& data);
	virtual inline void read_impl(const CString& name, unsigned long long& data);
	virtual inline void read_impl(const CString& name, float& data);
	virtual inline void read_impl(const CString& name, double& data);
	virtual inline void read_impl(const CString& name, long double& data);
	virtual inline void read_impl(const CString& name, CString& data);
	virtual inline void read_impl(const CString& name, char*& buffer, long& byte_count);

	virtual inline void write_impl(const CString& name, const serialize_flag& data);
	virtual inline void write_impl(const CString& name, const bool& data);
	virtual inline void write_impl(const CString& name, const char& data);
	virtual inline void write_impl(const CString& name, const unsigned char& data);
	virtual inline void write_impl(const CString& name, const wchar_t& data);
	virtual inline void write_impl(const CString& name, const short& data);
	virtual inline void write_impl(const CString& name, const unsigned short& data);
	virtual inline void write_impl(const CString& name, const int& data);
	virtual inline void write_impl(const CString& name, const unsigned int& data);
	virtual inline void write_impl(const CString& name, const long& data);
	virtual inline void write_impl(const CString& name, const unsigned long& data);
	virtual inline void write_impl(const CString& name, const long long& data);
	virtual inline void write_impl(const CString& name, const unsigned long long& data);
	virtual inline void write_impl(const CString& name, const float& data);
	virtual inline void write_impl(const CString& name, const double& data);
	virtual inline void write_impl(const CString& name, const long double& data);
	virtual inline void write_impl(const CString& name, const CString& data);
	virtual inline void write_impl(const CString& name, char* buffer, long byte_count);

private:
	CArchive& m_ar;
};


//////////////////////////////////////////////////////////////////////////


class HM_GlobalTool_EXT json_archive : public member_rw_interface
{
public:
	json_archive(Json::Value* json, bool is_storing = false);
	virtual ~json_archive();

public:
	virtual bool is_storing();

public:
	void clear_parent_nodes_cache();

protected:
	virtual inline void read_impl(const CString& name, const serialize_flag& data);
	virtual inline void read_impl(const CString& name, bool& data);
	virtual inline void read_impl(const CString& name, char& data);
	virtual inline void read_impl(const CString& name, unsigned char& data);
	virtual inline void read_impl(const CString& name, wchar_t& data);
	virtual inline void read_impl(const CString& name, short& data);
	virtual inline void read_impl(const CString& name, unsigned short& data);
	virtual inline void read_impl(const CString& name, int& data);
	virtual inline void read_impl(const CString& name, unsigned int& data);
	virtual inline void read_impl(const CString& name, long& data);
	virtual inline void read_impl(const CString& name, unsigned long& data);
	virtual inline void read_impl(const CString& name, long long& data);
	virtual inline void read_impl(const CString& name, unsigned long long& data);
	virtual inline void read_impl(const CString& name, float& data);
	virtual inline void read_impl(const CString& name, double& data);
	virtual inline void read_impl(const CString& name, long double& data);
	virtual inline void read_impl(const CString& name, CString& data);
	virtual inline void read_impl(const CString& name, char*& buffer, long& byte_count);

	virtual inline void write_impl(const CString& name, const serialize_flag& data);
	virtual inline void write_impl(const CString& name, const bool& data);
	virtual inline void write_impl(const CString& name, const char& data);
	virtual inline void write_impl(const CString& name, const unsigned char& data);
	virtual inline void write_impl(const CString& name, const wchar_t& data);
	virtual inline void write_impl(const CString& name, const short& data);
	virtual inline void write_impl(const CString& name, const unsigned short& data);
	virtual inline void write_impl(const CString& name, const int& data);
	virtual inline void write_impl(const CString& name, const unsigned int& data);
	virtual inline void write_impl(const CString& name, const long& data);
	virtual inline void write_impl(const CString& name, const unsigned long& data);
	virtual inline void write_impl(const CString& name, const long long& data);
	virtual inline void write_impl(const CString& name, const unsigned long long& data);
	virtual inline void write_impl(const CString& name, const float& data);
	virtual inline void write_impl(const CString& name, const double& data);
	virtual inline void write_impl(const CString& name, const long double& data);
	virtual inline void write_impl(const CString& name, const CString& data);
	virtual inline void write_impl(const CString& name, char* buffer, long byte_count);

private:
	Json::Value* m_json = nullptr; // 不能存为引用，jsoncpp库赋值的时候内部使用了swap，无法正常记住原对象
	int m_json_array_reading_index = 0; // 为json数组的时候用于记住当前读取的元素下标
	bool m_is_storing = false;
	
	// 仅临时标记对象，内存由外部处理，first为父json对象，second为父json对象的现场array_index（仅当为数组对象才有用）
	std::stack<std::pair<Json::Value*, int>> m_parent_nodes;
};


//////////////////////////////////////////////////////////////////////////


class HM_GlobalTool_EXT xml_archive : public member_rw_interface
{
public:
	xml_archive(tinyxml2::XMLElement* xml, bool is_storing = false);
	virtual ~xml_archive();

public:
	virtual bool is_storing();

protected:
	virtual inline void read_impl(const CString& name, const serialize_flag& data);
	virtual inline void read_impl(const CString& name, bool& data);
	virtual inline void read_impl(const CString& name, char& data);
	virtual inline void read_impl(const CString& name, unsigned char& data);
	virtual inline void read_impl(const CString& name, wchar_t& data);
	virtual inline void read_impl(const CString& name, short& data);
	virtual inline void read_impl(const CString& name, unsigned short& data);
	virtual inline void read_impl(const CString& name, int& data);
	virtual inline void read_impl(const CString& name, unsigned int& data);
	virtual inline void read_impl(const CString& name, long& data);
	virtual inline void read_impl(const CString& name, unsigned long& data);
	virtual inline void read_impl(const CString& name, long long& data);
	virtual inline void read_impl(const CString& name, unsigned long long& data);
	virtual inline void read_impl(const CString& name, float& data);
	virtual inline void read_impl(const CString& name, double& data);
	virtual inline void read_impl(const CString& name, long double& data);
	virtual inline void read_impl(const CString& name, CString& data);
	virtual inline void read_impl(const CString& name, char*& buffer, long& byte_count);

	virtual inline void write_impl(const CString& name, const serialize_flag& data);
	virtual inline void write_impl(const CString& name, const bool& data);
	virtual inline void write_impl(const CString& name, const char& data);
	virtual inline void write_impl(const CString& name, const unsigned char& data);
	virtual inline void write_impl(const CString& name, const wchar_t& data);
	virtual inline void write_impl(const CString& name, const short& data);
	virtual inline void write_impl(const CString& name, const unsigned short& data);
	virtual inline void write_impl(const CString& name, const int& data);
	virtual inline void write_impl(const CString& name, const unsigned int& data);
	virtual inline void write_impl(const CString& name, const long& data);
	virtual inline void write_impl(const CString& name, const unsigned long& data);
	virtual inline void write_impl(const CString& name, const long long& data);
	virtual inline void write_impl(const CString& name, const unsigned long long& data);
	virtual inline void write_impl(const CString& name, const float& data);
	virtual inline void write_impl(const CString& name, const double& data);
	virtual inline void write_impl(const CString& name, const long double& data);
	virtual inline void write_impl(const CString& name, const CString& data);
	virtual inline void write_impl(const CString& name, char* buffer, long byte_count);

private:
	tinyxml2::XMLElement* m_xml = nullptr;
	bool m_is_array_reading_mode = false;
	tinyxml2::XMLElement* m_last_child_xml = nullptr;
	CString m_last_child_name;
	bool m_is_storing = false;

	// 仅临时标记对象，内存由外部处理
	// <0>为父对象
	// <1>为父对象是否处于数组模式读取状态
	// <2>为父对象最后一次读取时使用的节点对象
	// <3>为父对象最后一次读取时使用的节点名，对象或数组模式开始时会置为空，结束时会从栈中恢复
	// 注：因为数组模式中也不一定放的是节点名完全相同的对象，如会放入个数及各元素，因此要记住当前对象最后一次读取时的节
	// 点名，以确定下一个对象在数组模式中是否要移到兄弟节点上，节点名不同时不需要移动
	std::stack<std::tuple<tinyxml2::XMLElement*, bool, tinyxml2::XMLElement*, CString>> m_parent_nodes;
};


#endif //__HMGLOBALTOOL_INTERFACE_SERIALIZE_INTERFACE_H__

