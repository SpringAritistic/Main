#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/serialize_interface.h"

#include "graph_data_base.h"

//基本参数
class PROJECT_MANAGER_API cap_beam_common
{
public:
	cap_beam_common();
	~cap_beam_common();
public:
public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_RdAndBKX_Angle;//布孔线与路线夹角
 	double m_PosH;			//横向定位
	double m_PosContrlSec;	//控制截面距路线
 	std::vector<double> m_TopSlope;//盖梁顶部横坡
	std::vector<double> m_BotSlope;//盖梁底部横坡
	double m_Length_H;		//盖梁横向长度
	double m_CapSec_Hei;	//盖梁高度
	double m_CapSec_Lever;	//盖梁顶高程
	int m_Fnd_Type;			//基础类型---0:桩柱式  1:承台桩 2:扩大基础
	std::vector<double>	m_JC_TopLever;	//顶高程
	std::vector<double>	m_JC_BotLever;	//底高程
};
//总体设置
class PROJECT_MANAGER_API cap_beam_Gen
{
public:
	cap_beam_Gen();
	~cap_beam_Gen();

public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_RdAndBKX_Angle;//布孔线与路线夹角
	double m_PosZ;			//纵向定位
	double m_PosH;			//横向定位
	double m_PosContrlSec;	//控制截面距路线
	bool   m_bSetDF;		//设置墩顶断缝
	double m_DF_Wide;		//墩顶断缝宽度
	std::vector<double> m_TopSlope;//盖梁顶部横坡
	std::vector<double> m_BotSlope;//盖梁底部横坡
};
//盖梁
class PROJECT_MANAGER_API cap_beam_GaiLiang
{
public:
	cap_beam_GaiLiang();
	~cap_beam_GaiLiang();

public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_Length_H;		//横向长度
	double m_length_Z;		//纵向宽度
	double m_CapSide_AngleL;//盖梁左边线与水平边线夹角
	double m_CapSide_AngleR;//盖梁右边线与水平边线夹角
	double m_WideXL;		//变高段长度L
	double m_WideXR;		//变高段长度R
	double m_CapSide_HeiL;	//左侧梁端高度
	double m_CapSide_HeiR;	//右侧梁端高度
	double m_CapTop_XLen;	//顶部切角宽度
	double m_CapSec_Hei;	//盖梁高度
	double m_CapSec_Lever;	//盖梁顶高程
	double m_PierToGLSide_LenL;//襟边长度L
	double m_PierToGLSide_LenR;//襟边长度R
};
//墩柱
class PROJECT_MANAGER_API cap_beam_Pier
{
public:
	cap_beam_Pier();
	~cap_beam_Pier();

public:
	void serialize_members(member_rw_interface& mt);

public:
	int m_PierSec_Type;//0:圆形 1:矩形 2:线倒角矩形 3:圆弧倒角矩形
	double m_Pier_D;
	bool m_bSetXL;
	double m_XL_Wide;
	double m_XL_Hei;
	double m_Pier_PosZ;
	std::vector<double> m_Pier_PosH;//横向定位
	std::vector<double> m_XL_Pos;	//系梁位置

	double dWide_H;			//矩形横桥向宽
	double dWide_Z;			//矩形纵桥向宽
	double dWide_LnDJ_H;	//线倒角矩形横桥向宽度
	double dWide_LnDJ_Z;	//线倒角矩形纵桥向宽度
	double dWide_ArcDJ_R;	//圆弧倒角矩形圆弧半径
};
//基础
class PROJECT_MANAGER_API cap_beam_JC
{
public:
	cap_beam_JC();
	~cap_beam_JC();

public:
	void serialize_members(member_rw_interface& mt);

public:
	int m_Fnd_Type;			//基础类型---0:桩柱式  1:承台桩 2:扩大基础
	int m_Found_Type;		//桩基类型---0:钻孔灌注桩 2:预制方桩
	double m_JC_PosZ;		//纵向定位
	double m_Found_D;		//直径或边长
	bool m_bFound_JiaCu;	//桩顶加粗
	double m_JiaCu_D;		//加粗段直径
	double m_JiaCu_Hei;		//加粗段高度
	double m_JiaCuGD_Hei;	//过渡段高度
	bool m_bFound_SetXL;	//设置系梁
	double m_FoundXL_Wide;	//系梁纵向宽度
	double m_FoundXL_Hei;	//系梁竖向高度
	bool m_FoundXL_bSetBotDC;//系梁底设置垫层
	double m_FoundXLDC_Hei;	//垫层厚度
	double m_FoundXLDC_Wide;//垫层外扩宽度
	bool m_bSetXL;			//设置系梁
	double m_XL_Wide;		//系梁纵向宽度
	double m_XL_Hei;		//系梁竖向高度

	double m_CTHor_Width;	//承台横向宽
	double m_CTVer_Width;	//承台纵向宽
	double m_CT_Hei;		//承台厚度
	double m_CTFound_DisCT;	//桩顶入承台深度
	int m_Layout_Type;		//布桩方式
	//扩大基础
	double m_KuoDa_TopHei;	//顶层高度
	double m_KuoDa_TopWZ;	//顶层纵向宽度
	double m_KuoDa_TopWH;	//顶层横向宽度

	std::vector<double>	m_FoundXL_Pos;	//系梁位置
	std::vector<double>	m_JC_PosH;		//横向定位
	std::vector<double>	m_JC_TopLever;	//顶高程
	std::vector<double>	m_JC_BotLever;	//底高程
};
//绘图设置
class PROJECT_MANAGER_API cap_beam_DrawSet
{
public:
	cap_beam_DrawSet();
	~cap_beam_DrawSet();

public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_Scale;			//绘制比例
	CString m_FZs;			//自定义附注
	double m_CtrBreakHei;	//墩高折断控制高度
	bool m_DrawDK;			//绘制挡块
	bool m_DrawDS;			//绘制垫石
	bool m_DrawDMX;			//绘制地面线
	bool m_DrawGCSLB;		//绘制工程数量表
	bool m_DrawCTPM;		//单独绘制承台桩基础平面图
	bool m_BZDSType;		//标注垫石类型
	bool m_BZDSLever_LM;	//标注垫石高程
	bool m_BZDSHei_LM;		//标注垫石高度
	bool m_bBZDSPosInPM;	//标注垫石平面布置
	bool m_BZSlopeInside;	//标注盖梁坡度于内侧
	bool m_BZGLLeverAtDKInside;	//标注盖梁高程于挡块内侧
	bool m_bBZGLDJSizeInPM;		//标注盖梁倒角平面尺寸
	bool m_bBZDunPMPos;			//标注墩柱平面布置
	bool m_bBZCMBKXdisDunCen;	//标注侧面布孔线距墩中心线距离
};
//参数表列表
class PROJECT_MANAGER_API cap_beam_CSHLB
{
public:
	cap_beam_CSHLB();
	~cap_beam_CSHLB();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString m_CSHLB_Comment;	//列表范围路径
	CString m_strCSHLBName;		//参数化列表标题
	bool m_bOnlyOneLB;			//单个构件参数化列表
	int m_iCSHLBBGType;			//参数化列表形式---0:墩名为行参数为列 1:参数为行墩名为列 
	int m_iDunNumType;			//墩柱编号原则---0:从左到右 1:从内到外
	bool m_bAllLeverUseOneName;	//列表高程均使用同一变量名
	bool m_bDSLeverLB;			//垫石高程单独列表
	bool m_bDSHeiLB;			//垫石高度单独列表

	//替换的参数名称
	CString m_Angle;		//路线与布孔线夹角
	CString m_GL_Len;		//盖梁长度
	CString m_BS;			//盖梁侧边线距路线距离
	CString m_left_Slope;	//盖梁左侧横坡
	CString m_right_Slope;	//盖梁右侧横坡
	CString m_left_DJ;		//盖梁底左倒角长
	CString m_right_DJ;		//盖梁底右倒角长
	CString m_left_JB;		//盖梁左襟边长
	CString m_right_JB;		//盖梁右襟边长
	CString m_Dun_Dis;		//墩间中心距
	CString m_GL_LeftTopLever;	//盖梁顶左标高
	CString m_GL_rightTopLever;	//盖梁顶右标高
	CString m_Dun_TopLever;	//墩顶标高
	CString m_DunXL_Lever;	//墩间系梁标高
	CString m_JC_TopLever;	//基础顶标高
	CString m_JC_BotLever;	//基础底标高
	CString m_Dun_Hei;		//墩高
	CString m_Found_Len;	//桩长
	CString m_DS_Lever;		//垫石高程
	CString m_DS_Hei;		//垫石高度
	CString m_DisBKXtoRd;	//布孔线与墩中心距离
};
//材料名称
class PROJECT_MANAGER_API cap_beam_Material
{
public:
	cap_beam_Material();
	~cap_beam_Material();

public:
	void serialize_members(member_rw_interface& mt);

public:
	CString m_GL_Mat;		//盖梁材料类型
	CString m_DS_Mat;		//垫石材料类型
	CString m_Dun_Mat;		//墩柱材料类型
	CString m_DunXL_Mat;	//墩柱系梁材料类型
	CString m_CTKD_Mat;		//承台/扩大基础材料类型
	CString m_CTDC_Mat;		//承台垫层材料类型
	CString m_JCXL_Mat;		//基础系梁材料类型
	CString m_Fnd_Mat;		//桩基材料类型

	CString m_GL_Type;		//盖梁结构类型
	CString m_DS_Type;		//垫石结构类型
	CString m_Dun_Type;		//墩柱结构类型
	CString m_DunXL_Type;	//墩柱系梁结构类型
	CString m_CTKD_Type;	//承台/扩大基础结构类型
	CString m_CTDC_Type;	//承台垫层结构类型
	CString m_JCXL_Type;	//基础系梁结构类型
	CString m_Fnd_Type;		//桩基结构类型
};



// 
class PROJECT_MANAGER_API cap_beam_data : public graph_data_base
{
	DECLARE_SERIAL(cap_beam_data)

public:
	cap_beam_data();
	~cap_beam_data();

private:
	virtual void serialize_members(member_rw_interface& mt);
public:
	//common
	cap_beam_common		m_Common;//基本属性
	
	//advance
	cap_beam_Gen		m_Gen;	//总体属性
	cap_beam_GaiLiang	m_GL;	//盖梁属性
	cap_beam_Pier		m_Pier;	//桩基属性
	cap_beam_JC			m_JC;	//基础属性

	cap_beam_DrawSet	m_DrawSet;	//绘图设置

	cap_beam_CSHLB	m_CSHLB;		//参数化列表

	cap_beam_Material	m_Materials;//材料统计

private:
	//CTHDataBase;
};

