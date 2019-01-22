#pragma once


class app_skin_manager;
class app_skin_manager_window_filter;


class app_skin_manager_window_filter : public IXTPSkinManagerWindowFilter
{
private:
	app_skin_manager_window_filter();
	virtual ~app_skin_manager_window_filter();

private:
	virtual BOOL FilterWindow(HWND hWnd, LPCTSTR lpszClassName, LPCREATESTRUCT lpcs);

private:
	friend class app_skin_manager;
};


class app_skin_manager : public CXTPSkinManager
{
public:
	app_skin_manager();
	virtual ~app_skin_manager();

public:
	static void load_config_info_from_xml();

private:
	static std::vector<CString> m_filter_out_registed_window_class_names;
	static std::vector<CString> m_excluded_module_names;
	static app_skin_manager_window_filter m_skin_filter;

private:
	friend class app_skin_manager_window_filter;
};

