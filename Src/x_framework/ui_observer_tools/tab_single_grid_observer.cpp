#include "stdafx.h"
#include "tab_single_grid_observer.h"

IMPLEMENT_SERIAL(tab_single_grid_observer,x_observer_base,1)
tab_single_grid_observer::tab_single_grid_observer()
{
}


tab_single_grid_observer::~tab_single_grid_observer()
{
}

void tab_single_grid_observer::get_table_template_names(std::vector<std::pair<CString, CString>>& all_tables) const
{

}

void tab_single_grid_observer::save_data_from_ui(std::map<CString, CTHDataBase*>& all_dbs)
{

}

void tab_single_grid_observer::load_data_to_ui(std::map<CString, CTHDataBase*>& all_dbs)
{

}

void tab_single_grid_observer::cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct)
{

}

void tab_single_grid_observer::cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct)
{

}

void tab_single_grid_observer::cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct)
{

}
