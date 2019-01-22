#include "stdafx.h"
#include <valarray>

#include "xml_tools.h"
#include "tinyxml2.h"
#include "string_tools.h"
#include "file_tools.h"

#include "THGridControl/THGridControlInc.h"

#include "../interface/console_interface.h"
#include "table_templates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


// 加static限定作用域为本文件
static std::vector<string> all_template_name;
static CTableTemplateManager all_table_tmeplates; // 加static限定作用域为本文件

table_templates::table_templates()
{
}

table_templates::~table_templates()
{
}

CTHDataBase* table_templates::create_data_base(const CString& template_name)
{
	return all_table_tmeplates.CreateDataBase(string_tools::CString_to_string(template_name).c_str());
}

void table_templates::load_table_template_name_from_dir(const CString& table_dir)
{
	all_template_name.clear();
	all_table_tmeplates.Clear();

	CString dir_full_path = table_dir;
	dir_full_path.Trim();

	if (dir_full_path.Left(1) == _T("\\")) dir_full_path.Delete(0);
	if (dir_full_path.Right(1) == _T("\\")) dir_full_path.Delete(dir_full_path.GetLength() - 1);

	dir_full_path.Trim(); // 删除字符后再次trim

	if (-1 == dir_full_path.Find(_T(':'))) // 没有:号认为是相对路径
		dir_full_path = file_tools::get_app_path_with_xg() + dir_full_path;
	else
		dir_full_path = dir_full_path;

	// 搜索指定路径下的所有表格模板文件名
	vector<CString> file_list;
	file_tools::get_all_files_full_path_in_dir(file_list, dir_full_path, _T("tab"));
	for (auto& x : file_list) all_template_name.push_back(string_tools::CString_to_string(x));

	//搜索指定路径下的txml表格
	vector<CString> file_list_xml;
	file_tools::get_all_files_full_path_in_dir(file_list_xml, dir_full_path, _T("txml"));
	for (auto& x : file_list_xml)
	{
		table_xml_template tmp_table;
		tmp_table.initialize_by_xml(x);

		for (auto& y : tmp_table.m_all_table)
		{
			CTHDataBase* db = y.generate_grid();
			if (!db) continue;

			CString template_name = y.m_template_name.CompareNoCase(_T("")) ? y.m_template_name
				: file_tools::get_name_from_file_path_name(x);
			CString printf_info = _T("加载") + template_name + _T("文件");
			app_print_info(message_type::MT_DEBUG, printf_info);
			table_templates::add_database_sample(template_name, db);
		}
	}
	// 加载模板数据
	all_table_tmeplates.LoadTableTemplates(all_template_name);
}

void table_templates::add_database_sample(const CString& name, CTHDataBase* db)
{
	CString trimed_name = name;
	trimed_name.Trim();
	if (trimed_name.IsEmpty()) return;

	if (!db) return;
	if (has_template_name(name)) return;

	// 模板会接管这个指针的内存管理
	all_table_tmeplates.AddTableTemplate(db, string_tools::CString_to_string(name).c_str());
}

bool table_templates::has_template_name(const CString& name)
{
	return all_table_tmeplates.isTemplateExist(string_tools::CString_to_string(name).c_str());
}

table_cell_config::table_cell_config()
{

}

table_cell_config::~table_cell_config()
{

}

single_table_config::single_table_config()
{

}

single_table_config::~single_table_config()
{

}

table_xml_template::table_xml_template()
{

}

table_xml_template::~table_xml_template()
{

}

void table_xml_template::initialize_by_xml(CString& path_name)
{
	if (path_name.IsEmpty()) return;

	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(path_name).c_str())) return;

	tinyxml2::XMLElement* node = doc.FirstChildElement("table_template_design");
	if (!node || node->NoChildren())
	{
		CString tmp_file_name =  file_tools::get_full_name_from_file_path_name(path_name);
		AfxMessageBox(tmp_file_name + _T("table_template_design") + _T("出错"));
		return;
	}
	node = node->FirstChildElement("table");
	if (!node)
	{
		CString tmp_file_name = file_tools::get_full_name_from_file_path_name(path_name);
		AfxMessageBox(tmp_file_name + _T("table") + _T("出错"));
		return;
	}

	do
	{
		single_template_config tmp_template;
		tmp_template.m_template_name = get_xml_node_attribute_str(node, _T("template_name"));

		if (0 != tmp_template.m_template_name.CompareNoCase(_T("")))
			app_print_info(message_type::MT_DEBUG, _T("加载") + tmp_template.m_template_name + _T("表格模板"));

		tmp_template.initialize_by_xml(node);
		m_all_table.push_back(tmp_template);

		node = node->NextSiblingElement("table");
	} while (node);
}

bool table_xml_template::is_empty()
{
	if (m_all_table.size() > 0)
		return false;
	else
		return true;
}

table_row_config::table_row_config()
{
	m_row_height = 0;
}

table_row_config::~table_row_config()
{

}

single_template_config::single_template_config()
{

}

single_template_config::~single_template_config()
{

}


void single_template_config::initialize_by_xml(tinyxml2::XMLElement* node)
{
	m_all_table_configs.clear();

	if (!node) return;

	m_column_count = 1; // 最少一列

	//////////////////////////////////////////////////////////////////////////

	if (node->FirstChildElement(_T("table")))
		node = node->FirstChildElement(_T("table"));

	//读取txml文件信息
	get_xml_table_node(node);

	////////////////////////////////////////////////////////////////////////

	//配置读取完成后对数据进行重整和校验，以满足模板的数据要求
	resize_verify_txml_data();
}

void single_template_config::get_xml_table_node(tinyxml2::XMLElement* node)
{
	if (!node) return;

	do
	{
		single_table_config tmp_table_config;
		tmp_table_config.m_table_type = get_xml_node_attribute_str(node, _T("table_type"));
		if (tinyxml2::XMLElement*  table_node = node->FirstChildElement("table"))
		{
			get_xml_table_node(table_node);
		}
		else if (tinyxml2::XMLElement* row_node = node->FirstChildElement("tr"))
		{
			get_xml_tr_node(row_node, tmp_table_config);
			m_all_table_configs.push_back(tmp_table_config);
		}
		else
		{
			//nothing
		}

		node = node->NextSiblingElement("table");

	} while (node);
}

void single_template_config::get_xml_tr_node(tinyxml2::XMLElement* tr_node, single_table_config& data)
{
	if (!tr_node) return;

	do
	{
		std::vector<CString> tmp_width;
		std::vector<CString> tmp_col_tag;
		table_row_config tmp_row;

		tinyxml2::XMLElement* cell_node = tr_node->FirstChildElement("td");
		if (!cell_node) return;

		do
		{
			table_cell_config tmp_cell;

			CString width = get_xml_node_attribute_str(cell_node, _T("width"));
			tmp_cell.m_colspan = get_xml_node_attribute_str(cell_node, _T("colspan"));
			tmp_cell.m_rowspan = get_xml_node_attribute_str(cell_node, _T("rowspan"));
			tmp_cell.m_valign = get_xml_node_attribute_str(cell_node, _T("valign"));
			tmp_cell.m_type = get_xml_node_attribute_str(cell_node, _T("type"));
			tmp_cell.m_label_text = get_xml_node_attribute_str(cell_node, _T("label"));
			tmp_cell.m_read = get_xml_node_attribute_str(cell_node, _T("read_only"));
			tmp_cell.m_cell_tag = get_xml_node_attribute_str(cell_node, _T("cell_tag"));
			CString col_tag = get_xml_node_attribute_str(cell_node, _T("col_tag"));

			tmp_cell.m_text = get_xml_node_text(cell_node);

			tmp_row.m_all_cell_configs.push_back(tmp_cell);
			tmp_width.push_back(width);
			tmp_col_tag.push_back(col_tag);

			cell_node = cell_node->NextSiblingElement("td");

		} while (cell_node);

		if (m_width.size() <= tmp_width.size()) m_width.swap(tmp_width);
		if (m_col_tag.size() <= tmp_col_tag.size()) m_col_tag.swap(tmp_col_tag);
		m_column_count = max(m_column_count, (int)tmp_row.m_all_cell_configs.size());

		tmp_row.m_row_tag = get_xml_node_attribute_str(tr_node, _T("row_tag"));
		tmp_row.m_row_height = _ttoi(get_xml_node_attribute_str(tr_node, _T("height")));

		data.m_all_row_configs.push_back(tmp_row);

		tr_node = tr_node->NextSiblingElement("tr");
	} while (tr_node);

}

void single_template_config::set_cell_property(CTHDataBase* db) const
{
	CTHCell * cell = nullptr;

	for (int row = 1; row != m_self_row_info.size() + 1; ++row)
	{
		table_row_config tmp_row = m_self_row_info[row - 1];
		db->SetRowTag(row, tmp_row.m_row_tag);
		if (tmp_row.m_row_height > 0)
			db->SetRowHeight(row, tmp_row.m_row_height);

		for (int col = 1; col != tmp_row.m_all_cell_configs.size() + 1; ++col)
		{
			db->SetColTag(col, m_col_tag[col - 1]);

			auto& tmp_cell = tmp_row.m_all_cell_configs[col - 1];
			cell = db->GetCell(row, col);

			if (tmp_cell.m_read.IsEmpty())
				cell->SetReadOnly(TRUE);
			else
				cell->SetReadOnly(FALSE);

			cell->SetTag(tmp_cell.m_cell_tag);
			cell->SetText(tmp_cell.m_text);
			cell->SetAlignment(DT_CENTER | DT_VCENTER);
			cell->SetLabelText(tmp_cell.m_label_text);

			if (0 == tmp_cell.m_type.CompareNoCase("CT_NORMAL")) cell->SetCellType(CT_NORMAL);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_DROPLIST")) cell->SetCellType(CT_DROPLIST);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_DROPDOWN")) cell->SetCellType(CT_DROPDOWN);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_CHECKBOX")) cell->SetCellType(CT_CHECKBOX);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_BUTTON")) cell->SetCellType(CT_BUTTON);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_TREECOMBOBOX")) cell->SetCellType(CT_TREECOMBOBOX);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_LINETYPECOMBOBOX")) cell->SetCellType(CT_LINETYPECOMBOBOX);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_LINEWIDTHCOMBOBOX")) cell->SetCellType(CT_LINEWIDTHCOMBOBOX);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_COLORCOMBOBOX")) cell->SetCellType(CT_COLORCOMBOBOX);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_EDITBUTTON")) cell->SetCellType(CT_EDITBUTTON);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_COLLAPSE")) cell->SetCellType(CT_COLLAPSE);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_ICONCOMBOXBOX")) cell->SetCellType(CT_ICONCOMBOXBOX);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_PATTERNCOMBOXBOX")) cell->SetCellType(CT_PATTERNCOMBOXBOX);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_TASKEDIT")) cell->SetCellType(CT_TASKEDIT);
			else if (0 == tmp_cell.m_type.CompareNoCase("CT_CHECKDROPLIST")) cell->SetCellType(CT_CHECKDROPLIST);
			else cell->SetCellType(CT_NORMAL);
		}
	}
}

void single_template_config::get_merge_info()
{
	//得到合并的单元格信息
	for (int row = 0; row != m_self_row_info.size(); ++row)
	{
		table_row_config& tmp_row_mess = m_self_row_info[row];
		for (int col = 0; col != (int)tmp_row_mess.m_all_cell_configs.size(); ++col)
		{
			table_cell_config& tmp_cell = tmp_row_mess.m_all_cell_configs[col];
			if (tmp_cell.m_colspan.IsEmpty() && tmp_cell.m_rowspan.IsEmpty()) continue;

			int merge_col_be = 0;
			int merge_col = _ttoi(tmp_cell.m_colspan) > 0 ? (_ttoi(tmp_cell.m_colspan) - 1) : 0;
			int merge_row = _ttoi(tmp_cell.m_rowspan) > 0 ? (_ttoi(tmp_cell.m_rowspan) - 1) : 0;

			for (auto& x : m_merge_info)
				if (x[2] - x[0] > 0 && x[2] >= row && row > x[0] && (col + merge_col_be) >= x[1])
					merge_col_be += (x[3] - x[1] + 1);

			m_merge_info.push_back({row, col + merge_col_be, row + merge_row, col + merge_col + merge_col_be});
		}
	}
}

void single_template_config::resize_verify_txml_data()
{
	std::vector<table_row_config> tmp_all_table_config;
	table_cell_config tmp_cell_new;

	//得到行组信息
	get_group_info();

	//合并到一张表格
	for (auto& x : m_all_table_configs)
		m_self_row_info.insert(m_self_row_info.end(), x.m_all_row_configs.begin(), x.m_all_row_configs.end());

	// 确保所有行的列数都相同,根据列合并信息调整行内位置
	for (int row = 0; row != m_self_row_info.size(); ++row)
	{
		table_row_config tmp_row_config;
		tmp_row_config.m_row_height = m_self_row_info[row].m_row_height;
		tmp_row_config.m_row_tag = m_self_row_info[row].m_row_tag;
		auto& ivec = m_self_row_info[row];

		for (int col = 0; col != ivec.m_all_cell_configs.size(); ++col)
		{
			table_cell_config& tmp_cell = ivec.m_all_cell_configs[col];
			tmp_row_config.m_all_cell_configs.push_back(tmp_cell);

			if (_ttoi(ivec.m_all_cell_configs[col].m_colspan) - 1 > 0)
				tmp_row_config.m_all_cell_configs.insert(tmp_row_config.m_all_cell_configs.end(),
				(_ttoi(ivec.m_all_cell_configs[col].m_colspan) - 1), table_cell_config());
		}

		if ((int)tmp_row_config.m_all_cell_configs.size() >= m_column_count)
			tmp_row_config.m_all_cell_configs.resize(m_column_count);
		else
			tmp_row_config.m_all_cell_configs.insert(tmp_row_config.m_all_cell_configs.end(),
			(m_column_count - (int)tmp_row_config.m_all_cell_configs.size()), table_cell_config());

		tmp_all_table_config.push_back(tmp_row_config);
	}

	m_self_row_info.swap(tmp_all_table_config);

	//得到合并信息
	get_merge_info();

	//根据表格行合并重整行内的位置
	for (auto& x : m_merge_info)
	{
		for (int row = 0; row != m_self_row_info.size(); ++row)
		{
			table_row_config& table = m_self_row_info[row];
			for (int col = 0; col < m_column_count; ++col)
				if (row > x[0] && x[2] - x[0] > 0 && x[2] >= row && col <= x[3] && col >= x[1])
					table.m_all_cell_configs.insert(table.m_all_cell_configs.begin() + col, table_cell_config());
		}
	}

	////调整合并后列数
	for (auto& x : m_self_row_info)
		x.m_all_cell_configs.resize(m_column_count);

	//// 确保至少有一行
	if (m_all_table_configs.empty()) m_all_table_configs.resize(1);
}

void single_template_config::set_row_count(int count)
{
	set_table_size(count, get_column_count());
}

int single_template_config::get_row_count() const
{
	return m_self_row_info.size();
}

void single_template_config::set_table_size(int row_count, int column_count)
{
	if (row_count <= 0 || column_count <= 0) return;

	for (auto& x : m_all_table_configs)
	{
		for (int i = 0; i < get_row_count(); ++i)
			x.m_all_row_configs[i].m_all_cell_configs.resize(column_count);

		x.m_all_row_configs.resize(row_count);
	}

	m_self_row_info.clear();
	for (auto& x : m_all_table_configs)
		m_self_row_info.insert(m_self_row_info.end(), x.m_all_row_configs.begin(), x.m_all_row_configs.end());
}

void single_template_config::set_column_count(int count)
{
	set_table_size(get_row_count(), count);
}

int single_template_config::get_column_count() const
{
	if (m_all_table_configs.empty()) return 0;

	int col_num = 0;

	for (size_t table_num = 0; table_num < m_all_table_configs.size(); ++table_num)
	{
		single_table_config tmp_table = m_all_table_configs[table_num];

		for (size_t row_num = 0; row_num < tmp_table.m_all_row_configs.size(); ++row_num)
		{
			table_row_config& tmp_row = tmp_table.m_all_row_configs[row_num];

			col_num = col_num >(int)tmp_row.m_all_cell_configs.size() ? col_num : (int)tmp_row.m_all_cell_configs.size();
		}
	}
	return col_num;
}

table_cell_config& single_template_config::get_cell_config(int row_index, int col_index)
{
	int row = row_index - 1;
	int col = col_index - 1;
	return m_self_row_info[row].m_all_cell_configs[col]; // 越界则让容器直接抛异常由外部处理，此处不限定
}

void single_template_config::get_rows_height(std::vector<std::pair<int, int> >& row_height)
{
	int row = 1;
	row_height.clear();

	for (size_t table_num = 0; table_num < m_all_table_configs.size(); ++table_num)
	{
		single_table_config& tmp_table = m_all_table_configs[table_num];
		for (size_t row_num = 0; row_num < tmp_table.m_all_row_configs.size(); ++row_num)
		{
			table_row_config& tmp_row = tmp_table.m_all_row_configs[row_num];

			int height = 20 > tmp_row.m_row_height ? 20 : tmp_row.m_row_height;

			row_height.push_back({ row,(int)height });
			++row;
		}
	}
}

void single_template_config::get_cols_width(std::vector<std::pair<int, int> >& col_width)
{
	int col = 1;
	for (size_t i = 0; i < m_width.size(); ++i)
	{
		col_width.push_back({ col, _ttoi(m_width[i]) });
		++col;
	}
}

void single_template_config::get_merge_data(std::vector<std::array<int, 4>>& data) const
{
	data.clear();

	for (auto& x : m_merge_info)
	{
		data.push_back({x[0] + 1,x[1] + 1, x[2] + 1,x[3] + 1});
	}
	return;
}

void single_template_config::get_group_info()
{
	for (int i = 0; i < (int)m_all_table_configs.size(); ++i)
	{
		int before_row = 0;
		for (int j = 0; j < i; ++j)
			before_row += m_all_table_configs[j].m_all_row_configs.size();

		m_group.push_back(
		{
			string_tools::num_to_string(before_row + 1),
			string_tools::num_to_string(m_all_table_configs[i].m_all_row_configs.size() + before_row),
			m_all_table_configs[i].m_table_type
		});
	}
}

CTHDataBase* single_template_config::generate_grid() const
{
	int col_num = get_column_count();
	int row_num = get_row_count();

	CTHDataBase* db = new CTHDataBase();

	db->SetRowCount(row_num + 1);
	db->SetColCount(col_num + 1);
	db->SetFixedRowCount(1);
	db->SetFixedColCount(1);
	db->EnableRowGroup(FALSE);
	db->EnableColMenu(FALSE);
	db->EnableRowMenu(TRUE);
	db->EnableColResize(TRUE);
	db->EnableRowResize(FALSE);
	db->EnableRowAutoSerialNumber(TRUE);

	db->SetRowHeight(0, 0);

	db->SetColWidth(0, 0);

	//设置列宽
	for (int i = 1; i != (int)(m_width.size() + 1); ++i)
		db->SetColWidth(i, _ttoi(m_width[i - 1]));

	if (m_width.size() < 1)
		for (int i = 1; i != col_num + 1; ++i)
			db->SetColWidth(i, 60);

	CTHCell* cell = nullptr;

	//校验表格合并信息
	if ( false == verify_merge_info()) 
		return db;

	//合并单元格
	for (auto& x : m_merge_info)
		db->MergeCells(x[0] + 1, x[1] + 1, x[2] + 1, x[3] + 1);

	//设置单元格属性及内容
	for (auto& x : m_all_table_configs)
	{
		if (0 == x.m_table_type.CompareNoCase(_T("line_table")))
		{
			db->EnableRowGroup(TRUE);
			break;
		}
	}

	set_cell_property(db);
	set_title_info(db); //设置标题格式
	set_group_property(db); //设置行组信息

	return db;
}

void single_template_config::set_title_info(CTHDataBase* db) const
{
	for (int i = 0; i < (int)m_all_table_configs.size(); ++i)
	{
		int row = 0;
		for (int j = 0; j < i; ++j)
			row += m_all_table_configs[j].m_all_row_configs.size();

		if (!m_all_table_configs[i].m_table_type.CompareNoCase(_T("attribute_table")))
			for (int row_t = 0; row_t < (int)m_all_table_configs[i].m_all_row_configs.size(); ++row_t)
				db->GetCell(row_t + row + 1, 1)->SetAlignment(DT_LEFT | DT_VCENTER);

		if (1 == row + 1)
		{
			db->SetTitleFormat(row + 1, 1, RT_TABLETITLE);
			db->GetCell(row + 1, 1)->SetAlignment(DT_CENTER | DT_VCENTER);
		}
		else
		{
			db->SetTitleFormat(row + 1, 1, RT_SUBTITLE);
			db->GetCell(row + 1, 1)->SetAlignment(DT_CENTER | DT_VCENTER);
		}
	}
}

bool single_template_config::verify_merge_info() const
{
	int row = get_row_count();
	int col = get_column_count();

	
	for (size_t i = 0; i < m_merge_info.size(); ++i)
	{
		std::array<int,4> tmp_merge = m_merge_info[i];

		//判断是否越界合并
		if (tmp_merge[0] + 1 > row || tmp_merge[1] + 1 > col || tmp_merge[2] < 0 || tmp_merge[3] < 0)
		{
			CString tmp_return = m_template_name + _T("\n 第 ") + string_tools::num_to_string(tmp_merge[0] + 1)
				+ _T(" 行,") + _T("第 ") + string_tools::num_to_string(tmp_merge[1] + 1) + _T("列 \n")
				+ _T("到 第 ") + string_tools::num_to_string(tmp_merge[2] + 1) + _T("行")
				+ _T("第 ") + string_tools::num_to_string(tmp_merge[3] + 1) + _T("列 \n")
				+ _T("合并单元格越界，合并信息错误!");

			AfxMessageBox(tmp_return);
			return false;
		}

		//判断是否合并重复
		int sum_num = 0;
		for (size_t num = 0; num < m_merge_info.size(); ++num)
		{
			std::array<int, 4> tmp_merge_info = m_merge_info[num];

			if (tmp_merge[0] > tmp_merge_info[0] && tmp_merge[0] < tmp_merge_info[2]
				&& tmp_merge[1] > tmp_merge_info[1] && tmp_merge[1] < tmp_merge_info[3])
				++sum_num;

			if (tmp_merge[0] > tmp_merge_info[0] && tmp_merge[0] < tmp_merge_info[2]
				&& tmp_merge[3] > tmp_merge_info[1] && tmp_merge[3] < tmp_merge_info[3])
				++sum_num;
		
			if (tmp_merge[2] > tmp_merge_info[0] && tmp_merge[2] < tmp_merge_info[2]
				&& tmp_merge[1] > tmp_merge_info[1] && tmp_merge[1] < tmp_merge_info[3])
				++sum_num;

			if (tmp_merge[2] > tmp_merge_info[0] && tmp_merge[2] < tmp_merge_info[2]
				&& tmp_merge[3] > tmp_merge_info[1] && tmp_merge[3] < tmp_merge_info[3])
				++sum_num;
		}
		if (sum_num > 0)
		{
			CString tmp_return = m_template_name + _T("\n 第 ") + string_tools::num_to_string(tmp_merge[0] + 1)
				+ _T(" 行,") + _T("第 ") + string_tools::num_to_string(tmp_merge[1] + 1) + _T("列 \n")
				+ _T("到 第 ") + string_tools::num_to_string(tmp_merge[2] + 1) + _T("行")
				+ _T("第 ") + string_tools::num_to_string(tmp_merge[3] + 1) + _T("列 \n")
				+ _T("合并单元格重复，合并信息错误!");

			AfxMessageBox(tmp_return);
			return false;
		}
	}

	return true;
}

void single_template_config::set_group_property(CTHDataBase* db) const
{
	int static_line = 1;
	int insert_line = 1;

	for (auto& x : m_group)
	{
		if (0 == x[2].CompareNoCase(_T("line_table")))
		{
			// 处理静态行组
			if (_ttoi(x[0]) > 0)
			{
				db->SetAsRowGroup(_ttoi(x[0]), _ttoi(x[1]) - 1, 0, _T("static_row")
					+ string_tools::num_to_string(static_line), FALSE);
				
				++static_line;
			}

			// 处理动态行组
			if (m_group.size() > 1)
			{
				db->SetAsRowGroup(_ttoi(x[1]), _ttoi(x[1]), 1, _T("insert_row_")
					+ string_tools::num_to_string(insert_line), TRUE, 0);
				
				++insert_line;
			}
			else
			{
				db->SetAsRowGroup(_ttoi(x[1]), _ttoi(x[1]), 1, _T("insert_row_")
					+ string_tools::num_to_string(insert_line), TRUE);
				
				++insert_line;
			}
		}
		else
		{
			db->SetAsRowGroup(_ttoi(x[0]), _ttoi(x[1]), 0, _T("static_row")
				+ string_tools::num_to_string(static_line), FALSE);
			
			++static_line;
		}
	}

	if (m_group.size() > 1)
		db->SetAsRowGroup(1, get_row_count(), 0, _T("sub_group"), TRUE, 0);
	
	db->SetAsRowGroup(1, get_row_count() + 1, 0, _T("whole_group"), FALSE);
}