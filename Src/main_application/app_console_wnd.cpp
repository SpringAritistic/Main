#include "stdafx.h"

#include <vector>

#include "x_framework/macro.h"
#include "x_framework/interface/console_interface.h"
#include "x_framework/interface/window_interface.h"
#include "x_framework/interface/cmd_container_interface.h"
#include "x_framework/tools/string_tools.h"
#include "x_framework/tools/math_tools.h"
#include "x_framework/tools/common_tools.h"


#include "app_console_wnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

#define MSG_COMMAND_EXECUTE_COMMAND		WM_USER + 1202
#define ID_COMMAND_EDIT_WND				201
#define ID_COMMAND_LIST_WND				202
#define ID_MENU_ITEM_COPY_ITEMS			300
#define ID_MENU_ITEM_CLEAR_ALL			301
#define ID_MENU_ITEM_CLEAR_REPEAT		302
#define ID_MENU_ITEM_SELECT_ALL			303
#define ID_MENU_ITEM_AUTO_SCROLL		304




BEGIN_MESSAGE_MAP(app_edit_for_console_wnd, CXTPEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

app_edit_for_console_wnd::app_edit_for_console_wnd()
{
	m_console_wnd = nullptr;

	m_text_color = RGB(0, 0, 0);
	m_back_color = RGB(255, 255, 255);
	m_border_color = RGB(0, 0, 0);
	m_back_brush.CreateSolidBrush(m_back_color);
}

app_edit_for_console_wnd::~app_edit_for_console_wnd()
{
}

void app_edit_for_console_wnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_console_wnd) return;

	if (VK_ESCAPE == nChar)
	{
		if (m_console_wnd) m_console_wnd->notify_console_action(0);
	}
	else if (VK_RETURN == nChar)
	{
		if (m_console_wnd) m_console_wnd->notify_console_action(1);
	}
	else if (VK_SPACE == nChar)
	{
		if (m_console_wnd) m_console_wnd->notify_console_action(2);
	}
	else
	{
		CXTPEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

HBRUSH app_edit_for_console_wnd::CtlColor(CDC* pDC, UINT nCtlColor)
{
	CDC* dc = GetDC(); // 必须要新的dc,不得使用传入的pDC，传入的参数不能直接绘制到窗口上，要此函数返回后才由系统绘制到窗口上
	if (dc && dc->GetSafeHdc())
	{
		CRect rect;
		GetWindowRect(&rect); // 不能通过客户区得到边框，因为客户区被改动了以便让文字垂直居中，要通过窗口区域来得到
		rect.MoveToXY(0, 0);
		CBrush border_brush(m_border_color);
		dc->FrameRect(rect, &border_brush); // 绘制矩形边框
	}

	pDC->SetTextColor(m_text_color);
	pDC->SetBkColor(m_back_color);
	return m_back_brush; // 返回画刷,用来绘制整个控件背景
}

void app_edit_for_console_wnd::set_care_to_end()
{
	int nLength = SendMessage(WM_GETTEXTLENGTH);
	SetSel(nLength, nLength);
}

void app_edit_for_console_wnd::set_back_color(COLORREF bk_color)
{
	if (bk_color == m_back_color) return;

	m_back_color = bk_color;
	if (m_back_brush.GetSafeHandle())
	{
		m_back_brush.DeleteObject();
		m_back_brush.CreateSolidBrush(m_back_color);
	}

	Invalidate();
}

void app_edit_for_console_wnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_BACK == nChar || VK_DELETE == nChar)
	{
		CString cur_text;
		GetWindowText(cur_text);

		int start_sel = 0, end_sel = 0;
		GetSel(start_sel, end_sel); // 注意此接口得到的是字符数，而不是字节数

		CString default_prompt = m_console_wnd->get_default_prompt();
		cur_text = cur_text.Left(default_prompt.GetLength());
		if (0 != cur_text.CompareNoCase(default_prompt))
		{
			CXTPEdit::OnChar(nChar, nRepCnt, nFlags);
		}
		else
		{
			wstring wide_prompt = string_tools::CString_to_wstring(default_prompt);
			int default_prompt_lenght_in_tchar = wide_prompt.length();

			// 在前导符处不得删除字符
			if (VK_BACK == nChar && start_sel > default_prompt_lenght_in_tchar // 不能等于
				|| VK_DELETE == nChar && start_sel >= default_prompt_lenght_in_tchar)
				CXTPEdit::OnChar(nChar, nRepCnt, nFlags);
		}

		return;
	}

	// 暂不考虑参数中有空格的情况，命令行不允许输入，空格目前当作动作来处理（比如open命令打开指定文件，这种
	// 情况就让其进入交互命令方式弹出打开对话框得了）
	if (nChar == VK_ESCAPE && nChar == VK_RETURN && nChar == VK_SPACE) return;

	CString wnd_text;
	GetWindowText(wnd_text);

	// 如果光标在前缀中间，则移到末尾，以便输入正常数据
	int start, end, row, column;
	GetSel(start, end); //start或end的值就是插入符的字符索引
	row = LineFromChar(start); //获取插入符的行坐标
	column = start - LineIndex(row); //获取插入符的列坐标(注意列以字符为单位，不是字节)

	// 前导符正常情况下会出现四种情况：
	// 1.系统默认的前导符DB_COMMAND_PROPT
	// 2.用户自定义的前导提示信息
	// 3.DB_COMMAND_PROPT + 用户自定义的前导提示信息（命令开始执行时可能有，具体可参考CAD2010）
	// 4.无前导符

	if (!m_console_wnd) return;
	CString prompt_default = m_console_wnd->get_default_prompt();
	CString prompt_custom = m_console_wnd->m_last_prompt;
	CString prompt_mixed = prompt_default + prompt_custom;


	// 判断光标位置以字符为单位,因此使用unicode可以简化算法逻辑,不用手动处理字符合并的逻辑
	std::wstring unicode_prompt_default = string_tools::CString_to_wstring(prompt_default);
	std::wstring unicode_prompt_custom = string_tools::CString_to_wstring(prompt_custom);
	std::wstring unicode_prompt_mixed = string_tools::CString_to_wstring(prompt_mixed);

	if (wnd_text.Left(prompt_default.GetLength()) == prompt_default)
	{
		if (column < (int) unicode_prompt_default.length()) // 可以在前缀的末尾
		{
			int nLength = SendMessage(WM_GETTEXTLENGTH);
			SetSel(nLength, nLength);
		}
	}
	else if (wnd_text.Left(prompt_custom.GetLength()) == prompt_custom)
	{
		if (column < (int) unicode_prompt_custom.length()) // 可以在前缀的末尾
		{
			int nLength = SendMessage(WM_GETTEXTLENGTH);
			SetSel(nLength, nLength);
		}
	}
	else if (wnd_text.Left(prompt_mixed.GetLength()) == prompt_mixed)
	{
		if (column < (int) unicode_prompt_mixed.length()) // 可以在前缀的末尾
		{
			int nLength = SendMessage(WM_GETTEXTLENGTH);
			SetSel(nLength, nLength);
		}
	}
	else
	{
		// nothing;
	}

	CXTPEdit::OnChar(nChar, nRepCnt, nFlags);
}



BEGIN_MESSAGE_MAP(app_listbox_for_console_wnd, CXTPListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MENU_ITEM_CLEAR_ALL, on_clear_all_items)
	ON_COMMAND(ID_MENU_ITEM_CLEAR_REPEAT, on_clear_repeat_items)
	ON_COMMAND(ID_MENU_ITEM_COPY_ITEMS, on_copy_selected_items_to_clipboard)
	ON_COMMAND(ID_MENU_ITEM_SELECT_ALL, on_select_all_items)
	ON_COMMAND(ID_MENU_ITEM_AUTO_SCROLL, on_auto_scroll_to_end_item)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

app_listbox_for_console_wnd::app_listbox_for_console_wnd()
{
	m_auto_scroll = true;
	m_back_color = RGB(255, 255, 255);
	m_back_brush.CreateSolidBrush(m_back_color);
}

app_listbox_for_console_wnd::~app_listbox_for_console_wnd()
{
}

void app_listbox_for_console_wnd::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);

	CString text = _T("");
	int index = (int) (lpDrawItemStruct->itemID);
	if (index >= 0 && index < GetCount()) GetText(lpDrawItemStruct->itemID, text);

	// 由于条目的绘制是即时显示的，因此必须用又缓冲才能杜绝闪烁
	CRect rect = lpDrawItemStruct->rcItem;
	CDC mem_dc;
	CBitmap mem_map;
	mem_dc.CreateCompatibleDC(dc);
	mem_map.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());
	mem_dc.SelectObject(&mem_map);
	mem_dc.SelectObject(GetFont()->GetSafeHandle());

	CRect mem_rect = rect;
	mem_rect.MoveToXY(0, 0);

	//-----------------------------------------------------------

	// 以下两种取色方式抄自XTP的ListBox绘制代码
	COLORREF list_back_color = m_back_color; // GetBackColor(); 不取xtp内置的背景色
	COLORREF list_text_color = IsWindowEnabled() ? GetXtremeColor(COLOR_WINDOWTEXT) : GetXtremeColor(COLOR_GRAYTEXT);

	bool has_focus = (TRUE == HasFocus());
	bool is_selected = (ODS_SELECTED == (lpDrawItemStruct->itemState & ODS_SELECTED));
	//bool is_highlighted = (lpDrawItemStruct->itemID == m_nHotItem);
	if ((is_selected /*|| is_highlighted*/) && IsWindowEnabled() && has_focus)
	{
		// 以下绘制代码来自XTP的ListBox绘制代码，此方法兼容主题文件及皮肤文件，会随主题变化
		CXTPResourceImages* resource_images = XTPResourceImages();
		CXTPResourceImage* list_image = (resource_images ? resource_images->LoadFile(_T("LISTBOX")) : nullptr);
		if (list_image)
			list_image->DrawImage(&mem_dc, mem_rect, list_image->GetSource(1, 4), CRect(4, 4, 4, 4), COLORREF_NULL);
		else
			mem_dc.FillSolidRect(&mem_rect, RGB(190, 190, 190)); // 主题文件中不一定定义了listbox的背景图片，有可能是颜色填充模式
	}
	else
	{
		mem_dc.FillSolidRect(&mem_rect, list_back_color);
	}

	mem_rect.left += 3; // 文字不要顶到头，否则不好看
	mem_dc.SetBkMode(TRANSPARENT);
	mem_dc.SetTextColor(get_text_color(text));
	mem_dc.DrawText(text, &mem_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_EXPANDTABS);

	//-----------------------------------------------------------

	dc->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &mem_dc, 0, 0, SRCCOPY);

	mem_dc.DeleteDC();
	mem_map.DeleteObject();
}

BOOL app_listbox_for_console_wnd::PreTranslateMessage(MSG* msg)
{
	// 高位全1表示按下，否则没按
	bool is_control_key_be_pressed = (0xff == HIBYTE(GetKeyState(VK_CONTROL)));
	bool is_shift_key_be_pressed = (0xff == HIBYTE(GetKeyState(VK_SHIFT)));
	bool is_alt_key_be_pressed = (0xff == HIBYTE(GetKeyState(VK_MENU)));

	if (is_control_key_be_pressed && !is_shift_key_be_pressed && !is_alt_key_be_pressed)
	{
		int key_value = msg->wParam;
		if ('C' == key_value || 'c' == key_value)
			on_copy_selected_items_to_clipboard();
		else if ('D' == key_value || 'd' == key_value)
			on_clear_all_items();
		else if ('R' == key_value || 'r' == key_value)
			on_clear_repeat_items();
		else if ('A' == key_value || 'a' == key_value)
			on_select_all_items();
		else
			; // nothing
	}

	return CXTPListBox::PreTranslateMessage(msg);
}

COLORREF app_listbox_for_console_wnd::GetBackColor()
{
	return m_back_color;
}

void app_listbox_for_console_wnd::on_clear_repeat_items()
{
	if (!GetSafeHwnd()) return;

	CWaitCursor tmp;

	// 原来的代码效率太低，一个项目计算下来，一万多条记录去重复就要半分钟，因此改为虚函数新实现一下，尽量不动老代码
	vector<CString> all_info;
	for (const auto& item : m_all_text)
	{
		auto it = std::find(all_info.begin(), all_info.end(), item);
		if (it == all_info.end()) all_info.push_back(item);
	}

	m_all_text = all_info;

	// 更新到界面上
	LockWindowUpdate();
	ResetContent();
	for (const auto& x : all_info) AddString(x);
	UnlockWindowUpdate();
	UpdateWindow();
}

void app_listbox_for_console_wnd::on_clear_all_items()
{
	if (!GetSafeHwnd()) return;

	CWaitCursor tmp;
	m_all_text.clear(); // 重载的目的是把缓存的数据也清空，否则内存占用不正确，错误统计也会有误
	ResetContent();

	m_auto_scroll = true;
}

void app_listbox_for_console_wnd::on_copy_selected_items_to_clipboard()
{
	CWaitCursor wait_cur;

	// 先清空，有数据再往上放，确保别的程序放的数据能清空
	if (!OpenClipboard()) return;
	EmptyClipboard();
	CloseClipboard();

	int row_count = GetCount();
	if (row_count <= 0) return;

	// 不要预先求值，因为不知道MFC是否缓存这个值，如果没有缓存，则每次调用都要遍历，时间消耗大，后面取选中内容的
	// 时候手工统计更节省总时间
	int selected_row_count = 0; // GetSelCount();
	
	std::wstring selected_merge_string;
	CString item_text;
	for (int i = 0; i < row_count; i++)
	{
		if (GetSel(i) > 0)
		{
			GetText(i, item_text);
			item_text += _T("\n");
			selected_merge_string += string_tools::CString_to_wstring(item_text);
			++selected_row_count;
		}
	}

	if (selected_row_count <= 0 || item_text.IsEmpty()) return;

	int buffer_size_in_byte = (selected_merge_string.length() + 1) * sizeof(wchar_t);

	if (!OpenClipboard()) return;
	
	EmptyClipboard();

	HGLOBAL global_buffer_handle = nullptr;
	global_buffer_handle = GlobalAlloc(GHND, buffer_size_in_byte);
	char* buffer = (char*) GlobalLock(global_buffer_handle);
	
	// 按字节为单位复制，不能用字符串复制函数，因为遇到结束符会停下来（unicode英文字母另一字节为'\0'）
	memcpy_s(buffer, buffer_size_in_byte, (char*) selected_merge_string.c_str(), buffer_size_in_byte);

	GlobalUnlock(global_buffer_handle);
	SetClipboardData(CF_UNICODETEXT, global_buffer_handle);

	CloseClipboard();
}

void app_listbox_for_console_wnd::on_select_all_items()
{
	CWaitCursor wait_cur;

	SetFocus();

	// 设置选中状态时会自动滚动到相应条目，浪费大量时间在界面刷新上，因此为提高效率特作处理
	LockWindowUpdate();
	int row_count = GetCount();
	for (int i = 0; i < row_count; i++) SetSel(i, TRUE);
	UnlockWindowUpdate();
	UpdateWindow();
}

void app_listbox_for_console_wnd::on_auto_scroll_to_end_item()
{
	m_auto_scroll = !m_auto_scroll;
}

void app_listbox_for_console_wnd::OnLButtonDown(UINT flag, CPoint pt)
{
	m_auto_scroll = false;
	CXTPListBox::OnLButtonDown(flag, pt);
}

void app_listbox_for_console_wnd::OnLButtonDblClk(UINT flag, CPoint pt)
{
	m_auto_scroll = false;
	CXTPListBox::OnLButtonDblClk(flag, pt);
}

HBRUSH app_listbox_for_console_wnd::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkColor(m_back_color);
	return m_back_brush; // 返回画刷,用来绘制整个控件背景
}

void app_listbox_for_console_wnd::add_string(const CString& text)
{
	m_all_text.push_back(text);
	int added_index = AddString(text);
	if (m_auto_scroll) SetTopIndex(added_index);
}

void app_listbox_for_console_wnd::set_back_color(COLORREF bk_color)
{
	if (bk_color == m_back_color) return;

	m_back_color = bk_color;
	if (m_back_brush.GetSafeHandle())
	{
		m_back_brush.DeleteObject();
		m_back_brush.CreateSolidBrush(m_back_color);
	}

	Invalidate();
}

COLORREF app_listbox_for_console_wnd::get_text_color(const CString& text) const
{
	COLORREF crText = RGB(0, 0, 0);
	if (text.Left(CString(_T("总计")).GetLength()) == _T("总计"))
		crText = RGB(0, 0, 0);
	else if (text.Left(CString(_T("严重警告")).GetLength()) == _T("严重警告"))
		crText = RGB(255, 0, 255);
	else if (text.Left(CString(_T("警告")).GetLength()) == _T("警告"))
		crText = RGB(0, 0, 255);
	else if (text.Left(CString(_T("错误")).GetLength()) == _T("错误"))
		crText = RGB(255, 0, 0);
	else
		crText = RGB(0, 0, 0);

	return crText;
}

void app_listbox_for_console_wnd::OnContextMenu(CWnd* wnd, CPoint pt)
{
	CXTPListBox::OnContextMenu(wnd, pt);

	SetFocus();

	CMenu pop_up_menu;
	pop_up_menu.CreatePopupMenu();
	pop_up_menu.AppendMenu(MF_STRING, ID_MENU_ITEM_COPY_ITEMS, _T("复制(&C)\tCtrl+C"));
	pop_up_menu.AppendMenu(MF_STRING, ID_MENU_ITEM_CLEAR_ALL, _T("清空(&D)\tCtrl+D"));
	pop_up_menu.AppendMenu(MF_STRING, ID_MENU_ITEM_CLEAR_REPEAT, _T("去除重复(&R)\tCtrl+R"));
	pop_up_menu.AppendMenu(MF_STRING, ID_MENU_ITEM_SELECT_ALL, _T("全选(&R)\tCtrl+A"));
	pop_up_menu.AppendMenu(MF_SEPARATOR);
	pop_up_menu.AppendMenu(MF_STRING, ID_MENU_ITEM_AUTO_SCROLL, _T("自动滚动"));
	
	//pop_up_menu.EnableMenuItem(ID_MENU_ITEM_COPY_ITEMS, MF_BYCOMMAND | MF_ENABLED);
	//pop_up_menu.EnableMenuItem(ID_MENU_ITEM_CLEAR_ALL, MF_BYCOMMAND | MF_ENABLED);
	//pop_up_menu.EnableMenuItem(ID_MENU_ITEM_CLEAR_REPEAT, MF_BYCOMMAND | MF_ENABLED);
	//pop_up_menu.EnableMenuItem(ID_MENU_ITEM_SELECT_ALL, MF_BYCOMMAND | MF_ENABLED);
	pop_up_menu.CheckMenuItem(ID_MENU_ITEM_AUTO_SCROLL, MF_BYCOMMAND | (m_auto_scroll ? MF_CHECKED : MF_UNCHECKED));
	
	m_auto_scroll = false;
	pop_up_menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	pop_up_menu.DestroyMenu();
}




IMPLEMENT_SERIAL(app_console_wnd, CWnd, 1)
BEGIN_MESSAGE_MAP(app_console_wnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


app_console_wnd::app_console_wnd() : CWnd(), console_interface()
{
}

app_console_wnd::~app_console_wnd()
{
}
 
BOOL app_console_wnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	if (CWnd::OnCreate(lpCreateStruct) == -1) return FALSE;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOINTEGRALHEIGHT | LBS_MULTIPLESEL | LBS_EXTENDEDSEL
		| WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | WS_CLIPCHILDREN/* | WS_CLIPSIBLINGS*/;
	if (!m_output_list_box.Create(dwStyle, CRect(), this, ID_COMMAND_LIST_WND)) return FALSE;
	m_output_list_box.SetFont(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
	
	// 列表的下边框覆盖住文本框，否则两个都显示太丑，因此列表框不要WS_CLIPSIBLINGS属性，应该加在文本框上
	if (!m_edit_box.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS, CRect(), this, ID_COMMAND_EDIT_WND)) return FALSE;
	m_edit_box.SetFont(CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
	m_edit_box.m_console_wnd = this;
	
	//考虑把输出字体换成等宽字体
	//{
	//	LOGFONT logfont;
	//	memset(&logfont, 0, sizeof(LOGFONT));
	//	logfont.lfHeight = 12;
	//	logfont.lfCharSet = GB2312_CHARSET;
	//	logfont.lfPitchAndFamily = (BYTE)FIXED_PITCH;
	//	//楷体_GB2312/新宋体/仿宋_GB2312/隶书/宋体-方正超大字符集
	//	_tcscpy_s(logfont.lfFaceName, LF_FACESIZE, "宋体");
	//	CFont  tempFont;
	//	tempFont.CreateFontIndirect(&logfont);
	//	m_pListCtrl.SetFont(&tempFont);
	//}

	// 向框架注册命令控制窗口
	set_console_interface(this);
	add_prompt(get_default_prompt(), false, false);

	// 窗口创建时手工设置起始状态的主题
	app_ui_interface* app = get_app_ui_interface();
	if (app)
	{
		theme_style_settings app_theme = app->get_style_and_settings();
		app_theme_changed(app_theme);
	}

	return TRUE;
}

void app_console_wnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (!GetSafeHwnd()) return;
	if (!m_edit_box.GetSafeHwnd()) return;
	if (!m_output_list_box.GetSafeHwnd()) return;

	// 往上移一个像素，让文本框的底边框被主窗口的底边框盖住，否则两个都显示太丑，左右两边也一样
	CRect list_wnd_rect(0, 0, cx + 1, cy - 22 + 1);
	m_output_list_box.MoveWindow(list_wnd_rect);

	CRect edit_wnd_rect(-2, list_wnd_rect.bottom - 1, cx + 1, cy + 1);
	m_edit_box.MoveWindow(edit_wnd_rect);
}

void app_console_wnd::OnClose()
{
	// 避免窗口销毁后再被调用导致崩溃
	set_console_interface(nullptr);
	CWnd::OnClose();
}

CString app_console_wnd::get_current_input() const
{
	if (!GetSafeHwnd()) return _T("");
	if (!m_edit_box.GetSafeHwnd()) return _T("");

	CString strText;
	m_edit_box.GetWindowText(strText);

	// 以DB_COMMAND_PROPT开头时把前导符去除
	CString prompt = get_default_prompt();
	if (strText.Left(prompt.GetLength()) == prompt)
		strText = strText.Mid(prompt.GetLength());

	strText.Trim();
	CString custom_propt = m_last_prompt; // 命令首次执行时会在DB_COMMAND_PROPT前导符后追加自定义提示，所以要去两次，命令执行过程中只有用户自己的前导符
	if (strText.Left(custom_propt.GetLength()) == custom_propt)
		strText = strText.Mid(custom_propt.GetLength());

	return strText;
}

void app_console_wnd::clear_current_input()
{
	if (!GetSafeHwnd()) return;
	if (!m_edit_box.GetSafeHwnd()) return;

	CString propt = get_default_prompt();

	CString strText;
	m_edit_box.GetWindowText(strText);

	// 命令首次执行时会在DB_COMMAND_PROPT前导符后追加自定义提示，所以要去两次，命令执行过程中只有用户自己的前导符
	if (strText.Left(m_last_prompt.GetLength()) == m_last_prompt && !m_last_prompt.IsEmpty())
		propt = m_last_prompt;

	m_edit_box.SetWindowText(propt);
	m_last_prompt = propt;
}

void app_console_wnd::clear_all_output()
{
	console_interface::clear_all_output();
	m_output_list_box.on_clear_all_items();
}

void app_console_wnd::add_prompt(const CString& prompt, bool new_line, bool end_line)
{
	if (!GetSafeHwnd()) return;
	if (!m_edit_box.GetSafeHwnd()) return;
	if (!m_output_list_box.GetSafeHwnd()) return;

	// 如果当前已经是默认提示符且没有输入任何参数则不用多刷一次空参数行到列表中
	if (get_current_input().IsEmpty() && prompt.IsEmpty()) return;

	CString text_in_edit; // 编辑框中内容，包括前导符和输入参数
	m_edit_box.GetWindowText(text_in_edit);

	// 命令行中输入内容为空时，把当前文本框中的数据刷到列表中，且回到原来的默认提示符状态下
	if (prompt.IsEmpty())
	{
		// 使用print_info能记录到日志，而不要直接输出到列表中
		print_info(OO_UI_LOG, MT_INFO, text_in_edit);

		m_last_prompt = get_default_prompt();
		m_edit_box.SetWindowText(get_default_prompt());
		m_edit_box.set_care_to_end();

		return;
	}

	// 记住上一次提示文字(不可能为空，空已经处理了)，以便用户在一个命令交互过程用使用自己的提示文字时命令行能把命令提示去除，只传输入的数据给用户
	// 不为空且不是默认提示符表示仍然在执行上一个命令的过程中，继续显示上次的提示符
	CString added_prompt = (prompt.IsEmpty() ? get_default_prompt() : prompt);

	// 先处理编辑框中的数据，并写入要输出的内容
	if (new_line)
	{
		// 保持原样内容刷到列表框，哪怕只有前导符或空白内容也添加
		print_info(OO_UI_LOG, MT_INFO, text_in_edit); // 使用此接口输出能记录到目录
		
		m_last_prompt = added_prompt; // 最后一次的提示符更新为最新的
		m_edit_box.SetWindowText(added_prompt);
		m_edit_box.set_care_to_end();
	}
	else
	{
		// 把当前内容追加到原来编辑框中内容的后面，作为一个更长的提示符重新输入（即原来输入的内容不再能删除）
		// 但如果输入框中只有的提示符(不论是不是默认的)，且没有数据，则不要显示默认的提示符
		if (get_current_input().IsEmpty())
			text_in_edit = added_prompt; // 清除提示符
		else
			text_in_edit += added_prompt; // 累加到原字符串中，后面还要用到

		m_last_prompt = text_in_edit;
		m_edit_box.SetWindowText(text_in_edit);
		m_edit_box.set_care_to_end();
	}

	if (end_line)
	{
		// 保持原样内容刷到列表框，哪怕只有前导符或空白内容也添加
		print_info(OO_UI_LOG, MT_INFO, m_last_prompt); // 使用此接口输出能记录到目录

		// 不要自动添加默认提示符或上一次的用户提示符，你永远不知道用户的命令要怎么交互，是显示默认提示符还是
		// 用户提示符，由用户决定，要显示默认提示符时用户会在他需要的时候手动处理
		m_last_prompt = _T(""); // 最后一次的提示符更新为最新的
		m_edit_box.SetWindowText(_T(""));
		m_edit_box.set_care_to_end();
	}
	
	// 实时更新界面
	m_edit_box.RedrawWindow();
	m_output_list_box.RedrawWindow();
}

void app_console_wnd::add_input(const CString& str)
{
	// 把参数送到编辑框中（不能把参数连同提示符通过AddPrompt送到编辑框中，否则参数会被认为是提示符的一部分
	// add_prompt(m_last_prompt + str, true, false);
	if (m_edit_box.GetSafeHwnd())
	{
		CString text;
		m_edit_box.GetWindowText(text);
		m_edit_box.SetWindowText(text + str);

		// 把光标放置到最末尾
		m_edit_box.set_care_to_end();
	}
}

void app_console_wnd::print_info_implement(output_object out, message_type type, const CString& text)
{
	console_interface::print_info_implement(out, type, text);

	// 界面输出时使用的是老构件，它自己会根据类型添加前缀，往日志中输出时要手动添加
	CString ouput_text = text;

	if (MT_ERROR == type)
		ouput_text = _T("错误：") + ouput_text;
	else if (MT_WARNING == type)
		ouput_text = _T("警告：") + ouput_text;
	else if (MT_SERIOUS_WARNING == type)
		ouput_text = _T("严重警告：") + ouput_text;
	else if (MT_INFO == type)
		; // nothing // 常规信息前不加前缀
	else if (MT_DEBUG == type)
		ouput_text = _T("调试：") + ouput_text;
	else
		; // nothing

	// 优先往日志输出(调试信息总是输出到日志）
	if (OO_UI_LOG == out || OO_LOG == out) save_text_to_log_file(ouput_text);

	if (!app_is_debug_mode() && MT_DEBUG == type) return; // 非调试版本不输出调试信息

	m_output_list_box.add_string(ouput_text);
}

void app_console_wnd::on_char_input(UINT char_value, UINT repeat_count, UINT flags)
{
	if (!GetSafeHwnd()) return;
	if (!m_edit_box.GetSafeHwnd()) return;

	// 始终将输入位置定位在最后
	m_edit_box.set_care_to_end();

	// 不能直接调OnChar()直接处理消息的接口，应改为由消息驱动，mfc会有相应的内部数据记录动作，然后才生成相应的数据
	// 并调用对应的响应函数，如果直接调处理函数会跳过这一环节导致在非OnChar消息中调用OnText时失效
	// 查阅MSDN可知WM_CHAR消息的参数含义:
	// 重复次数放在第二个参数的0-15位，因此用nFlags去掉低16位再与上nRepCnt低16位可以有效处理超范围的非法数据
	unsigned int key_data = (flags & 0xffff0000) | (repeat_count & 0x0000ffff);

	// on_text_input只处理OnChar消息，KeyDown消息编辑器窗口自己消化
	m_edit_box.SendMessage(WM_CHAR, (WPARAM)char_value, (LPARAM)key_data);
}

void app_console_wnd::app_theme_changed(theme_style_settings& settings)
{
	if (m_output_list_box.GetSafeHwnd())
		m_output_list_box.set_back_color(settings.m_window_back_color);

	if (m_edit_box.GetSafeHwnd())
		m_edit_box.set_back_color(settings.m_window_back_color);
}

