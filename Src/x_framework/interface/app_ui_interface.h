#pragma once
#ifndef __HMGLOBALTOOL_INTERFACE_APP_UI_INTERFACE_H__
#define __HMGLOBALTOOL_INTERFACE_APP_UI_INTERFACE_H__

#include <map>
#include <vector>
#include <utility>

#include "XTToolkit\XTToolkitPro.h"


class window_interface;


enum class app_theme_type
{
	theme_unknown,
	theme_office_2007_blue,
	theme_office_2007_silver,
	theme_office_2007_black,
	theme_office_2010_blue,
	theme_office_2010_silver,
	theme_office_2010_black,
	theme_office_2013_white,
	theme_office_2013_dark
};


// 皮肤和主题相关参数
struct X_FRAMEWORK_API theme_style_settings
{
	int							m_style_id;							// 当前的界面显示风格菜单项id
	app_theme_type				m_style_type;						// 主题样式
	XTPToolTipStyle				m_tooltip_style;					// 提示风格
	XTPDockingPanePaintTheme	m_docking_pane_theme;				// 停靠面板风格
	XTPTabAppearanceStyle		m_tab_appearance;					// 选项卡以及停靠面板停靠后的标签风格
	XTPPaintTheme				m_paint_theme;						// 主界面绘制相关风格
	CString						m_theme_dll_ini_config_file_name;	// 主题对应的dll中内嵌的ini配置文件名，不限大小写
	XTPButtonStyle				m_ribbon_system_button_style;		// ribbon系统按钮风格
	bool						m_enable_aero;						// 是否启用标题acro效果
	CString						m_skin_file_name;					// 主题文件的名称
	CString						m_skin_file_ini_section_name;		// 当前主题对应的皮肤文件中配置的主题模式节名，不限大小写
	XTPTabColorStyle			m_tab_color;						// 选项卡颜色风格

	//////////////////////////////////////////////////////////////////////////

	COLORREF					m_window_back_color;				// 推荐的窗口背景色
};


// 所有需要通过app_ui_interface由消息驱动的可调用对象必须从此类派生
class X_FRAMEWORK_API functor_by_message
{
public:
	functor_by_message();
	virtual ~functor_by_message();

public:
	virtual void call() = 0;
};


// 主界面对外提供的可调用接口，仅提供界面的操作功能，不涉及任何的业务功能
class X_FRAMEWORK_API app_ui_interface
{
public:
	app_ui_interface();
	virtual ~app_ui_interface();

public:
	// 判断工作区是否已经打开了指定的窗口（窗口类名一样和数据tag均一样则认为匹配，窗口类名不是Win32的注册窗口类，而是CWnd或派生类类名）
	bool is_working_space_view_exist(const CString& class_name, const CString& tag);

	// 切换到指定的工作区窗口，如果不存在，则直接返回
	void select_working_space_view(const CString& class_name, const CString& tag);

	// 得到工作区窗口的选项卡索引值（从0开始），不存在则返回-1
	int get_working_space_view_index(const CString& class_name, const CString& tag);
	int get_working_space_view_index(const window_interface* wnd);

public:
	// 添加一个项目到控件中（会记录原窗口的指针），并返回添加后的tab所在的标签序号
	// 类名和参数都一样的窗口已经存在的情况下是不会添加这个窗口的
	// 注意：
	//   如果窗口为CView的派生类，因CView会在窗口销毁之后OnPostDestroy调用delete this，且无法置空，导致框架中
	//   指针悬空，销毁窗口时崩溃，因此对CView及其派生类要特殊处理，重载PostNcDestroy虚函数并禁止调用CView的版本
	//   即可，也不用再调用delete，工作区间控件会正确析构，这并不会造成内存泄漏
	virtual int add_working_space_view(const CString window_class, const CString node_tag, int index = -1, int image_index = -1);
	
	// 添加一个外部实例化好的窗口到工作区，并返回添加后的tab所在的标签序号，内存由工作区拉管，注意：这个窗口必须是在堆上分配的
	// 外部只要创建好窗口即可，数据的加载等流程性接口和属性设置由框架自动完成
	// （除非特殊需求，否则不推荐使用，尽量使用窗口类的重载版本，如果传入的窗口类名和数据tag名均相同则不添加）
	// 注意：
	//   如果窗口为CView的派生类，因CView会在窗口销毁之后OnPostDestroy调用delete this，且无法置空，导致框架中
	//   指针悬空，销毁窗口时崩溃，因此对CView及其派生类要特殊处理，重载PostNcDestroy虚函数并禁止调用CView的版本
	//   即可，也不用再调用delete，工作区间控件会正确析构，这并不会造成内存泄漏
	virtual int add_working_space_view(window_interface* wnd, const CString node_tag, int index = -1, int image_index = -1);
	
	// 获得工作区中的所有窗口
	virtual void get_all_working_space_windows(std::vector<window_interface*>& wnds);

	// 更新一次所有工作区窗口的选项卡标题
	virtual void update_all_working_space_window_captions();
	
	// 获得工作区中当前激活的窗口信息,first为窗口类，second为管理对象tag,如果没有或获取失败，则first和secon均返回_T("")
	virtual void get_current_working_space_active_window(std::pair<CString, CString>& info);

	// 获得工作区中的当前窗口
	virtual window_interface* get_current_working_space_windows();

	// 获得工作区中的所有窗口,first为窗口类，second为数据节点的tag
	virtual void get_all_working_space_windows_info(std::vector<std::pair<CString, CString>>& info);

	// 获取工作区窗口总数
	virtual int get_working_space_view_count();

	// 获取指定索引的窗口信息0：窗口类名， 1：管理对象tag，2：窗口指针，成功返回true，否则返回false
	virtual bool get_tab_window_and_info(std::tuple<CString, CString, window_interface*>& info, int index) const;

	// 关闭指定工作区窗口
	virtual bool close_working_space_view_by_class(const CString& class_name, bool force_close = false); // 类名相同的全部关闭
	virtual bool close_working_space_view_by_tag(const CString& tag, bool force_close = false); // tag相同的全部关闭(一个数据用不同类型的窗口显示)
	virtual bool close_working_space_view_by_index(int index, bool force_close = false);

	// 保存工作区间的所有界面数据到模型中心
	virtual void save_all_workspace_window_ui_data();

	// 根据工作区窗口的相应接口重新更新对应的选项卡标题
	virtual void update_working_space_tab_caption(int index);

	// 获得所有停靠盘中管理的窗口
	virtual void get_all_docking_pane_window(std::vector<window_interface*>& wnds);

	// 获取所有指定窗口类的停靠盘中管理的窗口，如果类名为空表示获取所有从window_interface派生的对象
	virtual void get_all_docking_pane_window_by_class(std::vector<window_interface*>& wnds, const CString& class_name);

	// 获取指定程序标识的停靠盘中管理的窗口
	virtual window_interface* get_docking_pane_window_by_identify(const CString& app_identify);

	// 获取停靠盘中管理的指定id窗口（id不是停靠盘的id，而是内部管理的子窗口id）
	virtual window_interface* get_docking_pane_window_by_id(int id);

	// 创建或销毁所有停靠面板(如果已经创建过了，就不要再创建了)
	virtual void destroy_all_docking_panes();
	virtual void create_all_docking_panes();

	// 获取停靠盘中管理的窗口id,找不到返回0
	virtual int get_docking_pane_window_id(window_interface* wnd);

	// 显示或隐藏指定id的窗口(id不是停靠盘的id，而是内部管理的子窗口id，类名相同的窗口可能有多个)
	virtual void hide_docking_pane_by_id(int id);
	virtual void show_docking_pane_by_id(int id);

	// 显示或隐藏指定类名的窗口
	virtual void hide_docking_pane_by_class(const CString& class_name);
	virtual void show_docking_pane_by_class(const CString& class_name);

	// 显示或隐藏指定程序标识的窗口
	virtual void hide_docking_pane_by_identify(const CString& identify);
	virtual void show_docking_pane_by_identify(const CString& identify);
	
	// 重新加载指定id的停靠面板中的界面数据
	virtual void reload_docking_pane_by_id(int id, const CString& node_tag);
	virtual void reload_docking_pane_data_by_identify(const CString& identify, const CString& node_tag);
	virtual void reload_docking_pane_data_by_class(const CString& class_name, const CString& node_tag);

	// 保存所有停靠面板窗口中的数据
	virtual void save_all_docking_pane_ui_data();

	// 关闭工作区间的所有窗口（会自动调用保存界面数据）
	virtual bool close_all_workspace_window(bool force_close = false);

	// 获取或设置程序标题，此接口与窗口的标题设置不太一样，此接口实现时会有其它的标题内容处理逻辑，比如带上程序名等
	// 空参数表示默认标题
	virtual void set_title(const CString& title);
	virtual CString get_title() const;

	// 得到ribbon控件的tab区域中的控件对象
	// 注意：
	//   1.不是所有的控件都有命令对应，没有命令的控件表示不具备行为，这类控件没有意义。
	//   2.另一类控件行为类似，使用相同的命令并指定不同的参数完成动作的关联，这类命令需要传入查找的参数并
	//     将compare_param置为true，则框架会在查找控件时连同参数也进行比较（域、命令、参数的比较不区分大小写）
	//   3.有些控件是不允许指定参数的（指定了也无效，如combo控件，参数由框架决定，不由用户配置，具体参见ribbon的
	//     xml配置文件内容），这种控件必须使用不同的命令，不可以使用参数加之区别
	virtual CXTPControl* get_ribbon_control(const CString& domain, const CString& cmd, const CString& param = _T(""), bool compare_param = false);

	// 选中Ribbon标签(界面上看不到的标签不显示)
	virtual void select_ribbon_tab(const CString& tab_name);

	// 显示指定的上下文标签，未指定的上下文标签会被隐藏，如果xml中配置的上下文标签的auto_active为false也会强制显示
	virtual void show_ribbon_context_tabs(const std::vector<CString>& tab_names);

	// 保存文件全路径到历史列表
	virtual void add_recent_file_list(const CString& file_path_name);

	// 获取历史列表中的文件路径
	virtual void get_recent_file_list(std::vector<CString>& file_list);

	// 停靠窗口是否已经被创建完成
	virtual bool docking_panes_be_created();

	// 通过PostMessage由消息队列执行由主界面按序执行指定动作
	// 从非主界面线程中调用此函数时请确保参数在堆上分配的，操作完成后自动删除传入的参数
	// 正常投递到主程序消息队列中返回true，否则返回false
	virtual bool do_function_by_post_message(functor_by_message* fun);

	// 通过SendMessage由消息队列执行由主界面按序执行指定动作
	// 从非主界面线程中调用此函数时请确保参数在堆上分配的，操作完成后自动删除传入的参数
	// 此接口最大的作用是将其它线程操作转到主界面线程上执行，特别用于线程中调用主线程中的接口
	// 主线程正常执行返回true，否则返回false
	virtual bool do_function_by_send_message(functor_by_message* fun);

	// 获取当前的主题和皮肤信息
	virtual theme_style_settings get_style_and_settings() const;

	// 检查当前程序是否为安静模式（不弹出任何消息框，主要用于测试程序的自动化执行，遇到有消息框要弹出的
	// 时候变为写入到日志，并把程序选择的选项也写到日志），默认实现为false
	virtual bool is_silent_model() const;

	// 是否正在切换主题过程中(此接口具备多线程安全)
	virtual bool is_style_changing() const;

	// 返回安静模式下遇到弹出窗口时程序怎样自动取值，key为弹出窗口处的位置标识, value为默认取值
	// 值的内容需要设置和读取的地方格式保持一致，否则数据的解释或反序列化不一样，个人建议数据复杂时用json串
	// 如果没有指定，则程序处理行为不确定（看具体的实现）
	virtual void get_silent_model_config(std::map<CString, CString>& configs);

public:
	// 通过postmessage方式添加、关闭工作区窗口
	void post_add_working_space_view_by_class(const CString window_class, const CString node_tag, int index = -1, int image_index = -1);
	void post_add_working_space_view_by_wnd(window_interface* wnd_ix, const CString node_tag, int index = -1, int image_index = -1);
	void post_close_working_space_view_by_class(const CString& class_name, bool force_close = false);
	void post_close_working_space_view_by_tag(const CString& tag, bool force_close = false); // tag相同的全部关闭(一个数据用不同类型的窗口显示)
	void post_close_working_space_view_by_index(int index, bool force_close = false);
	void post_close_all_workspace_window(bool force_close = false);
};

X_FRAMEWORK_API app_ui_interface* get_app_ui_interface();
X_FRAMEWORK_API void set_app_ui_interface(app_ui_interface* app);
#endif //__HMGLOBALTOOL_INTERFACE_APP_UI_INTERFACE_H__
