#pragma once


// 一个面板（一个面板必定在一个容器中）
class single_pane_data : public CObject
{
	DECLARE_SERIAL(single_pane_data)

public:
	single_pane_data();
	single_pane_data(const single_pane_data& src);
	virtual ~single_pane_data();

public:
	single_pane_data& operator=(const single_pane_data& control);

public:
	// 从xml数据中加载配置参数，当前的node根节点为container
	void init_single_pane_data(tinyxml2::XMLElement* node);

public:
	int					m_id; // 面板id，由创建时自动分配
	CString				m_pane_title;
	CString				m_tab_title;
	bool				m_start_load;		// 启动时是否加载
	bool				m_no_close_button;	// 属性用于设置有无关闭按钮
	bool				m_no_caption;		// 属性用于设置有无标题栏-->
	bool				m_no_dockable;		// 属性用于设置能否停靠-->
	bool				m_no_hideable;		// 属性用于设置有无隐藏按钮-->
	bool				m_select;
	CString				m_icon_file;
	CString				m_child_window_class;
	int					m_child_window_id;	// 管理的子窗口id，由创建时自动分配
	CString				m_app_identify;		// 用于应用程序内部标识某种特殊对象，一般为空，目前为property_wnd表示这是属性表面板，为command_line表示是命令行面板
};

// 一个面板容器
class pane_container_data : public CObject
{
	DECLARE_SERIAL(pane_container_data)

public:
	pane_container_data();
	pane_container_data(const pane_container_data& src);
	virtual ~pane_container_data();

public:
	pane_container_data& operator=(const pane_container_data& control);

public:
	// 从xml数据中加载配置参数，当前的node根节点为container
	void init_pane_container_data(tinyxml2::XMLElement* node);

public:
	CString				m_direction_by_frame;
	CString				m_app_identify;
	CString				m_neighbor_identify;
	int					m_width;
	int					m_height;
	bool				m_show;
	std::vector<single_pane_data> m_panes;
};

// 停靠面板配置数据
class docking_pane_data: public CObject
{
	DECLARE_SERIAL(docking_pane_data)

public:
	docking_pane_data();
	docking_pane_data(const docking_pane_data& src);
	virtual ~docking_pane_data();

public:
	docking_pane_data& operator=(const docking_pane_data& control);

public:
	// 从xml数据中加载配置参数，当前的node根节点为db_docking_panes
	void init_docking_pane_data(tinyxml2::XMLElement* node);

	// 获取指定id的面板配置数据（pane已经被创建好且id已经分配好后再调用才有意义）
	single_pane_data* get_pane_data_by_pane_wnd_id(int id);

public:
	CString m_version;
	bool m_force_create_by_frame;
	bool m_auto_layout;
	std::vector<pane_container_data> m_container;
};

