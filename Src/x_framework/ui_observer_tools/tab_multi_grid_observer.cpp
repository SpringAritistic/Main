#include "stdafx.h"
#include "tab_multi_grid_observer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_SERIAL(tab_multi_grid_observer, x_observer_base, 1)

tab_multi_grid_observer::tab_multi_grid_observer()
{
}

tab_multi_grid_observer::~tab_multi_grid_observer()
{
}

void tab_multi_grid_observer::get_table_template_names(std::vector<std::pair<CString, std::vector<CString>>>& all_tables) const
{
}

void tab_multi_grid_observer::get_table_instances(std::vector<std::pair<CString, std::vector<std::pair<CString, CTHDataBase*>>>>& all_tables) const
{
}

bool tab_multi_grid_observer::is_tab_name_at_front(const CString& front, const CString& back) const
{
	return true;
}

void tab_multi_grid_observer::save_data_from_ui(std::map<CString, CTHDataBase*>& all_dbs)
{
}

void tab_multi_grid_observer::load_data_to_ui(std::map<CString, CTHDataBase*>& all_dbs)
{
}

void tab_multi_grid_observer::cell_button_be_clicked(CTHDataBase* db, NM_THGRID* notify_struct)
{
}

void tab_multi_grid_observer::cell_value_changed(CTHDataBase* db, NM_THGRID* notify_struct)
{
}

void tab_multi_grid_observer::cell_focus_changed(CTHDataBase* db, NM_THGRID* notify_struct)
{
}
