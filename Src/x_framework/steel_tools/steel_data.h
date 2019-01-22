#pragma once
#include <vector>
#include "../macro.h"
#include "../interface/data_center_interface.h"


// 子表类型枚举
enum sub_steel_type
{
	ST_NONE, 		// 条目不存在
	ST_DESIGN,		// 设计思想
	ST_BASIC,		// 基本信息
	ST_LAYOUT,		// 布置信息
	ST_EDEG,		// 边距信息
	ST_DETAIL		// 大样信息
};


// 单个设计思想条目
class X_FRAMEWORK_API steel_design_item
{
public:
	steel_design_item();
	steel_design_item(const CString& id, const CString& value); // 索引标识, 设计思想值(注释用于VAX提示)
	~steel_design_item();

public:
	// 索引标识, 设计思想值(注释用于VAX提示)
	void set(const CString& id, const CString& value);
	void serialize_members(member_rw_interface& mt);

public:
	CString			m_id;					// 索引标识
	bool			m_using_string_value;	// 给钢筋的设计思想CString形式赋值时是否使用原样赋值，否则赋当前选中的索引（此条目仅当界面为下拉框形式为true）
	CString			m_value;				// 设计思想值(如果界面上为下拉框形式，且m_bValueByStr为true则值为从0开始的条目整数索引，否则为界面填入的原样字符串)
};


// 单个基本信息条目
class X_FRAMEWORK_API steel_basic_item
{
public:
	steel_basic_item();
	// 索引标识, 直径, 钢种号, 弯钩类型, 端距, 受力类型(注释用于VAX提示)
	steel_basic_item(const CString& id,
					const CString& diameter,
					const CString& steel_type, 
					const CString& hook_type, 
					const CString& distance,
					const CString& force_type);
	~steel_basic_item();

public:
	// 索引标识, 直径, 钢种号, 弯钩类型, 端距, 受力类型(注释用于VAX提示)
	void set(	const CString& id,
				const CString& diameter, 
				const CString& steel_type, 
				const CString& hook_type, 
				const CString& distance,
				const CString& force_type);
	void serialize_members(member_rw_interface& mt);

public:
	CString			m_id;			// 索引标识
	CString			m_diameter;		// 直径
	CString			m_steel_type;	// 钢种号
	CString			m_hook_type;	// 弯钩类型，内容只能为这些值中的一个或多个："/", "无弯钩", "标准弯钩/90度", "标准弯钩/135度", "标准弯钩/180度", "主筋弯钩/90度", "主筋弯钩/135度", "主筋弯钩/180度", "箍筋弯钩/90度", "箍筋弯钩/135度", "箍筋弯钩/180度"
	CString			m_distance;		// 端距
	CString			m_force_type;	// 受力类型，内容只能为这些值中的若干个："CA", "CB", "CC", "CD"
};


// 单个布置信息条目
class X_FRAMEWORK_API steel_layout_item
{
public:
	steel_layout_item();
	// 索引标识, 子标识, 布置间距, 期望间距数或根数(注释用于VAX提示)
	steel_layout_item(	const CString& id,
						const CString& sub_id,
						const CString& space_distance,
						const CString& space_count);
	~steel_layout_item();

public:
	// 索引标识, 子标识, 布置间距, 期望间距数或根数(注释用于VAX提示)
	void set(const CString& id,
			const CString& sub_id,
			const CString& space_distance,
			const CString& space_count);
	void serialize_members(member_rw_interface& mt);

public:
	CString			m_id;				// 索引标识
	CString			m_sub_id;			// 子标识，用于索引 标识所对应的一个条目，一般为一个大写字母
	CString			m_space_distance;	// 布置间距
	CString			m_space_count;		// 期望 间距数 或 根数
};


// 单个边距信息条目
class X_FRAMEWORK_API steel_edge_item
{
public:
	steel_edge_item();
	// 索引标识, 子标识, 位置(定位点), 关系类型(定位方式), 边距值, 取整级别, 取整方式, 期望边距(注释用于VAX提示)
	steel_edge_item(const CString& id,
					const CString& sub_id,
					const CString& location,
					const CString& logic_type,
					const CString& edge_distance,
					const CString& round_level,
					const CString& round_type,
					const CString& expecting_edge);
	~steel_edge_item();

public:
	// 索引标识, 子标识, 位置(定位点), 关系类型(定位方式), 边距值, 取整级别, 取整方式, 期望边距(注释用于VAX提示)
	void set(const CString& id,
			const CString& sub_id,
			const CString& location, 
			const CString& logic_type, 
			const CString& edge_distance,
			const CString& round_level, 
			const CString& round_type, 
			const CString& expecting_edge);
	void serialize_members(member_rw_interface& mt);

public:
	CString			m_id;				// 索引标识
	CString			m_sub_id;			// 子标识，用于索引 标识所对应的一个条目，一般为一个大写字母
	CString			m_location;			// 位置（界面文档中为：定位点）内容只允许为“中心”和“边缘”，否则转换数据会错误
	CString			m_logic_type;		// 关系类型（界面文档中为：定位方式）内容只能为“不小于”，“等于”，“不小于取整”
	CString			m_edge_distance;	// 边距值
	CString			m_round_level;		// 取整级别，内容只能为这些值中的若干个：”/“或者一个整数字符串，用于表示取整精度
	CString			m_round_type;		// 取整方式，内容只能为这些值中的若干个："/", "偏大", "偏小", "四舍五入"
	CString			m_edge_expect;		// 期望边距（界面文档中为：期望间距）
};


// 单个大样信息条目
class X_FRAMEWORK_API steel_detail_item
{
public:
	steel_detail_item();
	// 索引标识, 子标识, 值(注释用于VAX提示)
	steel_detail_item(const CString& id, const CString& sub_id, const CString& value);
	~steel_detail_item();

public:
	// 索引标识, 子标识, 值(注释用于VAX提示)
	void set(const CString& id, const CString& sub_id, const CString& value);
	void serialize_members(member_rw_interface& mt);

public:
	CString			m_id;					// 索引标识
	CString			m_sub_id;				// 子标识，用于索引 标识所对应的一个条目，一般为一个大写字母
	bool			m_using_string_value;	// 给钢筋的大样CString形式赋值时是否使用原样赋值，否则赋当前选中的索引（此条目仅当界面为下拉框形式为true）
	CString			m_value;				// 大样值(如果界面上为下拉框形式，且m_bValueByStr为true则值为从0开始的条目整数索引，否则为界面填入的原样字符串)
};


//////////////////////////////////////////////////////////////////////////


// 一个通用钢筋表（含设计思想、基本信息、布置信息、边距信息、大样信息五个子表）
class X_FRAMEWORK_API single_steel_data
{
public:
	single_steel_data();
	~single_steel_data();

public:
	void clear_all();	// 清空当前单个通用钢筋表的所有数据
	//************************************
	// 向钢筋表中添加一条信息，信息类型在枚举中列出
	// 需要传入标识和数值，没有子标识则传空，数据值最多6个，每一个数据代表的意义与界面表格的列标签一致
	void add_steel_item(sub_steel_type type,
					   const CString& id,
					   const CString& sub_id = _T(""),
					   const CString& value_01 = _T(""),
					   const CString& value_02 = _T(""),
					   const CString& value_03 = _T(""),
					   const CString& value_04 = _T(""),
					   const CString& value_05 = _T(""),
					   const CString& value_06 = _T(""));
	
	// 根据标识来获取相应的信息，正常情况下不会有重复，只可能有0条或者1条，如果用户给了错误的数据，
	// 导致出标识相同的数据有多行，则只会返回最前面的一条，如果找到则返回true否则返回false
	CString get_component() const; // 按标识得到条目信息
	bool get_design_item(IN const CString& id, OUT steel_design_item& design) const; // 按标识得到条目信息
	bool get_basic_item(IN const CString& id, OUT steel_basic_item& basic_item) const; // 按标识得到条目信息
	bool get_layout_item(IN const CString& id, IN const CString& sub_id, OUT steel_layout_item& layout) const; // 按标识得到条目信息
	bool get_edge_item(IN const CString& id, IN const CString& sub_id, OUT steel_edge_item& edge) const; // 按标识得到条目信息
	bool get_detail_item(IN const CString& id, IN const CString& sub_id, OUT steel_detail_item& detail) const; // 按标识得到条目信息

	// 设置条目值，如果指定的条目中，标识在表格中不存在则函数不进行任何操作，否则把值设置到对应的条目中，
	// 注意设置函数只对本钢筋子表的P0数据有效，属于同一个钢筋DataBase中的不同钢筋子表，可以调用对应的成
	// 员，或者调用所有钢筋表的总的Set函数
	void set_component(IN const CString& component);
	void set_design_item(IN const steel_design_item& design);
	void set_basic_item(IN const steel_basic_item& basic_item);
	void set_layout_item(IN const steel_layout_item& layout);
	void set_edge_item(IN const steel_edge_item& edge);
	void set_detail_item(IN const steel_detail_item& detail);

	void serialize_members(member_rw_interface& mt);

public: // 为了支持一些特殊的操作，这些数据定义为公有的，上面的方法只是为了操作的便利才提供的。
	CString								m_component;		// 所使用的部件名,即应用部件
	std::vector<steel_design_item>		m_design_items;		// 设计思想表
	std::vector<steel_basic_item>		m_basic_items;		// 基本信息表
	std::vector<steel_layout_item>		m_layout_items;		// 布置信息表
	std::vector<steel_edge_item>		m_edge_items;		// 边距信息表
	std::vector<steel_detail_item>		m_detail_items;		// 大样信息表
};

//////////////////////////////////////////////////////////////////////////

// 通用钢筋表格（包含一系列的单个通用钢筋表）
class X_FRAMEWORK_API steel_data : public data_node_base
{
public:
	steel_data();
	~steel_data();

public:
	virtual void serialize_members(member_rw_interface& mt);

public:
	// 设置一个条目值，如果指定的条目中，标识在表格中不存在则函数不进行任何操作，否则
	// 把值设置到对应的条目中。注意设置函数对所有的钢筋子表都生效，如果只改某个钢筋
	// 子表中的数据，请调用相应子表的set函数。
	void set_all_component(IN const CString& component);

	void set_all_design_item(IN const steel_design_item& design);
	void set_all_design_item(const CString& id, const CString& value);

	void set_all_basic_item(IN const steel_basic_item& basic_item);
	void set_all_basic_item(const CString& id, const CString& diameter, const CString& steel_type, const CString& hook_type, const CString& distance, const CString& force_type);
	void set_all_basic_item_diameter(const CString& id, const CString& diameter);
	void set_all_basic_item_steel_type(const CString& id, const CString& steel_type);
	void set_all_basic_item_hook_type(const CString& id, const CString& hook_type);
	void set_all_basic_item_distance(const CString& id, const CString& distance);
	void set_all_basic_item_force_type(const CString& id, const CString& force_type);
	
	void set_all_layout_item(IN const steel_layout_item& layout);
	void set_all_layout_item(const CString& id, const CString& sub_id, const CString& space_distance, const CString& space_count);
	void set_all_layout_item_distance(const CString& id, const CString& sub_id, const CString& space_distance);
	void set_all_layout_item_count(const CString& id, const CString& sub_id, const CString& space_count);

	void set_all_edge_item(IN const steel_edge_item& edge);
	void set_all_edge_item(const CString& id, const CString& sub_id, const CString& location, const CString& logic_type, const CString& edge_distance, const CString& round_level, const CString& round_type, const CString& expecting_edge);
	void set_all_edge_item_location(const CString& id, const CString& sub_id, const CString& location);
	void set_all_edge_item_logic_type(const CString& id, const CString& sub_id, const CString& logic_type);
	void set_all_edge_item_distance(const CString& id, const CString& sub_id, const CString& edge_distance);
	void set_all_edge_item_round_level(const CString& id, const CString& sub_id, const CString& round_level);
	void set_all_edge_item_round_type(const CString& id, const CString& sub_id, const CString& round_type);
	void set_all_edge_item_edge_expect(const CString& id, const CString& sub_id, const CString& expecting_edge);
	
	void set_all_detail_item(IN const steel_detail_item& detail);
	void set_all_detail_item(const CString& id, const CString& sub_id, const CString& value);	

	// 往数据中添加一个条目，内部调用的是概念筋的相应添加函数，如果数据已经存在对应的
	// 条目数据，再添加，则函数的行为与概念筋的数据添加一样，本模块未对其行为进行过测试
	// 请保证不要对已经存在对应条目的数据使用添加函数。
	// 构件名不可添加，只能更改
	void add_all_design_item(IN const steel_design_item& design);
	void add_all_base_item(IN const steel_basic_item& basic_item);
	void add_all_layout_item(IN const steel_layout_item& layout);
	void add_all_edge_item(IN const steel_edge_item& edge);
	void add_all_detail_item(IN const steel_detail_item& detail);

	single_steel_data& get_steel_data(size_t index);
	const single_steel_data& get_steel_data(size_t index) const;
	bool has_steel_data() const;
	size_t get_steel_count() const;
	void add_single_steel(const single_steel_data& steel);
	void clear_steel_data();
	void delete_single_steel(size_t index);
	void resize_steel_count(size_t index); // 行为与vector的resize保持一致

public: // 为了支持一些特殊的操作，这些数据定义为公有的，上面的方法只是为了操作的便利才提供的。
	// 通用钢筋表(每一个元素是一个完整的钢筋表，它包含了基本信息、设计思想等五个小的子表)
	std::vector<single_steel_data> m_all_steel_grids;
};
