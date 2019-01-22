#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/string_tools.h"
#include "x_framework/tools/tinyxml2.h"
#include "x_framework/tools/xml_tools.h"

#include "cheng_tai_data.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

//////////////// serialize  ////////////////////////////

SDianCeng::SDianCeng() : m_set(false), m_height(100), m_width(100){}
void SDianCeng::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_set);
	mt & x_wrapper_macro(m_height);
	mt & x_wrapper_macro(m_width);
}

CCTXiLiang::CCTXiLiang(){}
void CCTXiLiang::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_xl);
	mt & x_wrapper_macro(m_dc);
	//mt & x_wrapper_macro(m_daoJiao);
}

SZhuangJi::SZhuangJi(){}
void SZhuangJi::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_x);
	mt & x_wrapper_macro(m_y);
}

CChengTaiJL::CChengTaiJL() : m_height(1000), m_buZhi(0), m_jiaodu(M_PI_2), m_dingDaoJiao(false), m_dLenTopDJ(0), m_topLever(0){}
void CChengTaiJL::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_height);
	mt & x_wrapper_macro(m_buZhi);
	mt & x_wrapper_macro(m_jiaodu);
	mt & x_wrapper_macro(m_dingDaoJiao);
	mt & x_wrapper_macro(m_dLenTopDJ);
	mt & x_wrapper_macro(m_topLever);
	mt & x_wrapper_macro(m_dc);
	mt & x_wrapper_macro(m_zj);
	mt & x_wrapper_macro(m_xl);
}

CJuXingCT::CJuXingCT() :m_length(3000), m_width(3000), m_daoJiao(0.0, 0.0){}
void CJuXingCT::serialize_members(member_rw_interface& mt)
{
	CChengTaiJL::serialize_members(mt);

	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_length);
	mt & x_wrapper_macro(m_width);
	//mt & x_wrapper_macro(m_daoJiao);
}

CYuanDuanCT::CYuanDuanCT() :m_xianGao(1000), m_banJing(1000){}
void CYuanDuanCT::serialize_members(member_rw_interface& mt)
{
	CChengTaiJL::serialize_members(mt);

	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_xianGao);
	mt & x_wrapper_macro(m_banJing);
}

CYuanXingCT::CYuanXingCT() :m_banJing(1000){}
void CYuanXingCT::serialize_members(member_rw_interface& mt)
{
	CChengTaiJL::serialize_members(mt);

	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_banJing);
}

cheng_tai_GZ::cheng_tai_GZ() :m_leiXing(1), m_pCT(1,boost::shared_ptr<CJuXingCT>(new CJuXingCT())){}
void cheng_tai_GZ::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_leiXing);
	mt & x_wrapper_macro(m_ctJianJu);
	//mt & x_wrapper_macro(m_pCT);
}

IMPLEMENT_SERIAL(cheng_tai_data, graph_data_base, 1)
cheng_tai_data::cheng_tai_data()
{

}
cheng_tai_data::~cheng_tai_data()
{

}

void cheng_tai_data::serialize_members(member_rw_interface& mt)
{
	graph_data_base::serialize_members(mt);

	int version = 1;
	mt & x_wrapper_macro(version);
	mt & x_wrapper_macro(m_gz);
}
