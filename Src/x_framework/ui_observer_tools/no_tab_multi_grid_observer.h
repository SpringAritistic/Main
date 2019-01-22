#pragma once
#include "x_observer_base.h"

class CTHDataBase;
struct NM_THGRID;

class X_FRAMEWORK_API no_tab_multi_grid_observer :public x_observer_base
{
	DECLARE_SERIAL(no_tab_multi_grid_observer)

public:
	no_tab_multi_grid_observer();
	virtual ~no_tab_multi_grid_observer();

public:
	virtual void get_table_template_names(std::pair<CString, std::vector<CString>>& all_tables) const;
	virtual void save_data_from_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void load_data_to_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct);
};

