///////////////////////////////////////////////////////////////////////////////////////////////////
// 概述：
//     提供了集合、区间的数据结构以及相应的运算功能，所有接口严格按照数学上的相关定义来实现，比如区
//     间的表达方式以及含义、集合的含义以及相应的数学运算功能及逻辑，除此之外，还扩展了一些数学学科
//     上没有的功能，以便提高使用者操作上的便利性，如对初始化的字符串进行识别和校正、对多个区间构成
//     的集合能自动进行重叠部分的合并和精简等
// 
// 作者：Wang Youzhou
// 维护：
//     --V2.2 2015-4-23------------------
//     *去除mfc依赖
//     *去除用不到的接口
//     *命名规范按STL库规则处理
//     *内置外部引用到的小工具函数，使代码不依赖于其它文件
//     *接口上同时支持MBCS和Unicode字符串
//     *去除序列化接口，由外部得到字符串结果后作序列化以便脱离MFC
//     *修正一些接口上的单词拼写错误
//     *scope_value()改为构造负无穷到正无穷（原来为单值0.0）
//     *scope_value新增使用单个常规数字进行构造和设置值
//     *assemblage_value新增使用单个常规数字进行构造和设置值
// 
//     --V2.1 2014-12-4------------------
//     *修正一些注释错误
//     *将废弃的is_in_assemblage方法重新归入对外功能接口中，用于判断是否属于指定集合，相当于数学上的∈操作，特作补全
//     *正、负无穷时返回数值不再用0表示，而是一个尽可能大或尽可能小的数字
//     
//     --V2.0----------------------------
//     *修正一些小错误
//     *完善序列化功能
//     *优化内部集合处理逻辑
//     *对原来接口进行重命名，以完全遵守数学上的叫法
//     *废弃所有数学上没有的运算接口以及名称不规范的接口（已经有人用了，所以仅保留接口并标记为废弃）
//     *新增数学上的集合求交、求并、子集超集判断接口
//     *新境number、区间、集合的逻辑运算符重载功能，以便调用的代码更简洁和人性化
//     *新增集合中各元素的优化、重叠区域的合并功能，以提高效率并更好的支持集合操作
//     *新增用于构造集合、区间的字符串使用了中文符号的功能，如将中文的括号和数字自动转为英文的字符等
//     
//     --V1.1----------------------------
//     *提供界面字符串原样保存和返回的功能
//     *新增集合数据结构及相应功能
//     *扩展用于生成集合、区间的字符串的格式，使用形式更人性化
//     
//     --V1.0----------------------------
//     *提供一个简易的区间表达及关系判断的功能
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_SCOPE_ASSEMBLAGE_H__
#define __HMGLOBALTOOL_TOOLS_SCOPE_ASSEMBLAGE_H__

#pragma warning(push)

// warning C4251: '***' : class needs to have dll-interface to be used by clients of class '***'
#pragma warning(disable: 4251)


// 数值类，可以表示正负无穷，内部使用double存储
class HM_GlobalTool_EXT number
{
public:
	number(); // 相当于用0.0初始化
	number(short value);
	number(int value);
	number(long value);
	number(unsigned short value);
	number(unsigned int value);
	number(unsigned long value);
	number(float value);
	number(double value);
	number(bool is_infinitely_great_or_small); // true表示值为正无穷，false表示值为负无穷
	number(const number& num);
	~number();

public:
	void set_value(short value);
	void set_value(int value);
	void set_value(long value);
	void set_value(unsigned short value);
	void set_value(unsigned int value);
	void set_value(unsigned long value);
	void set_value(float value);
	void set_value(double value);
	void set_value(const number& num);
	void set_value(bool is_infinitely_great_or_small); // true表示值为正无穷，false表示值为负无穷
	void set_infinitely_p();			// 设置值为正无穷
	void set_infinitely_n();			// 设置值为负无穷
	bool is_infinite() const;			// 当前值是否为极值，正无穷或负无穷时返回true，为一个特定的数值时返回false
	bool is_value() const;				// 当前值是否为一个特定的数字，相当于IsInfinite()结果取非，正负无穷返回false，否则返回true
	bool is_infinite_p() const;			// 是否为正无穷
	bool is_infinite_n() const;			// 是否为负无穷
	double get_value() const;			// 为负无穷时返回一个足够小的数字，为正无穷时返回一个足够大的数字，请不要认为返回值是正无穷或负无穷，这样做是因为计算机的浮点数是有范围限制的
	std::string to_string_a() const;	// 转换为字符串，保留6位有效数字，数字末尾的0会删除，如果为极值则返回空串
	std::wstring to_string_w() const;	// 转换为字符串，保留6位有效数字，数字末尾的0会删除，如果为极值则返回空串

public:
	// 支持的逻辑运算符
	friend bool operator<(double value, const number& num);
	friend bool operator<(const number& num, double value);
	friend bool operator>(double value, const number& num);
	friend bool operator>(const number& num, double value);
	friend bool operator>=(double value, const number& num);
	friend bool operator>=(const number& num, double value);
	friend bool operator<=(double value, const number& num);
	friend bool operator<=(const number& num, double value);
	friend bool operator!=(double value, const number& num);
	friend bool operator!=(const number& num, double value);

	bool operator<(const number& num) const;
	bool operator==(const number& num) const;
	bool operator>(const number& num) const;
	bool operator>=(const number& num) const;
	bool operator<=(const number& num) const;
	bool operator!=(const number& num) const;
	bool operator!() const;

	// CNum为左值时
	number& operator=(double value);
	number& operator=(const number& num);

	// CNum为右值时
	operator double() const; // 极值时返回0

private:
	// 支持的数学运算符
	friend const number operator+(const number& num, double value);
	friend const number operator+(double value, const number& num);
	friend const number operator+(const number& num1, const number& num2);
	friend const number operator-(const number& num, double value);
	friend const number operator-(double value, const number& num);
	friend const number operator-(const number& num1, const number& num2);
	friend const number operator*(const number& num, double value);
	friend const number operator*(double value, const number& num);
	friend const number operator*(const number& num1, const number& num2);
	friend const number operator/(const number& num, double value);
	friend const number operator/(double value, const number& num);
	friend const number operator/(const number& num1, const number& num2);

private:
	double m_value;
	int m_type; // 为0表示一个具体的实数，且数字存储在m_value中，为-1表示无穷小，为1表示无穷大
};


////////////////////////////////////////////////////////////////////////////////////////////////////

// 表示数学上的一个区间对象，这个值可以是一个定值，也可以是一个范围
// 注：
//   1.数学上正、负无穷用小括号，如果用中括号表示没有意义，仍按小括号处理，但程序内部则认为用小括号与
//     中括号含义不一样
//   2.数学上正、负无穷用一个带+号或带-号的横着的数字8表示，程序则认为逗号左边无数据或仅有一个-号表
//     示负无穷，逗号右边无数据或仅有一个+号表示正无穷
//   3.如果表示区间的字符串缺少小括号或中括号，则按小括号处理，即开区间
//   4.逗号、中括号、小括号和数字之间可以有任意多个空格，以便界面用户适当分隔两边数据看起来更清晰
// 
// 如下均为合法数据：
//   35				定值35
//   (11,)			区间：11到正无穷，不含11(正、负无穷在数学上用小括号，如果误用中括号表示无意义，仍按小括号处理，程序内部则认为小括号与中括号含义不一样)
//   [,11)			区间：负无穷到11，不含11
//   (,)或(-,+)		区间：负无穷到正无穷
//   (11,22)		区间：11到22，不含11，也不含22
//   [11,22]		区间：11到22，包含11，也包含22
//   (11,22]		区间：11到22，不含11，但包含22
//   [11,22)		区间：11到22，包含11，但不含22
//   要求区间的左右两个值均为可表示的常规实数值，支持正无穷、负无穷等特殊值
//   当存储的值为一个定值时也是一样的要求
//   注：区间不可能为空区间，和数学上一样，如果传入空字符串构造空区间则表示负无穷到正无穷
class HM_GlobalTool_EXT scope_value
{
public:
	// 相当于负无穷到正无穷
	scope_value();

	// 构造一个仅存储单个数值的scope_value
	scope_value(short value);
	scope_value(int value);
	scope_value(long value);
	scope_value(unsigned short value);
	scope_value(unsigned int value);
	scope_value(unsigned long value);
	scope_value(float value);
	scope_value(double value);
	scope_value(bool is_infinitely_great_or_small); // true表示值为正无穷，false表示值为负无穷
	
	// 构造一个仅存储单个数值的scope_value
	scope_value(number val);

	// 如果为空字符串表示负无穷到正无穷
	scope_value(const std::string& text);
	scope_value(const std::wstring& text);
	scope_value(const char* text);
	scope_value(const wchar_t* text);
	
	// left只能为半角的"("或者"["，right只能为半角的")"或者"]"，否则将按闭区间处理
	scope_value(const std::string& left, number l_val, number r_val, const std::string& right);
	scope_value(const std::wstring& left, number l_val, number r_val, const std::wstring& right);
	scope_value(const char* left, number l_val, number r_val, const char* right);
	scope_value(const wchar_t* left, number l_val, number r_val, const wchar_t* right);

	scope_value(bool is_open_left, const number& l_val, const number& r_val, bool is_open_right);

	~scope_value();

public:
	// 强制区间重置并存储一个数值
	void set_value(const number& val);

	// 强制区间重置并存储一个数值
	void set_value(short value);
	void set_value(int value);
	void set_value(long value);
	void set_value(unsigned short value);
	void set_value(unsigned int value);
	void set_value(unsigned long value);
	void set_value(float value);
	void set_value(double value);
	void set_value(bool is_infinitely_great_or_small); // true表示值为正无穷，false表示值为负无穷

	// 将表示区间的字符串转为一个区间对象(字符串格式可参考后面assemblage_value类中使用字符串初始化对象时关于区间的说明)
	void set_value(const std::string& strText);
	void set_value(const std::wstring& strText);
	void set_value(const char* text);
	void set_value(const wchar_t* text);

	// 强制区间重置并存入一个区间值，left只能为半角的"("或者"["，right只能为半角的")"或者"]"，否则将按开区间处理
	void set_value(const std::string& left, number l_val, number r_val, const std::string& right);
	void set_value(const std::wstring& left, number l_val, number r_val, const std::wstring& right);
	void set_value(const char* left, number l_val, number r_val, const char* right);
	void set_value(const wchar_t* left, number l_val, number r_val, const wchar_t* right);

	// 设置左区间值,右区间保持不变，strLeft只能为半角的"("或者"["，否则将按开区间处理
	void set_value(const std::string& left, const number& l_val);
	void set_value(const std::wstring& left, const number& l_val);
	void set_value(const char* left, const number& l_val);
	void set_value(const wchar_t* left, const number& l_val);

	// 设置右区间值,左区间保持不变，strRight只能为半角的")"或者"]"，否则将按开区间处理
	void set_value(const number& r_val, const std::string& right);
	void set_value(const number& r_val, const std::wstring& right);
	void set_value(const number& r_val, const char* right);
	void set_value(const number& r_val, const wchar_t* right);

	// 设置左区间值
	void set_value(bool is_open_left, const number& l_val, const number& r_val, bool is_open_right);

	// 设置左区间值,右区间保持不变
	void set_value(bool is_open_left, const number& l_val);

	// 设置右区间值,左区间保持不变
	void set_value(const number& r_val, bool is_open_right);

	// 设置左区间值,右区间保持不变，left只能为半角的"("或者"["，否则将按开区间处理
	void set_left_value(const std::string& left, const number& l_val);
	void set_left_value(const std::wstring& left, const number& l_val);
	void set_left_value(const char* left, const number& l_val);
	void set_left_value(const wchar_t* left, const number& l_val);

	// 设置右区间值,左区间保持不变，right只能为半角的")"或者"]"，否则将按开区间处理
	void set_right_value(const number& r_val, const std::string& right);
	void set_right_value(const number& r_val, const std::wstring& right);
	void set_right_value(const number& r_val, const char* right);
	void set_right_value(const number& r_val, const wchar_t* right);

	// 设置左区间值,右区间保持不变
	void set_left_value(bool is_open_left, const number& l_val);

	// 设置右区间值,左区间保持不变
	void set_right_value(const number& r_val, bool is_open_right);

	// 获取左区间的值，如果不是区间，而仅有一个数据，则返回该数据,is_open返回是否为开区间
	void get_left_value(number& l_val, bool& is_open) const;

	// 获取左区间的值，如果不是区间，而仅有一个数据，则返回该数据
	const number get_left_value() const;

	// 获取右区间的值
	void get_right_value(number& r_val, bool& is_open) const;

	// 获取左区间的值，如果不是区间，而仅有一个数据，则返回该数据
	const number get_right_value() const;

	// 如果区间中存储的是一个值，直接返回，否则返回左区间的值
	void get_single_value(number& val) const;

	// 获取左区间的值，如果不是区间，而仅有一个数据，则返回该数据
	const number get_single_value() const;

	// 判断左边或者右边是否为开区间
	bool is_open(bool is_left = true) const;

	// 判断左边或者右边是否为闭区间
	bool is_closed(bool is_left = true) const;

	// 判断当前区间中存储的是一个值，还是一个区间，如果为值则返回true，否则返回false
	bool is_single_value() const;

	// 判断某个值是否落在区间内（行为与数学上保持一致）
	bool is_in_scope(const number& val) const;

	// 判断指定的区间是否完全落在本区间内（行为与数学上保持一致）
	bool is_in_scope(const scope_value& scope) const;

	// 获取Scope当前的状态，返回-1表示区间状态正常且仅存储一个数据，返回0表示区间错误（如右值比左
	// 值大等错误）,返回1表示区间状态正常且存储的是一个区间范围，注：[3, 3]这种区间也返回0表示错误
	int get_scope_status() const;

	// 将区间按给定的值，拆分成左右两个区间, is_close为真表示拆开的两个区间接口处均为闭区间，否则为开
	// 区间，如果函数能正常拆为两个区间，则返回true，否则返回false，具体有以下规则：
	// 1.如果本区间仅是一个单值区间且is_close为true时将产生相同的两个单值区间并返回true，否则返回false
	// 2.如果拆分的数值不在本区间内，则返回false
	// 3.如果拆分的数值刚好是端点值且is_close为false时可能只能正常计算出一个区间，而不是两个，此时也返回false
	// 原则：只能拆分后的两个结果任何一个在数学角度上来看是非法的，那么结果就会返回false,否则返回true
	bool split_scope(const number& val, bool is_close, scope_value& out_left, scope_value& out_right) const;

	// 将当前区间与指定区间合并为一个区间并存放在本区间中
	// 如果无法连在一起，则返回false，成功返回true
	// 如果返回false则不会对本区间有任何改动，示例如下：
	// 3 和(3,5) 连接成为[3, 5)
	// (3, 5) 和 [5, 10]连接成为(3, 10)
	// (3, 5) 和 (5, 10]无法连接
	// (1, 5) 和 (3, 10)连接成为(1, 10)
	bool merge_scope_value(const number& val);
	bool merge_scope_value(const scope_value& val);

	// 获取scope_value当前值对应的字符串,(本对象使用字符串初始化时原样返回字符串)如果仅存储一个值，则直接
	// 返回对应的数字字符串，如果为区间，则按数学上的表示区间的方式返回，极值用空字符串代替
	std::string to_string_a() const;
	std::wstring to_string_w() const;

public:
	bool operator==(const scope_value& scope) const;
	bool operator!=(const scope_value& assem) const;

private:
	bool is_old_ui_string() const;	// 内部使用
	std::wstring get_old_ui_string() const;	// 内部使用

private:
	number		m_left;				// 存储左区间值或者当区间中仅存储一个值时也保存到这
	number		m_right;			// 存储右区间值
	bool		m_is_single_value;	// 是否存储的是一个值,true表示是，false表示当前是一个区间
	bool		m_is_left_open;		// 左区间是否为开区间，当m_is_single_value为false时生效
	bool		m_is_right_open;	// 右区间是否为开区间，当m_is_single_value为false时生效

	// 记录此值是否还保持着void set_value();使用单个字符串设置值时传入的数据,用户任何改动性
	// 的操作都会让这个值变为false,主要用于回填界面字符串
	bool		m_is_old_ui_string;

	std::wstring m_old_ui_string;	// 初始化时界面传入的字符串
};


////////////////////////////////////////////////////////////////////////////////////////////////////

// 表示由多个scope_value元素组成的一个集合
class HM_GlobalTool_EXT assemblage_value
{
public:
	assemblage_value();
	assemblage_value(const assemblage_value& assem);
	assemblage_value(const std::string& text);			// 字符串含义参考set_value()成员函数
	assemblage_value(const std::wstring& text);			// 字符串含义参考set_value()成员函数
	assemblage_value(const char* text);
	assemblage_value(const wchar_t* text);
	assemblage_value(short value);
	assemblage_value(int value);
	assemblage_value(long value);
	assemblage_value(unsigned short value);
	assemblage_value(unsigned int value);
	assemblage_value(unsigned long value);
	assemblage_value(float value);
	assemblage_value(double value);
	assemblage_value(bool is_infinitely_great_or_small); // true表示值为正无穷，false表示值为负无穷
	assemblage_value(const number& val);				// 区间存储一个区间,区间内仅有一个数值
	assemblage_value(const scope_value& val);			// 区间存储一个区间
	~assemblage_value();

public:
	// 使用字符串初始化集合,集合使用半角分号分隔多个元素,每一个元素的字符串都必须能合法的表示
	// 一个scope_value,最后一个元素之后不要分号,如果存在则会忽略，多个分号时仅删除一个，其余的
	// 按空字符串解析,如果字符串为空则放入一个表示负无穷到正无穷的区间元素.
	// 字符串格式说明如下：
	//   范围可由一个或多个元素组成，单个元素可以是一个区间或一个定值，元素之间用半角分号相隔
	//   数字前后或用于分隔scope_value的分号前后可以有任意多个空格分隔，以便数据看起来更清晰
	//   单个元素格式如下：
	//   ◆ 123.45  表示固定数值123.45
	//   ◆ (,)  表示负无穷到正无穷，也可采用形式(-,+)
	//   ◆ (,123)  表示负无穷到123，不含123，也可采用(-,123)
	//   ◆ [12.3,56.7)  含义和数学上的区间概念一致，即大于等于12.3且小于56.7
	//   ◆ 单元格为空表示负无穷到正无穷
	//   ◆ 元素为区间时，左、右区间值之间用半角逗号或半角空格隔开，如(12,34)等同于(12　34)
	//   ◆ 元素为区间时可以省略任意一边的括号，括号省略时表示开区间，如12　34等同于(12,34)或12,34)
	//   ◆ 非法数据或格式使用时将修正
	//   范围由多个元素组成的示例：
	//   [12.3,45.6);78;55,66;77  88表示这个范围是区间[12.3,45.6)、定值78、区间(55,66)、区间(77,88)的并集
	void set_value(const std::string& text);
	void set_value(const std::wstring& text);
	void set_value(const char* text);
	void set_value(const wchar_t* text);
	void set_value(short value);
	void set_value(int value);
	void set_value(long value);
	void set_value(unsigned short value);
	void set_value(unsigned int value);
	void set_value(unsigned long value);
	void set_value(float value);
	void set_value(double value);
	void set_value(bool is_infinitely_great_or_small); // true表示值为正无穷，false表示值为负无穷
	void set_value(const number& val);								// 强制集合重置并存储一个区间,区间内仅有一个数值
	void set_value(const scope_value& val);							// 强制集合重置并存储一个区间
	void set_value(const assemblage_value& value);					// 强制集合重置并存储一个集合
	void set_value(const std::vector<scope_value>& scope_items);	// 强制集合重置并存储由多个区间组成的集合

	void set_null();												// 清空所有元素并设为空集
	bool is_null() const;											// 是否是空集

	// 返回引用只是为了使用都方便且不耗费额外内存，只要调了下列函数均会对本集合生效，因此可以不用接收返回值再次赋值
	assemblage_value& intersection(const number& num);				// 求当前集合与另一个对象的交集
	assemblage_value& intersection(const scope_value& scope);		// 求当前集合与另一个对象的交集
	assemblage_value& intersection(const assemblage_value& scope);	// 求当前集合与另一个对象的交集

	assemblage_value& union_set(const number& num);					// 求当前集合与另一个对象的并集
	assemblage_value& union_set(const scope_value& scope);			// 求当前集合与另一个对象的并集
	assemblage_value& union_set(const assemblage_value& scope);		// 求当前集合与另一个对象的并集

	bool has_intersection(const number& num) const;					// 判断当前集合与另一个对象是否存在非空的交集,仅作判断，不会修改内部数据
	bool has_intersection(const scope_value& scope) const;			// 判断当前集合与另一个对象是否存在非空的交集,仅作判断，不会修改内部数据
	bool has_intersection(const assemblage_value& assem) const;		// 判断当前集合与另一个对象是否存在非空的交集,仅作判断，不会修改内部数据
	
	bool is_subset(const assemblage_value& assem) const;			// 判断当前集合是否是指定集合的子集（如果当前集合为空或与指定集合相等也返回true）
	bool is_superset(const assemblage_value& assem) const;			// 判断当前集合是否是指定集合的超集（如果指定集合为空或与当前集合相等也返回true）
	
	bool is_in_assemblage(const number& num) const;					// 指定的对象是否在本集合内（相当于判断与本集合的交集是否为空）
	bool is_in_assemblage(const scope_value& scope) const;			// 指定的对象是否在本集合内（相当于判断与本集合的交集是否为空）
	bool is_in_assemblage(const assemblage_value& scope) const;		// 指定的对象是否在本集合内（相当于判断与本集合的交集是否为空）

public:	
	const std::string to_string_a() const;							// 将集合元素转换成半角分号分隔的元素(本对象使用字符串初始化时原样返回字符串)
	const std::wstring to_string_w() const;							// 将集合元素转换成半角分号分隔的元素(本对象使用字符串初始化时原样返回字符串)
	void to_scope_items(std::vector<scope_value>& scope_items) const; // 将集合拆成多个整理好的区间，如果区间有重叠，会内部优化并合并，拆分后的所有区间构成的集合与原集合等价
	const std::vector<scope_value> to_scope_items() const;			// 将集合拆成多个整理好的区间，如果区间有重叠，会内部优化并合并，拆分后的所有区间构成的集合与原集合等价

public:
	bool operator==(const assemblage_value& assem) const;			// 只要逻辑区间范围完全重叠就认为相等,而不是指元素顺序,内容等完全相同
	bool operator!=(const assemblage_value& assem) const;			// 只要逻辑区间范围不同即认为不相等,并不要求具体的元素顺序,内容完全不同才认为是false
	assemblage_value& operator=(const assemblage_value& assem);

private:
	void arrange(); // 内部使用，整理集合内部的元素，将能合并的区间和元素全部合并，如[1,3][2,5]变为[1,5]，但不改构造时传入的初始化字符串

private:
	std::vector<scope_value> m_all_scope;

	// 记录此值是否还保持着void set_value(const CString& strText);接口传入的数据,用户的任何改动性的
	// 操作都会让这个值变为false,主要用于回填界面字符串
	bool			m_is_old_ui_string;

	std::wstring	m_old_ui_string; // 初始化时界面传入的字符串
};

#pragma warning(push)
#endif //__HMGLOBALTOOL_TOOLS_SCOPE_ASSEMBLAGE_H__
