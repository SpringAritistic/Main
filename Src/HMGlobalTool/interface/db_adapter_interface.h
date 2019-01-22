#pragma once
#ifndef __HMGLOBALTOOL_INTERFACE_DB_ADAPTER_INTERFACE_H__	
#define __HMGLOBALTOOL_INTERFACE_DB_ADAPTER_INTERFACE_H__	

#include <vector>
#include <utility>


// 数据库的一个字段信息
class HM_GlobalTool_EXT db_field
{
public:
	// 数据转换规则：
	// 数值类型向bool转换时，非0为true，其它为true，浮点数按8位二进制位考虑（比后面的其它数据类型转换精度略低，使bool判断更符合常理）
	// tp_integer之间的小类型转换（如int->long）内部直接强制转换
	// tp_real之间的小类型转换（如double->float）内部直接强制转换
	// tp_real与tp_integer之间的转换（如double->long）内部直接强制转换
	// tp_string与tp_integer之间的转换使用_ttoll()并根据需要进行高位截断
	// tp_string与tp_integer之间的转换使用_tcstold()并根据需要进行有效位截断，反向转换也支持
	// tp_string与tp_real之间的转换使用_tcstold()并根据需要进行有效位截断，反向转换默认保留12位小数
	// tp_buffer向数值类型转换结果为0,向字符串转换结果为空字符串，向bool转换结果为false，反向转换均得到空的buffer
	// 注意：时期、日期的处理在不同的数据库中略有区别，往往格式有小变化，无法使用通用的方式处理，加之生成SQL语句时
	//      也写作字符串形式（如where fd_test='2000-1-1'），因此本接口会将时间、日期按字符串形式处理，如需进一
	//      加工，请配合框架的string_tools.h中的时间相关转换接口可实现自由灵活的处理功能
	enum class type
	{
		tp_integer,			// 各种整数类型，如bool，int，char，unsigned long等
		tp_real,			// 各种浮点数，如float，double，long double
		tp_string,			// 字符串类型
		tp_buffer,			// 连续存储的二进制内存块
	};

public:
	db_field(type tp = type::tp_integer);
	db_field(const db_field& src);
	db_field(db_field&& src);
	db_field& operator=(const db_field& src);
	db_field& operator=(db_field&& src);
	bool operator==(const db_field& src) const; // 字符串类型时对大小敏感
	bool operator!=(const db_field& src) const;
	~db_field();

public:
	type get_type() const;

	void get_value(bool& val) const;
	void get_value(char& val) const;
	void get_value(unsigned char& val) const;
	void get_value(wchar_t& val) const;
	void get_value(short& val) const;
	void get_value(unsigned short& val) const;
	void get_value(int& val) const;
	void get_value(unsigned int& val) const;
	void get_value(long& val) const;
	void get_value(unsigned long& val) const;
	void get_value(long long& val) const;
	void get_value(unsigned long long& val) const;
	void get_value(float& val) const;
	void get_value(double& val) const;
	void get_value(long double& val) const;
	void get_value(CString& val) const;
	
	// 返回一个特定长度的buffer，内存空间仍由本类管理，如要副本，请自行拷贝
	void get_value(std::pair<size_t, char*>& val) const;

	// 提供一个函数值直接返回结果的版本便于简化用户代码
	template<typename T>
	T get_value() const
	{
		T tmp;
		get_value(tmp);
		return tmp;
	}

	void set_value(bool val);
	void set_value(char val);
	void set_value(unsigned char val);
	void set_value(wchar_t val);
	void set_value(short val);
	void set_value(unsigned short val);
	void set_value(int val);
	void set_value(unsigned int val);
	void set_value(long val);
	void set_value(unsigned long val);
	void set_value(long long val);
	void set_value(unsigned long long val);
	void set_value(float val);
	void set_value(double val);
	void set_value(long double val);
	void set_value(const CString& val);
	
	// 设置为特定长度的buffer
	// clone_buffer为false表示直接接管传入的内存空间
	// 为true表示不接管传入的内存空间，但会原样复制一份出来在内部管理
	// 可以设置为空内存块，即：make_pair(0, nullptr)
	void set_value(const std::pair<size_t, const char*>& val, bool clone_buffer = true);

private:
	void destory_value();

private:
	type m_value_type;
	void* m_value;
};


// 表示数据库中的的一条记录（即一行数据）
class HM_GlobalTool_EXT db_record
{
public:
	db_record();
	~db_record();

public:
	// 根据名称获取字段对象的指针，字段名称不区分大小写
	db_field* field(const CString& name);
	db_field* field(unsigned int index);

public:
	std::vector<std::pair<CString, db_field>> m_row; // first为字段名，second为字段内容
};


//////////////////////////////////////////////////////////////////////////


// 数据库操作的接口类，用于封装不同数据库对框架的影响
class HM_GlobalTool_EXT db_adapter_interface
{
public:
	db_adapter_interface();
	db_adapter_interface(const db_adapter_interface&) = delete;
	db_adapter_interface(db_adapter_interface&&) = delete;
	void operator=(const db_adapter_interface&) = delete;
	void operator=(db_adapter_interface&&) = delete;
	virtual ~db_adapter_interface();

public:
	// 执行一条select语句并返回有没有匹配的结果，有则返回true，否则返回false
	// 此接口经常用作用户登录、权限查询等敏感性信息的查询处理，为避免信息泄漏的风险，内部实现必须达到指定的安全
	// 和性能要求，内部实现使用get_query_result_count完成，详细要求参见其注释。
	bool has_query_result(const CString& sql);

public:
	// 判断一条sql语句是否为查询语句（语句不得以空白字符开头）
	static bool is_select_sql(const CString& sql);

public:
	// 测试当前时间能不能正确连接到数据库，成功返回true，否则返回false
	// 1.接口内部必须捕获处理数据库操作异常（非数据库异常可以不处理），以确保外部使用人员不需要处理数据库操作的异常
	// 2.此接口必须满足多线程安全
	virtual bool test_connection() const = 0;

	// 执行一条select语句并返回查询结果的记录总数
	// 此接口经常用作用户登录、权限查询等敏感性信息的查询处理，为避免信息泄漏的风险，此接口的派生类实现必须满足以下要求：
	// 1.不得使用能返回敏感数据作为结果的查询语句，如不得返回匹配的用户密码，而应该直接查询匹配的记录数是否大于等于1
	// 2.数据库中的敏感信息不得明文存储，此接口按加密要求对查询信息加密后按加密内容生成查询语句再查询，至于数据连接
	//   是否使用安全链接则不作特殊规定，有条件的情况下尽量使用安全链接（需要数据证书）
	// 3.此接口必须足够高效，不要返回和接收带内容性质的数据，返回的数据只要能判断出是否有匹配结果，则返回内容越少越好
	// 4.此语句不得接收除select之外的语句（如增、删数据等语句），允许有任意嵌套的子句
	// 5.接口内部自行合理处理数据库的连接和断开操作（即时连接、断开，还是使用连接对象池由派生类自己决定）
	// 6.接口内部必须捕获处理数据库操作异常（非数据库异常可以不处理），以确保外部使用人员不需要处理数据库操作的异常
	// 7.此接口必须满足多线程安全
	// 8.为了提升读取效率和避免误写代码导致数据源被改动，派生类打开连接时请使用只读权限的连接进行操作
	// 9.内部实现受sql语句的复杂性影响大，一般情况下用户使用此接口时请尽量使用count聚合函数，避开传入常规查询语句
	//   并由接口内部根据行数信息返回结果带来的效率损耗！！！，例如应该使用：
	//   select count(*) as fd_count from tb_test
	//   而不是：
	//   select * as fd_count from tb_test
	//   虽然得到的行数一样，但安全性和传输的数据量完全不在一个档次
	// 10.非虚接口has_query_result内部实现依赖于此接口，对于has_query_result的安全性及性能要求也必须满足
	virtual unsigned long long get_query_result_count(const CString& sql) = 0;

	// 查询指定的记录
	// 如果字段为二进制数据或文件，则以连续的内存块返回，如果为长字符串则以字符串类型返回，读取时请注意
	// 注意：
	//   如果返回的数据过多有可能导致内存分配失败造成程序异常终止，请根据业务需求决定使用一次性返回还是部分返回，或
	//   者是分页返回策略
	// 派生类的实现须满足以下要求：
	// 1.此接口仅支持select语句且允许有任意嵌套的子句，不得支持增、删数据等非查询性质的语句
	// 2.接口内部自行合理处理数据库的连接和断开操作（即时连接、断开，还是使用连接对象池由派生类自己决定）
	// 3.接口内部必须捕获处理数据库操作异常（非数据库异常可以不处理），以确保外部使用人员不需要处理数据库操作的异常
	// 4.此接口必须满足多线程安全
	// 5.为了提升读取效率和避免误写代码导致数据源被改动，派生类打开连接时请使用只读权限的连接进行操作
	// 6.查询失败必须保证结果数据为空
	virtual std::pair<bool, CString> get_query_result(std::vector<db_record>& result, const CString& sql) = 0;

	// 以开启事务的方式一次性批量执行完指定的全部sql语句，语句的类型和功能不限，只要数据库支持即可，如果错误发生，则
	// 会回滚所执行的全部改动性操作
	// 返回的first表示执行是否完全成功，成功执行完成返回true，否则返回false
	// 返回的second表示错误时的错误信息，如果成功则为空
	// 此接口的派生类实现必须满足以下要求：
	// 1.接口内部必须捕获处理数据库操作异常（非数据库异常可以不处理），以确保外部使用人员不需要处理数据库操作的异常
	// 2.此接口必须满足多线程安全
	virtual std::pair<bool, CString> run_as_a_batch(std::vector<const CString>& all_sql) = 0;

	// 以开启事务的方式一次性执行指定的全部sql语句，但返回最后一条查询语句的结果
	// 此接口强制要求最后一条语句必须为select语句，否则不予执行，整个执行过程中的中间结果一律不返回，仅返回最后一
	// 条查询语句的结果，其它语句的类型和功能不限，只要数据库支持即可
	// 执行过程中如果发生错误，则会回滚所执行的全部改动性操作
	// 本接口一般很少用到，如果需要保证多表查询或多个步骤查询的过程中不受中途其它会话对数据改动的影响而造成数据不同
	// 步这种要求时，务必使用本接口（一般金融系统都会有此要求）
	// 其它情况下，此接口的派生类要求与run_as_a_batch一致
	virtual std::pair<bool, CString> get_batch_result(std::vector<const CString>& all_sql, std::vector<db_record>& result) = 0;

	// 以不开启事务的方式执行一条任意的sql语句，语句的类型和功能不限，只要数据库支持即可
	// 返回的first表示执行是否完全成功，成功执行完成返回true，否则返回false
	// 返回的second表示错误时的错误信息，如果成功则为空
	// 此接口的派生类实现必须满足以下要求：
	// 1.接口内部必须捕获处理数据库操作异常（非数据库异常可以不处理），以确保外部使用人员不需要处理数据库操作的异常
	// 2.此接口必须满足多线程安全
	virtual std::pair<bool, CString> run_alone(const CString& all_sql) = 0;
};


// 要让框架能正常操作数据库，需要实例化x_database_interface对象并向框架注册，注册的对象由外部管理，框架不维护，因
// 此不需要反注册，可以传nullptr表示停用已经注册的实例并由默认的实例接管（默认实例什么也不做，只保证存在一个实例而已）
// 注意：
//   对于数据库而言，并发操作是家常便饭，原则上此接口也要支持并发操作，但考虑到注册数据操作实例一般只是在程序加载时
//   就已经完成，而且一般程序关闭前都不会动态改实例，这一点对于非数据库管理的业务程序来说更为明显，为简化程序逻辑，
//   此函数内部不处理并发的情况，即业务程序请在程序启动时就正确注册访问的实例，中途不要修改，如果以后并发的需求很
//   强烈再考虑支持，此处特作注释以记之。
HM_GlobalTool_EXT void regist_database_adapter(db_adapter_interface* adapter);

// 返回注册的数据库操作实例，如果没有注册过则返回内部默认生成的实例（默认实例什么也不做，只保证存在一个实例而已）
// 此接口支持并发访问
HM_GlobalTool_EXT db_adapter_interface* get_database_adapter();

#endif //__HMGLOBALTOOL_INTERFACE_DB_ADAPTER_INTERFACE_H__
