#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/console_interface.h"


class app_console_wnd;

class app_edit_for_console_wnd : public CXTPEdit
{
	DECLARE_MESSAGE_MAP()

public:
	app_edit_for_console_wnd();
	virtual ~app_edit_for_console_wnd();

public:
	app_console_wnd* m_console_wnd; // 临时记录对象，不负责管理内存

public:
	// 输入光标移到内容的末尾
	void set_care_to_end();
	void set_back_color(COLORREF bk_color);

private:
	COLORREF	m_text_color;			// 字体颜色
	COLORREF	m_back_color;			// 文本框背景颜色
	COLORREF	m_border_color;			// 文本框边框颜色
	CBrush		m_back_brush;			// 文本框背景画刷

public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};


class app_listbox_for_console_wnd : public CXTPListBox
{
	DECLARE_MESSAGE_MAP()

public:
	app_listbox_for_console_wnd();
	virtual ~app_listbox_for_console_wnd();

public:
	void add_string(const CString& text);
	void set_back_color(COLORREF bk_color);

private:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL PreTranslateMessage(MSG* msg);
	virtual COLORREF GetBackColor();

private:
	COLORREF get_text_color(const CString& text) const;

private:
	std::vector<CString> m_all_text;
	bool m_auto_scroll;
	COLORREF m_back_color;
	CBrush m_back_brush;

private:
	friend class app_console_wnd;

private:
	afx_msg void OnContextMenu(CWnd* wnd, CPoint pt);
	afx_msg void on_clear_all_items();
	afx_msg void on_clear_repeat_items();
	afx_msg void on_copy_selected_items_to_clipboard();
	afx_msg void on_select_all_items();
	afx_msg void on_auto_scroll_to_end_item();
	afx_msg void OnLButtonDown(UINT flag, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT flag, CPoint pt);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};


// 命令行窗口
class app_console_wnd : public CWnd, public console_interface, public window_interface
{
	DECLARE_SERIAL(app_console_wnd)
	DECLARE_MESSAGE_MAP()

public:
	app_console_wnd();
	virtual ~app_console_wnd();

public:
	virtual CString get_current_input() const;
	virtual void clear_current_input();
	virtual void clear_all_output();
	virtual void add_prompt(const CString& prompt, bool new_line = true, bool end_line = false);
	virtual void add_input(const CString& str);

private:
	virtual void print_info_implement(output_object out, message_type type, const CString& text);
	virtual void on_char_input(UINT char_value, UINT repeat_count, UINT flags);
	virtual void app_theme_changed(theme_style_settings& settings);

private:
	CString							m_last_prompt;
	app_listbox_for_console_wnd		m_output_list_box;
	app_edit_for_console_wnd		m_edit_box;

private:
	friend class app_edit_for_console_wnd;

protected:
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};