#pragma once
#include <utility>
#include "../interface/serialize_interface.h"
#include "../interface/data_center_interface.h"

// 骨架钢筋的控制点(P0数据)//对应"控制点表"的一行
class X_FRAMEWORK_API frame_data_node
{
public:
	frame_data_node();
	~frame_data_node();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString			m_ref_line;				// 参考线名称（不包含负号）
	CString			m_dist_to_ref_line;		// 控制点到参考线的距离
	CString			m_begin_layer;			// 弯起层(钢筋端点的层数总为0)
	CString			m_end_layer;			// 弯止层(钢筋端点的层数总为0)
	
	// 控制点起弯方向反置（即参考线包含负号，1为反置，0为不反置),此变量在算法器中与其它的逻辑有冲突，起用之后
	// 会引入很多画图怪异的行为，所以盖梁中是禁用此变量的，之所以放开是因为上部结构中箱梁有时候必须得反置一下，
	// 钢筋图才能出正确，人个觉得，算法器完全可不要这个尴尬的变量，它的功能冗余了。
	CString				m_transposed;

	// 控制点所在钢筋的直径，此值老构件不支持，只有新构件才支持（为0或空按填1处理，为负表示按直径的绝对值进
	// 行钢筋并置，为正表示钢筋直径，数字之前带了“\”则表示骨架信息中所填的直径的序号（从1开始），无法索引时将
	// 默认取第1个）
	CString				m_diameter;
};


// 骨架钢筋的弯钩类型数据P0数据
class X_FRAMEWORK_API frame_data_hook
{
public:
	frame_data_hook();
	~frame_data_hook();

public:
	void serialize_members(member_rw_interface& mt);

public:
	static CString get_general_hook_name(); // 返回通用弯钩的名称

public:
	CString			m_index_name;			// 索引名称
	CString			m_hook_type;			// 弯钩型式,0,直线弯钩;1,圆弧弯钩;2,折线弯钩;
	CString			m_hook_length;			// 弯钩直线段长度
	CString			m_arc_radius;			// 圆弧半径
	CString			m_hook_end_type;		// 端部截断点的弯钩的方向类型[0,弯钩为竖直方向;1,弯钩为垂直方向;2,指定角度dAngle]
	CString			m_hook_end_angle;		// 端部截断点的弯钩与钢筋线的夹角（角度）
	CString			m_hook_direction;		// 弯钩方向（true向内、false向外）
	CString			m_binded_segment;		// 弯钩所贴近的钢筋段名
};


// 一片骨架钢筋面P0数据
class X_FRAMEWORK_API frame_data_piece
{
public:
	frame_data_piece();
	~frame_data_piece();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString							m_index_name;					// 骨架钢筋面的类型索引名
	bool							m_need_weld;					// 是否焊成骨架[TRUE,骨架钢筋面;FALSE,非骨架钢筋面]
	bool							m_is_symmetric;					// 是否对称
	CString							m_steel_type;					// 钢筋的钢种号(缺省为2,即螺纹钢)
	CString							m_diameter;						// 钢筋直径(mm)，可填入多个直径，数字之间用半角空格、逗号或分号相隔，如果直径为负则并置钢筋
	CString							m_hook_weld_length;				// 弯钩的焊缝长度
	CString							m_top_space;					// 用户指定的顶部各层层间距(0-based,值为0表示取钢筋外径)
	CString							m_bottom_space;					// 用户指定的底部各层层间距(0-based,值为0表示取钢筋外径)
	CString							m_top_flag;						// 顶部各层是否顺顶部参考线(0-based)
	CString							m_bottom_flag;					// 底部各层是否顺底部参考线(0-based)
	CString							m_bent_angle;					// 弯起角度(角度)
	CString							m_arc_steel_radius;				// 圆弧筋过渡半径
	CString							m_mid_bent_radius;				// 中折点处的构造圆弧半径
	CString							m_mid_weld_length;				// 中间弯折点的焊缝长度
	bool							m_has_top_perforation_steel;	// 是否有顶层通长筋
	bool							m_has_bottom_perforation_steel;	// 是否有底层通长筋
	CString							m_hook_settings;				// 指定弯钩信息
	std::vector<frame_data_node>	m_top_nodes;					// 上部控制点
	std::vector<frame_data_node>	m_bottom_nodes;					// 下部控制点
	CString							m_laction;						// 应用部位（目前仅用于骨架编辑器，界面不显示）
};


// 界面获取的所有骨架相关数据
class X_FRAMEWORK_API frame_data : public data_node_base
{
public:
	frame_data();
	~frame_data();

public:
	std::vector<frame_data_piece>	m_all_frames;			// 全部的骨架信息
	frame_data_hook					m_general_hook;			// 通用弯钩信息，仅一个且必须有一个，且弯钩的索引名为“通用弯钩”
	std::vector<frame_data_hook>	m_all_hooks;			// 所有的弯钩定义信息

public:
	virtual void serialize_members(member_rw_interface& mt);

public:
	// 返回指定骨架面
	frame_data_piece* get_sepcial_frame(const CString& name);
	
	// 解析骨架数据处理中用到的乘号表达式，格式形如 100, 200,300*5 50 89 , 90, A B, C*3, Str*5, 100.123 *  5
	//【注】表达式于2012-3-26 12:21:08改为个数在后，数据在前，为防止错误，原注释不改，仅在此作注释
	// 一般用于解析界面表格中骨架的类型定义或布置表达式
	// 解析成功返回true，失败返回false
	static bool parse_frame_expression(IN const CString& text, OUT std::vector<CString>& result);
	
	//从一个参考线字符串中取出序号及参考线的类型名称，如"KD1"将得到"KD"和1,参考线没有序号的时候（如KM之类）返回-1
	static std::pair<int, CString> get_ref_line_info(const CString& ref_line_full_name);

	//查找KD和KZ的序号最大值
	static int find_max_kd_kz_index(const CString& strRefString, const std::vector<frame_data_node>& vecNodes);
};
