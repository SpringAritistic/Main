#include "stdafx.h"

#include "THGridControl/THGridControlMac.h"
#include "THGridControl/THExcel.h"
#include "THGridControl/THDataBase.h"

#include "dialog_tendon_random_curves.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

IMPLEMENT_DYNAMIC(dialog_tendon_random_curves, dialog_sub_grid)


dialog_tendon_random_curves::dialog_tendon_random_curves(CWnd* parent) :dialog_sub_grid(parent)
{

}

dialog_tendon_random_curves::dialog_tendon_random_curves(const CString& template_name, int default_height, int default_width, const CString& title /*= _T("")*/, CWnd* parent /*= nullptr*/)
	: dialog_sub_grid(template_name, default_height, default_width, title, parent)
{

}

dialog_tendon_random_curves::~dialog_tendon_random_curves()
{

}

void dialog_tendon_random_curves::get_data_from_ui()
{
	m_text = _T("");

	if (!m_db) return;
	if (m_excel) m_excel->Update();

	for (int i = 0; i < m_db->GetRowCount() - m_db->GetFixedRowCount(); ++i)
	{	
		bool is_all_blank = true;

		CString m_X;
		m_db->Get_Prop(i, _T("m_X"), m_X);
		if (!m_X.IsEmpty()) is_all_blank = false;

		CString m_Y;
		m_db->Get_Prop(i, _T("m_Y"), m_Y);
		if (!m_Y.IsEmpty()) is_all_blank = false;

		CString m_R;
		m_db->Get_Prop(i, _T("m_R"), m_R);
		if (!m_R.IsEmpty()) is_all_blank = false;
		
		if (!is_all_blank) m_text += m_X + _T(",") + m_Y + _T(",") + m_R + _T(",") + _T(";");
	}
}

void dialog_tendon_random_curves::set_data_to_ui()
{
	std::vector<CString> data_divide_by_semicolon;
	split_string_by_symbol(data_divide_by_semicolon, m_text, _T(';'));

	int template_row_count = m_db->GetRowCount() - m_db->GetFixedRowCount();
	m_db->RemoveRowsAt(2, template_row_count);

	int row_count = max((int)data_divide_by_semicolon.size(), 5);
	m_db->InsertRowPost(1, row_count);

	for (size_t i = 0; i != data_divide_by_semicolon.size(); ++i)
	{
		vector<CString> result;
		split_string_by_symbol(result, data_divide_by_semicolon[i], _T(','));
		result.resize(3);
		m_db->Set_Prop(i, _T("m_X"), result[0]);
		m_db->Set_Prop(i, _T("m_Y"), result[1]);
		m_db->Set_Prop(i, _T("m_R"), result[2]);
	}
	if (m_excel) m_excel->ReFresh(TRUE);
}

void dialog_tendon_random_curves::split_string_by_symbol(std::vector<CString>& result, const CString& text_entry, const TCHAR symbol)
{
	result.clear();

	CString text_copy = text_entry;
	if (text_copy.IsEmpty()) return;

	if (text_copy[text_entry.GetLength() - 1] != symbol) text_copy += symbol;

	CString text_item;
	for (int i = 0; i < text_copy.GetLength(); ++i)
	{
		if (text_copy[i] == symbol)
		{
			result.push_back(text_item);
			text_item = _T("");
			continue;
		}
		text_item += text_copy[i];
	}
}
