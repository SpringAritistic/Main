#pragma once


#include "../macro.h"
#include "tendon_data.h"
////////////////////////
#define TENDON_BASETYPE			(_T("钢束-基本类型"))
#define TENDON_TYPE				(_T("钢束-钢束类型"))
#define TENDON_GSMM				(_T("钢束-钢束锚面"))
#define TENDON_GSZLPC			(_T("钢束-张拉批次"))
#define TENDON_REFLINE			(_T("钢束-自定义参考线"))
#define TENDON_USERLINE			(_T("钢束-同豪任意曲线"))

//手动创建
#define TENDON_SWLX				(_T("钢束-竖弯类型"))
#define TENDON_PWLX				(_T("钢束-平弯类型"))
#define TENDON_LAY				(_T("钢束-钢束布置"))

//tag名
#define TAG_PSWINDEX			(_T("平竖弯索引名称"))
#define TAG_PSWINDEX2			(_T("竖弯纵向特征线"))
#define TAG_PSWROW				(_T("平竖弯行标签"))
#define TAG_SGF					(_T("施工缝标签"))
#define TAG_YYBW				(_T("应用部位"))
#define TAG_LAYROW				(_T("布置行标签"))

//参考线类型
#define HXTZX_PW				(_T("横向特征线[平弯]"))	//此处要与用户线表格中定义一致
#define HXTZX_SW				(_T("横向特征线[竖弯]"))
#define ZXTZX					(_T("纵向特征线"))
#define GDTZX					(_T("高度特征线"))

// 

class CTHDataBase;
struct NM_THGRID;
class tendon_data;


// 钢束表的环境变量类
class X_FRAMEWORK_API tendon_grid_environment_varible
{
public:
	tendon_grid_environment_varible();
	~tendon_grid_environment_varible();

public:
	std::vector<CString>      m_all_material;       // 混凝土索引名称
	std::vector<CString>      m_all_anchor;         // 锚具索引名称
	std::vector<CString>      m_all_pipe;           // 波纹管索引名称
	std::vector<CString>      m_all_linker;         // 连接器索引名称
	std::vector<CString>      m_all_chikuai;        // 齿块锚面索引名称

	//----
	std::vector<CString>      m_all_swhxtzx;        // 竖弯类型横向特征线
	std::vector<CString>      m_all_swgdtzx;        // 竖弯类型高度特征线
	std::vector<CString>      m_all_pwhxtzx;        // 平弯类型横向特征线
	std::vector<CString>      m_all_pwkzw;          // 平弯类型平弯控制位
	std::vector<CString>      m_all_bzckx;          // 钢束布置参考线名
};


// 钢束表格工具类
class X_FRAMEWORK_API tendon_grid_tools
{
private:
	tendon_grid_tools();
	~tendon_grid_tools();

public:
	// 获取表格中数据并全部填入到P0数据结构中
	static void get_data_from_grid(std::map<CString, CTHDataBase*>& all_dbs, tendon_data& data);
	static void get_data_from_grid_basetype(CTHDataBase* pdatabase, std::vector<tendon_data_basetype>& vec_basetype);
	static void get_data_from_grid_bendtype(CTHDataBase* pdatabase, std::vector<tendon_data_bend_type>& vec_bendtype);
	static void get_data_from_grid_anchor_plane(CTHDataBase* pdatabase, std::vector<tendon_data_anchor_plane>& vec_anchor_plane);
	static void get_data_from_grid_refline(CTHDataBase* pdatabase, std::vector<tendon_data_refline>& vec_refline);
	static void get_data_from_grid_tendontype(CTHDataBase* pdatabase, std::vector<tendon_data_type>& vec_refline);
	static void get_data_from_grid_tension_batch(CTHDataBase* pdatabase, std::vector<tendon_data_tension_batch>& vec_refline);
	static void get_data_from_grid_tendon_info(CTHDataBase* pdatabase, std::vector<tendon_data_info>& vec_tendon_info);

	// 把用户传入的P0数据全部填入到表格中
	static void set_data_to_grid(std::map<CString, CTHDataBase*>& all_dbs, const tendon_data& data);
	static void set_data_to_grid_basetype(CTHDataBase* pdatabase, const std::vector<tendon_data_basetype>& vec_basetype);
	static void set_data_to_grid_bendtype(CTHDataBase* pdatabase, const std::vector<tendon_data_bend_type>& vec_bendtype, bool bSW, bool bHXTZ = false);
	static void set_data_to_grid_anchor_plane(CTHDataBase* pdatabase, const std::vector<tendon_data_anchor_plane>& vec_anchor_plane);
	static void set_data_to_grid_refline(CTHDataBase* pdatabase, const std::vector<tendon_data_refline>& vec_refline);
	static void set_data_to_grid_tendontype(CTHDataBase* pdatabase, const std::vector<tendon_data_type>& vec_refline);
	static void set_data_to_grid_tension_batch(CTHDataBase* pdatabase, const std::vector<tendon_data_tension_batch>& vec_tension_batch);
	static void set_data_to_grid_tendon_info(CTHDataBase* pdatabase, const std::vector<tendon_data_info>& vec_tendon_info);	

	// 得到所有钢束表格的注册名称
	static void get_all_tendon_grid_template_names(std::vector<CString>& names);
	static std::vector<CString> get_all_tendon_grid_template_names();

	// 设置表格环境变量（在加载表格时调用）， 需传入所有表格数据及已经实例化的钢束表环境变量类
	static void set_tendon_grid_variable(std::map<CString, CTHDataBase*>& all_dbs, const tendon_grid_environment_varible& src);

	// 生成默认值
	static void make_default_value_by_grid(tendon_data& data);

	// 是否弹出“钢束-同豪任意曲线”的二级表
	static void should_pop_tendon_sub_grid(CTHDataBase* db, NM_THGRID* notify_struct);

	// 当基本类型、竖弯类型、平弯类型的名称有变动时，更新缓存变量
	static void reflesh_temporary_variable(CTHDataBase* db, NM_THGRID* notify_struct);

	// 当点击含有环境变量的单元格时，根据缓存变量填入环境变量值
	static void set_enviroment_variable(CTHDataBase* db, NM_THGRID* notify_struct);

	// 设置横向特征线和高度特征线（平弯控制位）的下拉框选项
	// 第一个参数为横向特征线下拉选项集，第二个参数为高度特征线（平弯控制位）下拉选项集
	static void set_bendtype_droplist(std::vector<CString>& vechxtzx, std::vector<CString>& vecgdtzx, std::map<CString, CTHDataBase*>& all_dbs);

private:
	// 初始化实时环境变量所需的缓存变量（读取数据时调用）
	static void init_temporary_variable(const tendon_data& data);

private:
	// 缓存钢束表格中需实时更新的环境变量，暂时有三个：基本类型名称，竖弯类型名称，平弯类型名称
	static std::vector<CString>                      m_basetype_names;			   // 基本类型名称
	static std::vector<tendon_data_bend_type>        m_all_ver_bend;			   // 竖弯类型名称
	static std::vector<tendon_data_bend_type>        m_all_hor_bend;			   // 平弯类型名称
};


// 注册竖弯/平弯钢束类型表/钢束类型表
X_FRAMEWORK_API void reigste_tendon_grid_template();
