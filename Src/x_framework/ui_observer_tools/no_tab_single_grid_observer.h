#pragma once
#include <utility>

#include "x_observer_base.h"

class CTHDataBase;
struct NM_THGRID;

class X_FRAMEWORK_API no_tab_single_grid_observer :public x_observer_base
{
	DECLARE_SERIAL(no_tab_single_grid_observer)

public:
	no_tab_single_grid_observer();
	virtual ~no_tab_single_grid_observer();

public:
	virtual void get_table_template_names(std::pair<CString, CString>& table) const;
	virtual void save_data_from_ui(std::pair<CString, CTHDataBase*>& data_base);
	virtual void load_data_to_ui(std::pair<CString, CTHDataBase*>& data_base);
	virtual void cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct);
	virtual void cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct);

};

