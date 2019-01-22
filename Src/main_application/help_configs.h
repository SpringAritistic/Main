#pragma once


class help_configs
{
private:
	help_configs();
	~help_configs();
	help_configs(const help_configs&);

public:
	static void initialize_by_xml();

	static CString get_default_page_full_path();
	static CString get_page_full_path_by_window_class_name(const CString& window_class_name);

private:
	static CString get_chm_full_path_name();

private:
	static CString m_default_page;
	static std::map<CString, CString> m_page_configs_by_window_class_names;
};

