#include "stdafx.h"
#include "no_tab_single_grid_observer.h"


IMPLEMENT_SERIAL(no_tab_single_grid_observer, x_observer_base,1)
no_tab_single_grid_observer::no_tab_single_grid_observer()
{
}


no_tab_single_grid_observer::~no_tab_single_grid_observer()
{
}

void no_tab_single_grid_observer::get_table_template_names(std::pair<CString, CString>& table) const
{

}

void no_tab_single_grid_observer::save_data_from_ui(std::pair<CString, CTHDataBase*>& data_base)
{

}

void no_tab_single_grid_observer::load_data_to_ui(std::pair<CString, CTHDataBase*>& data_base)
{

}

void no_tab_single_grid_observer::cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct)
{

}

void no_tab_single_grid_observer::cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct)
{

}

void no_tab_single_grid_observer::cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct)
{

}
