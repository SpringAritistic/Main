#pragma once

#include "tendon.h"

class CTHDataBase;

//////////方案设计师钢束通用界面，根据《方案设计师V1-审核1》于2011.06.15有较大改动，升级第二版本////////////
#define TENDON_SC001_ZL			 "纵梁"
#define TENDON_SC001_HL          "横梁"
#define TENDON_HXYYL             "横向预应力"
#define TENDON_SXYYL             "竖向预应力"


X_FRAMEWORK_API void RegisterGWTendonComType();

namespace  THSF
{
	class CTendon;
	class CBeamTypeTable;
	class CBeamLayOutTable;
	class CZLPCTable;
	class CMMTable;
	class CBendingType;
}

//
class X_FRAMEWORK_API CGWTendon
{
public:
	CGWTendon();
	~CGWTendon();
public:
	//优化的静态接口
	static void PU2P0(const CPUObj* pPU, CGWTendon_P0& P0, const CString& strID = "");
	static void P02PU(CPUObj* pPU, const CGWTendon_P0& P0, const CString& strID = "");
	//初始化钢束算法器
	static void InitTendonAlgor(const CGWTendon_P0& P0, THSF::CTendon*& m_pTendonAlgor);
	static void InitTendonAlgor(const CGWTendon_P0& P0, THSF::CTendon& TendonAlgor);
	static void InitTendonAlgor(const CPUObj* pPU, THSF::CTendon*& m_pTendonAlgor, const CString& strID = "");

	//钢束P0合并
	static void MergeTendons(const vector<CGWTendon_P0>& vec_tendon_info, 
		const vector<CString>& vecID, OUT CGWTendon_P0& Tendon);

	//获得张拉批次名称,除了钢束名称另外在first位置处添加All
	static vector<CString> GetName_zlpc(const CPUObj* pPU, const CString& strID = "");
	
	//changetype改变的类型：1为改变平弯类型表，2为改变竖弯类型表，3为改变钢束布置表，map.first为替换前
	void ChangeRefName(const std::map<CString,CString>& namemap,int changetype);

	//changetype改变的类型：1为改变平弯类型表，2为改变竖弯类型表，3为改变钢束布置表，map.first为替换前
	static void ChangeRefName(const std::map<CString,CString>& namemap,int changetype, CGWTendon_P0& P0);
public:
	//设置钢束界面ID号
	void SetID(const CString& strID){m_strID = strID;}
	//方法一：通过设置PU数据初始化钢束算法器
	//将界面database送往钢束算法器类
	void AddDataBase(const CPUObj* pPU);
	//初始化钢束算法器
	void InitTendonAlgor(); 
	//方法二：通过设置P0数据初始化钢束算法器
	void InitTendonAlgor2(const CGWTendon_P0& P0);  

	//得到需要注册的表格名称,传入的strTableName不会清空
	static void GetRegisterTableName(CStringArray& strArr, const CString& strID = "");
	static void GetRegisterTableName2(CStringArray& strArr, const CString& strID = "");//得到实例名

	//PU中需要调用的函数

	//创建手写表格 InitMyGrid()函数中调用
	static void CreateUserDataBase(CPUObj* pPU, OUT vector<CString>& vecTableName, const CString& strID = "");

	//设置表格环境变量 InitGridData()函数中调用
	static void SetEnvVariable(CPUObj* pPU, const CString& strID = "");

public:
	//添加构件参考线名称
	void AddComponentLineNames(CString Name);

	//清空构件参考线
	void ClearComponentLineNames(){m_ComponentLines.clear();}

	//pu<->p0
	void PU2P0();
	void P02PU();

	//手动创建平弯/竖弯表格
	//bSW=true创建竖弯表格
	//vecRowNum大行组数和当行组中小行组数 一般为1,3
	//bHXTZ:是否屏蔽横向特征线和X值 (目前竖向预应力的时候需要屏蔽)
	//bSGF 是否屏蔽施工缝（目前施工缝只有上部结构05的纵梁竖弯中才有）
	static CTHDataBase* CreatePSWTypeDataBase(vector<int> vecRowNum, bool bSW= true, bool bHXTZ = false, bool bSGF = false);
	
	/*手动创建钢束布置表*/
	//vecRowNum大行组数和当行组中小行组数 一般为1,3
	static CTHDataBase* CreateTendonLayDataBase(vector<int> vecRowNum);

	//设置表格初始化时的显示行数
	static void SetRowNumber(CPUObj* pPU, int nRow = 3, const CString& strID = "");

private:
	//设置参考线
	void SetRefCurve();

	////设置钢束算法器结构
	void SetTendonData();
	void SetTendonType(const vector<tendon_data_type>& vecP0, const vector<tendon_data_basetype>& vecBase, 
		vector<boost::shared_ptr<THSF::CBeamTypeTable>>& vecSFP0);
	void SetTendonLay(const vector<tendon_data_info> vecP0, vector<boost::shared_ptr<THSF::CBeamLayOutTable>>& vecSFP0);
	void SetTendonZLPC(const vector<tendon_data_tension_batch> vecP0, vector<boost::shared_ptr<THSF::CZLPCTable>>& vecSFP0);
	void SetTendonMM(const vector<tendon_data_anchor_plane> vecP0, vector<boost::shared_ptr<THSF::CMMTable>>& vecSFP0);
	void SetTendonLayPSW(const vector<tendon_data_bend_type> vecP0, vector<boost::shared_ptr<THSF::CBendingType>>& vecSFP0);

public:
	THSF::CTendon* m_pTendonAlgor;//钢束算法类
	CGWTendon_P0 m_P0;//P0数据
private:
	std::map<CString,CTHDataBase*> m_pTableData; //界面表格数据类
	vector<CString> m_ComponentLines;	//构件参考线,暂时没有作用，内定参考线不需要设置

	//支持一个构件注册多个钢束界面
	CString m_strID;
};


template <class Ty>
bool GetTypeFromVecByName(const vector<Ty>& vecT, const CString& strName,OUT Ty& T)
{
	if(strName == "")
		return false;

	for (int i = 0; i < static_cast<int>(vecT.size()); ++i)
	{
		if (strName == vecT[i].m_name)
		{
			T = vecT[i];
			return true;
		}
	}
	ASSERT(("类型名称对应的类型不存在", false));
	return false;
}


template <class Ty>
void Modify_Index(IN OUT vector<Ty>& vecT, const CString& strName)
{
	if(strName == "")
		return;
	
	for (size_t i = 0; i < vecT.size(); ++i)
		vecT[i].m_name += strName;
}

template <class Ty>
void Modify_Index2(IN OUT vector<Ty>& vecT, const CString& strName)
{
	if(strName == "")
		return;

	for (size_t i = 0; i < vecT.size(); ++i)
	{
		int nPos = vecT[i].m_name.Find("[");
		if (-1 != nPos)
			vecT[i].m_name.Insert(nPos, strName);
	}
}
template <class Ty>
void Add2Vector(IN OUT vector<Ty>& vecRes, const vector<Ty>& vecSrc)
{
	for (size_t i = 0; i < vecSrc.size(); ++i)
		vecRes.push_back(vecSrc[i]);
}