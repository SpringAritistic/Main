#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/string_tools.h"
#include "x_framework/tools/tinyxml2.h"
#include "x_framework/tools/xml_tools.h"

#include "cap_beam_data.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

cap_beam_common::cap_beam_common()
{
	m_RdAndBKX_Angle = 0.0;
 	m_PosH = 0.0;
	m_PosContrlSec = 0.0;
	m_Length_H = 0.0;
	m_CapSec_Hei = 0.0;
	m_CapSec_Lever = 0.0;
	m_Fnd_Type = 0;
}

cap_beam_common::~cap_beam_common()
{

}

void cap_beam_common::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		//总体
		mt.write(_T("m_RdAndBKX_Angle"), m_RdAndBKX_Angle);
 		mt.write(_T("m_PosH"), m_PosH);
		mt.write(_T("m_PosContrlSec"), m_PosContrlSec);
		mt.write(_T("m_TopSlope"), m_TopSlope);
		mt.write(_T("m_BotSlope"), m_BotSlope);
		mt.write(_T("m_Length_H"), m_Length_H);
		mt.write(_T("m_CapSec_Hei"), m_CapSec_Hei);
		mt.write(_T("m_CapSec_Lever"), m_CapSec_Lever);
		mt.write(_T("m_Fnd_Type"), m_Fnd_Type);
		mt.write(_T("m_JC_TopLever"), m_JC_TopLever);
		mt.write(_T("m_JC_BotLever"), m_JC_BotLever);
	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			//总体
			mt.read(m_RdAndBKX_Angle);
			mt.read(m_PosH);
			mt.read(m_PosContrlSec);
			mt.read(m_TopSlope);
			mt.read(m_BotSlope);
			mt.read(m_Length_H);
			mt.read(m_CapSec_Hei);
			mt.read(m_CapSec_Lever);
			mt.read(m_Fnd_Type);
			mt.read(m_JC_TopLever);
			mt.read(m_JC_BotLever);
		}
	}
}

///
cap_beam_Gen::cap_beam_Gen()
{
	m_RdAndBKX_Angle = 0.0;
	m_PosZ = 0.0;
	m_PosH = 0.0;
	m_PosContrlSec = 0.0;
	m_bSetDF = false;
	m_DF_Wide = 0.0;
}
cap_beam_Gen::~cap_beam_Gen()
{

}
void cap_beam_Gen::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		//总体
		mt.write(_T("m_RdAndBKX_Angle"), m_RdAndBKX_Angle);
		mt.write(_T("m_PosZ"), m_PosZ);
		mt.write(_T("m_PosH"), m_PosH);
		mt.write(_T("m_PosContrlSec"), m_PosContrlSec);
		mt.write(_T("m_bSetDF"), m_bSetDF);
		mt.write(_T("m_DF_Wide"), m_DF_Wide);
		mt.write(_T("m_TopSlope"), m_TopSlope);
		mt.write(_T("m_BotSlope"), m_BotSlope);
	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			//总体
			mt.read(m_RdAndBKX_Angle);
			mt.read(m_PosZ);
			mt.read(m_PosH);
			mt.read(m_PosContrlSec);
			mt.read(m_bSetDF);
			mt.read(m_DF_Wide);
			mt.read(m_TopSlope);
			mt.read(m_BotSlope);
		}
	}
}

cap_beam_GaiLiang::cap_beam_GaiLiang()
{
	m_Length_H = 0.0;
	m_length_Z = 0.0;
	m_CapSide_AngleL = 90.0;
	m_CapSide_AngleR = 90.0;
	m_WideXL = 0.0;
	m_WideXR = 0.0;
	m_CapSide_HeiL = 0.0;
	m_CapSide_HeiR = 0.0;
	m_CapTop_XLen = 0.0;
	m_CapSec_Hei = 0.0;
	m_CapSec_Lever = 0.0;
	m_PierToGLSide_LenL = 0.0;
	m_PierToGLSide_LenR = 0.0;
}
cap_beam_GaiLiang::~cap_beam_GaiLiang()
{

}
void cap_beam_GaiLiang::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		//盖梁
		mt.write(_T("m_Length_H"), m_Length_H);
		mt.write(_T("m_length_Z"), m_length_Z);
		mt.write(_T("m_CapSide_AngleL"), m_CapSide_AngleL);
		mt.write(_T("m_CapSide_AngleR"), m_CapSide_AngleR);
		mt.write(_T("m_WideXL"), m_WideXL);
		mt.write(_T("m_WideXR"), m_WideXR);
		mt.write(_T("m_CapSide_HeiL"), m_CapSide_HeiL);
		mt.write(_T("m_CapSide_HeiR"), m_CapSide_HeiR);
		mt.write(_T("m_CapTop_XLen"), m_CapTop_XLen);
		mt.write(_T("m_CapSec_Hei"), m_CapSec_Hei);
		mt.write(_T("m_CapSec_Lever"), m_CapSec_Lever);
		mt.write(_T("m_PierToGLSide_LenL"), m_PierToGLSide_LenL);
		mt.write(_T("m_PierToGLSide_LenR"), m_PierToGLSide_LenR);
	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			//盖梁
			mt.read(m_Length_H);
			mt.read(m_length_Z);
			mt.read(m_CapSide_AngleL);
			mt.read(m_CapSide_AngleR);
			mt.read(m_WideXL);
			mt.read(m_WideXR);
			mt.read(m_CapSide_HeiL);
			mt.read(m_CapSide_HeiR);
			mt.read(m_CapTop_XLen);
			mt.read(m_CapSec_Hei);
			mt.read(m_CapSec_Lever);
			mt.read(m_PierToGLSide_LenL);
			mt.read(m_PierToGLSide_LenR);
		}
	}
}

cap_beam_Pier::cap_beam_Pier()
{
	m_PierSec_Type = 0;
	m_Pier_D = 1600.0;
	m_bSetXL = false;
	m_XL_Wide = 0.0;
	m_XL_Hei = 0.0;
	m_Pier_PosZ = 0.0;

	dWide_H = 0.0;
	dWide_Z = 0.0;
	dWide_LnDJ_H = 0.0;
	dWide_LnDJ_Z = 0.0;
	dWide_ArcDJ_R = 0.0;
}
cap_beam_Pier::~cap_beam_Pier()
{
}
void cap_beam_Pier::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		//墩柱
		mt.write(_T("m_PierSec_Type"), m_PierSec_Type);
		mt.write(_T("m_Pier_D"), m_Pier_D);
		mt.write(_T("m_bSetXL"), m_bSetXL);
		mt.write(_T("m_XL_Wide"), m_XL_Wide);
		mt.write(_T("m_XL_Hei"), m_XL_Hei);
		mt.write(_T("m_Pier_PosZ"), m_Pier_PosZ);
		mt.write(_T("m_Pier_PosH"), m_Pier_PosH);
		mt.write(_T("m_XL_Pos"), m_XL_Pos);

		mt.write(_T("dWide_H"), dWide_H);
		mt.write(_T("dWide_Z"), dWide_Z);
		mt.write(_T("dWide_LnDJ_H"), dWide_LnDJ_H);
		mt.write(_T("dWide_LnDJ_Z"), dWide_LnDJ_Z);
		mt.write(_T("dWide_ArcDJ_R"), dWide_ArcDJ_R);
	}
	else
	{
		mt.read(version);
		if (1 == version)
		{
			mt.read(m_PierSec_Type);
			mt.read(m_Pier_D);
			mt.read(m_bSetXL);
			mt.read(m_XL_Wide);
			mt.read(m_XL_Hei);
			mt.read(m_Pier_PosZ);
			mt.read(m_Pier_PosH);
			mt.read(m_XL_Pos);

			mt.read(dWide_H);
			mt.read(dWide_Z);
			mt.read(dWide_LnDJ_H);
			mt.read(dWide_LnDJ_Z);
			mt.read(dWide_ArcDJ_R);
		}
	}
}

cap_beam_JC::cap_beam_JC()
{
	m_Fnd_Type = 0;
	m_Found_Type = 0;
	m_JC_PosZ = 0.0;
	m_Found_D = 0.0;
	m_bFound_JiaCu = false;
	m_JiaCu_D = 0.0;
	m_JiaCu_Hei = 0.0;
	m_JiaCuGD_Hei = 0.0;
	m_bFound_SetXL = 0.0;
	m_FoundXL_Wide = 0.0;
	m_FoundXL_Hei = 0.0;
	m_FoundXL_bSetBotDC = false;
	m_FoundXLDC_Hei = 0.0;
	m_FoundXLDC_Wide = 0.0;
	m_bSetXL = false;
	m_XL_Wide = 0.0;
	m_XL_Hei = 0.0;

	m_CTHor_Width = 0.0;
	m_CTVer_Width = 0.0;
	m_CT_Hei = 0.0;
	m_CTFound_DisCT = 0.0;
	m_Layout_Type = 0;
	//扩大基础
	m_KuoDa_TopHei = 0.0;
	m_KuoDa_TopWZ = 0.0;
	m_KuoDa_TopWH = 0.0;
}
cap_beam_JC::~cap_beam_JC()
{

}
void cap_beam_JC::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_Fnd_Type"), m_Fnd_Type);
		mt.write(_T("m_Found_Type"), m_Found_Type);
		mt.write(_T("m_JC_PosZ"), m_JC_PosZ);
		mt.write(_T("m_Found_D"), m_Found_D);
		mt.write(_T("m_bFound_JiaCu"), m_bFound_JiaCu);
		mt.write(_T("m_JiaCu_D"), m_JiaCu_D);
		mt.write(_T("m_JiaCu_Hei"), m_JiaCu_Hei);
		mt.write(_T("m_JiaCuGD_Hei"), m_JiaCuGD_Hei);
		mt.write(_T("m_bFound_SetXL"), m_bFound_SetXL);
		mt.write(_T("m_FoundXL_Wide"), m_FoundXL_Wide);
		mt.write(_T("m_FoundXL_Hei"), m_FoundXL_Hei);
		mt.write(_T("m_FoundXL_bSetBotDC"), m_FoundXL_bSetBotDC);
		mt.write(_T("m_FoundXLDC_Hei"), m_FoundXLDC_Hei);
		mt.write(_T("m_FoundXLDC_Wide"), m_FoundXLDC_Wide);
		mt.write(_T("m_bSetXL"), m_bSetXL);
		mt.write(_T("m_XL_Wide"), m_XL_Wide);
		mt.write(_T("m_XL_Hei"), m_XL_Hei);
		mt.write(_T("m_CTHor_Width"), m_CTHor_Width);
		mt.write(_T("m_CTVer_Width"), m_CTVer_Width);
		mt.write(_T("m_CT_Hei"), m_CT_Hei);
		mt.write(_T("m_CTFound_DisCT"), m_CTFound_DisCT);
		mt.write(_T("m_Layout_Type"), m_Layout_Type);
		mt.write(_T("m_KuoDa_TopHei"), m_KuoDa_TopHei);
		mt.write(_T("m_KuoDa_TopWZ"), m_KuoDa_TopWZ);
		mt.write(_T("m_KuoDa_TopWH"), m_KuoDa_TopWH);
		mt.write(_T("m_FoundXL_Pos"), m_FoundXL_Pos);
		mt.write(_T("m_JC_PosH"), m_JC_PosH);
		mt.write(_T("m_JC_TopLever"), m_JC_TopLever);
		mt.write(_T("m_JC_BotLever"), m_JC_BotLever);

	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_Fnd_Type);
			mt.read(m_Found_Type);
			mt.read(m_JC_PosZ);
			mt.read(m_Found_D);
			mt.read(m_bFound_JiaCu);
			mt.read(m_JiaCu_D);
			mt.read(m_JiaCu_Hei);
			mt.read(m_JiaCuGD_Hei);
			mt.read(m_bFound_SetXL);
			mt.read(m_FoundXL_Wide);
			mt.read(m_FoundXL_Hei);
			mt.read(m_FoundXL_bSetBotDC);
			mt.read(m_FoundXLDC_Hei);
			mt.read(m_FoundXLDC_Wide);
			mt.read(m_bSetXL);
			mt.read(m_XL_Wide);
			mt.read(m_XL_Hei);
			mt.read(m_CTHor_Width);
			mt.read(m_CTVer_Width);
			mt.read(m_CT_Hei);
			mt.read(m_CTFound_DisCT);
			mt.read(m_Layout_Type);
			mt.read(m_KuoDa_TopHei);
			mt.read(m_KuoDa_TopWZ);
			mt.read(m_KuoDa_TopWH);
			mt.read(m_FoundXL_Pos);
			mt.read(m_JC_PosH);
			mt.read(m_JC_TopLever);
			mt.read(m_JC_BotLever);
		}
	}
}

cap_beam_DrawSet::cap_beam_DrawSet()
{
	m_Scale = 0.0;
	m_CtrBreakHei = 8000.0;
	m_DrawDK = false;
	m_DrawDS = false;
	m_DrawDMX = false;
	m_DrawGCSLB = false;
	m_DrawCTPM = false;
	m_BZDSType = false;
	m_BZDSLever_LM = false;
	m_BZDSHei_LM = false;
	m_bBZDSPosInPM = false;
	m_BZSlopeInside = false;
	m_BZGLLeverAtDKInside = false;
	m_bBZGLDJSizeInPM = false;
	m_bBZDunPMPos = false;
	m_bBZCMBKXdisDunCen = false;
}
cap_beam_DrawSet::~cap_beam_DrawSet()
{

}
void cap_beam_DrawSet::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_Scale"), m_Scale);
		mt.write(_T("m_FZs"), m_FZs);
		mt.write(_T("m_CtrBreakHei"), m_CtrBreakHei);
		mt.write(_T("m_DrawDK"), m_DrawDK);
		mt.write(_T("m_DrawDS"), m_DrawDS);
		mt.write(_T("m_DrawDMX"), m_DrawDMX);
		mt.write(_T("m_DrawGCSLB"), m_DrawGCSLB);
		mt.write(_T("m_DrawCTPM"), m_DrawCTPM);
		mt.write(_T("m_BZDSType"), m_BZDSType);
		mt.write(_T("m_BZDSLever_LM"), m_BZDSLever_LM);
		mt.write(_T("m_BZDSHei_LM"), m_BZDSHei_LM);
		mt.write(_T("m_bBZDSPosInPM"), m_bBZDSPosInPM);
		mt.write(_T("m_BZSlopeInside"), m_BZSlopeInside);
		mt.write(_T("m_BZGLLeverAtDKInside"), m_BZGLLeverAtDKInside);
		mt.write(_T("m_bBZGLDJSizeInPM"), m_bBZGLDJSizeInPM);
		mt.write(_T("m_bBZDunPMPos"), m_bBZDunPMPos);
		mt.write(_T("m_bBZCMBKXdisDunCen"), m_bBZCMBKXdisDunCen);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_Scale);
			mt.read(m_FZs);
			mt.read(m_CtrBreakHei);
			mt.read(m_DrawDK);
			mt.read(m_DrawDS);
			mt.read(m_DrawDMX);
			mt.read(m_DrawGCSLB);
			mt.read(m_DrawCTPM);
			mt.read(m_BZDSType);
			mt.read(m_BZDSLever_LM);
			mt.read(m_BZDSHei_LM);
			mt.read(m_bBZDSPosInPM);
			mt.read(m_BZSlopeInside);
			mt.read(m_BZGLLeverAtDKInside);
			mt.read(m_bBZGLDJSizeInPM);
			mt.read(m_bBZDunPMPos);
			mt.read(m_bBZCMBKXdisDunCen);
			
		}
	}
}

cap_beam_CSHLB::cap_beam_CSHLB()
{
	m_bOnlyOneLB = false;
	m_iCSHLBBGType = 0;
	m_iDunNumType = 0;
	m_bAllLeverUseOneName = false;
	m_bDSLeverLB = false;
	m_bDSHeiLB = false;

	m_strCSHLBName = _T("参数化列表");
	m_Angle = _T("A");
	m_GL_Len = _T("L");
	m_BS = _T("S");
	m_left_Slope = _T("i");
	m_right_Slope = _T("i");
	m_left_DJ = _T("L1");
	m_right_DJ = _T("L2");
	m_left_JB = _T("K1");
	m_right_JB = _T("K2");
	m_Dun_Dis = _T("Bs");
	m_GL_LeftTopLever = _T("H1");

	m_GL_rightTopLever = _T("H2");
	m_Dun_TopLever = _T("H3");
	m_DunXL_Lever = _T("H4");
	m_JC_TopLever = _T("H5");
	m_JC_BotLever = _T("H6");
	m_Dun_Hei = _T("h");

	m_Found_Len = _T("l");
	m_DS_Lever = _T("dsH");
	m_DS_Hei = _T("dsh");
	m_DisBKXtoRd = _T("e");
}
cap_beam_CSHLB::~cap_beam_CSHLB()
{

}
void cap_beam_CSHLB::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_CSHLB_Comment"), m_CSHLB_Comment);
		mt.write(_T("m_strCSHLBName"), m_strCSHLBName);
		mt.write(_T("m_bOnlyOneLB"), m_bOnlyOneLB);
		mt.write(_T("m_iCSHLBBGType"), m_iCSHLBBGType);
		mt.write(_T("m_iDunNumType"), m_iDunNumType);
		mt.write(_T("m_bAllLeverUseOneName"), m_bAllLeverUseOneName);
		mt.write(_T("m_bDSLeverLB"), m_bDSLeverLB);
		mt.write(_T("m_bDSHeiLB"), m_bDSHeiLB);

		mt.write(_T("m_Angle"), m_Angle);
		mt.write(_T("m_GL_Len"), m_GL_Len);
		mt.write(_T("m_BS"), m_BS);
		mt.write(_T("m_left_Slope"), m_left_Slope);
		mt.write(_T("m_right_Slope"), m_right_Slope);
		mt.write(_T("m_left_DJ"), m_left_DJ);
		mt.write(_T("m_right_DJ"), m_right_DJ);
		mt.write(_T("m_left_JB"), m_left_JB);
		mt.write(_T("m_right_JB"), m_right_JB);
		mt.write(_T("m_Dun_Dis"), m_Dun_Dis);
		mt.write(_T("m_GL_LeftTopLever"), m_GL_LeftTopLever);
		mt.write(_T("m_GL_rightTopLever"), m_GL_rightTopLever);
		mt.write(_T("m_Dun_TopLever"), m_Dun_TopLever);
		mt.write(_T("m_DunXL_Lever"), m_DunXL_Lever);
		mt.write(_T("m_JC_TopLever"), m_JC_TopLever);
		mt.write(_T("m_JC_BotLever"), m_JC_BotLever);
		mt.write(_T("m_Dun_Hei"), m_Dun_Hei);
		mt.write(_T("m_Found_Len"), m_Found_Len);
		mt.write(_T("m_DS_Lever"), m_DS_Lever);
		mt.write(_T("m_DS_Hei"), m_DS_Hei);
		mt.write(_T("m_DisBKXtoRd"), m_DisBKXtoRd);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_CSHLB_Comment);
			mt.read(m_strCSHLBName);
			mt.read(m_bOnlyOneLB);
			mt.read(m_iCSHLBBGType);
			mt.read(m_iDunNumType);
			mt.read(m_bAllLeverUseOneName);
			mt.read(m_bDSLeverLB);
			mt.read(m_bDSHeiLB);

			mt.read(m_Angle);
			mt.read(m_GL_Len);
			mt.read(m_BS);
			mt.read(m_left_Slope);
			mt.read(m_right_Slope);
			mt.read(m_left_DJ);
			mt.read(m_right_DJ);
			mt.read(m_left_JB);
			mt.read(m_right_JB);
			mt.read(m_Dun_Dis);
			mt.read(m_GL_LeftTopLever);
			mt.read(m_GL_rightTopLever);
			mt.read(m_Dun_TopLever);
			mt.read(m_DunXL_Lever);
			mt.read(m_JC_TopLever);
			mt.read(m_JC_BotLever);
			mt.read(m_Dun_Hei);
			mt.read(m_Found_Len);
			mt.read(m_DS_Lever);
			mt.read(m_DS_Hei);
			mt.read(m_DisBKXtoRd);
		}
	}
}

cap_beam_Material::cap_beam_Material()
{

}
cap_beam_Material::~cap_beam_Material()
{

}
void cap_beam_Material::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_GL_Mat"), m_GL_Mat);
		mt.write(_T("m_DS_Mat"), m_DS_Mat);
		mt.write(_T("m_Dun_Mat"), m_Dun_Mat);
		mt.write(_T("m_DunXL_Mat"), m_DunXL_Mat);
		mt.write(_T("m_CTKD_Mat"), m_CTKD_Mat);
		mt.write(_T("m_CTDC_Mat"), m_CTDC_Mat);
		mt.write(_T("m_JCXL_Mat"), m_JCXL_Mat);
		mt.write(_T("m_Fnd_Mat"), m_Fnd_Mat);
		mt.write(_T("m_GL_Type"), m_GL_Type);
		mt.write(_T("m_DS_Type"), m_DS_Type);
		mt.write(_T("m_Dun_Type"), m_Dun_Type);
		mt.write(_T("m_DunXL_Type"), m_DunXL_Type);
		mt.write(_T("m_CTKD_Type"), m_CTKD_Type);
		mt.write(_T("m_CTDC_Type"), m_CTDC_Type);
		mt.write(_T("m_JCXL_Type"), m_JCXL_Type);
		mt.write(_T("m_Fnd_Type"), m_Fnd_Type);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_GL_Mat);
			mt.read(m_DS_Mat);
			mt.read(m_Dun_Mat);
			mt.read(m_DunXL_Mat);
			mt.read(m_CTKD_Mat);
			mt.read(m_CTDC_Mat);
			mt.read(m_JCXL_Mat);
			mt.read(m_Fnd_Mat);
			mt.read(m_GL_Type);
			mt.read(m_DS_Type);
			mt.read(m_Dun_Type);
			mt.read(m_DunXL_Type);
			mt.read(m_CTKD_Type);
			mt.read(m_CTDC_Type);
			mt.read(m_JCXL_Type);
			mt.read(m_Fnd_Type);
		}
	}
}




//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(cap_beam_data, graph_data_base, 1)

cap_beam_data::cap_beam_data()
{

}
cap_beam_data::~cap_beam_data()
{

}

void cap_beam_data::serialize_members(member_rw_interface& mt)
{
	int version = 1;

	if (mt.is_storing())
	{
		mt.write(_T("version"), version);
		mt.write(_T("m_Common"), m_Common);
		mt.write(_T("m_Gen"), m_Gen);
		mt.write(_T("m_GL"), m_GL);
		mt.write(_T("m_Pier"), m_Pier);
		mt.write(_T("m_JC"), m_JC);
		mt.write(_T("m_DrawSet"), m_DrawSet);
		mt.write(_T("m_CSHLB"), m_CSHLB);
		mt.write(_T("m_Materials"), m_Materials);
	}
	else
	{
		mt.read(version);

		if (1 == version)
		{
			mt.read(m_Common);
			mt.read(m_Gen);
			mt.read(m_GL);
			mt.read(m_Pier);
			mt.read(m_JC);
			mt.read(m_DrawSet);
			mt.read(m_CSHLB);
			mt.read(m_Materials);
		}
	}

	graph_data_base::serialize_members(mt);
}





