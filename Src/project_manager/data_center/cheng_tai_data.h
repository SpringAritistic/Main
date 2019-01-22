#ifndef cheng_tai_data_h__
#define cheng_tai_data_h__

#pragma once

#include "boost/shared_ptr.hpp"
#include "x_framework/macro.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/serialize_interface.h"

#include "graph_data_base.h"


typedef struct PROJECT_MANAGER_API SDianCeng
{
	SDianCeng();
	operator bool() const{ return m_set; }
	void serialize_members(member_rw_interface& mt);

	bool m_set;			//系梁底设置垫层
	double m_height;	//垫层厚度
	double m_width;		//垫层外扩宽度
} SXiLiang;

class PROJECT_MANAGER_API CCTXiLiang
{
public:
	CCTXiLiang();
	void serialize_members(member_rw_interface& mt);
	operator bool() const{ return m_xl; }

	SXiLiang m_xl;		//设置系梁
	SDianCeng m_dc;		//设置系梁垫层
	std::pair<double, double> m_daoJiao;//横桥向、纵桥向
};

struct PROJECT_MANAGER_API SZhuangJi
{
	SZhuangJi();
	void serialize_members(member_rw_interface& mt);

	std::vector<std::vector<double>> m_x;
	std::vector<double> m_y;
};
class PROJECT_MANAGER_API CChengTaiJL//承台基类
{
public:
	CChengTaiJL();
	virtual void serialize_members(member_rw_interface& mt);

	double m_height;	//承台厚度
	int m_buZhi;		//布置形式---0:基础正做 2:基础斜做
	double m_jiaodu;	//倾斜角度-单位°

	bool m_dingDaoJiao;	//设置顶倒角
	double m_dLenTopDJ;	//顶倒角尺寸

	double m_topLever;	//顶高程
	SDianCeng m_dc;		//承台垫层
	SZhuangJi m_zj;		//桩基
	CCTXiLiang m_xl;
};

class PROJECT_MANAGER_API CJuXingCT : public CChengTaiJL
{
public:
	CJuXingCT();
	virtual void serialize_members(member_rw_interface& mt);
	double m_length;	//横桥向宽
	double m_width;		//纵桥向宽
	std::pair<double, double> m_daoJiao;//横桥向、纵桥向
};

class PROJECT_MANAGER_API CYuanDuanCT : public CChengTaiJL
{
public:
	CYuanDuanCT();
	virtual void serialize_members(member_rw_interface& mt);
	double m_xianGao;		//弦高
	double m_banJing;		//半径
};

class PROJECT_MANAGER_API CYuanXingCT : public CChengTaiJL
{
public:
	CYuanXingCT();
	virtual void serialize_members(member_rw_interface& mt);
	double m_banJing;	//半径
};

class PROJECT_MANAGER_API cheng_tai_GZ
{
public:
	cheng_tai_GZ();
	void serialize_members(member_rw_interface& mt);
	int m_leiXing;		//承台端部类型---1:矩形  2:圆端形 3:切角形 4:圆弧倒角形 5：工字形
	std::vector<double> m_ctJianJu;//承台间距，即系梁长度
	std::vector<boost::shared_ptr<CChengTaiJL>> m_pCT; //多个承台,工字形承台时类型为矩形
};

// 
class PROJECT_MANAGER_API cheng_tai_data : public graph_data_base
{
	DECLARE_SERIAL(cheng_tai_data)
public:
	cheng_tai_data();
	~cheng_tai_data();

	virtual void serialize_members(member_rw_interface& mt);
public:
	cheng_tai_GZ m_gz;
};

#endif // cheng_tai_data_h__
