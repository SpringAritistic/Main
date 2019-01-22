#pragma once
#include <utility>
#include <map>
#include <vector>

struct NM_THGRID;
class CTHDataBase;

// 构件表格界面数据观察者类
class UI_MANAGER_API graph_grid_ui_observer_base : public CObject
{
	DECLARE_SERIAL(graph_grid_ui_observer_base)

public:
	graph_grid_ui_observer_base();
	virtual ~graph_grid_ui_observer_base();

public:
	// 设置要显示的表格模板名称
	// 注意，如果表格不能由框架自动加载的，而是使用者提供的自定义表格，则需要向表格模板管理器注册这个表才能正确识别
	// 推荐自定义表格统一包装成接口，并在类似DllMain()类似的位置调用一下，确保只加载一次即可
	virtual void get_table_template_names(std::vector<std::pair<CString, std::vector<CString>>>& all_tables) const;
	virtual void save_data_from_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void load_data_to_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct);

public:
	// 图纸数据的tag，用于从数据中心索取图纸数据
	CString m_graph_data_tag;
};
