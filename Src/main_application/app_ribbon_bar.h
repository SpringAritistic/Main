#pragma once

class CXTPRibbonTab;


typedef struct tag_ribbon_control_id_and_icon_id
{
	int			id;			// 命令ID
	int			icon_id;	// -1表示无效
} ribbon_control_id_and_icon_id;


class option_info
{
public:
	option_info();
	~option_info();

public:
	// 从xml数据中加载ribbon配置参数，当前的node根节点为option_info
	void initlize_config_data(tinyxml2::XMLElement* node);

public:
	CString  m_about_dialog_class_name;
};


class ribbon_bar_data_quick_access_button
{
public:
	ribbon_bar_data_quick_access_button();
	~ribbon_bar_data_quick_access_button();

public:
	// 从xml数据中加载ribbon配置参数，当前的node根节点为quick_access_button
	void initlize_config_data(tinyxml2::XMLElement* node);

	// 根据命令 id 获取 Ribbon 控件中所要执行的命令信息
	// 第一个参数为域，第二个参数为命令，第三个参数为命令的参数
	// 如果命令不存在则返回false，否则返回true
	virtual bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;

public:
	int					m_id;			// ID
	bool				m_is_hide;		// 是否隐藏
	CString				m_caption;		// 标题
	CString				m_tool_tip;		// 提示信息	
	int					m_icon_id;		// 图标ID，每一个选项卡下的所有图标ID均不相同（如果相同则图标一样）
	CString				m_acc_key;		// 键盘选择键
	CString				m_cmd;			// 该控件所要执行的命令，为空表示不执行任何操作
	CString				m_cmd_params;   // 命令参数
};


class ribbon_quick_access_bar
{
public:
	ribbon_quick_access_bar();
	~ribbon_quick_access_bar();

public:
	// 从xml数据中加载ribbon配置参数，当前的node根节点为quick_access
	void initlize_config_data(tinyxml2::XMLElement* node);

	// 根据命令 id 获取 Ribbon 控件中所要执行的命令信息
	// 第一个参数为域，第二个参数为命令，第三个参数为命令的参数
	// 如果命令不存在则返回false，否则返回true
	virtual bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;

private:
	//检测成员m_quick_access_bar中是否有这个id
	bool has_this_id(UINT id)const;

public:
	std::vector<ribbon_bar_data_quick_access_button> m_all_buttons;
	CString m_file_name;
	bool	m_image_has_alpha;
};


class ribbon_bar_data_file_menu_item
{
public:
	ribbon_bar_data_file_menu_item();
	~ribbon_bar_data_file_menu_item();

public:
	// 从xml数据中加载ribbon配置参数，当前的node根节点为file_menu_item
	void initlize_config_data(tinyxml2::XMLElement* node) ;

	// 根据命令 id 获取 Ribbon 控件中所要执行的命令信息
	// 第一个参数为域，第二个参数为命令，第三个参数为命令的参数
	// 如果命令不存在则返回false，否则返回true
	virtual bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;

public:
	int					m_id;			// ID
	CString				m_caption;		// 标题
	int					m_icon_id;		// 图标ID，每一个选项卡下的所有图标ID均不相同（如果相同则图标一样）
	CString				m_acc_key;		// 键盘选择键
	CString				m_cmd;			// 该控件所要执行的命令，为空表示不执行任何操作
	CString				m_cmd_params;	// 命令参数
};


class ribbon_file_menu_bar
{
public:
	ribbon_file_menu_bar();
	~ribbon_file_menu_bar();

public:
	// 从xml数据中加载ribbon配置参数，当前的node根节点为file_menu
	void initlize_config_data(tinyxml2::XMLElement* node) ;

	// 根据命令 id 获取 Ribbon 控件中所要执行的命令信息
	// 第一个参数为域，第二个参数为命令，第三个参数为命令的参数
	// 如果命令不存在则返回false，否则返回true
	virtual bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;

public:
	std::vector<ribbon_bar_data_file_menu_item> m_menu_items;
	CString										m_image_file_name;
	bool										m_image_has_alpha;
};


//////////////////////////////////////////////////////////////////////////
// 一个控件数据类的基类,不同控件的数据类必须从这个类派生
// 不能直接使用这个类，内部初始化的数据可能并不构成一个表现正常的控件
class ribbon_bar_data_controlbase : public CObject
{
	DECLARE_SERIAL(ribbon_bar_data_controlbase)

public:
	ribbon_bar_data_controlbase();
	ribbon_bar_data_controlbase(const ribbon_bar_data_controlbase& control);
	virtual ~ribbon_bar_data_controlbase();

public:
	ribbon_bar_data_controlbase* operator=(const ribbon_bar_data_controlbase& control);

public:
	// 根据自身数据生成一个界面控件,内存由外部调用者管理
	virtual void init_ribbon_control(CXTPControl* control);
	
	// 控件数据的基类，返回类型为xtpControlError，派生类返回相应的正确类型
	virtual XTPControlType get_ribbon_control_type() const;
	
	// 根节点为某个control.根据传入的控件所对应的xml节点配置初始化自身数据
	virtual void init_ribbon_control_data(tinyxml2::XMLElement* node);
	
	// 从堆上使用new分配派生类对象并拷贝数据后返回派生类指针，生成的对象所占内存由外部调用的地方
	// 处理，派生类自己生成自己的对象并使用拷贝函数复制数据并返回基类指针，最好不要调用基础的接口
	virtual ribbon_bar_data_controlbase* clone_control_data() const;
	
	// 返回本控件所包含的控件ID和图标ID（常规的为1个，有些特殊的，如带弹出菜单的，则还要包含这个菜单命
	// 令的个数）,派生类仅计算自己新加的，然后再计算基类中的，最终返回整个控件用到的ID
	virtual std::vector<ribbon_control_id_and_icon_id> get_control_and_icon_id() const;

	// 从start_id开始设置控件ID，返回内部最后一个设置的ID值再加1，派生类只要处理自己新加的ID值，然后
	// 再调用基类的就可以了
	virtual int set_control_id(int start_id);

	// 根据命令 id 获取 Ribbon 控件中所要执行的命令信息
	// 第一个参数为域，第二个参数为命令，第三个参数为命令的参数
	// 如果命令不存在则返回false，否则返回true
	virtual bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;

	// 获取当前控件包含其子控件（如果有的话）中的控件id以及相应的命令
	virtual void get_all_control_id_and_cmd(std::vector<std::pair<int, CString>>& info) const;
	
public:
	// 控件ID范围[20000,28999]，可以包含边界，整个Ribbon中不能
	// 重复，范围由宏 IDS_RIBBON_UI_MIN 和 IDS_RIBBON_UI_MAX 限定，但要去掉已经占用的29000到29999,
	// 所以只能在20000到28999值之间，而且最好与界面同一窗口上的菜单栏、工具栏中的ID不相同
	// 内部ID由程序统一处理，不从xml文件读取
	int					m_id;

	CString				m_caption;
	XTPButtonStyle		m_style;
	int					m_icon_id;		// 图标ID，每一个选项卡下的所有图标ID均不相同（如果相同则图标一样）
	CString				m_tool_tip;
	CString				m_acc_key;		// 键盘选择键
	CString				m_command;		// 该控件所要执行的命令，为空表示不执行任何操作
	CString				m_cmd_params;	// 命令参数
};


// 一个分组
class ribbon_bar_data_group
{
public:
	ribbon_bar_data_group();
	ribbon_bar_data_group(const ribbon_bar_data_group& group);
	~ribbon_bar_data_group();

public:
	ribbon_bar_data_group* operator=(const ribbon_bar_data_group& group);
	void InitRibbonGroupData(tinyxml2::XMLElement* node); // 根节点为某个group
	int GetMaxIconIndex() const; // 返回当前tab中最大的图标ID值(合法图标ID大于等于0),返回-1表示无法获取
	std::vector<ribbon_control_id_and_icon_id> get_control_and_icon_id() const;

public:
	CString				m_group_caption; // 分组标题（在分组下边）
	std::vector<ribbon_bar_data_controlbase*> m_control_in_group; // 数据中只允许存放new分配出的空间，且内存分配之后由类内部管理
};


// 一个选项卡
class ribbon_bar_data_tab
{
public:
	ribbon_bar_data_tab();
	~ribbon_bar_data_tab();

public:
	void InitRibbonTabData(tinyxml2::XMLElement* node); // 根节点为某个tab
	int GetMaxIconIndex() const; // 返回当前tab中最大的图标ID值(合法图标ID大于等于0),返回-1表示无法获取
	std::vector<ribbon_control_id_and_icon_id> get_control_and_icon_id() const;

public:
	CString		m_tab_caption;
	CString		m_tab_acc_key;
	
	// 每个选项卡下的所有控件对应一个图标文件，文件格式为png，图标尺寸32*32,且只能有一排，图片必须
	// 放在与当前dll模块同一目录的文件夹“image”中
	CString		m_icon_file_name;

	// 图像是否有alpah通道信息，32bit位图和png一般赋true（png可以没有alpha通道）如果此值设置不正确，会导致图标有白边或无法识别图像
	bool		m_image_has_alpha;

	// tab中不指定context或者指定为空字符串，表示标签为固定的，否则表示标签是上下文相关的，且上下文
	// 相同的标签会归为一个标签组，以便动态的显示或隐藏
	CString		m_context_name;
	
	// tab中当上下文字符串不为空时，context_color表示上下文的标签颜色
	XTPRibbonTabContextColor m_context_color;
	
	// tab中active_view_class用于指定活动标签所关联的窗口类名，只要工作区当前活动窗口的类名与之匹配就会自动激活，否则自动隐藏
	CString		m_active_view_class;

	// 当为上下文标签且关联窗口时，此值为true会自动激活tab，为false则不激活，此时可由用户自己激活，默认为false
	bool		m_context_auto_active;
	
	std::vector<ribbon_bar_data_group> m_all_groups;
};


// 一个Ribbon界面抽象布局数据
class ribbon_bar_config_data
{
public:
	ribbon_bar_config_data();
	~ribbon_bar_config_data();

public:
	// 获取当前控件包含其子控件（如果有的话）中的控件id以及相应的命令
	virtual void get_all_control_id_and_cmd(std::vector<std::pair<int, CString>>& info) const;

public:
	// 从xml数据中加载ribbon配置参数，当前的node根节点为db_ribbon
	void initlize_config_data(tinyxml2::XMLElement* node);
	
	// 根据命令 id 获取 Ribbon 控件中所要执行的命令信息
	// 第一个参数为域，第二个参数为命令，第三个参数为命令的参数
	// 如果命令不存在则返回false，否则返回true
	bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;

	// 根据命令参数获取对应的命令id, 找不到则返回-1
	// 注意：
	//   1.不是所有的控件都有命令对应，没有命令的控件表示不具备行为，这类控件没有意义。
	//   2.另一类控件行为类似，使用相同的命令并指定不同的参数完成动作的关联，这类命令需要传入查找的参数并
	//     将compare_param置为true，则框架会在查找控件时连同参数也进行比较（域、命令、参数的比较不区分大小写）
	//   3.有些控件是不允许指定参数的（指定了也无效，如combo控件，参数由框架决定，不由用户配置，具体参见ribbon的
	//     xml配置文件内容），这种控件必须使用不同的命令，不可以使用参数加之区别
	int get_id(const CString& domain, const CString& cmd, const CString& param = _T(""), bool compare_param = false);

	// 根据指定的窗口类名来返回所有关联的上下文标签中的所有命令
	void get_all_command_items_of_special_window_class_in_context_tab(std::vector<CString>& cmd_list, const CString& window_class);

public:
	ribbon_quick_access_bar          m_quick_access;
	ribbon_file_menu_bar			 m_file_menu;
	std::vector<ribbon_bar_data_tab> m_all_tabs;
	option_info						 m_dialog_info;
};


//////////////////////////////////////////////////////////////////////////
// 以下为当前用到的所有Ribbon界面控件，以后有新的类型再派生即可

// 一个点击按钮
class ribbon_bar_data_button : public ribbon_bar_data_controlbase
{
	DECLARE_SERIAL(ribbon_bar_data_button)

public:
	ribbon_bar_data_button();
	virtual ~ribbon_bar_data_button();

public:
	virtual void init_ribbon_control(CXTPControl* control);
	virtual XTPControlType get_ribbon_control_type() const;
	virtual void init_ribbon_control_data(tinyxml2::XMLElement* node);
	virtual ribbon_bar_data_controlbase* clone_control_data() const;
	virtual std::vector<ribbon_control_id_and_icon_id> get_control_and_icon_id() const;
	virtual int set_control_id(int start_id);
};

// 一个复选框
class ribbon_bar_data_check_box : public ribbon_bar_data_controlbase
{
	DECLARE_SERIAL(ribbon_bar_data_check_box)

public:
	ribbon_bar_data_check_box();
	virtual ~ribbon_bar_data_check_box();

public:
	virtual void init_ribbon_control(CXTPControl* control);
	virtual XTPControlType get_ribbon_control_type() const;
	virtual void init_ribbon_control_data(tinyxml2::XMLElement* node);
	virtual ribbon_bar_data_controlbase* clone_control_data() const;
	virtual std::vector<ribbon_control_id_and_icon_id> get_control_and_icon_id() const;
	virtual int set_control_id(int start_id);

public:
	bool	m_is_checked;
};

// 一个下拉组合框
class ribbon_bar_data_combo : public ribbon_bar_data_controlbase
{
	DECLARE_SERIAL(ribbon_bar_data_combo)

public:
	ribbon_bar_data_combo();
	virtual ~ribbon_bar_data_combo();

public:
	virtual void init_ribbon_control(CXTPControl* control);
	virtual XTPControlType get_ribbon_control_type() const;
	virtual void init_ribbon_control_data(tinyxml2::XMLElement* node);
	virtual ribbon_bar_data_controlbase* clone_control_data() const;
	virtual std::vector<ribbon_control_id_and_icon_id> get_control_and_icon_id() const;
	virtual int set_control_id(int start_id);
	virtual bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;

public:
	int						m_width; // 控件宽度

	typedef struct
	{
		CString		text;
		bool		is_selected;
	} list_item;
	std::vector<list_item>	m_all_item_list; // 选中哪个条目从组合框类中的接口可以得到
};

// 一个带弹出选项的整体按钮（仅下拉列表可以响应命令，按钮不响应，只是弹出列表）
class ribbon_bar_data_whole_popup_button : public ribbon_bar_data_controlbase
{
	DECLARE_SERIAL(ribbon_bar_data_whole_popup_button)

public:
	ribbon_bar_data_whole_popup_button();
	virtual ~ribbon_bar_data_whole_popup_button();

public:
	virtual void init_ribbon_control(CXTPControl* control);
	virtual XTPControlType get_ribbon_control_type() const;
	virtual void init_ribbon_control_data(tinyxml2::XMLElement* node);
	virtual ribbon_bar_data_controlbase* clone_control_data() const;
	virtual std::vector<ribbon_control_id_and_icon_id> get_control_and_icon_id() const;
	virtual int set_control_id(int start_id);
	virtual bool get_command_and_parameter(int id, std::array<CString, 3>& info) const;
	virtual void get_all_control_id_and_cmd(std::vector<std::pair<int, CString>>& info) const;

public:
	typedef struct
	{
		int			id;				// 每一个弹出条目都对应一个命令ID
		CString		text;			// 弹出的子菜单显示文字，弹出菜单不允许有子菜单，否则可能内部计算错误
		int			icon_id;
		CString		cmd;			// 以菜单的方式弹出，因此支持各条目指定命令
		CString		cmd_params;		// 命令参数
	}popup_list_item;
	std::vector<popup_list_item> m_all_item_list;
};

// 一个带弹出选项的拆分按钮（按钮和下拉列表均可以响应命令）
class ribbon_bar_data_split_popup_button : public ribbon_bar_data_whole_popup_button
{
	DECLARE_SERIAL(ribbon_bar_data_split_popup_button)

public:
	ribbon_bar_data_split_popup_button();
	virtual ~ribbon_bar_data_split_popup_button();

public:
	virtual XTPControlType get_ribbon_control_type() const;
	virtual ribbon_bar_data_controlbase* clone_control_data() const;
};

//////////////////////////////////////////////////////////////////////////

// 界面中使用的RibbonBar派生类
class app_ribbon_bar : public CXTPRibbonBar
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(app_ribbon_bar)

public:
	app_ribbon_bar(void);
	~app_ribbon_bar(void);

public:
	// 初始化控件外观，调用之前请先创建好相应的工具条窗口实例，如果创建Ribbon没有使用本函数，而
	// 是通过其它方式完成，那么可能导致问题，比如无法正常使用上下文标签激活等
	bool initlize_ribbon_bar(const ribbon_bar_config_data& uiData);
	void initlize_ribbon_bar_file_menu(const ribbon_bar_config_data &ribbon_ui_config);

	// 激活一个上下文标签，函数除了激活指定的标签外，还将关闭所有没有在激活列表all_active_view_class中的上下文标签
	// 如果指定的上下文标签设置自动激活为false则[不激活]
	// 如果all_active_view_class为空表示隐藏所有的上下文标签关联的窗口类名
	// is_selected如果为true表示当有上下文选项卡激活时自动切换到相应的选项卡，否则仅激活但不切换
	void active_context_tabs_by_window_class(const std::vector<CString>& all_active_view_class = std::vector<CString>(0), bool is_selected = true);

	// 激活一个上下文标签，函数除了激活指定的标签外，还将关闭所有没有在激活列表all_active_tabs中的上下文标签
	// 如果指定的上下文标签设置自动激活为false也[强制激活]
	// 如果all_active_tabs为空表示隐藏所有的上下文标签
	void active_context_tabs_always_by_tab_name(const std::vector<CString>& all_active_tabs);

	// 选中指定名称的标签（理论上是不允许标签同名的，同名的就放到一组了，如果真的错写了xml生成同名的，则选中第一个）
	void select_tab(const CString& tab_name);

	// 根据id返回控件对象
	CXTPControl* get_control(int id) const;

	// 检查最近文件列表中的文件路径，函数会修剪字符串两端的空白字符后再检查路径是否有效，无效路径将从列表中删除
	void check_recent_file_list();

protected:
	virtual void OnTabChanged(CXTPRibbonTab* tab);

private:
	int						m_recent_file_list_count;		// 最近文档记录多少条
	int						m_file_path_max_length;			// 最近文档路径字符串最大长度
	std::vector<CString>	m_recent_list;					// 缓存最近文档

	// 仅用于存储上下方标签，因定标签是不会存储的
	// CXTPRibbonTab指针仅暂存，不管理内存
	// 第一个值为创建出来的tab对象，第二个值为关联的激活窗口类，第三个值为context_auto_active在xml中的值
	std::vector<std::tuple<CXTPRibbonTab*, CString, bool>> m_context_tab_info_cache;
};

