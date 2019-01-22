#include "StdAfx.h"

#include "THGridControl/THGridControlInc.h"
#include "THGridControl\THBaseExcel.h"

#include "steel_grid_format.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



format_design steel_grid_format::get_design_format( const CString& id )
{
	for (size_t i = 0; i != m_design_format.size(); ++i)
	{
		if (id == m_design_format[i].m_id)
		{
			return m_design_format[i];
		}
	}

	_ASSERTE((_T("概念筋标识符不存在"), false));
	return format_design();
}

format_basic_grid steel_grid_format::get_basic_format( const CString& id )
{
	for (size_t i = 0; i != m_basic_format.size(); ++i)
	{
		if (id == m_basic_format[i].m_id)
		{
			return m_basic_format[i];
		}
	}

	_ASSERTE((_T("概念筋标识符不存在"), false));
	return format_basic_grid();

}

format_layout_grid steel_grid_format::get_layout_format( const CString& id, const CString& sub_id )
{
	for (size_t i = 0; i != m_layout_format.size(); ++i)
	{
		if (id == m_layout_format[i].m_id && sub_id == m_layout_format[i].m_sub_id)
		{
			return m_layout_format[i];
		}
	}

	_ASSERTE((_T("概念筋标识符不存在"), false));
	return format_layout_grid();

}

format_edge_grid steel_grid_format::get_edge_format( const CString& id, const CString& sub_id )
{
	for (size_t i = 0; i != m_edge_format.size(); ++i)
	{
		if (id == m_edge_format[i].m_id && sub_id == m_edge_format[i].m_sub_id)
		{
			return m_edge_format[i];
		}
	}

	_ASSERTE((_T("概念筋标识符不存在"), false));
	return format_edge_grid();

}

format_edge_detail steel_grid_format::get_detail_format( const CString& id, const CString& sub_id )
{
	for (size_t i = 0; i != m_detail_format.size(); ++i)
	{
		if (id == m_detail_format[i].m_id && sub_id == m_detail_format[i].m_sub_id)
		{
			return m_detail_format[i];
		}
	}

	_ASSERTE((_T("概念筋标识符不存在"), false));
	return format_edge_detail();

}

format_caption steel_grid_format::get_caption_format()
{
	return m_caption_format;

}

steel_grid_format::steel_grid_format( void )
{

}

steel_grid_format::~steel_grid_format( void )
{

}

format_cell::format_cell()
{
	m_text = _T("");					// 单元格中的文本
	m_is_read_only = false;				// 是否为只读
	m_hint_text = _T("");				// 提示文字
	m_data_type = CELLDATA_STRING;		// 数据类型
	m_alignment = DT_LEFT | DT_VCENTER | DT_HOR; // 对齐方式
	m_label_text = _T("");			// 下拉列表中的文字
	m_cell_type = CT_NORMAL;			// 单元格类型
}

format_cell::~format_cell()
{

}