#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_TABLE_TEMPLATES_H__
#define __HMGLOBALTOOL_TOOLS_TABLE_TEMPLATES_H__

#include <array>
#include <vector>
#include "tinyxml2.h"

class CTHDataBase;

// 仅用于管理和加载txml、tab、tof等表格模板，并提供实例创建功能
class X_FRAMEWORK_API table_templates
{
private:
	table_templates();
	~table_templates();

public:
	static CTHDataBase* create_data_base(const CString& template_name); // 指针由使用的人负责管理

public:
	// 从table_dir目录下加载所有模板模板，此接口由框架调用,
	static void load_table_template_name_from_dir(const CString& table_dir);

	// 把一个database交由表格管理器当作模板来管理（创建的时候根据此表格生成一个新的表格实例并返回），
	// 传入的表格对象由本类负责内存管理,模板名重名会导致表格返回错误，因此如果重名，则函数直接返回
	// 尽量保证传入的表格名称跟其它表不一样（推荐使用“业务名_分组名_功能名_表格名”来命名，例如：“总体信息_墩台_上部结构水平荷载类型”
	static void add_database_sample(const CString& name, CTHDataBase* db);

	// 检查表格模板以及用户送入的用于表格创建的样本表格实例名中有指定的名称则返回true，否则返回false;
	static bool has_template_name(const CString& name);
};

// xml格式的表格模板单元格配置信息
// 使用该函数从xml文件，生成表格模板
// 表格模板示例：
// 
// 	< ? xml version = "1.0" encoding = "utf-8" ? >
// 	<table_template_design>
// 	
// 	<table table_type = "line_table">
// 	<tr>
// 	<td width = '100' colspan = '8'  >< / td>
// 		...
// 	< / tr>
// 	  ...
// 	  ...
// 	< / table>
// 
// 	< / table_template_design>
// 
// 	表格模板参数使用说明:
// 	template_name	表格名称（单个表格时，可不写，以文件名称作为表格名称）		
// 	table_type		表格类型（行表：line_table; 属性表：attribute_table; 通用表：common_table）
// 	colspan			表格向右列合并数量
// 	rowspan			表格向下单元格合并数量
// 	read_only		默认为只读，添加值为读写
// 	width			单元格宽度
// 	cell_tag		单元格标签
// 	row_tag			行标签
// 	col_tag			列标签
class X_FRAMEWORK_API table_cell_config
{
public:
	table_cell_config();
	~table_cell_config();

public:
	CString		m_colspan;			// 单元格向右合并数量
	CString     m_rowspan;			// 单元格向下合并数量
	CString		m_valign;			// 对齐方式
	CString		m_text;				// 单元格内容
	CString		m_label_text;		// 环境变量
	CString		m_type;				// 单元格类型
	CString     m_read;				// 只读属性
	CString		m_cell_tag;			// 标签名
};

// xml格式的表格模板行配置信息
class X_FRAMEWORK_API table_row_config
{
public:
	table_row_config();
	~table_row_config();

public:
	int								m_row_height;		//行高
	CString							m_row_tag;			//行标签
	std::vector<table_cell_config>	m_all_cell_configs;	//每一行信息

private:
	friend class table_xml_template;
	friend class single_table_config;
};

//xml 单表配置信息
class X_FRAMEWORK_API single_table_config
{
public:
	single_table_config();
	~single_table_config();

private:
	CString m_table_type;
	std::vector<table_row_config> m_all_row_configs;	//单表信息

private:
	friend class table_xml_template;
	friend class single_template_config;
};

//单模板表格
class X_FRAMEWORK_API single_template_config
{
public:
	single_template_config();
	~single_template_config();

public:
	CTHDataBase* generate_grid() const;
	void initialize_by_xml(tinyxml2::XMLElement* node);

public:
	void set_row_count(int count);
	int get_row_count() const;
	void set_table_size(int row_count, int column_count); // 行、列数必须大于0
	void set_column_count(int count);
	int get_column_count() const;
	table_cell_config& get_cell_config(int row_index, int col_index);

public:
	//本接口主要用于材料数量表的构建
	void get_rows_height(std::vector<std::pair<int, int> >& row_height);			// pair<行数, 高度>
	void get_cols_width(std::vector<std::pair<int, int> >& col_width);				// pair<列数, 宽度>
	void get_merge_data(std::vector<std::array<int, 4>>& data) const;				// 得到合并信息数据,行列号，屏蔽首行首列

public:
	CString m_template_name;

private:
	void get_xml_table_node(tinyxml2::XMLElement* node);
	void get_xml_tr_node(tinyxml2::XMLElement* node, single_table_config& data);
	void set_cell_property(CTHDataBase* db) const;
	void set_group_property(CTHDataBase* db)const;
	void get_merge_info();
	void resize_verify_txml_data();
	void get_group_info();
	void set_title_info(CTHDataBase* db)const;
	bool verify_merge_info() const;

private:
	// 模板的行列比实例化后的表格小的多，不需要使用特殊优化的数据结构
	std::vector<single_table_config>	m_all_table_configs;	// 总的表格模板信息
	std::vector<std::array<int, 4>>		m_merge_info;			// 合并信息(单元格左上右下行列索引),未屏蔽首行、首列的数据
	std::vector<CString>				m_width;				// 表格列宽
	std::vector<CString>				m_col_tag;				// 表格列标签
	std::vector<table_row_config>		m_self_row_info;		// 表格行信息
	std::vector<std::array<CString, 3>> m_group;				// 表格行组信息, array<起始行，终止行，行组类型>>
	int m_column_count;

private:
	friend class table_xml_template;
};

// xml格式的表格模板
class X_FRAMEWORK_API table_xml_template
{
public:
	table_xml_template();
	~table_xml_template();

public:
	void initialize_by_xml(CString& path_name);
	bool is_empty();
public:
	std::vector<single_template_config> m_all_table;
};
#endif //__HMGLOBALTOOL_TOOLS_TABLE_TEMPLATES_H__
