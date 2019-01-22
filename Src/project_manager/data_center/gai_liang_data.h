#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/serialize_interface.h"
#include "graph_data_base.h"
//绘图设置	
class PROJECT_MANAGER_API GaiLiang_GJ_DrawSet
{
public:
	GaiLiang_GJ_DrawSet();
	~GaiLiang_GJ_DrawSet();
public:
	void serialize_members(member_rw_interface& mt);
public:
	CString m_FZs;	//自定义附注
	bool m_isMB;//是否套图模板
	double m_PLMScale;//平立面绘图比例
	double m_DMScale;//断面绘图比例
	double m_gridScale;//顶底面钢筋网比例
	double m_sketlScale;//骨架大样比例
	////////
	bool m_is1_2Break;//对称时是否在1/2处折断
	double m_forceBreakScope;//非对称情况下，盖梁长度大于此值时，在1/2处折断
	CString	m_DMAnnoType;//断面箍筋编号的注释类型：引线注释，直接注释
	double	m_FZScale;//附注绘图比例

};
//墩柱
class PROJECT_MANAGER_API GaiLiang_GJ_DZ
{
public:
	GaiLiang_GJ_DZ();
	~GaiLiang_GJ_DZ();
public:
	void serialize_members(member_rw_interface& mt);
public:
	CString DZType;//截面类型:矩形;圆形;圆弧倒角矩形;直线倒角矩形
	double DZArrangeVerDis;//墩柱中心到参考线的Y向距离
	CString DZArrangeHorDis;//各墩柱中心到参考线的X距离
	CString vecDZwidthH;//断面横向长度（横桥向）
	CString vecDZwidthZ;//断面纵向长度（纵桥向）
	CString vecDzR;//墩柱半径
	CString vecDZ_dj;//倒角尺寸
	double DZdjR;//倒角半径
	bool DZdjInJog;//圆倒角时是否内凸

};
//挡块
class PROJECT_MANAGER_API GaiLiang_GJ_DK
{
public:
	GaiLiang_GJ_DK();
	~GaiLiang_GJ_DK();
public:
	void serialize_members(member_rw_interface& mt);
public:
	//布置信息
	CString m_vecHX;
	CString m_vecZX;
	CString m_Direct;//挡块朝向，向左为0，向右为1；

	//尺寸信息
	CString m_vecHeight; //挡块高度，挡块侧面
	CString m_vecSBWide;   //挡块上部长度，挡块侧面
	CString m_vecXBWide;  //挡块下部长度，挡块侧面
	CString m_vecAngleL;  //路线方向，平面，小桩号侧边线侧，梯形的左侧内角，大桩号侧大小桩号分界线侧，梯形的左侧内角
	CString m_vecAngleR;  //路线方向，平面，小桩号侧边线侧，梯形的右侧内角，大桩号侧大小桩号分界线侧，梯形的右侧内角
};
//垫石
class PROJECT_MANAGER_API GaiLiang_GJ_DS
{
public:
	GaiLiang_GJ_DS();
	~GaiLiang_GJ_DS();
public:
	void serialize_members(member_rw_interface& mt);
public:
	bool DS_isRegular;// 是否正置
	CString DS_vecDistY;// 垫石中心Y值
	CString DS_vecDistX;// 垫石中心X值
	CString DS_Wide_hor;// 长度
	CString DS_Wide_ver;// 宽度
	CString DS_HeightMid;// 中心高度
	CString DS_Angle;// 角度

};
class PROJECT_MANAGER_API GaiLiang_GJ_CSHLB
{
public:
	GaiLiang_GJ_CSHLB();
	~GaiLiang_GJ_CSHLB();
public:
	void serialize_members(member_rw_interface& mt);
public:


};

//盖梁基本信息
class PROJECT_MANAGER_API GaiLiang_GJ_Base
{
public:
	GaiLiang_GJ_Base();
	~GaiLiang_GJ_Base();
public:
	void serialize_members(member_rw_interface& mt);
public:
	CString vecSlope_ZX;//顶部纵向坡度
	double slope_Bot_ZX;//底部纵向坡度
	CString vecWide;//盖梁宽度
	double Width;//盖梁长度
	CString vecHeightMid;//变坡点处梁高
	double WideMid_L;//变坡点左侧盖梁长度
	bool bSameElevS_B;//大小桩号侧高程是否相同



};
//盖梁构造详细信息
class PROJECT_MANAGER_API GaiLiang_GJ_Detail
{
public:
	GaiLiang_GJ_Detail();
	~GaiLiang_GJ_Detail();
public:
	void serialize_members(member_rw_interface& mt);
public:
	//整体信息
	CString m_PierType;//盖梁接的墩类型
	bool m_RigidRotation;//刚性旋转
	bool m_bGujie;//固结


	//
	CString vecSlope_ZX;//顶部纵向坡度
	double slope_Bot_ZX;//底部纵向坡度
	//盖梁平面数据
	CString vecWide;//盖梁宽度
	double Width;//盖梁长度
	CString vecAngle_LR;//左、右侧与盖梁中心线夹角
	//盖梁横断面数据
	CString vecUpDJ_S_left;//左上缘倒角尺寸
	CString vecDownDJ_S;//下缘倒角尺寸
	//盖梁立面数据
	CString vecHeightMid;//变坡点处梁高
	double WideMid_L;//变坡点左侧盖梁长度
	CString vecHeight;//盖梁左、右侧梁端高度
	CString BGType;//变高段类型,0为直线段，1位圆弧

	CString vecWide_BG;//左、右侧变高段长度
	double Radius_BG;//圆弧半径
	double Radius_DB;//端部小倒角圆弧半径
	bool isDoubleslope;//是否是双坡
	CString slope_S;//小桩号侧盖梁横坡坡度
	CString slope_B;//大桩号侧盖梁横坡坡度
	CString slope_Bot;//底部横坡坡度
	bool bSameElevS_B;//大小桩号侧高程是否相同
	double WideBQ;//背墙宽度



};


class PROJECT_MANAGER_API GaiLiang_GJ_data : public graph_data_base
{
	DECLARE_SERIAL(GaiLiang_GJ_data)
public:
	GaiLiang_GJ_data();
	~GaiLiang_GJ_data();
private:
	virtual void serialize_members(member_rw_interface& mt);
public:

	GaiLiang_GJ_Base	m_Base;	//盖梁属性
	GaiLiang_GJ_Detail	m_Detail;	//盖梁详细属性
	GaiLiang_GJ_DrawSet	m_DrawSet;	//绘图设置
	GaiLiang_GJ_DZ	m_DZ;	//墩柱
	GaiLiang_GJ_DK	m_DK;	//挡块
	GaiLiang_GJ_DS	m_DS;	//垫石
	GaiLiang_GJ_CSHLB	m_CSHLB;	//垫石
private:
	//CTHDataBase;		
};
