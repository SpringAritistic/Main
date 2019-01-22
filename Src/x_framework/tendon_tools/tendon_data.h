#pragma once
//#include <utility>
#include "../interface/serialize_interface.h"
#include "../interface/data_center_interface.h"


class CViewSetting;
class CGTEditorAssistDataInfo;

enum refline_type
{
	TENDON_Acr_PW,            //横向特征线（平弯）
	TENDON_Acr_SW,            //横向特征线（竖弯）
	TENDON_Por,               //纵向特征线
	TENDON_Ver,               //高度特征线
	TENDON_UnKnow             //未选择
};

//  段结构(平竖弯类型的几何特征)
class X_FRAMEWORK_API tendon_data_section
{
public:
	tendon_data_section();
	~tendon_data_section();

public:
	void serialize_members(member_rw_interface& mt);

public:
	//段结构输入部分
	CString		    m_x_curvename;               //横向特征线
	double		    m_x;                         //X坐标(mm)
	CString		    m_y_curvename;               //高度特征线
	double		    m_y;                         //Y坐标(mm)
	double		    m_r;                         //过渡半径m
	CString		    m_type;                      //段模式，"LL"表示导线段，"LA"表示直线倾角，"R"，表示参考线段，"RR"表示参考线段中的圆弧过渡点
	double		    m_angle;                     //倾角，仅当m_type为"LA"时有效，单位都是弧度
	CString		    m_line_dir;                  //m_s,m_e直线方向:"F"表示切向，"X"表示水平向
	double		    m_s;
	double		    m_e;
	int			    m_semodifier;                //S,E的取整修饰符
	int			    m_rmodifier;                 //R的取整修饰符
	CString		    m_zu;		                 //族定义
};

// 竖弯/平弯类型
class X_FRAMEWORK_API tendon_data_bend_type
{
public:
	tendon_data_bend_type();
	~tendon_data_bend_type();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString                             m_name;	               //索引名称
	CString                             m_porlines;            //纵向特征线，(仅竖弯类型有效)
	std::vector<tendon_data_section>    m_sections;            //段结构
	CString                             m_joint_list;          //施工缝列表

	//编辑器保存的数据
	CString	                            m_strObjectID;
};

//  同豪任意曲线
class X_FRAMEWORK_API tendon_data_userline
{
public:
	tendon_data_userline();
	~tendon_data_userline();

public:
	void serialize_members(member_rw_interface& mt);

	// 字符串与vector<tendon_data_userline>的相互转换函数
	static void string_to_vecUserline(const CString& text, std::vector<tendon_data_userline>& result);
	static void vecUserline_to_string(CString& text, const std::vector<tendon_data_userline>& vecUserline);


public:
	double       m_x;          // 坐标x(m)
	double       m_y;          // 坐标y(m)
	double       m_r;          // 半径R(m)
};

// 基本类型P0数据
class X_FRAMEWORK_API tendon_data_basetype
{
public:
	tendon_data_basetype();
	~tendon_data_basetype();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString       	m_name;                            //索引名称
	CString       	m_materail_type;                   //材料类型
	int		      	m_numbers;                         //股数
	CString       	m_start_anchor;                    //起始锚具
	CString       	m_end_anchor;                      //终止锚具
	CString       	m_corrugated_pipe;                 //波纹管
	double	      	m_tension_stress;                  //张拉控制应力MPa
	double	      	m_tension_concrete_strength;       //张拉时混凝土强度比
	double	      	m_tension;                         //超张拉系数(%)
	CString	      	m_linker;                          //连接器名字
};


// 钢束类型P0数据
class X_FRAMEWORK_API tendon_data_type
{
public:
	tendon_data_type();
	~tendon_data_type();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString	        m_name;		                	//钢束类型名称
	CString         m_base_type;			    	//基本类型
	CString	        m_vertical_bend_type;	    	//竖弯类型
	CString	        m_horizon_bend_type;	    	//平弯类型
	CString	        m_tendon_property;		        //钢束性质
	CString	        m_drawing_name;               	//图纸中名称
	CString         m_sub_type;				    	//子类型
};

// 钢束锚面P0数据
class X_FRAMEWORK_API tendon_data_anchor_plane
{
public:
	tendon_data_anchor_plane();
	~tendon_data_anchor_plane();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString				m_anchor_name;		        //锚面名称
	CString				m_anchor_tendon;         	//锚固钢束
	CString             m_gear_anchor_name;		    //齿块锚面名称
	CString				m_reference_surface;		//参考面
};

// 张拉批次表P0数据
class X_FRAMEWORK_API tendon_data_tension_batch
{
public:
	tendon_data_tension_batch();
	~tendon_data_tension_batch();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString		    	m_batch_name;			        //张拉批次名称
	CString		    	m_tension_tendon;				//张拉钢束
};

// 自定义参考线P0数据
class X_FRAMEWORK_API tendon_data_refline
{
public:
	tendon_data_refline();
	~tendon_data_refline();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString                            m_name;             //名称
	refline_type	                   m_type;             //参考线类型
	CString                            m_refline;          //参考线名称
	double                             m_dist;             //偏移值
	std::vector<tendon_data_userline>  m_userline;         //为空表示使用参考线偏移
	CString                            m_note;		       //备注

	//编辑器保存的数据
	CString                            m_guid;
};


// 钢束布置P0数据
class X_FRAMEWORK_API tendon_data_layout
{
public:
	tendon_data_layout();
	~tendon_data_layout();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString				m_tendon_name;		     	//钢束名称
	CString				m_refline_name;				//参考线名称
	CString				m_portrait_layout;	     	//纵向布置		
	CString             m_cross_layout;             //横向布置

	//编辑器保存的数据
	CString		        m_guid;
};

// 钢束P0数据
class X_FRAMEWORK_API tendon_data_info
{
public:
	tendon_data_info();
	~tendon_data_info();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString								m_application_pos;		//应用部位
	std::vector<tendon_data_layout>	    m_all_layout;		    //钢束布置
};




// 界面获取的所有钢束相关数据
class X_FRAMEWORK_API tendon_data : public data_node_base
{
	DECLARE_SERIAL(tendon_data)
public:
	tendon_data();
	~tendon_data();

public:
	void serialize_members(member_rw_interface& mt);

public:
	//tendon_data(const tendon_data& Other);
	//tendon_data& operator=(const tendon_data& Other);

	// 清空所有vector数据
	void clearall();

	//根据应用部位名称生成新的钢束
	tendon_data get_tendon_data_by_name(const CString& name)const;

public:
	CString                                    m_component_name;          //构件名称

	std::vector<tendon_data_basetype>          m_all_basetype;	          //基本类型
	std::vector<tendon_data_type>	           m_all_tendon_type;	      //钢束类型
	std::vector<tendon_data_anchor_plane>      m_all_anchor_plane;        //钢束锚面
	std::vector<tendon_data_tension_batch>     m_all_tension_batch;       //张拉批次
	std::vector<tendon_data_refline>           m_all_refline;             //自定义参考线
	std::vector<tendon_data_bend_type>         m_all_ver_bend_type;	      //竖弯类型
	std::vector<tendon_data_bend_type>         m_all_hor_bend_type;	      //平弯类型
	std::vector<tendon_data_info>              m_all_tendon_info;	      //钢束实例

	////编辑器保存的数据 2013.03.26 根据张亚辉需求添加
	//CGTEditorAssistDataInfo*                   m_pSetting;

};
