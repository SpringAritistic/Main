#pragma once

#include <vector>
#include <map>
#include "x_framework/macro.h"
#include "x_framework/interface/serialize_interface.h"
#include "x_framework/interface/data_center_interface.h"


namespace tinyxml2 { class XMLElement; }


// 图纸类型注册信息
class PROJECT_MANAGER_API graph_register_info : public CObject, public serialize_interface
{
	DECLARE_SERIAL(graph_register_info)

public:
	graph_register_info();
	graph_register_info(const graph_register_info& src);
	graph_register_info& operator=(const graph_register_info& src);
	virtual ~graph_register_info();

public:
	virtual void serialize_members(member_rw_interface& mt);

public:
	void initiate_by_xml(tinyxml2::XMLElement* node);

public:
	CString		m_type_name;						// 图纸类型名称，如“矩形盖梁柱式墩一般构造图”
	CString		m_data_class_name;					// 构件数据类的类名
	CString		m_common_grid_class_name;			// 构件常用表格界面参数类的类名
	CString		m_advance_grid_class_name;			// 构件高级表格界面参数类的类名
	CString		m_graphic_editor_class_name;		// 图形化参数编辑器界面的类名
	CString		m_drawing_strategy_class_name;		// 图纸绘制策略类的类名
};

//图纸数据信息
class graph_data_manager : public data_node_base
{
public:
	graph_data_manager();
	virtual ~graph_data_manager();
};
//零件库数据信息
class accessory_data_manager : public data_node_base
{
public:
	accessory_data_manager();
	virtual ~accessory_data_manager();
};


// 项目数据(直接注册到数据中心，因此不用写序列化宏即可正常工作)
class PROJECT_MANAGER_API project_data : public data_node_base
{
public:
	project_data();
	virtual ~project_data();

public:
	virtual void serialize_members(member_rw_interface& mt);
	virtual void set_default_value();

public:
	// 返回项目文件配套的临时文件夹根路径
	CString get_project_temp_path(bool end_with_backslashed = true);

	// 返回项目文件配套的图纸文件夹根路径
	CString get_project_graph_path(bool end_with_backslashed = true);

	graph_data_manager& get_graph_data_manager();

	accessory_data_manager& get_accessory_data_manager();

public:
	// 从xml配置文件中加载构件类型信息
	static void initiate_graph_register_info_by_xml();
	static bool get_graph_register_info(const CString& type_name, graph_register_info& info);
	static void add_graph_register_info(const graph_register_info& info);
	static void get_graph_register_info_list(std::vector<graph_register_info*>& all_items);

private:
	// 图纸类型注册信息，数据会在程序启动时加载完成，此处缓存在项目中供查询使用，不序列化到项目数据中
	static std::map<CString, graph_register_info> m_all_type_info;
};

project_data& get_project_data();
