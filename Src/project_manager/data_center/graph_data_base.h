#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/serialize_interface.h"
#include "project_data.h"


// 图纸数据的基类
class PROJECT_MANAGER_API graph_data_base : public data_node_base
{
	DECLARE_SERIAL(graph_data_base)

public:
	graph_data_base(const CString& graph_name = _T("")); // 此处为图纸数据类的名称，并不是绘制出的图纸文件名
	virtual ~graph_data_base();

public:
	virtual void serialize_members(member_rw_interface& mt);

public:
	// 返回当前节点对应的图纸注册信息
	bool get_graph_reg_info(graph_register_info& info) const;

public:
	// 图纸类型名称（用于索引图纸类型注册信息），不适合在图纸数据中直接存储注册信息，否则开发过程中修改配置文件
	// 时则老项目中的图纸出图工具类或其它窗口类将无法正常随配置变化（在序列化中做是不恰当的，会造成界面的变去与
	// 数据绑在一起，因此只存放类型名，使用到时自动查找）
	// 注意：用户指定的图纸节点名称放在基础的m_name中
	CString m_graph_type_name;

	CString m_graph_dxf_file_name; // 生成的dxf图纸文件相对路径及名称（有后缀，且必须相对于项目文件夹的根目录）
	CString m_graph_dwg_file_name; // 生成的dwg图纸文件相对路径及名称（有后缀，且必须相对于项目文件夹的根目录）
	CString m_graph_pdf_file_name; // 生成的pdf图纸文件相对路径及名称（有后缀，且必须相对于项目文件夹的根目录）
};
