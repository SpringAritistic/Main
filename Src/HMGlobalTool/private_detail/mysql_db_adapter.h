#pragma once
#include <vector>
#include <utility>
#include <string>
#include "../interface/db_adapter_interface.h"
#include "mysql\mysqlpp\mysql++.h"


// mysql数据库的适配器，注意mysql必须要求编码格式指定为utf8才能正常工作
class mysql_db_adapter : public db_adapter_interface
{
public:
	mysql_db_adapter();
	~mysql_db_adapter();

private:
	virtual bool test_connection() const;
	virtual unsigned long long get_query_result_count(const CString& sql);
	virtual std::pair<bool, CString> get_query_result(std::vector<db_record>& result, const CString& sql);
	virtual std::pair<bool, CString> run_as_a_batch(std::vector<const CString>& all_sql);
	virtual std::pair<bool, CString> get_batch_result(std::vector<const CString>& all_sql, std::vector<db_record>& result);
	virtual std::pair<bool, CString> run_alone(const CString& sql);

private:
	// 使用指定链接对象直接查询一个结果并返回
	static std::pair<bool, CString> get_select_result_by_connection(
		mysqlpp::Connection& conn, std::vector<db_record>& result, const CString& sql);

	// 返回一个已经连接的数据库连接对象，如果无法正常连接或连接异常则返回nullptr
	static std::unique_ptr<mysqlpp::Connection> make_db_connection();

	// 将查询结果的一个字段转为C++的数据类型，注意mysql必须要求编码格式指定为utf8
	static void convert_to_field_data(db_field& field_data,
		const mysqlpp::String& field_db, const mysqlpp::FieldTypes::value_type& type_db);
};
