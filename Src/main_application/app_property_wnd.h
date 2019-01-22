#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/property_grid_interface.h"
#include "x_framework/interface/window_interface.h"


class app_property_grid_control : public CXTPPropertyGrid
{
	DECLARE_MESSAGE_MAP()

 public:
	app_property_grid_control();
	virtual ~app_property_grid_control();

protected:
	afx_msg LRESULT on_msg_property_reset(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT on_msg_property_clear(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT on_msg_property_is_empty(WPARAM wparam, LPARAM lparam);
}; 


class app_property_wnd : public CWnd, public window_interface, public property_window_interface
{
	DECLARE_SERIAL(app_property_wnd)
	DECLARE_MESSAGE_MAP()

public:
	app_property_wnd();
	virtual ~app_property_wnd();

public:
	app_property_grid_control& get_grid_ctrl();

protected:
	virtual void app_theme_changed(theme_style_settings& settings);
	virtual void show_window(property_notification_interface* obj, bool set_focus = false);
	virtual void clear_content();
	virtual bool is_content_empty();

private:
	app_property_grid_control m_grid;

protected:
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT on_cell_message(WPARAM wParam, LPARAM lParam);
};