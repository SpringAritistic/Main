#include "stdafx.h"
#include "recbaseExtGj_data.h"

rectBaseGJ_Common::rectBaseGJ_Common()
{
}
rectBaseGJ_Common::~rectBaseGJ_Common()
{
	gzr_bKs = 9000;
	gzr_lKs = 5400;
	m_hKs_ADD = 1000;
	gzr_hK = "1000";
	gzr_bcl = "500,500";
	gzr_lcq = "500,500";
	m_bPG001Reinforce = 1;
	m_bPG001GouJin = 1;
	m_JiChuMat = "C40";

}
void rectBaseGJ_Common::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);

		mt.write(_T("gzr_bKs"), gzr_bKs);
		mt.write(_T("gzr_lKs"), gzr_lKs);
		mt.write(_T("m_hKs_ADD"), m_hKs_ADD);
		mt.write(_T("gzr_hK"), gzr_hK);
		mt.write(_T("gzr_bcl"), gzr_bcl);
		mt.write(_T("gzr_lcq"), gzr_lcq);
		mt.write(_T("m_bPG001Reinforce"), m_bPG001Reinforce);
		mt.write(_T("m_bPG001GouJin"), m_bPG001GouJin);
		mt.write(_T("m_JiChuMat"), m_JiChuMat);

	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			mt.read(gzr_bKs);
			mt.read(gzr_lKs);
			mt.read(m_hKs_ADD);
			mt.read(gzr_hK);
			mt.read(gzr_bcl);
			mt.read(gzr_lcq);
			mt.read(m_bPG001Reinforce);
			mt.read(m_bPG001GouJin);
			mt.read(m_JiChuMat);

		}
	}
}




rectBaseGJ_org::rectBaseGJ_org()
{



}
rectBaseGJ_org::~rectBaseGJ_org()
{
	gz_enSZA = "混凝土材料";
	gz_fApart = 0;
	gzr_fPDsn = 1;
	gzr_bKs = 9000;
	gzr_lKs = 5400;
	m_hKs_ADD = 1000;
	//
	gzr_hK = "1000";
	gzr_bcl = "500,500";
	gzr_lcq = "500,500";


	m_bPG001Reinforce = 1;
	m_bPG001GouJin = 1;
	m_JiChuMat = "C40";
	m_lkd = 0;
	m_DY = 0;
	m_fbS = 0;

}
void rectBaseGJ_org::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("gz_enSZA"), gz_enSZA);
		mt.write(_T("gz_fApart"), gz_fApart);
		mt.write(_T("gzr_fPDsn"), gzr_fPDsn);
		mt.write(_T("gzr_bKs"), gzr_bKs);
		mt.write(_T("gzr_lKs"), gzr_lKs);
		mt.write(_T("m_hKs_ADD"), m_hKs_ADD);

		mt.write(_T("gzr_hK"), gzr_hK);
		mt.write(_T("gzr_bcl"), gzr_bcl);
		mt.write(_T("gzr_lcq"), gzr_lcq);

		mt.write(_T("m_bPG001Reinforce"), m_bPG001Reinforce);
		mt.write(_T("m_bPG001GouJin"), m_bPG001GouJin);
		mt.write(_T("m_JiChuMat"), m_JiChuMat);
		mt.write(_T("m_lkd"), m_lkd);
		mt.write(_T("m_DY"), m_DY);
		mt.write(_T("m_fbS"), m_fbS);


	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			mt.read(gz_enSZA);
			mt.read(gz_fApart);
			mt.read(gzr_fPDsn);
			mt.read(gzr_bKs);
			mt.read(gzr_lKs);
			mt.read(m_hKs_ADD);
			mt.read(gzr_hK);
			mt.read(gzr_bcl);
			mt.read(gzr_lcq);

			mt.read(m_bPG001Reinforce);
			mt.read(m_bPG001GouJin);
			mt.read(m_JiChuMat);
			mt.read(m_lkd);
			mt.read(m_DY);
			mt.read(m_fbS);


		}
	}
}


IMPLEMENT_SERIAL(rectBaseGJ_data, graph_data_base, 1)
rectBaseGJ_data::rectBaseGJ_data()
{
}
rectBaseGJ_data::~rectBaseGJ_data()
{
}
void rectBaseGJ_data::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		//
		mt.write(_T("m_org"), m_org);
		//mt.write(_T("m_Common"), m_Common);

		mt.write(_T("m_gen"), m_gen);
		mt.write(_T("m_align"), m_align);

	}
	else
	{
		mt.read(version);
		if (1 == version)
		{

			mt.read(m_org);
			//mt.read(m_Common);

			mt.read(m_gen);
			mt.read(m_align);

		}
	}
	graph_data_base::serialize_members(mt);
}
rectBaseGJ_gen::rectBaseGJ_gen()
{
	gz_enPGTYPE = "四边形扩大基础";
	gz_zBs = "";
	gz_bBs = "";
	gzr_aA = 0;
	gz_fSplit = 0;
	gz_bS = 0;
	gz_fSect = 1;
	gz_zKs = 24;
	gz_zKsr = 24;

}
rectBaseGJ_gen::~rectBaseGJ_gen()
{
}
void rectBaseGJ_gen::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);


		mt.write(_T("gz_enPGTYPE"), gz_enPGTYPE);
		mt.write(_T("gz_zBs"), gz_zBs);
		mt.write(_T("gz_bBs"), gz_bBs);
		mt.write(_T("gzr_aA"), gzr_aA);
		mt.write(_T("gz_fSplit"), gz_fSplit);
		mt.write(_T("gz_bS"), gz_bS);
		mt.write(_T("gz_fSect"), gz_fSect);
		mt.write(_T("gz_zKs"), gz_zKs);
		mt.write(_T("gz_zKsr"), gz_zKsr);


	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			mt.read(gz_enPGTYPE);
			mt.read(gz_zBs);
			mt.read(gz_bBs);
			mt.read(gzr_aA);
			mt.read(gz_fSplit);
			mt.read(gz_bS);
			mt.read(gz_fSect);
			mt.read(gz_zKs);
			mt.read(gz_zKsr);

		}
	}
}
rectBaseGJ_align::rectBaseGJ_align()
{
	m_fKJ = 1;
	m_fGJ = 1;
	m_bKzx = 1000;
	m_lKzx = 6400;
	m_kDg = 1;

}
rectBaseGJ_align::~rectBaseGJ_align()
{
}
void rectBaseGJ_align::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);

		mt.write(_T("m_fKJ"), m_fKJ);
		mt.write(_T("m_fGJ"), m_fGJ);
		mt.write(_T("m_bKzx"), m_bKzx);
		mt.write(_T("m_lKzx"), m_lKzx);
		mt.write(_T("m_kDg"), m_kDg);

	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			mt.read(m_fKJ);
			mt.read(m_fGJ);
			mt.read(m_bKzx);
			mt.read(m_lKzx);
			mt.read(m_kDg);

		}
	}
}
RectBaseGJ_Detail::RectBaseGJ_Detail()
{
}
RectBaseGJ_Detail::~RectBaseGJ_Detail()
{
}
void RectBaseGJ_Detail::serialize_members(member_rw_interface& mt)
{
	int version = 1;
	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
		}
	}
}

