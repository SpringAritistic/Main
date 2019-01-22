#include "StdAfx.h"

#include "steel_grid_condition.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


steel_grid_pos::steel_grid_pos(IN const CString& id, IN int index)
{
	set(id, index);
}

steel_grid_pos::steel_grid_pos()
{
	m_id = _T("");
	m_value_index = 0;
}

void steel_grid_pos::set(IN const CString& id, IN int index)
{
	m_id = id;
	m_value_index = index % 6; // 取值只能从0到5
}

steel_grid_pos::~steel_grid_pos()
{

}

steel_grid_conditions::steel_grid_conditions()
{
	m_action = SGCA_READONLY;
	m_is_force_condition = false;
	m_is_condition_exist_and_true = false;
}

steel_grid_conditions::steel_grid_conditions( IN const CString& cond_id, IN int cond_index, IN const CString& cond_action, IN const CString& cond_value, IN const CString& des_id, IN int des_index, IN steel_grid_condition_action des_action )
{
	m_is_force_condition = false;
	m_is_condition_exist_and_true = false;
	set(cond_id, cond_index, cond_action, cond_value, des_id, des_index, des_action);
}

steel_grid_conditions::steel_grid_conditions( IN const steel_grid_pos& cond_pos, IN const CString& cond_action, IN const CString& cond_value, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action )
{
	m_is_force_condition = false;
	m_is_condition_exist_and_true = false;
	set(cond_pos, cond_action, cond_value, des_pos, des_action);
}

steel_grid_conditions::steel_grid_conditions(IN bool is_ok, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action )
{
	m_is_force_condition = true;
	m_is_condition_exist_and_true = is_ok;
	set(is_ok, des_pos, des_action);
}

steel_grid_conditions::~steel_grid_conditions()
{

}

void steel_grid_conditions::set(IN const CString& cond_id, IN int cond_index, IN const CString& cond_action, IN const CString& cond_value, IN const CString& des_id, IN int des_index, IN steel_grid_condition_action des_action )
{
	m_is_force_condition = false;
	m_is_condition_exist_and_true = false;
	steel_grid_pos cond_pos(cond_id, cond_index);
	steel_grid_pos des_pos(des_id, des_index);
	
	set(cond_pos, cond_action, cond_value, des_pos, des_action);

}

void steel_grid_conditions::set( IN const steel_grid_pos& cond_pos, IN const CString& cond_action, IN const CString& cond_value, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action )
{
	m_is_force_condition = false;
	m_is_condition_exist_and_true = false;
	m_conditions.clear();
	single_condition singCond = { cond_pos, cond_action, cond_value };
	m_des_cell = des_pos;
	m_conditions.push_back(singCond);
	m_action = des_action;
}

void steel_grid_conditions::set( IN bool is_ok, IN const steel_grid_pos& des_pos, IN steel_grid_condition_action des_action )
{
	m_is_force_condition = true;
	m_is_condition_exist_and_true = is_ok;
	m_conditions.clear();
	m_des_cell = des_pos;
	m_action = des_action;
}

void steel_grid_conditions::and( IN const steel_grid_pos& cond_pos, IN const CString& cond_action, IN const CString& cond_value )
{
	single_condition singCond = { cond_pos, cond_action, cond_value};
	m_conditions.push_back(singCond);
}

void steel_grid_conditions::and( IN const CString& cond_id, IN int cond_index, IN const CString& cond_action, IN const CString& cond_value )
{
	steel_grid_pos cond_pos(cond_id, cond_index);
	and(cond_pos, cond_action, cond_value);
}

void steel_grid_conditions::clear_all_conditions()
{
	m_conditions.clear();
	m_is_force_condition = false;
}

int steel_grid_conditions::get_condition_count() const
{
	return int(m_conditions.size());
}

const steel_grid_pos steel_grid_conditions::get_condition_pos_at( int index ) const
{
	return m_conditions[index].cell_condition;
}

const steel_grid_pos steel_grid_conditions::get_des_pos() const
{
	return m_des_cell;
}

const CString steel_grid_conditions::get_condition_value_at( int index ) const
{
	return m_conditions[index].value;
}

enum steel_grid_condition_action steel_grid_conditions::get_condition_action() const
{
	return m_action;
}

const CString steel_grid_conditions::get_condition_type_at( int index ) const
{
	if (_T("==") == m_conditions[index].condition_action_text)
		return _T("==");
	else if (_T("!=") == m_conditions[index].condition_action_text)
		return _T("!=");
	else
		return _T("");
}

bool steel_grid_conditions::is_force_condition() const
{
	return m_is_force_condition;
}

bool steel_grid_conditions::is_condition_exist_and_true() const
{
	return m_is_condition_exist_and_true;
}
