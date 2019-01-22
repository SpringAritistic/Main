#pragma once

class app_size_config_helper
{
private:
	app_size_config_helper();
	~app_size_config_helper();

public:
	// 获取或设置上次保存的主窗口位置及尺寸
	static CRect get_last_main_frame_pos_and_size();
	static void save_last_main_frame_pos_and_size(const CRect pos);

	// 获取或设置窗口是否最大化的状态信息
	static bool get_maximize_flag();
	static void save_maxmize_flag(bool is_maximize);

	// 获取或保存停靠面板当前配置xml的版本号
	static void save_docking_panes_config_version(const CString& ver);
	static CString get_docking_panes_config_version();
};

