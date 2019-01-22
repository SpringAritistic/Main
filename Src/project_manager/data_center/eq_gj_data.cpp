#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/string_tools.h"
#include "x_framework/tools/tinyxml2.h"
#include "x_framework/tools/xml_tools.h"

#include "eq_gj_data.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
///
eqgj_drawset::eqgj_drawset()
{
	fbq = true;
	fnt = true;
	fck = false;
	fTM = true;
	fQQ = false;
	fCQ = false;
	fDTB = false;
	dScale = 50;
}
eqgj_drawset::~eqgj_drawset()
{

}
void eqgj_drawset::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("fbq"), fbq);
		mt.write(_T("fnt"), fnt);
		mt.write(_T("fck"), fck);
		mt.write(_T("fTM"), fTM);
		mt.write(_T("fQQ"), fQQ);
		mt.write(_T("fCQ"), fCQ);
		mt.write(_T("fDTB"), fDTB);
		mt.write(_T("dScale"), dScale);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(fbq);
			mt.read(fnt);
			mt.read(fck);
			mt.read(fTM);
			mt.read(fQQ);
			mt.read(fCQ);
			mt.read(fDTB);
			mt.read(dScale);
		}
	}
}

///
eqgj_nt::eqgj_nt()
{
	m_hToBQ = 0.0;
	m_bT = 0.0;
	m_hVer = 0.0;
	m_hX = 0.0;
}
eqgj_nt::~eqgj_nt()
{

}
void eqgj_nt::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_hToBQ"), m_hToBQ);
		mt.write(_T("m_bT"), m_bT);
		mt.write(_T("m_hVer"), m_hVer);
		mt.write(_T("m_hX"), m_hX);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_hToBQ);
			mt.read(m_bT);
			mt.read(m_hVer);
			mt.read(m_hX);
		}
	}
}

///
eqgj_ck::eqgj_ck()
{
	m_hToBQ = 0.0;
	m_bT = 0.0;
}
eqgj_ck::~eqgj_ck()
{

}
void eqgj_ck::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_hToBQ"), m_hToBQ);
		mt.write(_T("m_bT"), m_bT);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_hToBQ);
			mt.read(m_bT);
		}
	}
}

///
eqgj_ssf::eqgj_ssf()
{
	m_h = 0.0;
	m_w = 0.0;
}
eqgj_ssf::~eqgj_ssf()
{

}
void eqgj_ssf::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_h"), m_h);
		mt.write(_T("m_w"), m_w);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_h);
			mt.read(m_w);
		}
	}
}

///
eqgj_bq::eqgj_bq()
{
	m_bT = 0.0;
	m_bB = 0.0;
	m_H = 0.0;
	//m_VerH = 0.0;
	m_iSlope = 0.0;
}
eqgj_bq::~eqgj_bq()
{

}
void eqgj_bq::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_bT"), m_bT);
		mt.write(_T("m_bB"), m_bB);
		mt.write(_T("m_H"), m_H);
		//mt.write(_T("m_VerH"), m_VerH);
		mt.write(_T("m_iSlope"), m_iSlope);
		mt.write(_T("nt"), nt);
		mt.write(_T("ck"), ck);
		mt.write(_T("ssf"), ssf);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_bT);
			mt.read(m_bB);
			mt.read(m_H);
			//mt.read(m_VerH);
			mt.read(m_iSlope);
			mt.read(nt);
			mt.read(ck);
			mt.read(ssf);
		}
	}
}

///
eqgj_tm::eqgj_tm()
{
	m_dH = 0.0;
	m_dW = 0.0;
	m_dDJ = 0.0;
}
eqgj_tm::~eqgj_tm()
{

}
void eqgj_tm::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_dH"), m_dH);
		mt.write(_T("m_dW"), m_dW);
		mt.write(_T("m_dDJ"), m_dDJ);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_dH);
			mt.read(m_dW);
			mt.read(m_dDJ);
		}
	}
}

///
eqgj_qq::eqgj_qq()
{
	m_dPosZ = 0.0;
	m_dH = 0.0;
	m_dW = 0.0;
	m_dDJ = 0.0;
}
eqgj_qq::~eqgj_qq()
{

}
void eqgj_qq::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_dPosZ"), m_dPosZ);
		mt.write(_T("m_dH"), m_dH);
		mt.write(_T("m_dW"), m_dW);
		mt.write(_T("m_dDJ"), m_dDJ);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_dPosZ);
			mt.read(m_dH);
			mt.read(m_dW);
			mt.read(m_dDJ);
		}
	}
}

///
eqgj_cq::eqgj_cq()
{
	m_dH = 0.0;
	m_dW = 0.0;
	m_dDisToBQ = 0.0;
}
eqgj_cq::~eqgj_cq()
{

}
void eqgj_cq::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_dH"), m_dH);
		mt.write(_T("m_dW"), m_dW);
		mt.write(_T("m_dDisToBQ"), m_dDisToBQ);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_dH);
			mt.read(m_dW);
			mt.read(m_dDisToBQ);
		}
	}
}

///
eqgj_dtb::eqgj_dtb()
{
	m_dH = 0.0;
	m_dL = 0.0;
	m_dW = 0.0;
}
eqgj_dtb::~eqgj_dtb()
{

}
void eqgj_dtb::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_dH"), m_dH);
		mt.write(_T("m_dL"), m_dL);
		mt.write(_T("m_dW"), m_dW);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_dH);
			mt.read(m_dL);
			mt.read(m_dW);
		}
	}
}

///
eqgj_eq::eqgj_eq()
{
	m_bLeft = true;
	m_dTopLen = 0.0;
	m_dBotLen = 0.0;
	m_dWide = 0.0;
	m_dDisToBQ = 0.0;
	m_dHVer = 0.0;
	m_dhX = 0.0;
	m_dSlope = 0.0;
	m_bSetDJ = 0.0;
	m_dDJWide = 0.0;
	m_dDJHei = 0.0;
	m_bHorToMLBot = true;
	m_dAnglePM = 0.0;
}
eqgj_eq::~eqgj_eq()
{

}
void eqgj_eq::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_bLeft"), m_bLeft);
		mt.write(_T("m_dTopLen"), m_dTopLen);
		mt.write(_T("m_dBotLen"), m_dBotLen);
		mt.write(_T("m_dWide"), m_dWide);
		mt.write(_T("m_dDisToBQ"), m_dDisToBQ);
		mt.write(_T("m_dHVer"), m_dHVer);
		mt.write(_T("m_dhX"), m_dhX);
		mt.write(_T("m_dSlope"), m_dSlope);
		mt.write(_T("m_bSetDJ"), m_bSetDJ);
		mt.write(_T("m_dDJWide"), m_dDJWide);
		mt.write(_T("m_dDJHei"), m_dDJHei);
		mt.write(_T("m_bHorToMLBot"), m_bHorToMLBot);
		mt.write(_T("m_dAnglePM"), m_dAnglePM);

	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_bLeft);
			mt.read(m_dTopLen);
			mt.read(m_dBotLen);
			mt.read(m_dWide);
			mt.read(m_dDisToBQ);
			mt.read(m_dHVer);
			mt.read(m_dhX);
			mt.read(m_dSlope);
			mt.read(m_bSetDJ);
			mt.read(m_dDJWide);
			mt.read(m_dDJHei);
			mt.read(m_bHorToMLBot);
			mt.read(m_dAnglePM);
		}
	}
}

///
IMPLEMENT_SERIAL(eq_gj_data, graph_data_base, 1)

eq_gj_data::eq_gj_data()
{

}
eq_gj_data::~eq_gj_data()
{

}
void eq_gj_data::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_strName"), m_strName);
		mt.write(_T("m_EQ"), m_EQ);
		mt.write(_T("m_BQ"), m_BQ);
		mt.write(_T("m_TM"), m_TM);
		mt.write(_T("m_QQ"), m_QQ);
		mt.write(_T("m_CQ"), m_CQ);
		mt.write(_T("m_DTB"), m_DTB);
		mt.write(_T("drawset"), drawset);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_strName);
			mt.read(m_EQ);
			mt.read(m_BQ);
			mt.read(m_TM);
			mt.read(m_QQ);
			mt.read(m_CQ);
			mt.read(m_DTB);
			mt.read(drawset);
		}
	}
	graph_data_base::serialize_members(mt);
}
