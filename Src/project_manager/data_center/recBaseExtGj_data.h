#pragma once

#include "x_framework/macro.h"
#include "x_framework/interface/data_center_interface.h"
#include "x_framework/interface/serialize_interface.h"
#include "graph_data_base.h"






//原始数据	
class PROJECT_MANAGER_API rectBaseGJ_org
{
public:
	rectBaseGJ_org();
	~rectBaseGJ_org();
public:
	void serialize_members(member_rw_interface& mt);
public:
	CString gz_enSZA;//混凝土材料、圬工材料		
	bool gz_fApart;//分离式基础		
	bool gzr_fPDsn;//是否平行于道路设计线		
	//__________________________________________________________
	double gzr_bKs;//顶层横向宽度bKs		
	double gzr_lKs;//顶层纵向宽度lKs		
	double m_hKs_ADD;//扩大基础顶层高度hKs(mm)	
	// 
	CString gzr_hK;//层高hK
	CString gzr_bcl;//左侧扩大量bcl
	CString gzr_lcq;//前向扩大量lcq

	//
	bool m_bPG001Reinforce;//扩大基础配置钢筋		
	bool m_bPG001GouJin;//扩大基础配置钩筋
	//__________________________________________________________ 
	CString m_JiChuMat;//基础材料类型		
	double m_lkd;//基础顶层横桥向左侧扩大量		
	double m_DY;//基础顶层中心线距肋板底缘中心线		
	bool m_fbS;//基础是否设置沉降缝		


};
class PROJECT_MANAGER_API rectBaseGJ_gen
{
public:
	rectBaseGJ_gen();
	~rectBaseGJ_gen();
public:
	void serialize_members(member_rw_interface& mt);
public:
	CString gz_enPGTYPE;//四边形扩大基础\U型桥台四边形扩大基础
	CString gz_zBs;//分离基础顶标高out
	CString gz_bBs;//分离基础中心距out
	double gzr_aA;//斜交角out
	bool gz_fSplit;//是否有沉降缝out:由上部结构的属性来定
	double gz_bS;//沉降缝宽度out:
	bool gz_fSect;//是否出图为完整构造(否则为内部线不会制)out
	double gz_zKs;//基础顶面标高out:等于对应的上部结构的底标高
	double gz_zKsr;//基础顶面标高右侧out

};
class PROJECT_MANAGER_API rectBaseGJ_align
{
public:
	rectBaseGJ_align();
	~rectBaseGJ_align();
public:
	void serialize_members(member_rw_interface& mt);
public:
	bool m_fKJ;//是否设置扩大基础		
	bool m_fGJ;//扩大基础是否配筋		
	double m_bKzx;//扩大基础的最底层的横向斜长		
	double m_lKzx;//扩大基础的最底层的纵向斜长		
	bool m_kDg;//扩大基础是否配置勾筋		

};
class PROJECT_MANAGER_API rectBaseGJ_Common
{
public:
	rectBaseGJ_Common();
	~rectBaseGJ_Common();
public:
	void serialize_members(member_rw_interface& mt);



public:
	double gzr_bKs;//顶层横向宽度bKs		
	double gzr_lKs;//顶层纵向宽度lKs		
	double m_hKs_ADD;//扩大基础顶层高度hKs(mm)		
	CString gzr_hK;//层高hK		
	CString gzr_bcl;//左侧扩大量bcl		
	CString gzr_lcq;//前向扩大量lcq		
	bool m_bPG001Reinforce;//扩大基础配置钢筋		
	bool m_bPG001GouJin;//扩大基础配置钩筋		
	CString m_JiChuMat;//基础材料类型		

	//__________________________________________________________ 
};
class PROJECT_MANAGER_API RectBaseGJ_Detail
{
public:
	RectBaseGJ_Detail();
	~RectBaseGJ_Detail();
public:
	void serialize_members(member_rw_interface& mt);
public:
};


class PROJECT_MANAGER_API rectBaseGJ_data : public graph_data_base
{
	DECLARE_SERIAL(rectBaseGJ_data)
public:
	rectBaseGJ_data();
	~rectBaseGJ_data();
private:
	virtual void serialize_members(member_rw_interface& mt);
public:
	//基本信息
	//rectBaseGJ_Common	m_Common;
	//详细信息
	rectBaseGJ_org	m_org;
	rectBaseGJ_gen	m_gen;
	rectBaseGJ_align	m_align;



private:
	//CTHDataBase;		
};


