#pragma once

#include <map>
#include <vector>
#include "x_observer_base.h"


struct NM_THGRID;
class CTHDataBase;


class X_FRAMEWORK_API tab_multi_grid_observer : public x_observer_base
{
	DECLARE_SERIAL(tab_multi_grid_observer)

public:
	tab_multi_grid_observer();
	virtual ~tab_multi_grid_observer();

public:
	virtual void get_table_template_names(std::vector<std::pair<CString, std::vector<CString>>>& all_tables) const;

	// 没有模板名的表格使用此接口送到界面，注意索引名称不能与get_table_template_names中的模板名称相同
	// observer一旦关联到响应的编辑器窗口后，CTHDataBase*的指针由框架接管，否则框架不负责管理指针
	virtual void get_table_instances(std::vector<std::pair<CString, std::vector<std::pair<CString, CTHDataBase*>>>>& all_tables) const;

	// 用于对observer拥有的选项卡顺序排序，front在back前面返回true，默认返回true
	virtual bool is_tab_name_at_front(const CString& front, const CString& back) const;

	virtual void save_data_from_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void load_data_to_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct);
};
