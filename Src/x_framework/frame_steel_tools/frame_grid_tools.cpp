#include "stdafx.h"

#include <map>
#include <cmath>

#include "THGridControl/THGridControlInc.h"
#include "THGridControl/THDataBase.h"

#include "../tools/table_templates.h"
#include "frame_data.h"
#include "hook_setting_dialog.h"

#include "frame_grid_tools.h"


//////////////////////////////////////////////////////////////////////////

#define OP_ERROR -1 // OpReturnValue错误时返回的受影响行数为-1

//////////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;




frame_grid_tools::frame_grid_tools(void)
{

}

frame_grid_tools::~frame_grid_tools( void )
{

}


std::vector<CString> frame_grid_tools::get_all_frame_grid_template_names(const std::vector<int>& vecRowNum, bool bT, bool bD, bool bEnableSteelType)
{
	CTHDataBase* db = create_frame_grid(vecRowNum, bT, bD, bEnableSteelType);
	table_templates::add_database_sample(_T("frame-frame_info"), db);

	vector<CString> names ={_T("骨架-骨架弯钩信息"), _T("frame-frame_info")};
	return names;
}

CString frame_grid_tools::get_template_name_frame_defi(const CString& strPart /*= _T("")*/)
{
	return  "frame-frame_info" + strPart;
}

CString frame_grid_tools::get_template_name_hooks( const CString& strPart )
{
	return  "骨架-骨架弯钩信息"+strPart;
}

CString frame_grid_tools::get_template_name_hook_settings( const CString& strPart )
{
	return  "指定控制点弯钩信息二级表"+strPart;
}

// 获取一个骨架面中的公共属性信息，内部不对表格进行检查，请保证传过来的表格是一个
// 钢筋面中的属性子表，不是整个钢筋面表
void PU2DataInnerFrameInfo(CGridDataBase &pGrid, frame_data_piece &P0 )
{
	CTHCell *pTempCell = NULL;
	int nRow = 0;
	int nCol = 4;
	
	// 索引的上一行为控制信息，其Label中保存用于骨架编辑器交互使用的应用部位字符串
	// 【Add By WYZ 2013年7月1日】
	pTempCell = pGrid.GetCell(nRow, nCol);
	if (pTempCell) P0.m_laction = pTempCell->GetLabelText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_index_name = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_need_weld = bool(pTempCell->GetText() != _T("0"));

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_is_symmetric = bool(pTempCell->GetText() != _T("0")); // 是否对称目前还不支持

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	// 钢种号在BW4GW中使用大样来存储，大样中没有钢种号成员变量，此处
	// 设置这个值不生效，在BW4GW中该值根据钢筋类型自动计算得出，这个单元格
	// 留备用，此处就算设置了它的值也不会被BW4GW内部采用
	P0.m_steel_type = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_diameter = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_hook_weld_length = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_top_space = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_bottom_space = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_top_flag = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_bottom_flag = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_bent_angle = pTempCell->GetText();// 单位为角度

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_arc_steel_radius = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_mid_bent_radius = pTempCell->GetText();

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_mid_weld_length = pTempCell->GetText();

	// 骨架面设计思想
	// 注：以下两个变量只有当钢筋面为非骨架面时设置才生效，为骨架面时不生效
	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_has_top_perforation_steel = bool(pTempCell->GetText() != _T("0"));

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_has_bottom_perforation_steel = bool(pTempCell->GetText() != _T("0"));

	++nRow;
	pTempCell = pGrid.GetCell(nRow, nCol);
	P0.m_hook_settings = pTempCell->GetText();

}

// 获取一个骨架面中的所有控制点信息，内部不对表格进行检查，请保证传来的表格是控制点表
void PU2DataInnerFrameNodes_Top(CGridDataBase &grid, vector<frame_data_node>& vecP0)
{
	int nNodesCount = grid.GetRowCount();
	CString strTemp;
	CTHCell *pTempCell = NULL;

	vecP0.clear();
	for (int i = 1; i < nNodesCount; ++i) // 0行为标题行
	{
		int nCol = 3; // 列号（第三列开始获取数据）
		frame_data_node nodeInfo;
		bool bAllBlank = true; // 是否当前行中的所有列均为空，如果全空则不取这一行数据

		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_ref_line = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_dist_to_ref_line = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_begin_layer = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_end_layer = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_transposed = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_diameter = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		if (false == bAllBlank) vecP0.push_back(nodeInfo);
		
	}

}

// 获取一个骨架面中的所有控制点信息，内部不对表格进行检查，请保证传来的表格是控制点表
void PU2DataInnerFrameNodes_Bottom(CGridDataBase& grid, vector<frame_data_node>& vecP0)
{
	int nNodesCount = grid.GetRowCount();
	CString strTemp;
	CTHCell *pTempCell = NULL;

	vecP0.clear();
	for (int i = 1; i < nNodesCount; ++i) // 0行为标题行
	{
		int nCol = 3; // 列号（第三列开始获取数据）
		frame_data_node nodeInfo;
		bool bAllBlank = true; // 是否当前行中的所有列均为空，如果全空则不取这一行数据

		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_ref_line = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_dist_to_ref_line = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_begin_layer = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_end_layer = strTemp;
		if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_transposed = strTemp;
		// 可以为空，所以注释掉
		// if (false == strTemp.IsEmpty()) bAllBlank = false;

		++nCol;
		pTempCell = grid.GetCell(i, nCol);
		strTemp = (pTempCell == NULL ? _T("") : pTempCell->GetText());
		nodeInfo.m_diameter = strTemp;
		// 可以为空，所以注释掉
		// if (false == strTemp.IsEmpty()) bAllBlank = false;

		if (false == bAllBlank) vecP0.push_back(nodeInfo);
	}

}

// 获取一个骨架面中的所有弯钩信息，内部不对表格进行检查，请保证传来的表格是弯钩表
void PU2DataInnerHooks(CTHDataBase& DB, frame_data& P0)
{
	vector<frame_data_hook>& vecHooks = P0.m_all_hooks;
	vecHooks.clear();

	int nRowCounts = DB.GetNumberRows(); // 数据行数，不是表行数
	if (nRowCounts <= 0)
	{
		return;
	}

	frame_data_hook hook;

	bool bgeneral = false;
	for (int i = 0; i < nRowCounts; ++i)
	{
		bool flag = false;

		flag = DB.Get_Prop(i, _T("m_strIndex"), hook.m_index_name);
		hook.m_index_name.Trim();
		_ASSERTE((_T("DB.Get_Prop(...)，转换操作失败"), flag ));

		flag = DB.Get_Prop(i, _T("m_strHookType"), hook.m_hook_type);
		_ASSERTE((_T("DB.Get_Prop(...)，转换操作失败"), flag ));

		flag = DB.Get_Prop(i, _T("m_strHookL"), hook.m_hook_length);
		_ASSERTE((_T("DB.Get_Prop(...)，转换操作失败"), flag ));

		flag = DB.Get_Prop(i, _T("m_strArcR"), hook.m_arc_radius);
		_ASSERTE((_T("DB.Get_Prop(...)，转换操作失败"), flag ));

		flag = DB.Get_Prop(i, _T("m_strEndDirect"), hook.m_hook_end_type);
		_ASSERTE((_T("DB.Get_Prop(...)，转换操作失败"), flag ));

		flag = DB.Get_Prop(i, _T("m_strDirectAngle"), hook.m_hook_end_angle);
		_ASSERTE((_T("DB.Get_Prop(...)，转换操作失败"), flag ));

		flag = DB.Get_Prop(i, _T("m_strHookDirect"), hook.m_hook_direction);
		_ASSERTE((_T("DB.Get_Prop(...)，转换操作失败"), flag ));

	
		 if (_T("通用弯钩") == hook.m_index_name)
		 {
			 if(!bgeneral)
			 {
				 P0.m_general_hook = hook;
			 }	
			 bgeneral=true;	
			 continue; // 不能把通用弯钩放到vecHooks中去
		 }			
		if (_T("自定义") == hook.m_index_name) // 第一个弯钩为通用弯钩(此处检测人为的原因导致第一行不是通用弯钩的情况)
		{
			CString strError;
			strError.Format(_T("第%d行弯钩名不能为“自定义”，程序将丢弃此弯钩数据"), i + 1);
			////////////ReportMessage(1, strError);
			continue;
		}		
		if (!hook.m_index_name.IsEmpty())
		{
			vecHooks.push_back(hook);
		}
	} // end of for(...)

////////如果不存在通用弯钩，内部增加通用弯钩
	if(!bgeneral)
	{
		frame_data_hook hookTmp;
		hookTmp.m_index_name = _T("通用弯钩");	// 索引名称
		hookTmp.m_hook_type = _T("直线弯钩");	// 弯钩型式,0,直线弯钩;1,圆弧弯钩;2,折线弯钩;
		hookTmp.m_hook_length = _T("100");			// 弯钩直线段长度
		hookTmp.m_arc_radius = _T("100");			// 圆弧半径
		hookTmp.m_hook_end_type = _T("垂直方向"); // 端部截断点的弯钩的方向类型[0,弯钩为竖直方向;1,弯钩为垂直方向;2,指定角度dAngle]
		hookTmp.m_hook_end_angle = _T("0");		// 端部截断点的弯钩与钢筋线的夹角（角度）
		hookTmp.m_hook_direction = _T("向外");	// 弯钩方向（true向内、false向外）
		hookTmp.m_binded_segment = _T("");				// 弯钩所贴近的钢筋段名

		P0.m_general_hook = hookTmp;
	}

}

void PU2DataInnerFrames(CGridDataBase &pGrid, vector<frame_data_piece> &P0)
{
	P0.clear();
	int nFramesCount = pGrid.GetSubDataBaseCount();
	for (int i = 0; i < nFramesCount; ++i)
	{
		CGridDataBase oneFrame;
		frame_data_piece frame;
		BOOL flagFrame = pGrid.GetSubDataBase(i, oneFrame); // 第i片钢筋面
		if (!flagFrame) continue;

		// 每片骨架钢筋面有四个子表，第一个为控制信息（且最后多一行），第二个为顶部控制点，第三个为分隔行，第四个为底部控制点
		CGridDataBase oneInfo;
		BOOL flagInfo = oneFrame.GetSubDataBase(0, oneInfo);// 获取第i个钢筋面的控制信息表
		if (flagInfo)
		{
			PU2DataInnerFrameInfo(oneInfo, frame);
		}
		else
		{
			_ASSERTE((_T("BOOL flagInfo = oneFrame.GetSubDataBase(...); 传入的表结构不正确"), false));
			continue;
		}

		// 没有控制点也可以算是一个钢筋面，因为程序执行到此处时说明有钢筋面的公共信息表
		frame.m_top_nodes .clear();
		frame.m_bottom_nodes.clear();

		CGridDataBase topNodes;
		BOOL flagTopNode = oneFrame.GetSubDataBase(1, topNodes); // 获取第i个钢筋面的顶部控制点表
		if (flagTopNode)
		{
			PU2DataInnerFrameNodes_Top(topNodes, frame.m_top_nodes);
		}
		else
		{
			_ASSERTE((_T("BOOL flagInfo = oneFrame.GetSubDataBase(...); 传入的表结构不正确"), false));
			continue;
		}

		CGridDataBase botNodes;
		BOOL flagBotNode = oneFrame.GetSubDataBase(3, botNodes); // 获取第i个钢筋面的底部控制点表
		if (flagBotNode)
		{
			PU2DataInnerFrameNodes_Bottom(botNodes, frame.m_bottom_nodes);
		}
		else
		{
			_ASSERTE((_T("BOOL flagInfo = oneFrame.GetSubDataBase(...); 传入的表结构不正确"), false));
			continue;
		}

		// 查找当前骨架面在FrameIn中有没有重名的，有则断言之（名称为空不认为是重复）
		bool flagSameFrame = false;
		for (size_t j = 0; j != P0.size(); ++j)
		{
			if (frame.m_index_name == P0[j].m_index_name && frame.m_index_name != _T(""))
			{
				_ASSERTE((_T("钢筋面类型标识不允许重复"), false));
				flagSameFrame = true;
				break;
			}
		}

		// 重复的骨架面不再放到内部数据中
		if (!flagSameFrame) P0.push_back(frame);

	}
}

inline bool IsSameHookName(const frame_data_hook &firstHook, const frame_data_hook &secondHook)
{
	return firstHook.m_index_name == secondHook.m_index_name;
}

// 设置内部数据到一个骨架面中的公共属性信息，内部不对表格进行检查，请保证传过来的表格是一个
// 钢筋面中的属性子表，不是整个钢筋面表（即包含多个钢筋面的表）
void DataInner2PUFrameInfo(CTHDataBase &db, const frame_data_piece &dataInner)
{
	CTHCell *pTempCell = NULL;
	CString strTemp;

	int nRow = 2;
	int nCol = 4;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetLabelText(dataInner.m_laction);
	
	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_index_name);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_need_weld ? _T("1") : _T("0"));
	if (dataInner.m_need_weld)
	{
		// 焊成骨架后没有顶底层通长筋
		CTHCell* pCellTopBot;
		pCellTopBot = db.GetCell(nRow + 13, nCol);
		pCellTopBot->SetReadOnly(TRUE);

		pCellTopBot = db.GetCell(nRow + 14, nCol);
		pCellTopBot->SetReadOnly(TRUE);
	}

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_is_symmetric ? _T("1") : _T("0"));

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_steel_type);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_diameter);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_hook_weld_length);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_top_space);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_bottom_space);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_top_flag);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_bottom_flag);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_bent_angle);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_arc_steel_radius);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_mid_bent_radius);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_mid_weld_length);

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_has_top_perforation_steel ? _T("1") : _T("0"));

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_has_bottom_perforation_steel ? _T("1") : _T("0"));

	++nRow;
	pTempCell = db.GetCell(nRow, nCol);
	if (pTempCell) pTempCell->SetText(dataInner.m_hook_settings);
}

// 设置内部数据到一个骨架面中的所有控制点信息，内部不对表格进行检查，请保证
// 传过来的表格是控制点表，不对控制点表中控制点的弯钩合法性进行检查
void DataInner2PUFrameNodes_Top(CTHDataBase& db, const vector<frame_data_node>& dataInner)
{
	const int nBeginRow = 22;
	db.RemoveRowsAt(nBeginRow, 1, _T("顶部控制点分组"));

	// 填充数据到控制点中
	OpReturnValue flagOp;
	for (int i = int(dataInner.size()) - 1; i >= 0 ; --i) // 从后往前填，i不能用size_t型，否则死循环
	{
		const frame_data_node &node = dataInner[i];
		CTHCell *pTempCell = NULL;
	
		// 在最前面插入一行弯钩行
		flagOp = db.InsertRowPost(nBeginRow - 1, 1, _T("顶部控制点分组"));// 控制点标题行上向后插入
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.InsertRowPost(...)异常"), false));
			return; // 表异常就没有必要再向后操作了，表结构异常，其它数据已经不可能正常插入了。
		}

		pTempCell = db.GetCell(nBeginRow, 3);
		if (pTempCell) pTempCell->SetText(node.m_ref_line);

		pTempCell = db.GetCell(nBeginRow, 4);
		if (pTempCell) pTempCell->SetText(node.m_dist_to_ref_line);

		pTempCell = db.GetCell(nBeginRow, 5);
		if (pTempCell) pTempCell->SetText(node.m_begin_layer);

		pTempCell = db.GetCell(nBeginRow, 6);
		if (pTempCell) pTempCell->SetText(node.m_end_layer);

		pTempCell = db.GetCell(nBeginRow, 7);
		if (pTempCell) pTempCell->SetText(node.m_transposed);

		pTempCell = db.GetCell(nBeginRow, 8);
		if (pTempCell) pTempCell->SetText(node.m_diameter);
	}

}

// 设置内部数据到一个骨架面中的所有控制点信息，内部不对表格进行检查，请保证
// 传过来的表格是控制点表，不对控制点表中控制点的弯钩合法性进行检查
void DataInner2PUFrameNodes_Bottom(CTHDataBase& db, const vector<frame_data_node>& dataInner)
{
	const int nBeginRow = 25;
	db.RemoveRowsAt(nBeginRow, 1, _T("底部控制点分组"));
	// 填充数据到控制点中

	OpReturnValue flagOp;
	for (int i = int(dataInner.size()) - 1; i >= 0 ; --i) // 从后往前填，i不能用size_t型，否则死循环
	{
		const frame_data_node &node = dataInner[i];
		CTHCell *pTempCell = NULL;

		// 在最前面插入一行弯钩行
		flagOp = db.InsertRowPost(nBeginRow -1, 1, _T("底部控制点分组"));// 控制点标题行上向后插入
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.InsertRowPost(...)异常"), false));
			return; // 表异常就没有必要再向后操作了，表结构异常，其它数据已经不可能正常插入了。
		}

		pTempCell = db.GetCell(nBeginRow, 3);
		if (pTempCell) pTempCell->SetText(node.m_ref_line);

		pTempCell = db.GetCell(nBeginRow, 4);
		if (pTempCell) pTempCell->SetText(node.m_dist_to_ref_line);

		pTempCell = db.GetCell(nBeginRow, 5);
		if (pTempCell) pTempCell->SetText(node.m_begin_layer);

		pTempCell = db.GetCell(nBeginRow, 6);
		if (pTempCell) pTempCell->SetText(node.m_end_layer);

		pTempCell = db.GetCell(nBeginRow, 7);
		if (pTempCell) pTempCell->SetText(node.m_transposed);

		pTempCell = db.GetCell(nBeginRow, 8);
		if (pTempCell) pTempCell->SetText(node.m_diameter);
	}

}

// 设置内部数据到全部骨架面的信息，请保证传过来的表是包含所有骨架面的钢
// 筋面表，请保证调用本函数之前保证表有足够的钢筋面，且每个钢筋面中的控制点个数
// 也满足要求，为满足这个条件，请调用InitFramesTable(...)对表进行处理，以生成
// 足够的钢筋面和控制点
void DataInner2PUFrames(CTHDataBase &DB, const vector<frame_data_piece> &dataInner)
{
	// 要获取子表之前必须调用它才能进行表数据的存储（整个表或子表获取的时候只需要在第一次调用就行）
	if (DB.GetSubDataBaseCount() <= 0) return;

	// 把所有的钢筋面表删除。
	// 【重要说明】：统计骨架面的时候注意要先获取整个骨架表再计算子表数，否则少一层获取子
	//               表操作得到的就是骨架大表的个数，不管骨架有多少个，这个骨架大表始终有
	//               一个，会导致程的逻辑错误。【Add By Wang Youzhou 2011-06-28】
	// 1.统计有几个钢筋子表
	CGridDataBase framesGrid;
	DB.GetSubDataBase(0, framesGrid); // 只有一个骨架大表，里边有很多的骨架表
	int nFramesCount = framesGrid.GetSubDataBaseCount();

	// 2.删除要对DB进行，对子表操作不会改变大的DB，因为子表是拷贝出来的。
	OpReturnValue flagOp;
	if (nFramesCount >= 1)
	{
		flagOp = DB.RemoveRowsAt(2, nFramesCount, _T("钢筋面定义分组")); // 没有第2行则会崩溃，即表格至少有2行
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.RemoveRowsAt(2, 1, ...)异常"), false));
			return;
		}
	}
	
	// 插入足够多的钢筋面，每一个钢筋面都插入足够多的控制，为了便于操作，逆序访问数组，
	// 并采用向前插入的方法。如果对效率要求较高，可以把第一个表提到循环外面，这样循环中
	// 就少一个if了，但一般钢筋面插入一般就两三个，没多少关系。
	if (dataInner.empty())return; // 允许钢筋面的个数为空
	for (int i = int(dataInner.size()) - 1; i >= 0; --i) // 注：千万不能把 i 定义为size_t无符号类型，会死循环
	{
		//////////////////////////////////////////////////////////////////////////
		// 【1】插入第一个钢筋面表，为了控制每个钢筋面表要插入多少个控制点，采用向前插入，并每次
		//      对第一个钢筋面表插入足够多的控制点这种方法最方便，但有个问题，第一个只能在标题行
		//      上进行插入，且标题行不能进行前向插入，所以第一个钢筋面表只能使用后向插入，完成之
		//      后再使用逆序循环向前插入足够多的钢筋面
		// 【2】插入钢筋面可以在第一行后面插入，保证新的钢筋面始终在第一个，但不能采用在第二行之
		//      前插入，因为当表为空的时候没有第二行，采用向第二行前面插入会导致程序崩溃。
		flagOp = DB.InsertRowPost(1, 1, _T("钢筋面定义分组"));

		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.InsertRowPost(...)异常"), false));
			return;
		}

		// 为避免插入数据后表的行数变化，导致数据定位不准确，最好先插入底部控制点，再插入
		// 顶部控制点，骨架控制信息不是动态增长的，放在哪插入数据都行。这样可以始终保证在
		// 表格的下标为25的行一定是底部点的第一行，下标为22的一定是顶部点的第一行，插入控
		// 制点就可以不用统计点的个数了，因为采用逆序插入，始终是插入到第一行。
		// 第18行为“控制点信息”，第19行是控制点表的标题;由于建表时的代码原因，可以保证在
		// 插入一个新的钢筋面后，这个新的钢筋面肯定为24行，由于始终插入到最前面，所以新插
		// 入的表起始行标一定是2，结束行标一定是25，且肯定有一个空白的顶部控制点，行标为
		// 22，肯定有一个空白的底部控制点，行标为25，明确知道这些信息后，控制点的插入才不
		// 会出错，且可以让代码更简洁。
		DataInner2PUFrameNodes_Bottom(DB, dataInner[i].m_bottom_nodes); // 必须先填写顶部，再填写底部，否则底部控制点起始行难以确定。
		DataInner2PUFrameNodes_Top(DB, dataInner[i].m_top_nodes);
		DataInner2PUFrameInfo(DB, dataInner[i]);// 往新插入的钢筋面表中填充控制信息
	} // end of for(i)
}

// 根据弯钩表中的数据，设置关联的单元格的属性（调用它的函数保证传入的DB有效）
void frame_grid_tools::refresh_hook_cells_status(CTHDataBase& db)
{
	// 把通用弯钩名称改为只读
	CTHCell* cell = db.GetCell(2, 1);
	if (cell) cell->SetReadOnly(TRUE);

	// 根据所填内容修改相关联的单元格的只读属性
	for (int i = 2; i < db.GetRowCount(); ++i)
	{
		// 获取弯钩类型单元格
		cell = db.GetCell(i, 2);
		if (!cell) continue;
		CString strText = cell->GetText();

		// 获取圆弧半径单元格
		cell = db.GetCell(i, 4);
		if (!cell) continue;
		if (_T("圆弧弯钩") == strText)
			cell->SetReadOnly(FALSE);
		else
			cell->SetReadOnly(TRUE);

		// 获取端点方向单元格
		cell = db.GetCell(i, 5);
		if (!cell) continue;
		strText = cell->GetText();

		// 获取夹角单元格
		cell = db.GetCell(i, 6);
		if (!cell) continue;
		if (_T("自定义角度") == strText)
			cell->SetReadOnly(FALSE);
		else
			cell->SetReadOnly(TRUE);

	} // end of for (int i = 2; i < DB.GetRowCount(); ++i)

}

// 设置内部数据到一个骨架面中的所有弯钩信息，内部不对表格进行检查，请保证传来的表格是弯钩表
void DataInner2PUHooks(CTHDataBase &DB, const frame_data& frames)
{
	int nRowCounts = DB.GetNumberRows();

	// 删除弯钩表中的所有行，包括通用弯钩
	DB.RemoveRowsAt(2, nRowCounts); // 2为通用弯钩所在行

	// 表格清理干净后，先整理传入的弯钩数据
	// 1.删除弯钩数组中所有名为“通用弯钩”的弯钩，因为这个弯钩有特殊的地方放置，不在数组中【出现这种情况是编程错误，断言之，不要向界面报错】
	// 2.检查有没有名称为空的弯钩，有则删除之【出现这种错误是编程错误，断言之，不要向界面报错】
	// 3.检查有没有重复的弯钩，有则删除第一个以后的全部同名弯钩【出现这种错误是编程错误，断言之，不要向界面报错】

	const vector<frame_data_hook>& hooks = frames.m_all_hooks;

	// 插入足够的弯钩行
	int nInsertRowsCount = int(hooks.size() + 1); // 标题行之后插入数据行，多插入一个通用弯钩的数据行
	if (nInsertRowsCount <= 0) // 数据过大，由size_t转换成int时溢出，变成一个负数
	{
		_ASSERTE((_T("表格行数异常"), false));
		return;
	}

	OpReturnValue opFlag = DB.InsertRowPost(1, nInsertRowsCount);
	if (OP_ERROR == opFlag.count)
	{
		_ASSERTE((_T("弯钩数据表行插入操作失败，无法正常完成表格数据的插入操作"), false ));
		return;
	}

	// 填写通用弯钩的信息
	const frame_data_hook& generalHook = frames.m_general_hook;
	{
		int nRow = 0;

		DB.Set_Prop(nRow, _T("m_strIndex"), CTHVariant(_T("通用弯钩")));
		DB.Set_Prop(nRow, _T("m_strHookType"), CTHVariant(generalHook.m_hook_type));
		DB.Set_Prop(nRow, _T("m_strHookL"), CTHVariant(generalHook.m_hook_length));
		DB.Set_Prop(nRow, _T("m_strArcR"), CTHVariant(generalHook.m_arc_radius));
		DB.Set_Prop(nRow, _T("m_strEndDirect"), CTHVariant(generalHook.m_hook_end_type));
		DB.Set_Prop(nRow, _T("m_strDirectAngle"), CTHVariant(generalHook.m_hook_end_angle));
		DB.Set_Prop(nRow, _T("m_strHookDirect"), CTHVariant(generalHook.m_hook_direction));
	}

	// 将弯钩数据插入到弯钩表格中
	for (int i = 1; i <= int(hooks.size()); ++i) // 从下标为1的行开始填入，下标为0的是通用弯钩。
	{
		DB.Set_Prop(i, _T("m_strIndex"), CTHVariant(hooks[i - 1].m_index_name));
		DB.Set_Prop(i, _T("m_strHookType"), CTHVariant(hooks[i - 1].m_hook_type));
		DB.Set_Prop(i, _T("m_strHookL"), CTHVariant(hooks[i - 1].m_hook_length));
		DB.Set_Prop(i, _T("m_strArcR"), CTHVariant(hooks[i - 1].m_arc_radius));
		DB.Set_Prop(i, _T("m_strEndDirect"), CTHVariant(hooks[i - 1].m_hook_end_type));
		DB.Set_Prop(i, _T("m_strDirectAngle"), CTHVariant(hooks[i - 1].m_hook_end_angle));
		DB.Set_Prop(i, _T("m_strHookDirect"), CTHVariant(hooks[i - 1].m_hook_direction));
	}
	//frame_grid_tools::refresh_hook_cells_status(DB);
}

// 向骨架表中添加一种格式修饰符，这种格式符专门用于指定弯钩信息二级表使用
void AddFormatInfoToDatabase(CTHDataBase& db, const CString& strFormatName)
{
	// 函数的步骤：1.定义一个修饰符对象
	//   2.按所需格式填充修饰符对象
	//   3.把修饰符对象添加到Database中
	CTHFormatCellInfo format;

	CString strFormat = _T("通用弯钩/(自定义);ZD1/KZ1/(自定义);T/B;LB1/(自定义);S/E"); // 修饰符
	CString strDefaulFormat = _T("通用弯钩;ZD1;T;LB1;S"); // 默认修饰符
	CString strMeansGroup = _T("弯钩名;所在控制点参考线;控制点位置;弯钩参考段;弯钩位置"); // 修饰符分组含义
	CString strMeansFormat = _T("ZD1(跨中线1);KZ1(支承线1);T(顶部点);B(底部点);LB1(顶缘左段);S(弯起层);E(弯止层)"); // 修饰符的含义

	format.m_strFormatIDName = FORMAT_HOOKS_SETTING_ID;
	format.m_nDefaultRowNum = 0;
	format.m_nRowNumFixed = false;
	format.m_strRowName = _T("");
	format.m_strValueAddType = _T("零值");
	format.m_strValueAddType2 = _T("零值");
	format.m_ValueAddType = CTHFormatCellInfo::ZERO_VALEU;
	format.m_strRowNameList.clear();
	format.m_ColList.clear(); // 其它的成员采用默认即可
	format.m_ColList.resize(1);
	CTHOneCellColumnInfo& colInfo = format.m_ColList[0];
	colInfo.m_strTitle = FORMAT_HOOKS_SETTING_NAME;
	colInfo.m_strVarType = _T("整型");
	colInfo.m_strPermitOrder = _T("否");
	colInfo.m_strVarPrecise = _T(".0");
	colInfo.m_strDefaultQualifer = strDefaulFormat;
	colInfo.m_strQualiferNameList = strFormat;
	colInfo.m_strQualiferNoteList = strMeansGroup;
	colInfo.m_strQualiferDetailNoteList = strMeansFormat;
	colInfo.m_strDefaultValue = _T("1");

	db.AddFormat(format); // 修改的时候用db.UpdateFormat(format);
}

// 创建上部属性表
void CreateFramesTable_Info(CTHDataBase * pDatabase, bool bEnableSteelType)
{
	int nSCapN = 2; // 控制信息标题合并的起始列号
	int nECapN = 3; // 控制信息标题合并的终止列号
	int nSCN = 4; // 属性表用户输入单无格合并的起始列号
	int nECN = 8; // 属性表用户输入单无格合并的终止列号
	int nRow = 0; // 当前操作所在行号
	CString strHintText; // 储存提示文字

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN - 1, nRow, nECN);
	CTHCell *pTempCell =pDatabase->GetCell(nRow, nSCapN - 1);
	pTempCell->SetFontID(FONT_TITLE);
	pTempCell->SetText(_T("骨架定义"));
	pTempCell->SetReadOnly(TRUE);
	// pTempCell->SetBKcolor(RGB(201, 220, 255));
	pDatabase->SetTitleFormat(nRow, nSCapN - 1, RT_SUBTITLE);

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("----- 控制信息 -----"));
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	//////////////////////////////////////////////////////////////////////////
	// 骨架增加一个变量用于记录和保存骨架编辑器中每一片骨架应用部位字符串，这
	// 个值不显示在界面上，仅用于与骨架编辑器交互，为了保证界面骨架表格中的数
	// 据修改后获取时能填充这个字符串，必须在界面表格中找个地方来放置这个数据
	// ，由于骨架表格结构复杂，新增行或删除行，会导致整个骨架表取数据时的行列
	// 号完全发生变化，分组及表结构也要变动，风险较大，所以在每个骨架的控制信
	// 息单元格的Label属性中保存这个值，取值和赋值时也作相应处理，默认为空
	// [Add By WYZ 2013年7月1日]
	//////////////////////////////////////////////////////////////////////////
	pTempCell->SetLabelText(_T(""));
	// pTempCell->SetBKcolor(RGB(201, 220, 255));
	pDatabase->SetTitleFormat(nRow, nSCapN, RT_SUBTITLE);

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("类型名称"));
	strHintText = _T("当前钢筋面的索引标识，不能与其它的钢筋面标识相同");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetTag(TAG_FRAME_NAME);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetText(_T("A"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("是否需要焊接"));
	strHintText = _T("勾选表示将钢筋面焊接为骨架面，否则表示钢筋面为非骨架面");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetCellType(CT_CHECKBOX);
	pTempCell->SetDataType(CELLDATA_INT);
	pTempCell->SetTag(TAG_IS_FRAME);
	pTempCell->SetText(_T("1")); // 默认为骨架面
	pTempCell->SetHintText(strHintText);

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("是否对称"));
	strHintText = _T("骨架钢筋面输入的控制点是否按对称方式输入，目前只能按非对称处理，以后再加功能");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetCellType(CT_CHECKBOX);
	pTempCell->SetDataType(CELLDATA_INT);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetReadOnly(TRUE); // 目前只能按非对称处理，以后再加功能

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("钢种号"));
	strHintText = _T("钢种号为 0 时表示由程序内部根据第一个直径值自动设置（直径小于10为1类，否则为2类）\n1、2、3、4分别对应设计零件中相应的钢筋类型");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_INT);
	pTempCell->SetLabelText(_T("0\n1\n2\n3\n4"));
	pTempCell->SetText(_T("0"));
	pTempCell->SetCellType(CT_DROPLIST);
	pTempCell->SetHintText(strHintText);
	// 有些构件的钢种号在使用骨架的钢筋表大样中指定，所以加开关设置
	if (!bEnableSteelType)
	{
		pTempCell->SetReadOnly(TRUE);
		pDatabase->SetRowHeight(nRow, 0);
	}
	else
	{
		pTempCell->SetReadOnly(FALSE);
	}

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("钢筋直径(mm)"));
	strHintText = _T("钢筋直径(mm)，可填入多个直径，数字之间用半角空格、逗号或分号相隔，如果直径为负则并置钢筋");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetText(_T("28"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("端部弯钩焊缝长(mm)"));
	strHintText = _T("钢筋面中左、右两端的钢筋布置时，程序内部为避免钢筋与外框架冲突，可能会对两端\n");
	strHintText += _T("的钢筋及弯钩形状进行智能设计，这需要用户指定弯钩与其它钢筋焊接的焊缝长度。\n");
	strHintText += _T("当数据绝对值>=0且<=20时，焊缝长为: 数据绝对值*钢筋直径，否则直接将所填数据作为焊缝长。");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_DOUBLE);
	pTempCell->SetText(_T("140"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--D#15990"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("顶部层间距(mm)"));
	strHintText = _T("顶部各层的层间距(mm)，数字之间用一个半角逗号隔开，如：12,34.5,67,8");
	strHintText += _T("\n数字的个数需等于钢筋面中顶部钢筋的层数（不含顶缘线）");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_ARRAY | CELLDATA_DOUBLE);
	pTempCell->SetText(_T("0"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--D#15989"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("底部层间距(mm)"));
	strHintText = _T("底部各层的层间距，数字之间用一个半角逗号隔开，如：12,34.5,67,8");
	strHintText += _T("\n数字的个数需等于钢筋面中底部钢筋的层数（不含底缘线）");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_ARRAY | CELLDATA_DOUBLE);
	pTempCell->SetText(_T("0"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--D#15988"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("顶部顺轮廓线"));
	strHintText = _T("顶部各层是否顺顶部轮廓线，1为是，0为否，数字之间用一个半角逗号隔开，如：1,1,0,1");
	strHintText += _T("\n数字的个数需等于钢筋面中顶部钢筋的层数（不含顶缘线），个数不够时程序按最后一个数值补齐。");
	strHintText += _T("\n第一个数字对应第一层，第二个对应第二层，依次类推。");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_ARRAY | CELLDATA_INT);
	pTempCell->SetText(_T("1"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-顶底部顺轮廓线#99999"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("底部顺轮廓线"));
	strHintText = _T("底部各层是否顺底部轮廓线，1为是，0为否，数字之间用一个半角逗号隔开，如：1,1,0,1");
	strHintText += _T("\n数字的个数需等于钢筋面中底部钢筋的层数（不含底缘线）");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_ARRAY | CELLDATA_INT);
	pTempCell->SetText(_T("1"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-顶底部顺轮廓线#99999"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("斜筋弯起角(度)"));
	strHintText = _T("钢筋面中的斜筋向上弯起时锐角的角度，单位：角度");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_DOUBLE);
	pTempCell->SetText(_T("45"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--D#15985"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("圆弧筋半径(mm)"));
	strHintText = _T("圆弧筋的圆弧过渡半径");
	strHintText += _T("当数据绝对值>=0且<=20时，圆弧筋半径为: 数据绝对值*钢筋直径，否则直接将所填数据作为圆弧筋半径。");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_DOUBLE);
	pTempCell->SetText(_T("120"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--D#15984"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("中折点圆弧半径(mm)"));
	strHintText = _T("中间折点指除焊接到钢筋面左、右两端钢筋以外的\n");
	strHintText += _T("钢筋弯折点，比如单斜筋、圆弧筋的弯钩弯折点等");
	strHintText += _T("当数据绝对值>=0且<=20时，圆弧半径为: 数据绝对值*钢筋直径，否则直接将所填数据作为圆弧半径。");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_DOUBLE);
	pTempCell->SetText(_T("140"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--D#15983"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("中间弯折点焊缝长(mm)"));
	strHintText = _T("中间折点指除焊接到钢筋面左、右两端钢筋以外的\n");
	strHintText += _T("钢筋弯折点，比如单斜筋、圆弧筋的弯钩弯折点等");
	strHintText += _T("当数据绝对值>=0且<=20时，焊缝长为: 数据绝对值*钢筋直径，否则直接将所填数据作为焊缝长。");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetDataType(CELLDATA_DOUBLE);
	pTempCell->SetText(_T("140"));
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--D#15982"));

	++nRow;
	// 钢筋面设计思想属性
	// 注：以下两个单元格只有当钢筋面为非骨架面时设置才生效，为骨架面时不生效
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("是否有顶层通长筋"));
	strHintText = _T("只有当钢筋面为非骨架面时设置才生效，为骨架面时不生效");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetTag(TAG_TOP);
	pTempCell->SetCellType(CT_CHECKBOX);
	pTempCell->SetDataType(CELLDATA_DOUBLE);
	pTempCell->SetText(_T("1")); //默认勾选
	pTempCell->SetReadOnly(TRUE); // 默认建表时为骨架面，所以此处只读
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--A#99999"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("是否有底层通长筋"));
	strHintText = _T("只有当钢筋面为非骨架面时设置才生效，为骨架面时不生效");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetTag(TAG_BOT);
	pTempCell->SetCellType(CT_CHECKBOX);
	pTempCell->SetDataType(CELLDATA_DOUBLE);
	pTempCell->SetText(_T("1")); // 默认勾选
	pTempCell->SetReadOnly(TRUE); // 默认建表时为骨架面，所以此处只读
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THBW-PM-001-A01--A#99999"));

	++nRow;
	pDatabase->MergeCells(nRow, nSCapN, nRow, nECapN);
	pTempCell = pDatabase->GetCell(nRow, nSCapN);
	pTempCell->SetText(_T("指定控制点的弯钩"));
	strHintText = _T("指定控制点的弯钩信息，指定的弯钩信息仅在钢筋布置时与骨架左、右两端外框架筋打架时才有用。");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pDatabase->MergeCells(nRow, nSCN, nRow, nECN);
	pTempCell = pDatabase->GetCell(nRow, nSCN);
	pTempCell->SetCellType(CT_BUTTON);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetReadOnly(FALSE);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetTag(TAG_HOOKS_SETTING);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-控制点弯钩#99999"));
	// 二级表中获取到的hooksStetting字符串行之间用'\x0D'分隔，列之间用'\x1F'
	// CString strTmpText = _T("\x0D\x0D\x0D\x0D\x0D"); // 列不够6个元素， 界面会自动补齐
	// pTempCell->SetText(strTmpText); // 创建表格的时候为二级表指定一些空行
	// 这种控件不能用SetText更改文本，算法的机制是如果表为空，则显示的时候插入几行空行数据

}

// 创建顶部控制点表，通过参数指定顶底缘参考线名以及所有的竖向参考线名
// bT为true表示启用界面钢筋反置参数，否则不启用
// bD为true表示启用控制点直径，否则不启用
void CreateFramesTable_TopNodes(CTHDataBase * pDatabase, bool bT, bool bD)
{
	CString strHintText;

	int nSRN = 20; // 控制点表所在的起始行号
	pDatabase->MergeCells(nSRN, 2, nSRN, 8);
	CTHCell *pTempCell = pDatabase->GetCell(nSRN, 2);
	pTempCell->SetText(_T("----- 顶部控制点 -----"));
	strHintText = _T("所有的顶部控制点数据必须全部填写完整才生效，否则当作无效点处理\n弯起点在顶部定义为顶部控制点");
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pTempCell->SetHintText(strHintText);
	// pTempCell->SetBKcolor(RGB(201, 220, 255));
	pDatabase->SetTitleFormat(nSRN, 2, RT_SUBTITLE);


	// 顶部控制点
	pTempCell = pDatabase->GetCell(nSRN + 1, 2);
	pTempCell->SetText(_T("编号"));
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 2);
	pTempCell->SetDataType(CELLDATA_STRING);

	pTempCell = pDatabase->GetCell(nSRN + 1, 3);
	pTempCell->SetText(_T("参考线"));
	strHintText = _T("控制点的参考线");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 3);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetTag(TAG_REF_LINES_TOP);
	// pTempCell->SetCellType(CT_DROPLIST);
	pTempCell->SetCellType(CT_DROPDOWN); // 支持手动输入
	pTempCell->SetHintText(strHintText);
	pTempCell->SetText(_T(""));
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-控制点#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 4);
	pTempCell->SetText(_T("间距布置(mm)"));
	strHintText = _T("从左到右依次输入间距值（水平坐标差），参考线位置用0表示。\n");
	strHintText += _T("多个数据用空格或半角逗号隔开，所有数据中必须有数字0出现，\n");
	strHintText += _T("如果数据中没有0，将自动按0在最右边的情况进行处理。\n");
	strHintText += _T("数据格式支持乘号表达式，如300*2 200 500*10 0 1000\n");
	strHintText += _T("例1: 300 0表示参考线左边有一个控制点，距离参考线300mm\n");
	strHintText += _T("例2: 0 300表示参考线右边有一个控制点，距离参考线300mm\n");
	strHintText += _T("例3: 300 200 0 100 表示参考线左边有两个控制点，右边有一个点，距离参考线300mm\n");
	strHintText += _T("例4: 0 0 表示参考线上有一个控制点\n");
	strHintText += _T("例5: 100 0 0 200 表示参考线左边有一个点，右边有一个点，还有一个点在参考线上\n");
	strHintText += _T("例6: 100 将按照 100 0 一样的含义进行处理，0 将按照 0 0 处理。\n");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 4);
	pTempCell->SetDataType(CELLDATA_STRING); // CELLDATA_DOUBLE | CELLDATA_ARRAY 类型不支持100*3这类表达式的界面显示
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-控制点#15976"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 5);
	pTempCell->SetText(_T("弯起点层号"));
	strHintText = _T("n = 0: 表示端点；\n");
	strHintText += _T("n > 0: 表示从第n行起弯；\n");
	strHintText += _T("n < 0: 表示从第|n|行截断；\n");
	strHintText += _T("层数从上往下依次增大，且只能为整数；\n");
	strHintText += _T("组成圆弧筋的两个控制点要求参考线相同，距参考线距离的数值相同；\n");
	strHintText += _T("斜筋弯起点应为上部点，弯止点应为下部点；\n当斜筋与左、右两端端部钢筋打架时，弯起点为下部点，弯止点为上部点；\n");
	strHintText += _T("圆弧筋弯起点应为下部点，弯止点应为上部接圆弧处的点；\n");
	strHintText += _T("局部截断筋弯起、弯止层均为负数，所在层数为其绝对值；\n");
	strHintText += _T("弯起层可以输入多个，原则个数及顺序应和间距相对应，如果过少，则按最后一个数据补齐(支持乘号表达式)");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 5);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-弯起弯止#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 6);
	pTempCell->SetText(_T("弯止点层号"));
	strHintText = _T("n = 0: 表示端点；\n");
	strHintText += _T("n > 0: 表示从第n行起弯；\n");
	strHintText += _T("n < 0: 表示与另一个相关控制点组成圆弧筋；\n");
	strHintText += _T("层数从上往下依次增大，且只能为整数；\n");
	strHintText += _T("组成圆弧筋的两个控制点要求参考线相同，距参考线距离的数值相同；\n");
	strHintText += _T("斜筋弯起点应为上部点，弯止点应为下部点；\n当斜筋与左、右两端端部钢筋打架时，弯起点为下部点，弯止点为上部点；\n");
	strHintText += _T("圆弧筋弯起点应为下部点，弯止点应为上部接圆弧处的点；\n");
	strHintText += _T("局部截断筋弯起、弯止层均为负数，所在层数为其绝对值；\n");
	strHintText += _T("弯止层可以输入多个，原则个数及顺序应和间距相对应，如果过少，则按最后一个数据补齐(支持乘号表达式)");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 6);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-弯起弯止#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 7);
	pTempCell->SetText(_T("是否反置"));
	strHintText = _T("设置钢筋弯起方向反置\n");
	strHintText += _T("0表示不反置，1表示反置，非法数据按0处理\n");
	strHintText += _T("可输入多个值，数值之间可用逗号、空格、分号相隔，且支持支持乘号表达式\n");
	strHintText += _T("数值与间距布置中的数据一一对应，数据过少将按最后一个补齐，过多则忽略");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 7);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetReadOnly(bT ? FALSE : TRUE);
	// pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-弯起弯止#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 8);
	pTempCell->SetText(_T("直径(mm)"));
	strHintText = _T("设置控制点所在钢筋的直径\n");
	strHintText += _T("可输入多个值，数值之间可用逗号、空格、分号相隔，且支持支持乘号表达式。\n");
	strHintText += _T("1.为0或空按填1处理。\n");
	strHintText += _T("2.为负表示按直径的绝对值进行钢筋并置。\n");
	strHintText += _T("3.为正表示钢筋直径。\n");
	strHintText += _T("4.数字之前带了“\\”则表示骨架信息中所填的直径的序号（从1开始）。\n");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 8);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetReadOnly(bD ? FALSE : TRUE);
}

// 创建底部控制点表，通过参数指定顶底缘参考线名以及所有的竖向参考线名
// bT为true表示启用反置参数，否则不启用
// bD为true表示启用控制点直径，否则不启用
void CreateFramesTable_BotNodes(CTHDataBase * pDatabase, bool bT, bool bD)
{
	CString strHintText;

	int nSRN = 23; // 控制点表所在的起始行号
	pDatabase->MergeCells(nSRN, 2, nSRN, 8);
	CTHCell *pTempCell = pDatabase->GetCell(nSRN, 2);
	pTempCell->SetText(_T("----- 底部控制点 -----"));
	strHintText = _T("所有的底部控制点数据必须全部填写完整才生效，否则当作无效点处理\n弯起点在底部定义为底部控制点");
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	pTempCell->SetReadOnly(TRUE);
	pTempCell->SetHintText(strHintText);
	// pTempCell->SetBKcolor(RGB(201, 220, 255));
	pDatabase->SetTitleFormat(nSRN, 2, RT_SUBTITLE);

	// 底部控制点
	pTempCell = pDatabase->GetCell(nSRN + 1, 2);
	pTempCell->SetText(_T("编号"));
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 2);
	pTempCell->SetDataType(CELLDATA_STRING);

	pTempCell = pDatabase->GetCell(nSRN + 1, 3);
	pTempCell->SetText(_T("参考线"));
	strHintText = _T("控制点的参考线");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 3);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetTag(TAG_REF_LINES_BOT);
	pTempCell->SetCellType(CT_DROPDOWN); // 支持手动输入
	pTempCell->SetHintText(strHintText);
	pTempCell->SetText(_T(""));
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-控制点#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 4);
	pTempCell->SetText(_T("间距布置(mm)"));
	pTempCell->SetText(_T("间距布置(mm)"));
	strHintText = _T("从左到右依次输入间距值（水平坐标差），参考线位置用0表示。\n");
	strHintText += _T("多个数据用空格或半角逗号隔开，所有数据中必须有数字0出现，\n");
	strHintText += _T("如果数据中没有0，将自动按0在最右边的情况进行处理。\n");
	strHintText += _T("数据格式支持乘号表达式，如300*2 200 500*10 0 1000\n");
	strHintText += _T("例1: 300 0表示参考线左边有一个控制点，距离参考线300mm\n");
	strHintText += _T("例2: 0 300表示参考线右边有一个控制点，距离参考线300mm\n");
	strHintText += _T("例3: 300 200 0 100 表示参考线左边有两个控制点，右边有一个点，距离参考线300mm\n");
	strHintText += _T("例4: 0 0 表示参考线上有一个控制点\n");
	strHintText += _T("例5: 100 0 0 200 表示参考线左边有一个点，右边有一个点，还有一个点在参考线上\n");
	strHintText += _T("例6: 100 将按照 100 0 一样的含义进行处理，0 将按照 0 0 处理。\n");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 4);
	pTempCell->SetDataType(CELLDATA_STRING); // CELLDATA_DOUBLE | CELLDATA_ARRAY 类型不支持100*3这类表达式的界面显示
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-控制点#15976"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 5);
	pTempCell->SetText(_T("弯起点层号"));
	strHintText = _T("n = 0: 表示端点；\n");
	strHintText += _T("n > 0: 表示从第n行起弯；\n");
	strHintText += _T("n < 0: 表示从第|n|行截断；\n");
	strHintText += _T("层数从下往上依次增大，且只能为整数；\n");
	strHintText += _T("组成圆弧筋的两个控制点要求参考线相同，距参考线距离的数值相同；\n");
	strHintText += _T("斜筋弯起点应为上部点，弯止点应为下部点；\n当斜筋与左、右两端端部钢筋打架时，弯起点为下部点，弯止点为上部点；\n");
	strHintText += _T("圆弧筋弯起点应为下部点，弯止点应为上部接圆弧处的点；\n");
	strHintText += _T("局部截断筋弯起、弯止层均为负数，所在层数为其绝对值；\n");
	strHintText += _T("弯起层可以输入多个，原则个数及顺序应和间距相对应，如果过少，则按最后一个数据补齐(支持乘号表达式)");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 5);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-弯起弯止#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 6);
	pTempCell->SetText(_T("弯止点层号"));
	strHintText = _T("n = 0: 表示端点；\n");
	strHintText += _T("n > 0: 表示从第n行起弯；\n");
	strHintText += _T("n < 0: 表示与另一个相关控制点组成圆弧筋；\n");
	strHintText += _T("层数从下往上依次增大，且只能为整数；\n");
	strHintText += _T("组成圆弧筋的两个控制点要求参考线相同，距参考线距离的数值相同；\n");
	strHintText += _T("斜筋弯起点应为上部点，弯止点应为下部点；\n当斜筋与左、右两端端部钢筋打架时，弯起点为下部点，弯止点为上部点；\n");
	strHintText += _T("圆弧筋弯起点应为下部点，弯止点应为上部接圆弧处的点；\n");
	strHintText += _T("局部截断筋弯起、弯止层均为负数，所在层数为其绝对值；\n");
	strHintText += _T("弯止层可以输入多个，原则个数及顺序应和间距相对应，如果过少，则按最后一个数据补齐(支持乘号表达式)");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 6);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-弯起弯止#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 7);
	pTempCell->SetText(_T("是否反置"));
	strHintText = _T("设置钢筋弯起方向反置\n");
	strHintText += _T("0表示不反置，1表示反置，非法数据按0处理\n");
	strHintText += _T("可输入多个值，数值之间可用逗号、空格、分号相隔，且支持支持乘号表达式\n");
	strHintText += _T("数值与间距布置中的数据一一对应，数据过少将按最后一个补齐，过多则忽略");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 7);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetReadOnly(bT ? FALSE : TRUE);
	// pTempCell->SetSketchMapName(_T("THGW-盖梁骨架-弯起弯止#99999"));

	pTempCell = pDatabase->GetCell(nSRN + 1, 8);
	pTempCell->SetText(_T("直径(mm)"));
	strHintText = _T("设置控制点所在钢筋的直径\n");
	strHintText += _T("可输入多个值，数值之间可用逗号、空格、分号相隔，且支持支持乘号表达式。\n");
	strHintText += _T("1.为0或空按填1处理。\n");
	strHintText += _T("2.为负表示按直径的绝对值进行钢筋并置。\n");
	strHintText += _T("3.为正表示钢筋直径。\n");
	strHintText += _T("4.数字之前带了“\\”则表示骨架信息中所填的直径的序号（从1开始）。\n");
	pTempCell->SetHintText(strHintText);
	pTempCell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR); // 水平居中 | 垂直居中 | 文字水平
	pTempCell->SetReadOnly(TRUE);
	pTempCell = pDatabase->GetCell(nSRN + 2, 8);
	pTempCell->SetDataType(CELLDATA_STRING);
	pTempCell->SetHintText(strHintText);
	pTempCell->SetReadOnly(bD ? FALSE : TRUE);

}

// 重新初始化钢筋面表为指定的行数和列数，如果原来表中有数据，则全部清除。
// Parameter: const std::vector<int> & vecRowNum
//   每一个钢筋面中的子表行数，vector的size为钢筋面的个数，
//   内部的元素值为对应的钢筋面中的控制点的预留行数，每一个值同时对顶部及底部点生效，例如值
//   为3表示顶部点有三行，底部点也有三行。
void InitFramesTableByDatabase(IN CTHDataBase &DB, IN const std::vector<int> &vecRowNum )
{
	if (vecRowNum.empty())
	{
		_ASSERTE((_T("控制钢筋面个数的数组不能为空"), false));
		return;
	}

	if (DB.GetSubDataBaseCount() <= 0) return; // 要获取子表之前必须调用它才能进行表数据的存储（整个表或子表获取的时候只需要在第一次调用就行）

	OpReturnValue flagOp;
	// 把所有的钢筋面表删除，且仅留一个表
	// 全部清空，最后只剩表的标题行，一个钢筋表最少有19行，只要行数大于2就说明还没有全
	// 部删除干净，为安全起见，这里判断行数用个整数10吧
	while (DB.GetRowCount() >= 10)
	{
		// 插入钢筋面可以在第一行上插入，删除钢筋面则不行，必须从第二行开始。第一
		// 行为“钢筋面信息”，第二行为“钢筋面公共信息”，无论有多少个钢筋面，第一行
		// 只有一个，而第二行则每个钢筋面有一行。
		flagOp = DB.RemoveRowsAt(2, 1, _T("钢筋面定义分组")); // 没有第2行则会崩溃，即表格至少有2行
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.RemoveRowsAt(2, 1, ...)异常"), false));
			return;
		}
	}

	// 插入足够多的钢筋面，每一个钢筋面都插入足够多的控制，为了便于操作，逆序访问数组，
	// 并采用向前插入的方法。如果对效率要求较高，可以把第一个表提到循环外面，这样循环中
	// 就少一个if了，但一般钢筋面插入一般就两三个，没多少关系。
	for (int i = int(vecRowNum.size()) - 1; i >= 0; --i) // 注：千万不能把 i 定义为size_t无符号类型，会死循环
	{
		// 【1】插入第一个钢筋面表，为了控制每个钢筋面表要插入多少个控制点，采用向前插入，并每次
		//      对第一个钢筋面表插入足够多的控制点这种方法最方便，但有个问题，第一个只能在标题行
		//      上进行插入，且标题行不能进行前向插入，所以第一个钢筋面表只能使用后向插入，完成之
		//      后再使用逆序循环向前插入足够多的钢筋面
		// 【2】插入钢筋面可以在第一行上插入，删除钢筋面则不行，必须从第二行开始。第一
		//      行为“钢筋面信息”，第二行为“钢筋面公共信息”，无论有多少个钢筋面，第一行
		//      只有一个，而第二行则每个钢筋面有一行。
		if (i == int(vecRowNum.size()) - 1)
		{
			flagOp = DB.InsertRowPost(1, 1, _T("钢筋面定义分组"));
		}
		else
		{
			flagOp = DB.InsertRowPre(2, 1, _T("钢筋面定义分组")); // 每次在最前面插入一个表，如果插入多个表不太方便对控制点进行插入
		}
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.InsertRowPost(...)异常"), false));
			return;
		}

		// 第22行为“顶部控制点信息”，第21行是顶部控制点表的标题;插入一个新的钢筋面时已经有一个控制点行了。
		if (vecRowNum[i] <= 1) continue;

		// 第25行为“底部控制点信息”，第24行是底部控制点表的标题;插入一个新的钢筋面时已经有一个控制点行了。
		// 顶部点和底部点的数目一样多，此处必须先插入底部点，否则无法定位底部点了
		flagOp = DB.InsertRowPost(24, vecRowNum[i] - 1, _T("底部控制点分组"));// 控制点标题行上只能向后插入
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.InsertRowPost(24, vecRowNum[i] - 1, ...)异常"), false));
			return;
		}

		flagOp = DB.InsertRowPost(21, vecRowNum[i] - 1, _T("顶部控制点分组"));// 控制点标题行上只能向后插入
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = db.InsertRowPost(21, vecRowNum[i] - 1, ...)异常"), false));
			return;
		}

	} // end of for(i)

	return;

}

// 每一个钢筋面中的子表行数，vector的size为钢筋面的个数，内部的元素值为对应的钢筋面中的控制点的预留行数
// bT为true表示启用反置参数，否则不启用
// bD为true表示启用控制点直径，否则不启用
// bEnableSteelType为true表示启用钢种号，否则不启用
CTHDataBase *CreateFramesTable( const std::vector<int> &vecRowNum, bool bT, bool bD, bool bEnableSteelType)
{
	CTHDataBase* pDatabase = new CTHDataBase();

	// 设置模板格式
	// 模板名相关的接口为受保护的，无法设置，只能通过设置标记的方式来识别是否是骨架表
	pDatabase->SetTag(0, 0, _T("ThisIsFrameDB")); // 不要设置行标签，因为怕与行组信息干扰
	pDatabase->SetRowCount(26);
	pDatabase->SetColCount(9); // 列数一旦变化，则FrameInfo和FrameNodes中也需要变化
	pDatabase->SetFixedRowCount(1);
	pDatabase->SetFixedColCount(1);

	pDatabase->EnableRowAutoSerialNumber(TRUE);
	pDatabase->EnableColAutoSerialNumber(TRUE);
	pDatabase->EnableMergeMenu(TRUE);
	pDatabase->EnableColMenu(FALSE);
	pDatabase->EnableRowGroup(TRUE);

	// 创建表格
	CreateFramesTable_Info(pDatabase, bEnableSteelType); // 上部属性表
	CreateFramesTable_TopNodes(pDatabase, bT, bD); // 下部控制点
	CreateFramesTable_BotNodes(pDatabase, bT, bD); // 下部控制点

	// 行高为0则表示隐藏行，列宽为0表示隐藏列
	pDatabase->EnableRowResize(TRUE);
	pDatabase->SetRowHeight(0, 0);
	pDatabase->SetColWidth(0, 0);
	pDatabase->SetColWidth(1, 30);
	pDatabase->SetColWidth(2, 70);
	pDatabase->SetColWidth(3, 70);
	pDatabase->SetColWidth(4, 175);
	pDatabase->SetColWidth(5, 70);
	pDatabase->SetColWidth(6, 70);
	if (bT)
		pDatabase->SetColWidth(7, 70);
	else
		pDatabase->SetColWidth(7, 0);
	if (bD)
		pDatabase->SetColWidth(8, 70);
	else
		pDatabase->SetColWidth(8, 0);
	pDatabase->EnableRowResize(FALSE);

	// 设置表格分组
	// 第一个分组注释了，原因是会导致钢筋面的标题变成数字序号，顶部、底部控制点中间的分隔行
	// 不放到控制点的分组中是因为动态表需要以标题行为基点向后增长行数，所以标题行不能为中间
	// 的分隔行，只能把分隔行单独定义成一个静态行组。
	pDatabase->SetAsRowGroup(2, 20, 1, CString(_T("控制信息分组")), FALSE); // 为方便管理表格，第20行“顶部控制点”封装到了本组中
	pDatabase->SetAsRowGroup(22, 22, 2, CString(_T("顶部控制点分组")), TRUE, 21);
	pDatabase->SetAsRowGroup(23, 23, 2, CString(_T("中间分隔的一行")), FALSE);
	pDatabase->SetAsRowGroup(25, 25, 2, CString(_T("底部控制点分组")), TRUE, 24);
	pDatabase->SetAsRowGroup(2, 25, 1, CString(_T("钢筋面定义分组")), TRUE, 1);

	pDatabase->SetTitleFormat(1, 0, RT_TABLETITLE); // 一级标题的设置要表格创建好后设置，否则容易因设置其它信息导致这个设置失效

	// 分组结束之后，就认为是模板创建结束
	//////////////////////////////////////////////////////////////////////////

	InitFramesTableByDatabase(*pDatabase, vecRowNum); // 表格创建完成后按指定的行、列数初始化表

	return pDatabase;
}

// 所有创建表的函数最终都是调用这个函数实现的
CTHDataBase *frame_grid_tools::create_frame_grid( const std::vector<int> &vecRowNum, bool bT, bool bD, bool bEnableSteelType)
{
	return CreateFramesTable(vecRowNum, bT, bD, bEnableSteelType);
}

CTHDataBase *frame_grid_tools::create_frame_grid( bool bT, bool bD, bool bEnableSteelType)
{
	vector<int> vecRowNum;
	vecRowNum.clear();
	vecRowNum.push_back(1);
	return create_frame_grid(vecRowNum, bT, bD, bEnableSteelType);
}


void frame_grid_tools::get_frame_data_from_db(CTHDataBase& DBFrame, CTHDataBase& DBHooks, frame_data& P0 )
{
	// 外部人员自己保证传入的表格数满足要求，如行列无误
	CGridDataBase pGridFrames;
	if (DBFrame.GetSubDataBaseCount() <= 0) return; // 要获取子表之前必须调用它才能进行表数据的存储（整个表或子表获取的时候只需要在第一次调用就行）
	DBFrame.GetSubDataBase(0, pGridFrames);
	PU2DataInnerFrames(pGridFrames, P0.m_all_frames);

	int nRowsCount = DBHooks.GetRowCount();
	if (nRowsCount <= 2) return; // 弯钩表不是通用表，大的DB下面只有一个子表为整个弯钩表，不用获取子表来准备数据，标题行、表头共2行
	PU2DataInnerHooks(DBHooks, P0);
}

void frame_grid_tools::set_frame_data_to_db(CTHDataBase& frame_db, CTHDataBase& hook_db, const frame_data& data)
{
	DataInner2PUHooks(hook_db, data);
	DataInner2PUFrames(frame_db, data.m_all_frames);
}

void frame_grid_tools::should_pop_frame_sub_grid(dialog_node_hook_setting& dlg, CTHDataBase* db, NM_THGRID* notify_struct)
{
	if (!db) return;
	if (!notify_struct) return;

	int clicked_row = notify_struct->rowNo;
	int clicked_col = notify_struct->colNo;
	
	CString cell_tag = db->GetCell(clicked_row, clicked_col)->GetTag();

	if (TAG_HOOKS_SETTING == cell_tag)
	{
		CTHCell* cell = db->GetCell(clicked_row, clicked_col);
		dlg.m_setting_info_text = (cell ? cell->GetText() : _T(""));
		if (IDOK == dlg.DoModal())
		{
			db->GetExcel()->GetActiveCellType()->GetInpalceWnd()->SetWindowText(dlg.m_setting_info_text);
			db->GetExcel()->DeActiveCell();
		}
	}
}

void frame_grid_tools::initlize_hooks_grid( CTHDataBase& hook_db )
{
	hook_db.SetDefRowHeight(22); // 保证表与界面中其它的表一样高

	// 外部人员自己保证传入的表格数满足要求，如行列无误
	int nRowCounts = hook_db.GetNumberRows();

	// 删除弯钩表中的所有行，包括通用弯钩
	hook_db.RemoveRowsAt(2, nRowCounts); // 2为通用弯钩所在行

	// 表格清理干净后，先整理传入的弯钩数据
	// 1.删除弯钩数组中所有名为“通用弯钩”的弯钩，因为这个弯钩有特殊的地方放置，不在数组中【出现这种情况是编程错误，断言之，不要向界面报错】
	// 2.检查有没有名称为空的弯钩，有则删除之【出现这种错误是编程错误，断言之，不要向界面报错】
	// 3.检查有没有重复的弯钩，有则删除第一个以后的全部同名弯钩【出现这种错误是编程错误，断言之，不要向界面报错】


	// 默认插入2行弯钩
	const int nInsertRowsCount = 2;
	OpReturnValue opFlag = hook_db.InsertRowPost(1, nInsertRowsCount + 1); // 标题行之后插入数据行，多插入一个通用弯钩的数据行
	if (OP_ERROR == opFlag.count)
	{
		_ASSERTE((_T("弯钩数据表行插入操作失败，无法正常完成表格数据的插入操作"), false));
		return;
	}

	// 填写通用弯钩的信息
	{
		const int general_hook_row_index = 0;

		hook_db.Set_Prop(general_hook_row_index, _T("m_index_name"), CTHVariant(_T("通用弯钩")));
		hook_db.Set_Prop(general_hook_row_index, _T("m_hook_type"), CTHVariant(_T("直线弯钩")));
		hook_db.Set_Prop(general_hook_row_index, _T("m_hook_length"), CTHVariant(_T("200")));
		hook_db.Set_Prop(general_hook_row_index, _T("m_arc_radius"), CTHVariant(_T("190")));
		hook_db.Set_Prop(general_hook_row_index, _T("m_hook_direction"), CTHVariant(_T("向外")));
		hook_db.Set_Prop(general_hook_row_index, _T("m_hook_end_type"), CTHVariant(_T("垂直方向")));
		hook_db.Set_Prop(general_hook_row_index, _T("m_strDirectAngle"), CTHVariant(_T("0")));


		// 把弯钩名称改为只读
		CTHCell * pCell = hook_db.GetCell(2, 1);
		if (NULL == pCell)
		{
			_ASSERTE((_T("CTHCell * pCell = hook_db.GetCell(...);为空"), false));
		}
		else
		{
			pCell->SetReadOnly(TRUE);
			pCell->SetHintText(_T("通用弯钩不能删除，必须保留，如果删除了，重新打开会伪造一个并显示"));
		}
	}

	for (int i = 1; i <= nInsertRowsCount; ++i)
	{
		hook_db.Set_Prop(i, _T("m_index_name"), CTHVariant(_T("")));
		hook_db.Set_Prop(i, _T("m_hook_type"), CTHVariant(_T("")));
		hook_db.Set_Prop(i, _T("m_hook_length"), CTHVariant(_T("")));
		hook_db.Set_Prop(i, _T("m_arc_radius"), CTHVariant(_T("")));
		hook_db.Set_Prop(i, _T("m_hook_direction"), CTHVariant(_T("")));
		hook_db.Set_Prop(i, _T("m_hook_end_type"), CTHVariant(_T("")));
		hook_db.Set_Prop(i, _T("m_strDirectAngle"), CTHVariant(_T("")));
	}
}
