#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/serialize_interface.h"

#include "graph_data_base.h"

//绘图设置
class PROJECT_MANAGER_API eqgj_drawset	// 绘图设置
{
public:
	eqgj_drawset();
	~eqgj_drawset();
public:
	void serialize_members(member_rw_interface& mt);
	// 
	bool fbq;	// 是否设置背墙
	bool fnt;	// 是否设置牛腿
	bool fck;	// 是否设置槽口
	bool fTM;	// 是否设置台帽
	bool fQQ;	// 是否设置前墙
	bool fCQ;	// 是否设置侧墙
	bool fDTB;	// 是否设置挡土板
	//
	double dScale;	// 绘图比例
};

//基本参数
class PROJECT_MANAGER_API eqgj_nt
{
public:
	eqgj_nt();
	~eqgj_nt();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_hToBQ;	// 到背墙顶距离
	double m_bT;	// 顶宽
	double m_hVer;	// 竖直段高度
	double m_hX;	// 倾斜段高度
};

class PROJECT_MANAGER_API eqgj_ck	// 背墙槽口
{
public:
	eqgj_ck();
	~eqgj_ck();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_hToBQ;	// 到背墙顶距离
	double m_bT;	// 槽口宽度
};

class PROJECT_MANAGER_API eqgj_ssf	// 背墙伸缩缝预留槽
{
public:
	eqgj_ssf();
	~eqgj_ssf();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_h;		// 高度
	double m_w;		// 宽度
};

class PROJECT_MANAGER_API eqgj_bq	// 背墙
{
public:
	eqgj_bq();
	~eqgj_bq();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double m_bT;	// 背墙顶宽
	double m_bB;	// 背墙底宽
	double m_H;		// 背墙控制截面高度
	//double m_VerH;	// 背墙竖直段高度
	double m_iSlope;// 纵桥向顶坡度

	eqgj_nt nt;		// 牛腿
	eqgj_ck ck;		// 槽口
	eqgj_ssf ssf;	// 伸缩缝预留槽
};

class PROJECT_MANAGER_API eqgj_tm	// 台帽
{
public:
	eqgj_tm();
	~eqgj_tm();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double	m_dH;	// 高度
	double	m_dW;	// 宽度,包含背墙底宽
	double	m_dDJ;	// 倒角尺寸,竖直尺寸相同
};

class PROJECT_MANAGER_API eqgj_qq	// 前墙
{
public:
	eqgj_qq();
	~eqgj_qq();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double  m_dPosZ;// 纵向距离背墙前侧距离
	double	m_dH;	// 高度lunkuo
	double	m_dW;	// 宽度,包含背墙底宽
	double	m_dDJ;	// 倒角尺寸,竖直尺寸相同
};

class PROJECT_MANAGER_API eqgj_cq	// 侧墙
{
public:
	eqgj_cq();
	~eqgj_cq();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double	m_dH;		// 高度
	double	m_dW;		// 宽度
	double	m_dDisToBQ;	// 高出背墙顶缘高度
};

class PROJECT_MANAGER_API eqgj_dtb	// 挡土板
{
public:
	eqgj_dtb();
	~eqgj_dtb();
public:
	void serialize_members(member_rw_interface& mt);

public:
	double	m_dH;	// 高度
	double	m_dL;	// 宽度
	double	m_dW;	// 厚度
};

class PROJECT_MANAGER_API eqgj_eq	// 耳墙
{
public:
	eqgj_eq();
	~eqgj_eq();
public:
	void serialize_members(member_rw_interface& mt);

public:
	bool	m_bLeft;	// 是否为左侧耳墙
	double	m_dTopLen;	// 断面顶宽
	double	m_dBotLen;	// 断面底宽
	double	m_dWide;	// 厚度
	double	m_dDisToBQ;	// 高出背墙顶缘高度
	double	m_dHVer;	// 竖直段高度
	double	m_dhX;		// 倾斜段高度
	double  m_dSlope;	// 顶缘坡度
	bool    m_bSetDJ;	// 是否设置倒角
	double  m_dDJWide;	// 倒角宽度
	double  m_dDJHei;	// 倒角高度
	bool	m_bHorToMLBot; // 是否与帽梁底水平
	double	m_dAnglePM;	// 平面倾斜角度-弧度

};


// 
class PROJECT_MANAGER_API eq_gj_data : public graph_data_base
{
	DECLARE_SERIAL(eq_gj_data)

public:
	eq_gj_data();
	~eq_gj_data();

private:
	virtual void serialize_members(member_rw_interface& mt);
public:
	CString	m_strName;	 // 桥台名称
	eqgj_eq m_EQ;		 // 耳墙
	eqgj_bq m_BQ;		 // 背墙
	eqgj_tm m_TM;		 // 台帽
	eqgj_qq m_QQ;		 // 前墙
	eqgj_cq m_CQ;		 // 侧墙
	eqgj_dtb m_DTB;		 // 挡土板

	eqgj_drawset drawset;// 绘图设置
};

