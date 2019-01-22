#pragma once


class format_cell
{
public:
	format_cell();
	~format_cell();

public:
	CString				m_text;				// 单元格中的文本
	bool				m_is_read_only;		// 是否为只读
	CString				m_hint_text;		// 提示文字
	long				m_data_type;		// 数据类型
	short				m_alignment;		// 对齐方式
	CString				m_label_text;		// 下拉列表中的文字(如果以@开头，则为环境变量，禁止读取外部表格的环境变量，否则会干扰内部使用的环境变量)
	int					m_cell_type;		// 单元格类型
	CString				m_sketch_map_name;	// 示意图表达式
};

class format_caption
{
public:
	CString				m_main_title;		// 整个表的标题
	format_cell			m_component;		// 模板中子表的应用部件				
};

class format_design
{
public:
	CString				m_id;				// 标识
	CString				m_item_name;		// 条目名称
	format_cell			m_value;			// 参数值
	bool				m_using_string_value; // 如果单元格为下拉列表时，内容按字面值解析，否则按条目索引解析
};

class format_basic_grid
{
public:
	CString				m_id;				// 标识
	CString				m_steel_name;		// 钢筋名称
	format_cell			m_diameter;			// 直径
	format_cell			m_steel_type;		// 钢种号
	format_cell			m_hook_type;		// 弯钩类型，内容只能为这些值中的若干个："/", "无弯钩", "标准弯钩/90度", "标准弯钩/135度", "标准弯钩/180度", "主筋弯钩/90度", "主筋弯钩/135度", "主筋弯钩/180度", "箍筋弯钩/90度", "箍筋弯钩/135度", "箍筋弯钩/180度"
	format_cell			m_distance;			// 端距
	format_cell		 	m_force_type;		// 受力类型，内容只能为这些值中的若干个："CA", "CB", "CC", "CD"
};

class format_layout_grid
{
public:
	CString				m_id;				// 标识
	CString				m_sub_id;			// 子标识，用于索引 标识所对应的一个条目，一般为一个大写字母
	CString				m_steel_name;		// 钢筋名称
	CString				m_area;				// 区域名称
	format_cell			m_spaces_distance;	// 布置间距
	format_cell			m_spaces_count;		// 期望 间距数 或 根数
};

class format_edge_grid
{
public:
	CString				m_id;				// 索引标识
	CString				m_sub_id;			// 子标识，用于索引 标识所对应的一个条目，一般为一个大写字母
	CString				m_steel_name;		// 钢筋名称
	CString				m_edge_name;		// 边距名称
	format_cell			m_location;			// 位置（界面文档中为：定位点）内容只允许为, “中心”和“边缘”中的若干个，否则转换数据会错误
	format_cell			m_logic_type;		// 关系类型（界面文档中为：定位方式）内容只能为“不小于”，“等于”，“不小于取整”中的若干个
	format_cell			m_edge_distance;	// 边距值
	format_cell			m_round_level;		// 取整级别，内容只能为这些值中的若干个：”/“或者一个整数字符串，用于表示取整精度
	format_cell			m_round_type;		// 取整方式，内容只能为这些值中的若干个："/", "偏大", "偏小", "四舍五入"
	format_cell			m_expect_edge;		// 期望边距（界面文档中为：期望间距）
};

class format_edge_detail
{
public:
	CString				m_id;				// 索引标识
	CString				m_sub_id;			// 子标识，用于索引 标识所对应的一个条目，一般为一个大写字母
	CString				m_steel_name;		// 钢筋名称
	CString				m_item_name;		// 条目名称
	format_cell			m_value;			// 值
	bool				m_using_string_value; // 如果单元格为下拉列表时，内容按字面值解析，否则按条目索引解析
};

class steel_grid_format
{
public:
	steel_grid_format(void);
	~steel_grid_format(void);

public:
	format_caption get_caption_format();
	format_design get_design_format(const CString& id);								// 按标识得到条目信息
	format_basic_grid get_basic_format(const CString& id);							// 按标识得到条目信息
	format_layout_grid get_layout_format(const CString& id, const CString& sub_id); // 按标识得到条目信息
	format_edge_grid get_edge_format(const CString& id, const CString& sub_id);		// 按标识得到条目信息
	format_edge_detail get_detail_format(const CString& id, const CString& sub_id);	// 按标识得到条目信息

public:
	format_caption						m_caption_format;	// 表头
	std::vector<format_design>			m_design_format;	// 设计思想
	std::vector<format_basic_grid>		m_basic_format;		// 基本信息
	std::vector<format_layout_grid>		m_layout_format;	// 布置信息
	std::vector<format_edge_grid>		m_edge_format;		// 边距信息
	std::vector<format_edge_detail>		m_detail_format;	// 大样信息

public:
	std::vector<CString>			m_vecTemplateName; // 此变量仅供内部使用，用于记录模板名以便调试时跟踪表信息
};
