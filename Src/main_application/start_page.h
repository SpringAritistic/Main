#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/window_interface.h"

class CXTPMarkupObject;
class CXTPMarkupRoutedEventArgs;


class start_page : public CWnd, public CXTPMarkupContext, public window_interface
{
	DECLARE_SERIAL(start_page)
	DECLARE_MESSAGE_MAP()

public:
	start_page();
	virtual ~start_page();

private:
	virtual CString get_title();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnWndMsg(UINT message, WPARAM wparam, LPARAM lparam, LRESULT* result);
	virtual void app_theme_changed(theme_style_settings& settings);
	virtual void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);

private:
	CXTPMarkupUIElement* m_markup_page;
	bool m_is_page_in_rendering;

private:
	CString get_markup_xml_file_path_name(app_theme_type& style_type);
	void create_markup_page(const CString& file_path_name);
	void load_dynamic_markup();
	void DrawMarkup(CXTPMarkupDrawingContext* pDC, CRect rc);
	void on_hyperlink_clicked_command(const CString& command, const CString& param);
	void on_hyperlink_clicked_url(const CString& url);
	void on_hyperlink_clicked_file(const CString& file_path);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void on_hyperlink_clicked(CXTPMarkupObject* sender, CXTPMarkupRoutedEventArgs* args);
};
