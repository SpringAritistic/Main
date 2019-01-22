#include "stdafx.h"

#include "THGridControl/THGridControlInc.h"
#include "THGridControl/THDataBase.h"

#include "frame_data.h"
#include "frame_grid_tools.h"
#include "../tools/table_templates.h"
#include "../tools/string_tools.h"

#include "hook_setting_dialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


IMPLEMENT_DYNAMIC(dialog_node_hook_setting, CDialog)
BEGIN_MESSAGE_MAP(dialog_node_hook_setting, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY_EX(GN_FOCUSCHANGE_PRE, IDC_STATIC_HOOKS, on_focus_changed)
END_MESSAGE_MAP()


dialog_node_hook_setting::dialog_node_hook_setting(CWnd* parent /*=NULL*/)
	: CDialog(dialog_node_hook_setting::IDD, parent)
{
	m_hook_excel = new CTHExcel();
	m_hook_settings_db = NULL;

}

dialog_node_hook_setting::~dialog_node_hook_setting()
{
	safe_delete(m_hook_excel);
	safe_delete(m_hook_settings_db);
}

void dialog_node_hook_setting::DoDataExchange(CDataExchange* dx)
{
	CDialog::DoDataExchange(dx);
}

BOOL dialog_node_hook_setting::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_hook_settings_db = table_templates::create_data_base(FRAME_HOOKS_SUB_TABLE);
	if (NULL == m_hook_settings_db)
	{
		_ASSERTE((_T("m_pDB = pTemplate->CreateDataBase(...)"), false));
		return TRUE;
	}

	m_hook_excel->CreateFromStatic(this, IDC_STATIC_HOOKS);
	m_hook_excel->SetUsingDataBase(m_hook_settings_db);
	m_hook_settings_db->SetDefRowHeight(22); // 保持与方案设计师的默认行高一致

	m_hook_settings_db->SetEnvVariable(_T("m_strHookName"), m_hook_name_list);
	m_hook_settings_db->SetEnvVariable(_T("m_strRefLine"), m_ref_lines);
	m_hook_settings_db->SetEnvVariable(_T("m_strSegName"), m_ref_segment);

	vector<frame_hook_setting> vecSettings;
	string_to_hook_settings(m_setting_info_text, vecSettings);
	set_data_to_grid(vecSettings);

	m_hook_settings_db->ReFresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// 检测一个字符串中是否含有某个字符,有则返回true
bool IsCharExist(const CString& strText, TCHAR chr)
{
	return (-1 != strText.Find(chr));
}

// 检测一个字符串中是否含有 [ ] , ; 这四个字符,正确则返回true
bool is_right_hook_setting_item_text(const CString& strText)
{
	if (IsCharExist(strText, _T('['))) return false;
	if (IsCharExist(strText, _T(']'))) return false;
	if (IsCharExist(strText, _T(','))) return false;
	if (IsCharExist(strText, _T(';'))) return false;
	
	return true;
}

// 检测弯钩中的数据是否正确,如果含有不该有的字符则错误,正确则返回true
bool check_hook_setting_data(const frame_hook_setting& setting)
{
	if (!is_right_hook_setting_item_text(setting.m_hook_name)) return false;
	if (!is_right_hook_setting_item_text(setting.m_ref_line_name)) return false;
	if (!is_right_hook_setting_item_text(setting.m_is_top_hook)) return false;
	if (!is_right_hook_setting_item_text(setting.m_segment_name)) return false;
	if (!is_right_hook_setting_item_text(setting.m_node_begin_layer)) return false;
	if (!is_right_hook_setting_item_text(setting.m_node_index)) return false;

	return true;
}
void dialog_node_hook_setting::OnOK()
{
	vector<frame_hook_setting> hook_settings;
	get_data_from_grid(hook_settings);
	// [2011-08-11]由于更改了分隔符为可显示的[ ] , ; 用户可能输入这些特殊的数据导致解析异常
	// 所以要检测,如果输入了,则提示,并保持对话框不关闭.
	
	bool bIsWrong = false;
	for (size_t i = 0; i != hook_settings.size(); ++i)
	{
		if (!check_hook_setting_data(hook_settings[i]))
		{
			bIsWrong = true;
			break;
		}
	}

	if (!bIsWrong)
	{
		hook_settings_to_string(hook_settings, m_setting_info_text);
		CDialog::OnOK();
	}
	else
	{
		MessageBox(_T("输入的数据中不允许存在 [ ] , ; 四个字符"));
	}

}

void dialog_node_hook_setting::OnCancel()
{
	CDialog::OnCancel();
}

void dialog_node_hook_setting::OnClose()
{
	CDialog::OnClose();
}

void dialog_node_hook_setting::get_data_from_grid(OUT vector<frame_hook_setting>& setting_data)
{
	setting_data.clear();
	
	if (NULL == m_hook_settings_db)
	{
		_ASSERTE((_T("if (NULL == m_pHooksSettingDB)"), false));
		return;
	}

	int row_count = m_hook_settings_db->GetNumberRows(); // 数据行数，不是表行数
	if (row_count <= 0) return; // 空表没有数据

	frame_hook_setting setting;
	for (int i = 0; i < row_count; ++i)
	{
		m_hook_settings_db->Get_Prop(i, _T("m_hook_name"), setting.m_hook_name);
		m_hook_settings_db->Get_Prop(i, _T("m_ref_line_name"), setting.m_ref_line_name);
		m_hook_settings_db->Get_Prop(i, _T("m_is_top_hook"), setting.m_is_top_hook);
		m_hook_settings_db->Get_Prop(i, _T("m_segment_name"), setting.m_segment_name);
		m_hook_settings_db->Get_Prop(i, _T("m_node_begin_layer"), setting.m_node_begin_layer);
		m_hook_settings_db->Get_Prop(i, _T("m_node_index"), setting.m_node_index);
		
		setting_data.push_back(setting);

		// 界面保持用户操作的行为，哪怕是错误的弯钩数据照样存储起来，不到绘图不对其处理。

	} // end of for(...)

}

void dialog_node_hook_setting::set_data_to_grid(IN const vector<frame_hook_setting>& setting_data)
{
	int row_count = m_hook_settings_db->GetNumberRows();

	// 删除表中的所有行
	m_hook_settings_db->RemoveRowsAt(2, row_count);

	int nInsertRowsCount = int(setting_data.size());
	if (nInsertRowsCount <= 0) nInsertRowsCount = 5; // 如果表为空,则填充5行

	// 表格清理干净后，先插入足够多的行，为提高效率没有在循环写入数据的时候一行一行的插入，
	// 而是在写数据之前一次性插入足够多的行后再填充数据。
	OpReturnValue opFlag = m_hook_settings_db->InsertRowPost( 1, nInsertRowsCount );
	if (-1 == opFlag.count)
	{
		_ASSERTE((_T("表行插入操作失败，无法正常完成表格数据的插入操作"), false ));
		return;
	}

	// 将弯钩数据插入到弯钩表格中
	for (int i = 0; i < int(setting_data.size()); ++i) // 从下标为1的行开始填入，下标为0的是通用弯钩。
	{
		m_hook_settings_db->Set_Prop(i, _T("m_hook_name"), CTHVariant(setting_data[i].m_hook_name));
		m_hook_settings_db->Set_Prop(i, _T("m_ref_line_name"), CTHVariant(setting_data[i].m_ref_line_name));
		m_hook_settings_db->Set_Prop(i, _T("m_is_top_hook"), CTHVariant(setting_data[i].m_is_top_hook));
		m_hook_settings_db->Set_Prop(i, _T("m_segment_name"), CTHVariant(setting_data[i].m_segment_name));
		m_hook_settings_db->Set_Prop(i, _T("m_node_begin_layer"), CTHVariant(setting_data[i].m_node_begin_layer));
		m_hook_settings_db->Set_Prop(i, _T("m_node_index"), CTHVariant(setting_data[i].m_node_index));
	}
}

BOOL dialog_node_hook_setting::on_focus_changed( UINT nID, NMHDR* notify_struct, LRESULT* result )
{
	if (!m_hook_settings_db)
	{
		_ASSERTE((_T("CTHDataBase* pDataBase = pExcel->GetDataBase(); 返回值为空指针"), false));
		return FALSE;
	}
	NM_THGRID* msg = (NM_THGRID*)notify_struct;
	if (NULL == msg)
	{
		_ASSERTE((_T("NM_THGRID *msg=(NM_THGRID *)pNotifyStruct; 返回值为空指针"), false));
		return FALSE;
	}
	int row = msg->rowNo;
	int col = msg->colNo;

	if (2 != col || row < 2) return FALSE; // 第二列是参考段，内容行从2行开始

	CTHCell* cell = m_hook_settings_db->GetCell(row, col);
	if (!cell) return FALSE;

	cell->SetSketchMapName(m_setment_sketch);
	
	return TRUE;
}