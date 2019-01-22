#pragma once

class CTHDataBase;

// 仅用于根据xml配置数据加载程序全局配置参数
class app_configs
{
private:
	app_configs() = delete;
	~app_configs() = delete;

public:
	//此接口由框架调用
	static void load_settings_from_xml();

	// 是否启用皮肤
	static bool is_skin_enable();

public:
	static bool m_enable_splash; // 是否显示启动画面
	static bool m_enable_skin; // 是否启用皮肤
	static CString m_image_name; //启动画面图片名称
	static CString m_application_name; // 应用程序名称
	static bool m_show_start_page; // 程序启动时是否显示起始页（仅用于启动程序不带文件参数的情况，例如双击直接启动）
	static bool m_only_one_instance; // 是否只允许一个程序实例
};
