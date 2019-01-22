#pragma once
#include <vector>
#include <map>
#include "x_observer_base.h"

struct NM_THGRID;
class CTHDataBase;

class X_FRAMEWORK_API tab_single_grid_observer :public x_observer_base
{
	DECLARE_SERIAL(tab_single_grid_observer)
public:
	tab_single_grid_observer();
	~tab_single_grid_observer();

public:
	virtual void get_table_template_names(std::vector<std::pair<CString, CString>>& all_tables) const;
	virtual void save_data_from_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void load_data_to_ui(std::map<CString, CTHDataBase*>& all_dbs);
	virtual void cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct);
};

