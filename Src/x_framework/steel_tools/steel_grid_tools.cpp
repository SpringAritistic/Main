#include "StdAfx.h"
#include <map>
#include <memory>

#include "THGridControl/THGridControlInc.h"
#include "THGridControl\THBaseExcel.h"

#include "../tools/table_templates.h"
#include "../tools/math_tools.h"
#include "../interface/console_interface.h"

#include "steel_data.h"
#include "steel_grid_format.h"
#include "steel_grid_condition.h"

#include "steel_grid_tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace ZYH_GRID;


#define OP_ERROR -1 // OpReturnValue错误时返回的受影响行数为-1;

// 宏定义 环境变量（到目前为止，所有的环境变量仅用于下拉列表中的项目）
#define ENV_HUN_NING_TU				(_T("@HNTLX"))		// 混凝土类型
#define ENV_SHENG_CE_GUAN			(_T("@SHCGLX"))		// 声测管类型
#define ENV_GANG_JIAO_XIAN			(_T("@GJXLX"))		// 钢绞线类型(即交通部规范预应力钢束)
#define ENV_YING_YONG_BU_WEI 		(_T("@YYBW"))		// 应用部位下拉选项（内部使用，不对外公开）

// 宏定义 特殊字符串
#define STR_FEN_GE					(_T("***")) // 内容中的分隔带标签

// 宏定义 全部的分组名
#define GP_STEEL					(_T("概念筋分组"))

// 宏定义 标题所在行的行标签
#define TAG_ROW_DESIGN				(_T("TAG_ROW_DESIGN"))
#define TAG_ROW_BASE				(_T("TAG_ROW_BASE"))
#define TAG_ROW_ARRANGE				(_T("TAG_ROW_ARRANGE"))
#define TAG_ROW_EDGE				(_T("TAG_ROW_EDGE"))
#define TAG_ROW_DETAIL				(_T("TAG_ROW_DETAIL"))

// 宏定义 用户可填写的单元格标识
#define TAG_STEEL_COMPONENT_NAME	(_T("TAG_STEEL_COMPONENT_NAME"))
#define TAG_STEEL_COMPONENT_VALUE	(_T("TAG_STEEL_COMPONENT_VALUE"))
#define TAG_DESIGN_NAME				(_T("TAG_DESIGN_NAME"))
#define TAG_DESIGN_VALUE			(_T("TAG_DESIGN_VALUE"))
#define TAG_BASE_NAME				(_T("TAG_BASE_NAME"))
#define TAG_BASE_DIAMETER			(_T("TAG_BASE_DIAMETER"))
#define TAG_BASE_STEEL_TYPE			(_T("TAG_BASE_STEEL_TYPE"))
#define TAG_BASE_HOOK_TYPE			(_T("TAG_BASE_HOOK_TYPE"))
#define TAG_BASE_DISTANCE			(_T("TAG_BASE_DISTANCE"))
#define TAG_BASE_FORCE_TYPE			(_T("TAG_BASE_FORCE_TYPE"))
#define TAG_ARRANGE_NAME			(_T("TAG_ARRANGE_SPACES_COUNT"))
#define TAG_ARRANGE_SUB_NAME		(_T("TAG_ARRANGE_SPACES_COUNT"))
#define TAG_ARRANGE_SPACES_DISTANCE	(_T("TAG_ARRANGE_SPACES_DISTANCE"))
#define TAG_ARRANGE_SPACES_COUNT	(_T("TAG_ARRANGE_SPACES_COUNT"))
#define TAG_EDGE_NAME				(_T("TAG_EDGE_LOCATION"))
#define TAG_EDGE_EDGE_NAME			(_T("TAG_EDGE_LOCATION"))
#define TAG_EDGE_LOCATION			(_T("TAG_EDGE_LOCATION"))
#define TAG_EDGE_LOGIC_TYPE			(_T("TAG_EDGE_LOGIC_TYPE"))
#define TAG_EDGE_EDGE_DISTANCE		(_T("TAG_EDGE_EDGE_DISTANCE"))
#define TAG_EDGE_ROUND_LEVEL		(_T("TAG_EDGE_ROUND_LEVEL"))
#define TAG_EDGE_ROUND_TYPE			(_T("TAG_EDGE_ROUND_TYPE"))
#define TAG_EDGE_EDGE_EXPECT		(_T("TAG_EDGE_EDGE_EXPECT"))
#define TAG_DATAIL_NAME				(_T("TAG_DATAIL_VALUE"))
#define TAG_DATAIL_SUB_NAME			(_T("TAG_DATAIL_VALUE"))
#define TAG_DATAIL_VALUE			(_T("TAG_DATAIL_VALUE"))


const int gnRowHeight = 22; // 设置当表格隐藏之后再显示时的行高

// 单一标识与单元格的映射关系
typedef struct
{
	CString				id;				// 标识，格式与条件中单元格标识一致
	int					value_index;	// 有效数据列的第几列，格式与条件中的单元格对应变量一致
	int					row;			// 在表格中的行号,-1表示无效行
	int					col;			// 在表格中的列号,-1表示无效列
}id_pos_mapping;

// 一个单元格规则映射关系
typedef struct
{
	CString				src_cell_text;  // 单元格内容也缓存下来，避免操作表格进一步提高效率
	id_pos_mapping		src_pos;		// 源单元格位置
	CString				condition_type;	// 条件类型
	CString				value;			// 条件预期值
}condition_item_mapping;

// 多条规则组成的一个条件的映射关系
typedef struct
{
	bool							src_pos_disable;				// 禁用源单元格内容对此条件的影响（即条件永远执行，不判断源单元格内容
	bool							is_ok_when_src_pos_disabled;	// 无条件此规则时是否按true处理
	vector<condition_item_mapping>	condition_ref_cell;				// 用于条件判断的多个条件
	id_pos_mapping					des_cell;						// 要操作的目标单元格
	enum steel_grid_condition_action opt;							// 要执行的行为
}condition_mapping;

// 一个钢筋子表的所有映射关系
typedef struct
{
	vector<condition_mapping>		conditions_in_db;
	int								db_data_start_row;	// 当前子表的数据起始行（内部以设计思想标题为分界，稍有偏差，只要保证数据区判断不出错即可，不要求严格匹配界面上的对应行号，以减少不必要的判断逻辑）
	int								db_data_end_row;	// 当前子表的数据终止行（内部以设计思想标题为分界，稍有偏差，只要保证数据区判断不出错即可，不要求严格匹配界面上的对应行号，以减少不必要的判断逻辑）
}single_db_conditions;

// 把可以由外部人员指定单元格式的单元格按指定格式进行赋值
void set_cell_format(CTHCell* cell, const format_cell& cell_format)
{
	if (!cell)
	{
		_ASSERT((_T("void set_cell_format(CTHCell* cell, ...) cell 为空" ), false));
		return;
	}

	cell->SetCellType(cell_format.m_cell_type);
	cell->SetDataType(cell_format.m_data_type);
	cell->SetReadOnly(cell_format.m_is_read_only);
	cell->SetText(cell_format.m_text);
	cell->SetHintText(cell_format.m_hint_text);
	cell->SetLabelText(cell_format.m_label_text);
	cell->SetAlignment(cell_format.m_alignment);
	cell->SetSketchMapName(cell_format.m_sketch_map_name);
}

//////////////////////////////////////////////////////////////////////////
typedef struct tagFormatRowInfo
{
	int nRowNum; // 行号
	CString strText; // 要合并的行单元格文字
	CString strMainTag; // 要合并的行标签的主标识
}FormatRowInfo;

typedef struct tagMerageRowInfo
{
	int nStartRowNum; // 起始行号
	int nEndRowNum; // 终止行号
	CString strText; // 合并后单元格的内容
}MerageRowInfo;

// 根据表格的数据获取当前要合并的单元格范围,函数会对传入的FormatRowInfo进行整理再分析合并信息
void calc_steel_grid_merger_info(std::vector<FormatRowInfo>& rowInfo, std::vector<MerageRowInfo>& merageInfo)
{
	// 以两个##号结束表示不按行标识进行合并，而是按单元格名称进行合并，合并的规则是：
	// 1.每一组的起始行的单元格文本必须以两个#号结束
	// 2.每一组的其它行的单元格文本必须为空，或者与起始行单元格文本相同
	//   或者为起始行单元格文本去掉##号后的内容相同，或者为两个#号
	// 3.不满足第二条的行认为是新的分组
	
	merageInfo.clear();

	// 首先按行号从小到大对rowInfo进行排序
	for (size_t i = 0; i < rowInfo.size(); ++i)
	{
		for (size_t j = i + 1; j < rowInfo.size(); ++j)
		{
			if (rowInfo[i].nRowNum > rowInfo[j].nRowNum)
			{
				FormatRowInfo tmpInfo = rowInfo[i];
				rowInfo[i] = rowInfo[j];
				rowInfo[j] = tmpInfo;
			}
		}
	}

	for (size_t i = 0; i < rowInfo.size(); /* Nothing */)
	{
		CString strRight = rowInfo[i].strText.Right(2);
		
		CString strText = rowInfo[i].strText; // 单元格内容
		bool bMerageByText = false;

		// 检查有名有单元格名称中指定了合并信息的
		// true表示当前行所在的分组按单元格文本合并，false表示按行标识合并
		if (_T("##") == strRight)
		{
			bMerageByText = true;
			strText = strText.Left(strText.GetLength() - 2); // 去除##号，也可以用Delete效率更高
		}
		
		int nRowMax = rowInfo[i].nRowNum;
		// 检查后面有多少行可以与当前行组成为一个分组，如果没有，则当前行不合并，跳过
		for (size_t j = i + 1; j < rowInfo.size(); ++j)
		{
			if (bMerageByText)
			{
				if ( _T("") == rowInfo[j].strText									//  为空表示与上一行是一个分组
					|| _T("##") == rowInfo[j].strText								// 为##号表示与上一行是一个分组
					|| 0 == rowInfo[j].strText.CompareNoCase(rowInfo[i].strText)	// 去除##前的文本相同表示与上一行是一个分组
					|| 0 == rowInfo[j].strText.CompareNoCase(strText) )				// 去除##号的文本相同表示与上一行是一个分组
				{
					nRowMax = rowInfo[j].nRowNum;
				}
				else
				{
					break; // 开始新一轮的分组检测
				}
			}
			else
			{
				if (0 == rowInfo[i].strMainTag.CompareNoCase(rowInfo[j].strMainTag))
				{
					nRowMax = rowInfo[j].nRowNum;
				}
				else
				{
					break;
				}
			}
		}

		// 当前分组就只有一行时也标记合并，目的是执行合并时将标题中的##号去掉，具体的合并操作可不执行
		MerageRowInfo merageItem;
		merageItem.nStartRowNum = rowInfo[i].nRowNum;
		merageItem.nEndRowNum = nRowMax;
		merageItem.strText = strText;

		merageInfo.push_back(merageItem);

		++i; // 不能少，否则可能死循环
	}
}

//////////////////////////////////////////////////////////////////////////

void create_steel_main_caption(CTHDataBase * db, 
							  const steel_grid_format& format, 
							  int start_row, 		/* 表格的起始行 */ int start_column,		/* 表格的起始列 */ bool bUsedLoc) // 是否启用应用部位
{
	CTHCell *cell = nullptr;	// 单元格
	CString strHintText; // 提示文字
	format_cell cellFormat = format.m_caption_format.m_component;

	db->MergeCells(start_row, start_column, start_row, start_column + 8);
	cell = db->GetCell(start_row, start_column);
	cell->SetText(format.m_caption_format.m_main_title);
	// 将表格模板信息存入此单元格的Label信息中，便于以后调试时对表格进行定位和跟踪
	CString strTemplateName;
	for (size_t i = 0; i != format.m_vecTemplateName.size(); ++i)
	{
		strTemplateName += format.m_vecTemplateName[i] + _T("\n");
	}
	cell->SetLabelText(strTemplateName);
	// cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	cell->SetHintText(format.m_caption_format.m_main_title);
	cell->SetReadOnly(TRUE);
	// cell->SetFontID(FONT_TITLE);
	// cell->SetBKcolor(RGB(201, 220, 255));

	db->MergeCells(start_row + 1, start_column + 1, start_row + 1, start_column + 2);
	cell = db->GetCell(start_row + 1, start_column + 1);
	cell->SetText(_T("应用部位"));
	strHintText = _T("空表示全部可能的对象");
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetHintText(strHintText);
	cell->SetReadOnly(TRUE);
	cell->SetTag(TAG_STEEL_COMPONENT_NAME);

	db->MergeCells(start_row + 1, start_column + 3, start_row + 1, start_column + 8);
	cell = db->GetCell(start_row + 1, start_column + 3);
	cell->SetAlignment(cellFormat.m_alignment);
	cell->SetTag(TAG_STEEL_COMPONENT_VALUE);
	set_cell_format(cell, cellFormat);
	// 如果应用部位的下拉选项中有内容，就认为是用户定死了的，否则认为是可变动的，此时会设置环境变量
	CString strComponent = cellFormat.m_label_text;
	strComponent.Trim(); // 读出来的下拉列表值会有一个换行符，所以要Trim
	if (_T("@") == strComponent.Left(1)) cell->SetLabelText(ENV_YING_YONG_BU_WEI);
	if (!bUsedLoc)
	{
		cell->SetText(_T("All"));
		cell->SetReadOnly(TRUE);
	}
}

void create_steel_main_design(CTHDataBase * db,
							 const steel_grid_format& format,
							 int start_row, // 表格的起始行
							 int start_column) // 表格的起始列
{
	int nTitleS = start_column;		// 标题合并的起始列
	int nTitleE = start_column + 1;	// 标题合并的终止列
	int nValueS = start_column + 2;	// 内容合并的起始列
	int nValueE = start_column + 7;	// 内容合并的终止列
	CTHCell *cell = nullptr;		// 单元格
	CString strHintText;			// 提示文字
	int nRow = start_row;			// 操作的起始行
	format_cell cellFormat;

	// 为了更改表的样式，需要为Database中添加一个加粗的字体
	db->MergeCells(nRow, nTitleS, nRow, nValueE);
	cell = db->GetCell(nRow, nTitleS);
	cell->SetText(_T("设计思想"));
	strHintText = _T("设计思想");
	cell->SetHintText(strHintText);
	//	cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	cell->SetReadOnly(TRUE);
	cell->SetFontID(FONT_TITLE);
	//cell->SetBKcolor(RGB(201, 220, 255));
	db->SetRowTag(start_row, TAG_ROW_DESIGN);
	db->SetRowExtendData(nRow, nTitleS,"SUB_TITLE");


	for (size_t i = 0; i != format.m_design_format.size(); ++i)
	{
		++nRow;
		// 设置行标签，把标识转换成以下划线分隔的行标签
		CTHRow* pRow = db->GetRow(nRow);

		// 如果是分隔带，则合并这一行，填充文本，并设置只读格式，后面不再填充其它数据
		if (STR_FEN_GE == format.m_design_format[i].m_id)
		{
			db->MergeCells(nRow, start_column, nRow, start_column + 7);
			cell = db->GetCell(nRow, start_column);
			cell->SetDataType(CELLDATA_STRING);
			cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
			cell->SetText(format.m_design_format[i].m_item_name);
			cell->SetHintText(format.m_design_format[i].m_item_name);
			cell->SetReadOnly(TRUE);
			cell->SetTag(TAG_DESIGN_NAME);
			pRow->SetTag(STR_FEN_GE);

			continue; // 后面不再有数据填充
		}

		pRow->SetTag(format.m_design_format[i].m_id);

		db->MergeCells(nRow, nTitleS, nRow, nTitleE);
		cell = db->GetCell(nRow, nTitleS);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_design_format[i].m_item_name);
		cell->SetHintText(format.m_design_format[i].m_item_name);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_DESIGN_NAME);
		cell->SetExtendData(_T("$$"), (format.m_design_format[i].m_using_string_value ? _T("$$") : _T("")));

		cellFormat = format.m_design_format[i].m_value;
		db->MergeCells(nRow, nValueS, nRow, nValueE);
		cell = db->GetCell(nRow, nValueS);
		cell->SetTag(TAG_DESIGN_VALUE);
		set_cell_format(cell, cellFormat);
	}
}

void create_steel_main_basic(CTHDataBase * db,
						   const steel_grid_format& format,
						   int start_row, 		/* 表格的起始行 */ int start_column) // 表格的起始列
{
	int nTitleS = start_column;		// 标题合并的起始列
	int nValueS = start_column + 2;	// 内容合并的起始列
	CTHCell *cell = nullptr;			// 单元格
	CString strHintText;			// 提示文字
	int nRow = start_row;			// 当前行
	int nCol = 0;					// 当前列


	db->MergeCells(nRow, nTitleS, nRow, nTitleS + 7);
	cell = db->GetCell(nRow, nTitleS);
	cell->SetText(_T("基本信息"));

	strHintText = _T("基本信息");
	cell->SetHintText(strHintText);
	//	cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	cell->SetReadOnly(TRUE);
	cell->SetFontID(FONT_TITLE);
	//cell->SetBKcolor(RGB(201, 220, 255));
	db->SetRowTag(start_row, TAG_ROW_BASE);
	db->SetRowExtendData(nRow, nTitleS,"SUB_TITLE");

	++nRow;
	db->MergeCells(nRow, nTitleS, nRow, nValueS - 1);
	cell = db->GetCell(nRow, nTitleS);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("钢筋名称"));
	cell->SetHintText(_T("钢筋名称"));
	cell->SetReadOnly(TRUE);

	nCol = nValueS;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("直径"));
	cell->SetHintText(_T("直径"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("钢种"));
	cell->SetHintText(_T("钢种"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("弯钩类型"));
	cell->SetHintText(_T("选择或输入弯钩类型，如果有多个弯钩，用半角逗号隔开"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("端距"));
	cell->SetHintText(_T("端距"));
	cell->SetReadOnly(TRUE);

	++nCol;
	db->MergeCells(nRow, nCol, nRow, nCol + 1);
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("受力类型"));
	cell->SetHintText(_T("受力类型"));
	cell->SetReadOnly(TRUE);

	format_cell cellFormat;
	for (size_t i = 0; i != format.m_basic_format.size(); ++i)
	{
		++nRow;
		// 设置行标签，把标识转换成以下划线分隔的行标签
		CTHRow* pRow = db->GetRow(nRow);

		// 如果是分隔带，则合并这一行，填充文本，并设置只读格式，后面不再填充其它数据
		if (STR_FEN_GE == format.m_basic_format[i].m_id)
		{
			db->MergeCells(nRow, start_column, nRow, start_column + 7);
			cell = db->GetCell(nRow, start_column);
			cell->SetDataType(CELLDATA_STRING);
			cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
			cell->SetText(format.m_basic_format[i].m_steel_name);
			cell->SetHintText(format.m_basic_format[i].m_steel_name);
			cell->SetReadOnly(TRUE);
			cell->SetTag(TAG_DESIGN_NAME);
			pRow->SetTag(STR_FEN_GE);

			continue; // 后面不再有数据填充
		}

		pRow->SetTag(format.m_basic_format[i].m_id);

		db->MergeCells(nRow, nTitleS, nRow, nValueS - 1);
		cell = db->GetCell(nRow, nTitleS);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_basic_format[i].m_steel_name);
		cell->SetHintText(format.m_basic_format[i].m_steel_name);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_BASE_NAME);

		nCol = nValueS;
		cellFormat = format.m_basic_format[i].m_diameter;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_BASE_DIAMETER);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_basic_format[i].m_steel_type;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_BASE_STEEL_TYPE);
		set_cell_format(cell, cellFormat);
		//////////////////////////////////////////////////////////////////////////
		// 【Add By WYZ 2012-2-16 15:06:09】
		// 1.应陈刚要求，为所有的钢种添加提示信息，此处仅对非只读的单元格添加提示，因为只
		// 读的单元格有两种情况，一种是钢种不允许修改，第二种是钢种对应的值取决于其它钢筋
		// 的钢种，此时会注明这类文字，不应该提示钢种号的含义，因为这类单元格中显示的文字
		// 不是钢种号。
		// 2.钢种目前有5种取值，为0表示由程序内部自定，1~4表示取1类~4类钢筋，具体的钢筋属
		// 于哪类需要参照公司使用的公路钢筋、预应力混凝土设计规范。
		// 3.提示信息较多，为保证显示的美观性，在适当的长度处换行较好。
		// 4.之所以不在函数SetCellFormat()中设置是考虑到函数的通用性及效率，放在此处覆盖函
		// 数设计的提示内容是比较好的选择。
		if (!cellFormat.m_is_read_only)
		{
			CString strSteelTypeHint = _T("1、2、3、4对应零件库中设计零件中的1、2、3、4类钢筋。\n")
				_T("0表示由程序内部根据钢筋直径自动选择(直径小于10mm的为1类，否则为2类)");
			cell->SetHintText(strSteelTypeHint);
		}

		++nCol;
		cellFormat = format.m_basic_format[i].m_hook_type;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_BASE_HOOK_TYPE);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_basic_format[i].m_distance;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_BASE_DISTANCE);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_basic_format[i].m_force_type;
		db->MergeCells(nRow, nCol, nRow, nCol + 1);
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_BASE_FORCE_TYPE);
		set_cell_format(cell, cellFormat);

	}

}

void create_steel_main_layout(CTHDataBase * db,
							  const steel_grid_format& format,
							  int start_row, 		/* 表格的起始行 */ int start_column) // 表格的起始列
{
	int nTitleS = start_column;	// 标题合并的起始列
	int nValueS = start_column + 2;// 内容合并的起始列
	CTHCell *cell = nullptr;		// 单元格
	CString strHintText;		// 提示文字
	int nRow = start_row;		// 当前行
	int nCol = 0;				// 当前列


	db->MergeCells(nRow, nTitleS, nRow, nTitleS + 7);
	cell = db->GetCell(nRow, nTitleS);
	cell->SetText(_T("布置信息"));
	strHintText = _T("布置信息");
	cell->SetHintText(strHintText);
	//	cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	cell->SetReadOnly(TRUE);
	cell->SetFontID(FONT_TITLE);
	//cell->SetBKcolor(RGB(201, 220, 255));
	db->SetRowTag(start_row, TAG_ROW_ARRANGE);
	db->SetRowExtendData(nRow, nTitleS,"SUB_TITLE");

	++nRow;
	cell = db->GetCell(nRow, nTitleS);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("钢筋名称"));
	cell->SetHintText(_T("钢筋名称"));
	cell->SetReadOnly(TRUE);

	cell = db->GetCell(nRow, nTitleS + 1);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("区域名称"));
	cell->SetHintText(_T("区域名称"));
	cell->SetReadOnly(TRUE);

	nCol = nValueS;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("布置间距"));
	cell->SetHintText(_T("布置间距"));
	cell->SetReadOnly(TRUE);

	++nCol;
	db->MergeCells(nRow, nCol, nRow, nCol + 4);
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("期望间距数或根数"));
	cell->SetHintText(_T("期望间距数或根数"));
	cell->SetReadOnly(TRUE);

	format_cell cellFormat;
	for (size_t i = 0; i != format.m_layout_format.size(); ++i)
	{
		++nRow;
		// 设置行标签，把标识转换成以下划线分隔的行标签
		CTHRow* pRow = db->GetRow(nRow);

		// 如果是分隔带，则合并这一行，填充文本，并设置只读格式，后面不再填充其它数据
		if (STR_FEN_GE == format.m_layout_format[i].m_id)
		{
			db->MergeCells(nRow, start_column, nRow, start_column + 7);
			cell = db->GetCell(nRow, start_column);
			cell->SetDataType(CELLDATA_STRING);
			cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
			cell->SetText(format.m_layout_format[i].m_steel_name);
			cell->SetHintText(format.m_layout_format[i].m_steel_name);
			cell->SetReadOnly(TRUE);
			cell->SetTag(TAG_DESIGN_NAME);
			pRow->SetTag(STR_FEN_GE);

			continue; // 后面不再有数据填充
		}

		pRow->SetTag(format.m_layout_format[i].m_id + _T("_")
			+ format.m_layout_format[i].m_sub_id);

		cell = db->GetCell(nRow, nTitleS);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_layout_format[i].m_steel_name);
		cell->SetHintText(format.m_layout_format[i].m_steel_name);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_ARRANGE_NAME);

		cell = db->GetCell(nRow, nTitleS + 1);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_layout_format[i].m_area);
		cell->SetHintText(format.m_layout_format[i].m_area);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_ARRANGE_SUB_NAME);

		nCol = nValueS;
		cellFormat = format.m_layout_format[i].m_spaces_distance;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_ARRANGE_SPACES_DISTANCE);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_layout_format[i].m_spaces_count;
		db->MergeCells(nRow, nCol, nRow, nCol + 4);
		cell = db->GetCell(nRow, nCol);
		set_cell_format(cell, cellFormat);
	}

	// 合并标识相同的钢筋名称行（主标识相同即可，与子标识无关）
	if (format.m_layout_format.size() <= 1) return; // 只有一行或没有信息则不执行合并

	nRow = start_row + 2; 	// 数据所在的行
	nCol = start_column;		// 钢筋名所在的列

	vector<FormatRowInfo> vecRowInfo;

	for (size_t i = 0; i < format.m_layout_format.size(); ++i)
	{
		FormatRowInfo row;
		row.nRowNum = nRow++; // 不能写成++nRow
		row.strMainTag = format.m_layout_format[i].m_id;
		row.strText = format.m_layout_format[i].m_steel_name;

		vecRowInfo.push_back(row);
	}

	vector<MerageRowInfo> vecMerageInfo;
	calc_steel_grid_merger_info(vecRowInfo, vecMerageInfo);

	for (size_t i = 0; i < vecMerageInfo.size(); ++i)
	{
		if (vecMerageInfo[i].nStartRowNum != vecMerageInfo[i].nEndRowNum)
			db->MergeCells(vecMerageInfo[i].nStartRowNum, nCol,
			vecMerageInfo[i].nEndRowNum, nCol);

		// 合并后多个单元格中的文本会拼接在一起放在第一个单元格中，所以要重新设置一下
		// 就算只有一行不合并，但也要重设一下文本，不显示有特殊作用的##号
		CTHCell* pCe = db->GetCell(vecMerageInfo[i].nStartRowNum, nCol);
		if (pCe) pCe->SetText(vecMerageInfo[i].strText);
	}
}

void create_steel_main_edge(CTHDataBase * db,
						   const steel_grid_format& format,
						   int start_row, 		/* 表格的起始行 */ int start_column) // 表格的起始列
{
	int nTitleS = start_column;	// 标题合并的起始列
	CTHCell *cell = nullptr;		// 单元格
	CString strHintText;		// 提示文字
	int nRow = start_row;		// 当前行
	int nCol = 0;				// 当前列


	db->MergeCells(nRow, nTitleS, nRow, nTitleS + 7);
	cell = db->GetCell(nRow, nTitleS);
	cell->SetText(_T("边距信息"));
	strHintText = _T("边距信息");
	cell->SetHintText(strHintText);
	//	cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	cell->SetReadOnly(TRUE);
	cell->SetFontID(FONT_TITLE);
	//cell->SetBKcolor(RGB(201, 220, 255));
	db->SetRowTag(start_row, TAG_ROW_EDGE);
	db->SetRowExtendData(nRow, nTitleS,"SUB_TITLE");

	++nRow;
	nCol = nTitleS;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("钢筋名称"));
	cell->SetHintText(_T("钢筋名称"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("边距名称"));
	cell->SetHintText(_T("边距名称"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("定位点"));
	cell->SetHintText(_T("定位点"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("定位方式"));
	cell->SetHintText(_T("定位方式"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("边距值"));
	cell->SetHintText(_T("边距值"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("取整级别"));
	cell->SetHintText(_T("取整级别"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("取整方式"));
	cell->SetHintText(_T("取整方式"));
	cell->SetReadOnly(TRUE);

	++nCol;
	db->MergeCells(nRow, nCol, nRow, nCol);
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("期望边距"));
	cell->SetHintText(_T("期望边距"));
	cell->SetReadOnly(TRUE);

	format_cell cellFormat;
	for (size_t i = 0; i != format.m_edge_format.size(); ++i)
	{
		++nRow;
		// 设置行标签，把标识转换成以下划线分隔的行标签
		CTHRow* pRow = db->GetRow(nRow);

		// 如果是分隔带，则合并这一行，填充文本，并设置只读格式，后面不再填充其它数据
		if (STR_FEN_GE == format.m_edge_format[i].m_id)
		{
			db->MergeCells(nRow, start_column, nRow, start_column + 7);
			cell = db->GetCell(nRow, start_column);
			cell->SetDataType(CELLDATA_STRING);
			cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
			cell->SetText(format.m_edge_format[i].m_steel_name);
			cell->SetHintText(format.m_edge_format[i].m_steel_name);
			cell->SetReadOnly(TRUE);
			cell->SetTag(TAG_DESIGN_NAME);
			pRow->SetTag(STR_FEN_GE);

			continue; // 后面不再有数据填充
		}

		pRow->SetTag(format.m_edge_format[i].m_id + _T("_")
			+ format.m_edge_format[i].m_sub_id);

		nCol = nTitleS;
		cell = db->GetCell(nRow, nCol);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_edge_format[i].m_steel_name);
		cell->SetHintText(format.m_edge_format[i].m_steel_name);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_EDGE_NAME);

		++nCol;
		cell = db->GetCell(nRow, nCol);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_edge_format[i].m_edge_name);
		cell->SetHintText(format.m_edge_format[i].m_edge_name);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_EDGE_EDGE_NAME);

		++nCol;
		cellFormat = format.m_edge_format[i].m_location;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_EDGE_LOCATION);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_edge_format[i].m_logic_type;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_EDGE_LOGIC_TYPE);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_edge_format[i].m_edge_distance;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_EDGE_EDGE_DISTANCE);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_edge_format[i].m_round_level;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_EDGE_ROUND_LEVEL);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_edge_format[i].m_round_type;
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_EDGE_ROUND_TYPE);
		set_cell_format(cell, cellFormat);

		++nCol;
		cellFormat = format.m_edge_format[i].m_expect_edge;
		db->MergeCells(nRow, nCol, nRow, nCol);
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_EDGE_EDGE_EXPECT);
		set_cell_format(cell, cellFormat);
	}

	// 合并标识相同的钢筋名称行（主标识不同即可，与子标识无关）
	if (format.m_edge_format.size() <= 1) return; // 只有一行或没有信息则不执行合并

	nRow = start_row + 2; 	// 数据所在的行
	nCol = start_column;		// 钢筋名所在的列

	vector<FormatRowInfo> vecRowInfo;

	for (size_t i = 0; i < format.m_edge_format.size(); ++i)
	{
		FormatRowInfo row;
		row.nRowNum = nRow++; // 不能写成++nRow
		row.strMainTag = format.m_edge_format[i].m_id;
		row.strText = format.m_edge_format[i].m_steel_name;

		vecRowInfo.push_back(row);
	}

	vector<MerageRowInfo> vecMerageInfo;
	calc_steel_grid_merger_info(vecRowInfo, vecMerageInfo);

	for (size_t i = 0; i < vecMerageInfo.size(); ++i)
	{
		if (vecMerageInfo[i].nStartRowNum != vecMerageInfo[i].nEndRowNum)
			db->MergeCells(vecMerageInfo[i].nStartRowNum, nCol,
				vecMerageInfo[i].nEndRowNum, nCol);

		// 合并后多个单元格中的文本会拼接在一起放在第一个单元格中，所以要重新设置一下
		// 就算只有一行不合并，但也要重设一下文本，不显示有特殊作用的##号
		CTHCell* pCe = db->GetCell(vecMerageInfo[i].nStartRowNum, nCol);
		if (pCe) pCe->SetText(vecMerageInfo[i].strText);
	}
}

void create_steel_main_detail(CTHDataBase * db,
							 const steel_grid_format& format,
							 int start_row, 	/* 表格的起始行 */ int start_column) // 表格的起始列
{
	int nTitleS = start_column;	// 标题合并的起始列
	CTHCell *cell = nullptr;		// 单元格
	CString strHintText;		// 提示文字
	int nRow = start_row;		// 当前行
	int nCol = 0;				// 当前列


	db->MergeCells(nRow, nTitleS, nRow, nTitleS + 7);
	cell = db->GetCell(nRow, nTitleS);
	cell->SetText(_T("大样信息"));
	strHintText = _T("大样信息");
	cell->SetHintText(strHintText);
	//	cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
	cell->SetReadOnly(TRUE);
	cell->SetFontID(FONT_TITLE);
	//cell->SetBKcolor(RGB(201, 220, 255));
	db->SetRowTag(start_row, TAG_ROW_DETAIL);
	db->SetRowExtendData(nRow, nTitleS,"SUB_TITLE");

	++nRow;
	nCol = nTitleS;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("钢筋名称"));
	cell->SetHintText(_T("钢筋名称"));
	cell->SetReadOnly(TRUE);

	++nCol;
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("条目名称"));
	cell->SetHintText(_T("条目名称"));
	cell->SetReadOnly(TRUE);

	++nCol;
	db->MergeCells(nRow, nCol, nRow, nCol + 5);
	cell = db->GetCell(nRow, nCol);
	cell->SetDataType(CELLDATA_STRING);
	cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
	cell->SetText(_T("参数值"));
	cell->SetHintText(_T("参数值"));
	cell->SetReadOnly(TRUE);

	format_cell cellFormat;
	for (size_t i = 0; i != format.m_detail_format.size(); ++i)
	{
		++nRow;
		// 设置行标签，把标识转换成以下划线分隔的行标签
		CTHRow* pRow = db->GetRow(nRow);

		// 如果是分隔带，则合并这一行，填充文本，并设置只读格式，后面不再填充其它数据
		if (STR_FEN_GE == format.m_detail_format[i].m_id)
		{
			db->MergeCells(nRow, start_column, nRow, start_column + 7);
			cell = db->GetCell(nRow, start_column);
			cell->SetDataType(CELLDATA_STRING);
			cell->SetAlignment(DT_CENTER | DT_VCENTER | DT_HOR);
			cell->SetText(format.m_detail_format[i].m_item_name);
			cell->SetHintText(format.m_detail_format[i].m_item_name);
			cell->SetReadOnly(TRUE);
			cell->SetTag(TAG_DESIGN_NAME);
			pRow->SetTag(STR_FEN_GE);

			continue; // 后面不再有数据填充
		}

		pRow->SetTag(format.m_detail_format[i].m_id + _T("_")
			+ format.m_detail_format[i].m_sub_id);

		nCol = nTitleS;
		cell = db->GetCell(nRow, nCol);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_detail_format[i].m_steel_name);
		cell->SetHintText(format.m_detail_format[i].m_steel_name);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_DATAIL_NAME);

		++nCol;
		cell = db->GetCell(nRow, nCol);
		cell->SetDataType(CELLDATA_STRING);
		cell->SetAlignment(DT_LEFT | DT_VCENTER | DT_HOR);
		cell->SetText(format.m_detail_format[i].m_item_name);
		cell->SetHintText(format.m_detail_format[i].m_item_name);
		cell->SetReadOnly(TRUE);
		cell->SetTag(TAG_DATAIL_SUB_NAME);
		cell->SetExtendData(_T("$$"), (format.m_detail_format[i].m_using_string_value ? _T("$$") : _T("")));

		++nCol;
		cellFormat = format.m_detail_format[i].m_value;
		db->MergeCells(nRow, nCol, nRow, nCol + 5);
		cell = db->GetCell(nRow, nCol);
		cell->SetTag(TAG_DATAIL_VALUE);
		set_cell_format(cell, cellFormat);

	}

	// 合并标识相同的钢筋名称行（主标识不同即可，与子标识无关）
	if (format.m_detail_format.size() <= 1) return; // 只有一行或没有信息则不执行合并

	nRow = start_row + 2; 	// 数据所在的行
	nCol = start_column;		// 钢筋名所在的列

	vector<FormatRowInfo> vecRowInfo;

	for (size_t i = 0; i < format.m_detail_format.size(); ++i)
	{
		FormatRowInfo row;
		row.nRowNum = nRow++; // 不能写成++nRow
		row.strMainTag = format.m_detail_format[i].m_id;
		row.strText = format.m_detail_format[i].m_steel_name;

		vecRowInfo.push_back(row);
	}

	vector<MerageRowInfo> vecMerageInfo;
	calc_steel_grid_merger_info(vecRowInfo, vecMerageInfo);

	for (size_t i = 0; i < vecMerageInfo.size(); ++i)
	{
		if (vecMerageInfo[i].nStartRowNum != vecMerageInfo[i].nEndRowNum)
			db->MergeCells(vecMerageInfo[i].nStartRowNum, nCol,
			vecMerageInfo[i].nEndRowNum, nCol);

		// 合并后多个单元格中的文本会拼接在一起放在第一个单元格中，所以要重新设置一下
		// 就算只有一行不合并，但也要重设一下文本，不显示有特殊作用的##号
		CTHCell* pCe = db->GetCell(vecMerageInfo[i].nStartRowNum, nCol);
		if (pCe) pCe->SetText(vecMerageInfo[i].strText);
	}
}

steel_grid_tools::steel_grid_tools( void )
{

}

steel_grid_tools::~steel_grid_tools(void)
{

}

// 把一个用下划线分隔的字符串分成前后两部分
void split_steel_geid_line_id(const CString& strID, CString& main_id, CString& sub_id)
{
	// 字符中如果有多个下划线，只认第一个，其它的当作普通字符处理
	// 如果没有下划线或者唯一的一个下划线在最后面，则subID为空串
	// 如果以下划线开头，则mainID为空
	// 如果strID为空，则mainID和subID皆为空
	main_id = _T("");
	sub_id = _T("");

	if (_T("") == strID) return;

	int flag = strID.Find(_T('_'));
	if (-1 == flag)
	{
		main_id = strID;
		sub_id = _T("");
		return;
	}
	else if (0 == flag)
	{
		main_id = _T("");
		sub_id = strID;
		return;
	}
	else if (flag == strID.GetLength() - 1)
	{
		main_id = strID;
		sub_id = _T("");
		return;
	}
	else
	{
		main_id = strID.Left(flag); // 不要下划线
		sub_id = strID.Mid(flag + 1); // 不要下划线
	}
}

//////////////////////////////////////////////////////////////////////////
void get_table_format_single_cell(CTHCell* cell, format_cell& cellF)
{
	if (NULL == cell) return;

	cellF.m_is_read_only = bool(cell->GetReadOnly() == TRUE);
	cellF.m_data_type = cell->GetDataType();
	cellF.m_cell_type = cell->GetCellType();
	cellF.m_alignment = cell->GetAlignment();
	cellF.m_hint_text = cell->GetHintText();
	cellF.m_label_text = cell->GetLabelText();
	cellF.m_text = cell->GetText();
	cellF.m_sketch_map_name = cell->GetSketchMapName();

	// 单元格TAG在程序中有其它作用，已经内定了，不可再获取TAG覆盖了原内容，所以不获取CellTAG
	// 单元格有效条件不获取,因为新的钢筋表已经不是模板中的表中,有效条件可能会导致界面钢筋表错乱,而
	// 且界面合成的钢筋表有自己独特的有效条件控制方式(参考CGWGridCondition类)
}

void get_table_format_caption(const CString& caption, steel_grid_format& format)
{
	if (_T("") == caption)
	{
		app_print_info(MT_DEBUG, _T(">>>>表头表格模板名为空"));
		return; // 没有表头模板字符串，这是合法情况，不要断言
	}

	CTHDataBase *pDB = table_templates::create_data_base(caption);
	if (NULL == pDB)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】无法实例化，GWCreateDatabase()失败"), caption);
		return;
	}

	// 读取表格式
	int nRowCount = pDB->GetRowCount();
	if (nRowCount < 3) return; // 没有内容

	CTHCell *cell = nullptr;

	cell = pDB->GetCell(1, 0);
	if (NULL == cell)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，表标题GetCell(1,0)失败"), caption);
		return;
	}
	format.m_caption_format.m_main_title = cell->GetText();

	cell = pDB->GetCell(2, 1);
	if (NULL == cell)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，子表应用部件GetCell(2,1)失败"), caption);
		return;
	}
	get_table_format_single_cell(cell, format.m_caption_format.m_component);

	safe_delete(pDB);
}

void get_table_format_design(const CString& desing, steel_grid_format& format)
{
	if (_T("") == desing)
	{
		app_print_info(MT_DEBUG, _T(">>>>设计思想表格模板名为空"));
		return; // 没有设计思想，这是合法情况，不要断言
	}

	CTHDataBase *pDB = table_templates::create_data_base(desing);
	if (NULL == pDB)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】无法实例化，GWCreateDatabase()失败"), desing);
		return;
	}

	// 读取表格式
	int nRowCount = pDB->GetRowCount();
	if (nRowCount <= 1)
	{
		app_print_info(MT_DEBUG, _T(">>>>设计思想表格行数<=1，不解析"), desing);
		return; // 没有内容
	}

	CTHCell *cell = nullptr;
	CString cell_tag;

	format.m_design_format.clear();
	for (int i = 2; i < nRowCount; ++i) // 内容从第3行（下标为2）开始（有标题行和表头）
	{
		format_design dsg_fmt;

		// 标识及条目或者分隔行名称
		cell = pDB->GetCell(i, 0);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，标识及条目GetCell(%d,0)失败"), desing, i);
			continue;
		}
		cell_tag = pDB->GetRowTag(i);
		dsg_fmt.m_id = cell_tag;
		dsg_fmt.m_item_name = cell->GetText();
		dsg_fmt.m_using_string_value = false;

		// 如果标识为三个*号，表明是分隔带，内部定一个标识，方便画表即可，注分隔带整行合并为一个单元格
		// 本模块开发及测试时此功能正常，后来由于表格控件相关的人对导出的表格格式作了改动，属性表中三个
		// 星号代表空单格，与本模块冲突了，导致分隔标记传不过来，此处为兼容以前的表格，只能改为判断为三
		// 个星号开关的标记当作分隔行，表格设计的人遇到属性表时要求分隔符多敲几个，最好敲5个或以上，以防
		// 被表格控件处理掉。[WYZ 2014年6月3日16:35:56]
		if (0 == dsg_fmt.m_id.Find( STR_FEN_GE ))//20140603//修改为5个******
		{
			dsg_fmt.m_id = STR_FEN_GE;
			format.m_design_format.push_back(dsg_fmt);
			continue; // 后面的参数值无效，只认上面读取的文本即可
		}

		// 如果标识以两个半角的$$符号结尾，表示单元格中内容的控件如果是带下拉列表的，则按原来字符串
		// 解析，否则按原来的做法采用从0开始的索引号解析【应韩厚正要求加的，目的是他想要字符串以便
		// 更好的进行序列化，并且考虑兼容原来老数据才这样扩展的】
		
		// 到2014年1月25日为止，在钢筋表格的条目名称单元格中用到了两种标志字符串，分别是“##”和“$$”
		// 两处地方都是认为标志在最后，但由于“$$”的检测在读取表格格式信息时进行，“##”则在根据格式
		// 信息创建表格时进行，所以当两个标志都需要时要求“$$”必须放在“##”之后，目前出现这种情况的
		// 地方在大样信息表，其它子表只用到其中的一个，因为检测完标志后必须从字符串中删除。
		// 特作注释，如果以后要再加可能需要重构代码，统一在读取格式信息时全部检查并保存这些标志，这样
		// 可以做到扩展多个标识时不用关心标识的先后顺序，而且集中管理标识对程序的健壮性有好处，目前暂
		// 时不改【WYZ】
		CString strEnd = dsg_fmt.m_item_name.Right(2);
		if (_T("$$") == strEnd)
		{
			dsg_fmt.m_item_name.Delete(dsg_fmt.m_item_name.GetLength() - 2, 2);
			dsg_fmt.m_using_string_value = true;
		}

		if (dsg_fmt.m_id.IsEmpty()) // 表格不限制标识中使用空格,所以不Trim()
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据标识错误，不能为空"), desing, i);
			// 通用概念筋说明文档.docx中注明必须使用主标识
			dsg_fmt.m_id = _T("表格数据标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		// 参数值
		cell = pDB->GetCell(i, 1); // 上一单元格有合并，所以这一列为3
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，参数值GetCell(%d,1)失败"), desing, i);
			continue;
		}
		get_table_format_single_cell(cell, dsg_fmt.m_value);


		format.m_design_format.push_back(dsg_fmt);

	}

	safe_delete(pDB);
}

void get_table_format_basic(const CString& basic, steel_grid_format& format)
{
	if (_T("") == basic)
	{
		app_print_info(MT_DEBUG, _T(">>>>基本信息表格模板为空"));
		return; // 没有基本信息，这是合法情况，不要断言
	}


	CTHDataBase *pDB = table_templates::create_data_base(basic);
	if (NULL == pDB)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】无法实例化，GWCreateDatabase()失败"), basic);
		TRACE(_T("ERROR:%s\n"), basic);
		return;
	}

	// 读取表格式
	int nRowCount = pDB->GetRowCount();
	if (nRowCount <= 1) return; // 没有内容

	CTHCell *cell = nullptr;
	CString cell_tag;

	format.m_basic_format.clear();
	for (int i = 2; i < nRowCount; ++i) // 内容从第3行（下标为2）开始（有标题行和表头）
	{
		format_basic_grid base_fmt;

		// 标识及条目或者分隔行名称
		cell = pDB->GetCell(i, 0); // 内容从第2列开始，0列为序号，1列为条目名
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，标识及条目GetCell(%d,0)失败"), basic, i);
			continue;
		}
		cell_tag = pDB->GetRowTag(i);
		base_fmt.m_id = cell_tag;
		base_fmt.m_steel_name = cell->GetText();

		// 如果标识为三个*号，表明是分隔带，内部定一个标识，方便画表即可，注分隔带整行合并为一个单元格
		if (STR_FEN_GE == base_fmt.m_id)
		{
			format.m_basic_format.push_back(base_fmt);
			continue; // 后面的参数值无效，只认上面读取的文本即可
		}

		if (base_fmt.m_id.IsEmpty()) // 表格不限制标识中使用空格,所以不Trim()
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据标识错误，不能为空"), basic, i);
			// 通用概念筋说明文档.docx中注明必须使用主标识
			base_fmt.m_id = _T("表格数据标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		// 直径
		cell = pDB->GetCell(i, 1); // 上一单元格有合并，所以这一列为3
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，直径GetCell(%d,1)失败"), basic, i);
			continue;
		}
		get_table_format_single_cell(cell, base_fmt.m_diameter);

		// 钢种
		cell = pDB->GetCell(i, 2);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，钢种GetCell(%d,2)失败"), basic, i);
			continue;
		}
		get_table_format_single_cell(cell, base_fmt.m_steel_type);

		// 弯钩类型
		cell = pDB->GetCell(i, 3);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，弯钩类型GetCell(%d,3)失败"), basic, i);
			continue;
		}
		get_table_format_single_cell(cell, base_fmt.m_hook_type);
		
		// 弯钩类型的下拉列表中支持的条目类型参考钢筋文档“frame\gui_resource\readme\通用概念筋说明文档.docx”
		// 由于不同类型的钢筋下拉的列表可能只是全集中的一部分（如箍筋可能只出下箍筋相关的弯钩类型），目前从方案设
		// 计通用钢筋表制作以来均设置为文档中的全集，没有任何相关人员反馈过说条目过多，要根据不同钢筋类型提供相应
		// 的选项，因此目前暂不支持这种细化设置的功能，留注释于此，以便后续维护人员参看。
		base_fmt.m_hook_type.m_label_text =
			_T("无\n")
			_T("标准弯钩/90度\n")
			_T("标准弯钩/135度\n")
			_T("标准弯钩/180度\n")
			_T("主筋弯钩/90度\n")
			_T("主筋弯钩/135度\n")
			_T("主筋弯钩/180度\n")
			_T("箍筋弯钩/90度\n")
			_T("箍筋弯钩/135度\n")
			_T("箍筋弯钩/180度");
		base_fmt.m_hook_type.m_cell_type = CT_DROPDOWN; // 有下拉列表的同时还要支持输入自定义的弯钩数据

		// 端距
		cell = pDB->GetCell(i, 4);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，端距GetCell(%d,4)失败"), basic, i);
			continue;
		}
		get_table_format_single_cell(cell, base_fmt.m_distance);

		// 受力类型
		cell = pDB->GetCell(i, 5);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，受力类型GetCell(%d,5)失败"), basic, i);
			continue;
		}
		get_table_format_single_cell(cell, base_fmt.m_force_type);

		format.m_basic_format.push_back(base_fmt);

	}

	safe_delete(pDB);
}

void get_table_format_layout(const CString& layout, steel_grid_format& format)
{
	if (_T("") == layout)
	{
		app_print_info(MT_DEBUG, _T(">>>>布置信息表格模板名为空"));
		return; // 没有布置信息，这是合法情况，不要断言
	}

	CTHDataBase *pDB = table_templates::create_data_base(layout);
	if (NULL == pDB)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】无法实例化，GWCreateDatabase()失败"), layout);
		return;
	}

	// 读取表格式
	int nRowCount = pDB->GetRowCount();
	if (nRowCount <= 1)
	{
		app_print_info(MT_DEBUG, _T(">>>>布置信息表格行数<=1，不解析"), layout);
		return; // 没有内容
	}

	CTHCell *cell = nullptr;
	CString cell_tag;

	format.m_layout_format.clear();
	for (int i = 2; i < nRowCount; ++i) // 内容从第3行（下标为2）开始（有标题行和表头）
	{
		format_layout_grid layout_fmt;

		// 条目名称或者分隔行名称
		cell = pDB->GetCell(i, 0); // 内容从第2列开始，0列为序号，1列为条目名
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，条目名称GetCell(%d,0)失败"), layout, i);
			continue;
		}
		layout_fmt.m_steel_name = cell->GetText();

		// 标识
		cell_tag = pDB->GetRowTag(i);
		split_steel_geid_line_id(cell_tag, layout_fmt.m_id, layout_fmt.m_sub_id);

		// 如果标识为三个*号，表明是分隔带，内部定一个标识，方便画表即可，注分隔带整行合并为一个单元格
		if (STR_FEN_GE == layout_fmt.m_id)
		{
			format.m_layout_format.push_back(layout_fmt);
			continue; // 后面的参数值无效，只认上面读取的文本即可
		}

		if (layout_fmt.m_id.IsEmpty()) // 表格不限制标识中使用空格,所以不Trim()
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据主标识错误，【%s】主标识和子标识之间用一个半角下划线分隔且不能为空"), layout, i, cell_tag);
			// 通用概念筋说明文档.docx中注明必须同时使用主标识和子标识,且主标识和子标识之间用一个半角下划线分隔
			layout_fmt.m_id = _T("表格数据主标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		if (layout_fmt.m_sub_id.IsEmpty())
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据子标识错误，【%s】主标识和子标识之间用一个半角下划线分隔且不能为空"), layout, i, cell_tag);
			layout_fmt.m_sub_id = _T("表格数据子标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		// 区域名称
		cell = pDB->GetCell(i, 1);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，区域名称GetCell(%d,1)失败"), layout, i);
			continue;
		}
		layout_fmt.m_area = cell->GetText();

		// 布置间距
		cell = pDB->GetCell(i, 2);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，布置间距GetCell(%d,2)失败"), layout, i);
			continue;
		}
		get_table_format_single_cell(cell, layout_fmt.m_spaces_distance);

		// 期望间距或根数
		cell = pDB->GetCell(i, 3);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，期望间距或根数GetCell(%d,3)失败"), layout, i);
			continue;
		}
		get_table_format_single_cell(cell, layout_fmt.m_spaces_count);

		format.m_layout_format.push_back(layout_fmt);
	} // end of for(...)

	safe_delete(pDB);
}

void get_table_format_edge(const CString& edge, steel_grid_format& format)
{
	if (_T("") == edge)
	{
		app_print_info(MT_DEBUG, _T(">>>>边距信息表格模板名为空"));
		return; // 没有边距信息，这是合法情况，不要断言
	}

	CTHDataBase *pDB = table_templates::create_data_base(edge);
	if (NULL == pDB)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】无法实例化，GWCreateDatabase()失败"), edge);
		return;
	}

	// 读取表格式
	int nRowCount = pDB->GetRowCount();
	if (nRowCount <= 1)
	{
		app_print_info(MT_DEBUG, _T(">>>>边距信息表格行数<=1，不解析"), edge);
		return; // 没有内容
	}

	CTHCell *cell = nullptr;
	CString cell_tag;

	format.m_edge_format.clear();
	for (int i = 2; i < nRowCount; ++i) // 内容从第3行（下标为2）开始（有标题行和表头）
	{
		format_edge_grid edge_fmt;

		// 条目名称或者分隔行名称
		cell = pDB->GetCell(i, 0); // 内容从第2列开始，0列为序号，1列为条目名
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，条目名称GetCell(%d,0)失败"), edge, i);
			continue;
		}
		edge_fmt.m_steel_name = cell->GetText();

		// 标识
		cell_tag = pDB->GetRowTag(i);
		split_steel_geid_line_id(cell_tag, edge_fmt.m_id, edge_fmt.m_sub_id);

		// 如果标识为三个*号，表明是分隔带，内部定一个标识，方便画表即可，注分隔带整行合并为一个单元格
		if (STR_FEN_GE == edge_fmt.m_id)
		{
			format.m_edge_format.push_back(edge_fmt);
			continue; // 后面的参数值无效，只认上面读取的文本即可
		}

		if (edge_fmt.m_id.IsEmpty()) // 表格不限制标识中使用空格,所以不Trim()
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据主标识错误，【%s】主标识和子标识之间用一个半角下划线分隔且不能为空"), edge, i, cell_tag);
			// 通用概念筋说明文档.docx中注明必须同时使用主标识和子标识,且主标识和子标识之间用一个半角下划线分隔
			edge_fmt.m_id = _T("表格数据主标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		if (edge_fmt.m_sub_id.IsEmpty())
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据子标识错误，【%s】主标识和子标识之间用一个半角下划线分隔且不能为空"), edge, i, cell_tag);
			edge_fmt.m_sub_id = _T("表格数据子标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		// 边距名称
		cell = pDB->GetCell(i, 1);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，边距名称GetCell(%d,1)失败"), edge, i);
			continue;
		}
		edge_fmt.m_edge_name = cell->GetText();

		// 定位点
		cell = pDB->GetCell(i, 2);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，定位点GetCell(%d,2)失败"), edge, i);
			continue;
		}
		get_table_format_single_cell(cell, edge_fmt.m_location);

		// 定位方式
		cell = pDB->GetCell(i, 3);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，定位方式GetCell(%d,3)失败"), edge, i);
			continue;
		}
		get_table_format_single_cell(cell, edge_fmt.m_logic_type);

		// 边距
		cell = pDB->GetCell(i, 4);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，边距GetCell(%d,4)失败"), edge, i);
			continue;
		}
		get_table_format_single_cell(cell, edge_fmt.m_edge_distance);

		// 取整级别
		cell = pDB->GetCell(i, 5);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，取整级别GetCell(%d,5)失败"), edge, i);
			continue;
		}
		get_table_format_single_cell(cell, edge_fmt.m_round_level);

		// 取整方式
		cell = pDB->GetCell(i, 6);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，取整方式GetCell(%d,6)失败"), edge, i);
			continue;
		}
		get_table_format_single_cell(cell, edge_fmt.m_round_type);

		// 期望边距
		cell = pDB->GetCell(i, 7);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，期望边距GetCell(%d,7)失败"), edge, i);
			continue;
		}
		get_table_format_single_cell(cell, edge_fmt.m_expect_edge);

		format.m_edge_format.push_back(edge_fmt);
	} // end of for(...)

	safe_delete(pDB);
}

void get_table_format_detail(const CString& detail, steel_grid_format& format)
{
	if (_T("") == detail)
	{
		app_print_info(MT_DEBUG, _T(">>>>大样信息表格模板名为空"));
		return; // 没有大样信息，这是合法情况，不要断言
	}

	CTHDataBase *pDB = table_templates::create_data_base(detail);
	if (NULL == pDB)
	{
		app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】无法实例化，GWCreateDatabase()失败"), detail);
		return;
	}

	// 读取表格式
	int nRowCount = pDB->GetRowCount();
	if (nRowCount <= 1)
	{
		app_print_info(MT_DEBUG, _T(">>>>大样信息表格行数<=1，不解析"), detail);
		return; // 没有内容
	}

	CTHCell *cell = nullptr;
	CString cell_tag;

	format.m_detail_format.clear();
	for (int i = 2; i < nRowCount; ++i) // 内容从第3行（下标为2）开始（有标题行和表头）
	{
		format_edge_detail detail_fmt;

		// 钢筋名称或者分隔行名称
		cell = pDB->GetCell(i, 0); // 内容从第2列开始，0列为序号，1列为条目名
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，条目名称GetCell(%d,0)失败"), detail, i);
			continue;
		}
		detail_fmt.m_steel_name = cell->GetText();
		detail_fmt.m_using_string_value = false;

		// 标识
		cell_tag = pDB->GetRowTag(i);
		split_steel_geid_line_id(cell_tag, detail_fmt.m_id, detail_fmt.m_sub_id);

		// 如果标识为三个*号，表明是分隔带，内部定一个标识，方便画表即可，注分隔带整行合并为一个单元格
		if (STR_FEN_GE == detail_fmt.m_id)
		{
			format.m_detail_format.push_back(detail_fmt);
			continue; // 后面的参数值无效，只认上面读取的文本即可
		}

		// 如果标识以两个半角的$$符号结尾，表示单元格中内容的控件如果是带下拉列表的，则按原来字符串
		// 解析，否则按原来的做法采用从0开始的索引号解析【应韩厚正要求加的，目的是他想要字符串以便
		// 更好的进行序列化，并且考虑兼容原来老数据才这样扩展的】
		
		// 到2014年1月25日为止，在钢筋表格的条目名称单元格中用到了两种标志字符串，分别是“##”和“$$”
		// 两处地方都是认为标志在最后，但由于“$$”的检测在读取表格格式信息时进行，“##”则在根据格式
		// 信息创建表格时进行，所以当两个标志都需要时要求“$$”必须放在“##”之后，目前出现这种情况的
		// 地方在大样信息表，其它子表只用到其中的一个，因为检测完标志后必须从字符串中删除。
		// 特作注释，如果以后要再加可能需要重构代码，统一在读取格式信息时全部检查并保存这些标志，这样
		// 可以做到扩展多个标识时不用关心标识的先后顺序，而且集中管理标识对程序的健壮性有好处，目前暂
		// 时不改【WYZ】

		CString strEnd = detail_fmt.m_item_name.Right(2); // 按条目名称，而不是钢筋名称，即按子项目
		if (_T("$$") == strEnd)
		{
			detail_fmt.m_item_name.Delete(detail_fmt.m_item_name.GetLength() - 2, 2);
			detail_fmt.m_using_string_value = true;
		}

		if (detail_fmt.m_id.IsEmpty()) // 表格不限制标识中使用空格,所以不Trim()
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据主标识错误，【%s】主标识和子标识之间用一个半角下划线分隔且不能为空"), detail, i, cell_tag);
			// 通用概念筋说明文档.docx中注明必须同时使用主标识和子标识,且主标识和子标识之间用一个半角下划线分隔
			detail_fmt.m_id = _T("表格数据主标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		if (detail_fmt.m_sub_id.IsEmpty())
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】第%d行数据子标识错误，【%s】主标识和子标识之间用一个半角下划线分隔且不能为空"), detail, i, cell_tag);
			detail_fmt.m_sub_id = _T("表格数据子标识错误"); // 强制改为这个可以让使用数据的人调试时更容易知道错误原因
		}

		// 条目名称
		cell = pDB->GetCell(i, 1);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，区域名称GetCell(%d,1)失败"), detail, i);
			continue;
		}
		detail_fmt.m_item_name = cell->GetText();

		// 参数值
		cell = pDB->GetCell(i, 2);
		if (NULL == cell)
		{
			app_print_info(MT_DEBUG, _T(">>>>表格模板【%s】列数不够，布置间距GetCell(%d,2)失败"), detail, i);
			continue;
		}
		get_table_format_single_cell(cell, detail_fmt.m_value);

		format.m_detail_format.push_back(detail_fmt);

	} // end of for(...)

	safe_delete(pDB);
}

void get_table_format(const CString& caption, const CString& desing, const CString& basic,
	const CString& layout, const CString& edge, const CString& detail, steel_grid_format& format)
{
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>开始读取并解析表格模板信息"));

	// 先获取模板名存储起来以便于表格调试
	format.m_vecTemplateName.empty();
	format.m_vecTemplateName.push_back(caption);
	format.m_vecTemplateName.push_back(desing);
	format.m_vecTemplateName.push_back(basic);
	format.m_vecTemplateName.push_back(layout);
	format.m_vecTemplateName.push_back(edge);
	format.m_vecTemplateName.push_back(detail);

	// 获取表格格式信息
	get_table_format_caption(caption, format);
	get_table_format_design(desing, format);
	get_table_format_basic(basic, format);
	get_table_format_layout(layout, format);
	get_table_format_edge(edge, format);
	get_table_format_detail(detail, format);

	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>完成读取并解析表格模板信息"));
}
//////////////////////////////////////////////////////////////////////////

void create_steel_main(CTHDataBase* db, const steel_grid_format& format, bool using_component) // 是否启用应用部位
{
	int start_row = 1; // 起始行
	int start_column = 1; // 起始列
	create_steel_main_caption(db, format, start_row, start_column, using_component); // 表头
	
	// 一级标题的设置要表格创建好后设置，否则容易因设置其它信息导致这个设置失效，所以此语句移到外面一层函数尾部
	// db->SetTitleFormat(start_row, start_column, RT_TABLETITLE);

	start_row += 2;
	start_column = 2;
	create_steel_main_design(db, format, start_row, start_column);		// 设计思想
	db->SetTitleFormat(start_row, start_column, RT_SUBTITLE);

	start_row += int(format.m_design_format.size() + 1);
	start_column = 2;
	create_steel_main_basic (db, format, start_row, start_column);		// 基本信息
	db->SetTitleFormat(start_row, start_column, RT_SUBTITLE);

	start_row += int(format.m_basic_format.size() + 2);
	start_column = 2;
	create_steel_main_layout(db, format, start_row, start_column);		// 布置信息
	db->SetTitleFormat(start_row, start_column, RT_SUBTITLE);

	start_row += int(format.m_layout_format.size() + 2);
	start_column = 2;
	create_steel_main_edge(db, format, start_row, start_column);		// 边距信息
	db->SetTitleFormat(start_row, start_column, RT_SUBTITLE);

	start_row += int(format.m_edge_format.size() + 2);
	start_column = 2;
	create_steel_main_detail(db, format, start_row, start_column);		// 大样信息
	db->SetTitleFormat(start_row, start_column, RT_SUBTITLE);
}

// 计算表总共要画多少行
int calculate_total_row_count(const steel_grid_format& format)
{
	int row_count = 1; // 包含隐藏的第0行
	row_count += 2; // caption
	row_count += int(format.m_design_format.size() + 1);
	row_count += int(format.m_basic_format.size() + 2);
	row_count += int(format.m_layout_format.size() + 2);
	row_count += int(format.m_edge_format.size() + 2);
	row_count += int(format.m_detail_format.size() + 2);

	return row_count;
}

// 创建表，需要指定五个子表的模板名和默认的应用部位
CTHDataBase* steel_grid_tools::create_steel_grid(IN const CString& caption, IN const CString& design_grid,
	IN const CString& basic_grid, IN const CString& layout_grid, IN const CString& edge_grid,
	IN const CString& detail_grid, IN bool is_fixed /*= true*/, /* 钢筋表是固定的，还是可由调用者增加、删除钢筋表 */ IN bool enable_component /*= false*/)
{
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T("开始调用CSteelGrid::create_steel_grid()创建钢筋表..."));
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>1.表头模板:%s"), strTmpCaption);
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>2.设计思想模板:%s"), strTmpDesing);
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>3.基本信息模板:%s"), strTmpBase);
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>4.布置信息模板:%s"), strTmpArrange);
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>5.边距信息模板:%s"), strTmpEdge);
	////// CLOG::GetLog()->Write(LOG_PLAINTEXT, _T(">>6.大样信息模板:%s"), strTmpDetail);

	// 获取DataBase模板中的单元格格式信息
	steel_grid_format format;
	get_table_format(caption, design_grid, basic_grid, layout_grid, edge_grid, detail_grid, format);

	// 根据格式创建表格
	CTHDataBase* db = new CTHDataBase();

	// 计算要多少行，多少列
	int nColCount = 10;
	int nRowCount = calculate_total_row_count(format);
	// 设置行列数
	db->SetRowCount(nRowCount);
	db->SetColCount(nColCount); // 列数一旦变化，则FrameInfo和FrameNodes中也需要变化
	db->SetFixedRowCount(1);
	db->SetFixedColCount(1);

	db->EnableRowAutoSerialNumber(TRUE);
	db->EnableColAutoSerialNumber(TRUE);
	db->EnableMergeMenu(TRUE);
	db->EnableColMenu(FALSE); // 禁止列菜单（列菜单可以插入列，删除列）
	db->EnableRowGroup(TRUE);

	// 创建表格
	create_steel_main(db, format, enable_component);

	// 行高为0则表示隐藏行，列宽为0表示隐藏列
	db->EnableRowResize(TRUE);
	db->SetRowHeight(0, 0);
	db->SetRowHeight(1, 30);
	db->SetColWidth(0, 0);

	if (is_fixed)
	{
		db->SetColWidth(1, 0);
		db->EnableRowMenu(FALSE); // 禁止行菜单，可以禁止增加行、删除行相关的快捷键和菜单选项
	}
	else
	{
		db->EnableRowMenu(TRUE);
		db->SetColWidth(1, 30);
	}
	db->SetColWidth(2, 100);
	db->SetColWidth(3, 200);
	//pDatabase->SetColWidth(3, 50);
	//pDatabase->SetColWidth(4, 80);
	//db->SetColWidth(5, 50);
	//db->SetColWidth(6, 60);
	//db->SetColWidth(7, 50);

	// 为保证创建和内部算法的需要（特别是取子表行的需要），表格必须完整的创建，
	// 但是有的表模板名为空，这时需要把这些表所对应的标题行也隐藏起来，调用以
	// 下函数就是为了让其能隐藏不需要的表。【表头不可隐藏】
	if (_T("") == design_grid) hide_sub_grid(*db, ST_DESIGN);
	if (_T("") == basic_grid) hide_sub_grid(*db, ST_BASIC);
	if (_T("") == layout_grid) hide_sub_grid(*db, ST_LAYOUT);
	if (_T("") == edge_grid) hide_sub_grid(*db, ST_EDEG);
	if (_T("") == detail_grid) hide_sub_grid(*db, ST_DETAIL);

	// 设计好格式之后再处理表属性及分组
	db->EnableRowResize(FALSE);
	db->EnableColMenu(FALSE);
	db->EnableRowMenu(is_fixed ? FALSE : TRUE);	

	// 设置表格分组
	db->SetAsRowGroup(2, nRowCount - 1, 1, GP_STEEL, TRUE, 1);
	db->SetTitleFormat(1, 0, RT_TABLETITLE); // 一级标题的设置要表格创建好后设置，否则容易因设置其它信息导致这个设置失效

	// 悄悄放一个表格模板名在0,0单元格的下拉文本中以便调试信息输出(表格没有放这种信息的地方，只能先这样处理)
	CString template_name = _T("五合一钢筋表:") + caption + _T("、") + design_grid + _T("、")
		+ basic_grid + _T("、") + layout_grid + _T("、") + edge_grid + _T("、") + detail_grid;
	CTHCell* cell = db->GetCell(0, 0);
	cell->SetLabelText(template_name);

	// 分组结束之后，就认为是模板创建结束
	//////////////////////////////////////////////////////////////////////////

	// 创建完成后不用刷新内部的环境变量了，这个动作由创建表格的人创建后设置环境变量即可
	db->AutoFillRowNo(); // 这个函数会更新内部的缓存数据，否则行组信息可能错误。[Add By WYZ 2012年8月20日19:50:49]
	return db;
}

void get_single_steel_grid_data_caption(/*const */CGridDataBase& grid, CString& strComponent)
{
	int nRowCount = grid.GetRowCount();
	int start_column = 4; // 第一个用户可输入数据的单元格所在的列数
	CTHCell *cell;
	CString cell_tag;
	CString strText;

	for (int i = 0; i < nRowCount; ++i)
	{
		cell = grid.GetCell(i, start_column);

		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(i, j); 返回空指针"), false));
			return;
		}

		cell_tag = cell->GetTag();
		strText = cell->GetText();

		if (TAG_STEEL_COMPONENT_VALUE == cell_tag)
		{
			strComponent = strText;
			return;
		} 
		else
		{
			// Nothing;
		}
	} // end of for (int i = 0; i < nRowCount; ++i)
}

// 计算strItem在strText中的索引号，索引号基于0
// 数据涉及到以后概念筋的数据转换，如果不存在，也返回0，保证它有一个合法的数据完成其它模块的数据处理
// strText中的数据使用\n来分隔
int get_item_index(IN const CString& strItem, IN const CString& text)
{
	vector<CString>all_text_items = string_tools::split_string(text, _T("\n"));
	CString strTmpItem = strItem;
	strTmpItem.Trim(); // 去除首尾两端的空格，可以保证结果尽可能的匹配正确

	for (size_t i = 0; i != all_text_items.size(); ++i)
		all_text_items[i].Trim(); // 去除首尾两端的空格，可以保证结果尽可能的匹配正确

	for (size_t i = 0; i != all_text_items.size(); ++i)
		if (all_text_items[i] == strItem) return int(i);

	return 0;
}

// 返回字符串text中索引号为index的文本，如果不存在，也返回0，保证它有一个合法的数据完成其它模块的数据处理
// text中的数据使用\n来分隔
// 注意，本函数不能处理设置环境变量的单元格，环境变量只能存储或传字面字符串给绘图模块，不能传序号，
// 所以会特殊处理，不会用到本模块。
const CString get_item_text(IN const CString& cond_value, IN bool using_string_value, IN const CString& text)
{
	if (using_string_value) return cond_value;

	int index = _ttoi(cond_value);

	if (index < 0 || _T("") == text) return _T("");

	CString trimed_text = text;
	trimed_text.Trim();

	vector<CString>all_text_items = string_tools::split_string(trimed_text, _T("\n"));
	if (all_text_items.empty()) return _T("");

	if ( int(all_text_items.size()) <= index || index < 0) return _T("");
	
	return all_text_items[index];
}

// 检测一个字符串是否是环境变量格式
// 环境变量的格式要求文本以@开头的字符串
bool is_environment_variable(const CString strText)
{
	if (strText.GetLength() <= 1) return false;
	if (_T('@') == strText[0]) return true; else return false;
}

void get_single_steel_grid_data_design(/*const */CGridDataBase& grid, single_steel_data& steel, bool bReplaceIDBySteelName)
{
	int nRowCount = grid.GetRowCount();
	int start_column = 4; // 第一个用户可输入数据的单元格所在的列数
	int nColTitleStart = 2; // 第一个条目标题单元格所在的列数
	CTHCell *cell;
	CString cell_tag;
	CString strText;
	steel_design_item design;
	int nRowS = 0;
	int nRowE = 0;

	steel.m_design_items.clear();

	// 先找到设计思想这一个标题所在的行
	for (int i = 0; i < nRowCount; ++i)
	{
		cell_tag = grid.GetRowTag(i);
		if (TAG_ROW_DESIGN == cell_tag) nRowS = i;
		if (TAG_ROW_BASE == cell_tag) nRowE = i;
	}

	if (nRowE <= 0 || nRowS <= 0) // 起始行和终止行如果有一个为初始的0说明没有这一行
	{
		// _ASSERTE((_T("表格错误，没有找到匹配行"), false)); 当钢筋表为0个，只有一个总标题时会出这个断言，这也算是合法数据的极端情况，不要断言
		return;
	}

	if (nRowE < nRowS + 2) // 设计思想与下一个标题行（基本信息）如果有数据至少相差2行
	{
		// 表格没有设计思想数据
		return;
	}

	// 获取数据被大量调用，变量放在循环外，从效率来讲好一点
	CString strLabel;
	int nCellType = 0;
	int index = 0;

	for (int i = nRowS + 1; i < nRowE; ++i) // 有效数据为标题行后面一行
	{
		cell = grid.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}

		// 以下为处理逻辑：
		// 获取单元格的控件类型
		// 获取单元格的LabelText
		// 获取单元格的Text文本
		// 按以下逻辑对design.m_value进行赋值：
		// 如果为checkbox选中为1，未选中为0
		// 如果有下拉列表，则一律传索引值，索引从0开始（咨询过韩厚正可以这么做）
		// 如果为其它类型的控件，则直接赋值
		// design.m_strID则直接赋予cell_tag值

		//////////////////////////////////////////////////////////////////////////
		// 说明：
		//     如果表格中的文本就是实际的数据的字符形式，则在制作表格时需要把单元格
		//     类型设置为CELLDATA_STRING，表示按字符串字面值取
		//     如果表格中的文本只是用于显示选项，而需要的是各项对应的序号，则要把
		//     单元格的类型设置为CELLDATA_INT这样算法才会取索引号
		//////////////////////////////////////////////////////////////////////////

		cell_tag = grid.GetRowTag(i);
		if (STR_FEN_GE == cell_tag) continue; // 分隔带不转换

		// 获取数据
		strText = cell->GetText();
		strText.Trim(); // 防止可编辑的单元格首尾有不可见的空格
		nCellType = cell->GetCellType();
		strLabel = cell->GetLabelText(); // 任何类型的控件都有LabelText，只不过只在有下拉列表的控件中才生效
		index = get_item_index(strText, strLabel); // 仅支持单个条目选择

		// 按逻辑处理数据，为设计思想赋值
		if (bReplaceIDBySteelName)
		{
			CTHCell* pNameCell = grid.GetCell(i, nColTitleStart);
			if (pNameCell)
			{
				design.m_id = pNameCell->GetText();
				design.m_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}
		}
		else
		{
			design.m_id = cell_tag;
		}

		if (CT_CHECKBOX == nCellType)
		{
			design.m_value = strText; // CheckBox本身的字符串选中就是"1"，没选中就是"0"
		}
		else if ( (CT_DROPLIST == nCellType || CT_DROPDOWN == nCellType)
			&& false == is_environment_variable(strLabel) ) // 目前只支持这两种有下拉列表的控件，其它有下拉列表的控件还用不到，如有需要，以后再加
		{
			// 如果是环境变量要特殊处理，因为目前只有下拉列表才支持环境变量，当下拉列表为
			// 环境变量的时候，大多数情况下它需要的字符串内容而不是索引值，所以对不同的环
			// 境变量作特殊处理并覆盖原来的值。

			// 取出来的内容不能直接与目标字符串比较，因为表格中取出来的字符串多一个换行符
			// e.g: (ENV_HUN_NING_TU == strLabel) // Be FALSE when strLabel is "@HNTLX", because the '\n' is HIDE
			strLabel.Trim(); // 去除两端的空白字符（包括空格、换行、回车、制表符）

			//////////////////////////////////////////////////////////////////////////
			// 老的代码是依次判断strLabel是否为某个环境变量，当增加新的环境变量时，代码也要添加分支，不太好维护，
			// 现改为统一处理，可避免这个问题，如果某个环境变量需要特殊处理，可用原来的模式，再将原来的代码也注
			// 释在下面，以备需要进参考。(大样信息和设计思想采用同样的方式处理，注意上面的if中多加的IsEnvironmentVariable条件)
			//////////////////////////////////////////////////////////////////////////
			//	if (ENV_GANG_JIAO_XIAN == strLabel)
			//	{
			//		strText = design.m_value;
			//	}	
			//	else if (ENV_HUN_NING_TU == strLabel)
			//	{
			//		strText = design.m_value;
			//	}
			//	else if (ENV_SHENG_CE_GUAN == strLabel)
			//	{
			//		strText = design.m_value;
			//	}
			////////////////////////////////////////////////////////////////////////// 

			// 应韩厚正2014年1月23日要求，下拉框形式进字符串类型的变量要原样传给钢筋，他说是为了
			// 好做序列化，考虑整个框架不会造成老代码受影响，具体参见相应的头文件
			CTHCell* pCellItem = grid.GetCell(i, start_column - 1); // 是否按原样字符串处理这个标记存放在条目名单元格的扩展数据中，具体可看创建表格的代码
			CString strExtData = _T("");
			if (pCellItem) pCellItem->GetExtendData(_T("$$"), strExtData);

			bool bValueByStr = (_T("$$") == strExtData ? true : false);
			design.m_using_string_value = bValueByStr;
			if (bValueByStr)
				design.m_value = strText;
			else
				design.m_value.Format(_T("%d"), index);
		}
		else // 其它类型的控件
		{
			design.m_value = strText;
		}

		steel.m_design_items.push_back(design);

	} // end of for (int i = 0; i < nRowCount; ++i)

}

void get_single_steel_grid_data_basic(/*const */CGridDataBase& grid, single_steel_data& steel, bool bReplaceIDBySteelName)
{
	int nRowCount = grid.GetRowCount();
	int start_column = 4; // 第一个用户可输入数据的单元格所在的列数
	int nColTitleStart = 2; // 第一个条目标题单元格所在的列数
	CTHCell *cell;
	CString cell_tag;
	CString strText;
	steel_basic_item base;
	int nRowS = 0;
	int nRowE = 0;

	steel.m_basic_items.clear();

	// 先找到设计思想这一个标题所在的行
	for (int i = 0; i < nRowCount; ++i)
	{
		cell_tag = grid.GetRowTag(i);
		if (TAG_ROW_BASE == cell_tag) nRowS = i;
		if (TAG_ROW_ARRANGE == cell_tag) nRowE = i;
	}

	if (nRowE <= 0 || nRowS <= 0) // 起始行和终止行如果有一个为初始的0说明没有这一行
	{
		// _ASSERTE((_T("表格错误，没有找到匹配行"), false)); 当钢筋表为0个，只有一个总标题时会出这个断言，这也算是合法数据的极端情况，不要断言
		return;
	}

	if (nRowE < nRowS + 3) // 基本信息与下一个标题行（布置信息）如果有数据至少相差3行
	{
		// 表格没有基本信息数据
		return;
	}

	for (int i = nRowS + 2; i < nRowE; ++i) // 有效数据为标题行后面两行
	{
		cell = grid.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}

		cell_tag = grid.GetRowTag(i);
		if (STR_FEN_GE == cell_tag) continue; // 分隔带不转换

		if (bReplaceIDBySteelName)
		{
			CTHCell* pNameCell = grid.GetCell(i, nColTitleStart);
			if (pNameCell)
			{
				base.m_id = pNameCell->GetText();
				base.m_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}
		}
		else
		{
			base.m_id = cell_tag;
		}

		strText = cell->GetText();
		base.m_diameter = strText;

		cell = grid.GetCell(i, start_column + 1);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		base.m_steel_type = strText;

		cell = grid.GetCell(i, start_column + 2);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		base.m_hook_type = strText;

		cell = grid.GetCell(i, start_column + 3);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		base.m_distance = strText;

		cell = grid.GetCell(i, start_column + 4);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		base.m_force_type = strText;

		steel.m_basic_items.push_back(base);

	} // end of for (int i = 0; i < nRowCount; ++i)

}

void get_single_steel_grid_data_layout(/*const */CGridDataBase& grid, single_steel_data& steel, bool bReplaceIDBySteelName)
{
	int nRowCount = grid.GetRowCount();
	int start_column = 4; // 第一个用户可输入数据的单元格所在的列数
	int nColTitleStart = 2; // 第一个条目标题单元格所在的列数
	CTHCell *cell;
	CString cell_tag;
	CString strText;
	steel_layout_item arrange;
	int nRowS = 0;
	int nRowE = 0;

	steel.m_layout_items.clear();

	// 先找到设计思想这一个标题所在的行
	for (int i = 0; i < nRowCount; ++i)
	{
		cell_tag = grid.GetRowTag(i);
		if (TAG_ROW_ARRANGE == cell_tag) nRowS = i;
		if (TAG_ROW_EDGE == cell_tag) nRowE = i;
	}

	if (nRowE <= 0 || nRowS <= 0) // 起始行和终止行如果有一个为初始的0说明没有这一行
	{
		// _ASSERTE((_T("表格错误，没有找到匹配行"), false)); 当钢筋表为0个，只有一个总标题时会出这个断言，这也算是合法数据的极端情况，不要断言
		return;
	}

	if (nRowE < nRowS + 3) // 布置信息与下一个标题行（边距信息）如果有数据至少相差3行
	{
		// 表格没有布置信息数据
		return;
	}

	for (int i = nRowS + 2; i < nRowE; ++i) // 有效数据为标题行后面两行
	{
		cell = grid.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}

		cell_tag = grid.GetRowTag(i);
		if (STR_FEN_GE == cell_tag) continue; // 分隔带不转换

		if (bReplaceIDBySteelName)
		{
			// 先取子标识对应的钢筋名
			CTHCell* pNameCell = grid.GetCell(i, nColTitleStart + 1);
			if (pNameCell)
			{
				arrange.m_sub_id = pNameCell->GetText();
				arrange.m_sub_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}

			// 后取主标识对应的钢筋名，原因是主标识可能因为合并了，要从合并区的第一个单元格取，而不是
			// 当前单元格的左边
			// 如果当前单元格被合并了，则取合并区开始的一个单元格内容，否则钢筋名不正确
			pNameCell = grid.GetCell(i, nColTitleStart);
			if (pNameCell->IsMerged(nColTitleStart, i))
			{
				int nLeftTopX = 0;
				int nLeftTopY = 0;

				pNameCell->GetMerge(nLeftTopX, nLeftTopY);
				pNameCell = grid.GetCell(nLeftTopX, nLeftTopY);
			}

			if (pNameCell)
			{
				arrange.m_id = pNameCell->GetText();
				arrange.m_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}
		}
		else
		{
			split_steel_geid_line_id(cell_tag, arrange.m_id, arrange.m_sub_id);
		}

		strText = cell->GetText();
		arrange.m_space_distance = strText;

		cell = grid.GetCell(i, start_column + 1);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		arrange.m_space_count = strText;

		steel.m_layout_items.push_back(arrange);

	} // end of for (int i = 0; i < nRowCount; ++i)

}

void get_single_steel_grid_data_edge(/*const */CGridDataBase& grid, single_steel_data& steel, bool bReplaceIDBySteelName)
{
	int nRowCount = grid.GetRowCount();
	int start_column = 4; // 第一个用户可输入数据的单元格所在的列数
	int nColTitleStart = 2; // 第一个条目标题单元格所在的列数
	CTHCell *cell;
	CString cell_tag;
	CString strText;
	steel_edge_item edge;
	int nRowS = 0;
	int nRowE = 0;

	steel.m_edge_items.clear();

	// 先找到设计思想这一个标题所在的行
	for (int i = 0; i < nRowCount; ++i)
	{
		cell_tag = grid.GetRowTag(i);
		if (TAG_ROW_EDGE == cell_tag) nRowS = i;
		if (TAG_ROW_DETAIL == cell_tag) nRowE = i;
	}

	if (nRowE <= 0 || nRowS <= 0) // 起始行和终止行如果有一个为初始的0说明没有这一行
	{
		// _ASSERTE((_T("表格错误，没有找到匹配行"), false)); 当钢筋表为0个，只有一个总标题时会出这个断言，这也算是合法数据的极端情况，不要断言
		return;
	}

	if (nRowE < nRowS + 3) // 边距信息与下一个标题行（大样信息）如果有数据至少相差3行
	{
		// 表格没有基本信息数据
		return;
	}

	for (int i = nRowS + 2; i < nRowE; ++i) // 有效数据为标题行后面两行
	{
		cell = grid.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}

		cell_tag = grid.GetRowTag(i);
		if (STR_FEN_GE == cell_tag) continue; // 分隔带不转换

		if (bReplaceIDBySteelName)
		{
			// 先取子标识对应的钢筋名
			CTHCell* pNameCell = grid.GetCell(i, nColTitleStart + 1);
			if (pNameCell)
			{
				edge.m_sub_id = pNameCell->GetText();
				edge.m_sub_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}

			// 后取主标识对应的钢筋名，原因是主标识可能因为合并了，要从合并区的第一个单元格取，而不是
			// 当前单元格的左边
			// 如果当前单元格被合并了，则取合并区开始的一个单元格内容，否则钢筋名不正确
			pNameCell = grid.GetCell(i, nColTitleStart);
			if (pNameCell->IsMerged(nColTitleStart, i))
			{
				int nLeftTopX = 0;
				int nLeftTopY = 0;

				pNameCell->GetMerge(nLeftTopX, nLeftTopY);
				pNameCell = grid.GetCell(nLeftTopX, nLeftTopY);
			}

			if (pNameCell)
			{
				edge.m_id = pNameCell->GetText();
				edge.m_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}
		}
		else
		{
			split_steel_geid_line_id(cell_tag, edge.m_id, edge.m_sub_id);
		}

		strText = cell->GetText();
		edge.m_location = strText;

		cell = grid.GetCell(i, start_column + 1);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		edge.m_logic_type = strText;

		cell = grid.GetCell(i, start_column + 2);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		edge.m_edge_distance = strText;

		cell = grid.GetCell(i, start_column + 3);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		edge.m_round_level = strText;

		cell = grid.GetCell(i, start_column + 4);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		edge.m_round_type = strText;

		cell = grid.GetCell(i, start_column + 5);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}
		strText = cell->GetText();
		edge.m_edge_expect = strText;

		steel.m_edge_items.push_back(edge);

	} // end of for (int i = 0; i < nRowCount; ++i)

}

void get_single_steel_grid_data_detail(/*const */CGridDataBase& grid, single_steel_data& steel, bool bReplaceIDBySteelName)
{
	int nRowCount = grid.GetRowCount();
	int start_column = 4; // 第一个用户可输入数据的单元格所在的列数
	int nColTitleStart = 2; // 第一个条目标题单元格所在的列数
	CTHCell *cell;
	CString cell_tag;
	CString strText;
	steel_detail_item detail;
	int nRowS = 0;
	int nRowE = 0;

	steel.m_detail_items.clear();

	// 先找到大样这一个标题所在的行
	for (int i = 0; i < nRowCount; ++i)
	{
		cell_tag = grid.GetRowTag(i);
		if (TAG_ROW_DETAIL == cell_tag) nRowS = i;
	}
	nRowE = nRowCount;

	if (nRowE <= 0 || nRowS <= 0) // 起始行和终止行如果有一个为初始的0说明没有这一行
	{
		// _ASSERTE((_T("表格错误，没有找到匹配行"), false)); 当钢筋表为0个，只有一个总标题时会出这个断言，这也算是合法数据的极端情况，不要断言
		return;
	}

	if (nRowE < nRowS + 2) // 大样信息与到总行数如果有数据至少相差2行
	{
		// 表格没有大样信息数据
		return;
	}

	// 获取数据被大量调用，变量放在循环外，从效率来讲好一点
	CString strLabel;
	int nCellType = 0;
	int index = 0;

	for (int i = nRowS + 2; i < nRowE; ++i) // 有效数据为标题行后面两行
	{
		cell = grid.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("cell = grid.GetCell(...); 返回空指针"), false));
			return;
		}

		// 以下为处理逻辑：
		// 获取单元格的控件类型
		// 获取单元格的LabelText
		// 获取单元格的Text文本
		// 按以下逻辑对design.m_value进行赋值：
		// 如果为checkbox选中为1，未选中为0
		// 如果有下拉列表，则一律传索引值，索引从0开始（咨询过韩厚正可以这么做）
		// 如果为其它类型的控件，则直接赋值
		// detail.m_strID则直接赋予cell_tag值

		//////////////////////////////////////////////////////////////////////////
		// 说明：
		//     如果表格中的文本就是实际的数据的字符形式，则在制作表格时需要把单元格
		//     类型设置为CELLDATA_STRING，表示按字符串字面值取
		//     如果表格中的文本只是用于显示选项，而需要的是各项对应的序号，则要把
		//     单元格的类型设置为CELLDATA_INT这样算法才会取索引号
		//////////////////////////////////////////////////////////////////////////

		cell_tag = grid.GetRowTag(i);
		if (STR_FEN_GE == cell_tag) continue; // 分隔带不转换

		// 获取数据
		strText = cell->GetText();
		strText.Trim(); // 防止可编辑的单元格首尾有不可见的空格
		nCellType = cell->GetCellType();
		strLabel = cell->GetLabelText(); // 任何类型的控件都有LabelText，只不过只在有下拉列表的控件中才生效
		index = get_item_index(strText, strLabel);

		// 按逻辑处理数据，为大样赋值
		if (bReplaceIDBySteelName)
		{
			// 先取子标识对应的钢筋名
			CTHCell* pNameCell = grid.GetCell(i, nColTitleStart + 1);
			if (pNameCell)
			{
				detail.m_sub_id = pNameCell->GetText();
				detail.m_sub_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}

			// 后取主标识对应的钢筋名，原因是主标识可能因为合并了，要从合并区的第一个单元格取，而不是
			// 当前单元格的左边
			// 如果当前单元格被合并了，则取合并区开始的一个单元格内容，否则钢筋名不正确
			pNameCell = grid.GetCell(i, nColTitleStart);
			if (pNameCell->IsMerged(nColTitleStart, i))
			{
				int nLeftTopX = 0;
				int nLeftTopY = 0;

				pNameCell->GetMerge(nLeftTopX, nLeftTopY);
				pNameCell = grid.GetCell(nLeftTopX, nLeftTopY);
			}

			if (pNameCell)
			{
				detail.m_id = pNameCell->GetText();
				detail.m_id.Trim(_T(", ;$*#")); // 去除首尾的空白字符及特殊的模板标记
			}
		}
		else
		{
			split_steel_geid_line_id(cell_tag, detail.m_id, detail.m_sub_id);
		}

		if (CT_CHECKBOX == nCellType)
		{
			detail.m_value = strText; // CheckBox本身的字符串选中就是"1"，没选中就是"0"
		}
		else if ( (CT_DROPLIST == nCellType || CT_DROPDOWN == nCellType )
			&& false == is_environment_variable(strLabel) ) // 目前只支持这两种有下拉列表的控件，其它有下拉列表的控件还用不到，如有需要，以后再加
		{
			// 如果是环境变量要特殊处理，因为目前只有下拉列表才支持环境变量，当下拉列表为
			// 环境变量的时候，大多数情况下它需要的字符串内容而不是索引值，所以对不同的环
			// 境变量作特殊处理并覆盖原来的值。

			// 取出来的内容不能直接与目标字符串比较，因为表格中取出来的字符串多一个换行符
			// e.g: (ENV_HUN_NING_TU == strLabel) // Be FALSE when strLabel is "@HNTLX", because the '\n' is HIDE
			strLabel.Trim(); // 去除两端的空白字符（包括空格、换行、回车、制表符）

			//////////////////////////////////////////////////////////////////////////
			// 老的代码是依次判断strLabel是否为某个环境变量，当增加新的环境变量时，代码也要添加分支，不太好维护，
			// 现改为统一处理，可避免这个问题，如果某个环境变量需要特殊处理，可用原来的模式，再将原来的代码也注
			// 释在下面，以备需要进参考。(大样信息和设计思想采用同样的方式处理，注意上面的if中多加的IsEnvironmentVariable条件)
			//////////////////////////////////////////////////////////////////////////
			//	if (ENV_GANG_JIAO_XIAN == label_text)
			//	{
			//		strText = detail.m_value;
			//	}	
			//	else if (ENV_HUN_NING_TU == label_text)
			//	{
			//		strText = detail.m_value;
			//	}
			//	else if (ENV_SHENG_CE_GUAN == label_text)
			//	{
			//		strText = detail.m_value;
			//	}
			////////////////////////////////////////////////////////////////////////// 

			// 应韩厚正2014年1月23日要求，下拉框形式进字符串类型的变量要原样传给钢筋，他说是为了
			// 好做序列化，考虑整个框架不会造成老代码受影响，具体参见相应的头文件
			CTHCell* pCellItem = grid.GetCell(i, start_column - 1); // 是否按原样字符串处理这个标记存放在条目名单元格的扩展数据中，具体可看创建表格的代码
			CString strExtData = _T("");
			if (pCellItem) pCellItem->GetExtendData(_T("$$"), strExtData);

			bool bValueByStr = (_T("$$") == strExtData ? true : false);
			detail.m_using_string_value = bValueByStr;
			if (bValueByStr)
				detail.m_value = strText;
			else
				detail.m_value.Format(_T("%d"), index);

		}
		else // 其它类型的控件
		{
			detail.m_value = strText;
		}

		steel.m_detail_items.push_back(detail);

	} // end of for (int i = 0; i < row_count; ++i)
}

void get_single_steel_grid_data(/*const*/ CTHDataBase& db, steel_data& puData, bool bReplaceIDBySteelName)
{
	// 获取子表前必须调用,否则内部缓存有问题,无法正确获取子表数,此问题在新构件启用自定时设计方法
	// 时发现,早期表格没有此问题,由于这个函数有点耗时,所以只要表格行组数没有变化时,只用调一次就
	// 可以,当表格结构发后变化时,最好再重新调用,为保证表填值正确,在最开始时调用一次
	// [Add By WYZ 2012年9月7日10:27:27]
	db.AutoFillRowNo();

	// 使用子表前先调用这个函数准备子表数据
	db.GetSubDataBaseCount();

	CGridDataBase steel_db; // 多个钢筋子表
	db.GetSubDataBase(0, steel_db);
	int grid_count = steel_db.GetSubDataBaseCount();
	if (grid_count <= 0) return;

	puData.clear_steel_data();
	puData.resize_steel_count(grid_count);
	for (int i = 0; i < grid_count; ++i)
	{
		single_steel_data& single = puData.get_steel_data(i);
		CGridDataBase tmp_db; // 一个钢筋子表
		steel_db.GetSubDataBase(i, tmp_db);
		get_single_steel_grid_data_caption(tmp_db, single.m_component);
		get_single_steel_grid_data_design(tmp_db, single, bReplaceIDBySteelName);
		get_single_steel_grid_data_basic(tmp_db, single, bReplaceIDBySteelName);
		get_single_steel_grid_data_layout(tmp_db, single, bReplaceIDBySteelName);
		get_single_steel_grid_data_edge(tmp_db, single, bReplaceIDBySteelName);
		get_single_steel_grid_data_detail(tmp_db, single, bReplaceIDBySteelName);
	}
}

// 不使用const的原因是内部获取子表的函数需要非const的参数
void steel_grid_tools::get_steel_data_from_grid(IN CTHDataBase& db, OUT steel_data& data,
	IN bool parse_by_component /*= false*/, IN bool replace_steel_item_id_to_name /*= false*/)
{
	data.clear_steel_data();

	steel_data tmp_data;
	get_single_steel_grid_data(db, tmp_data, replace_steel_item_id_to_name);

	// parse_by_component为false时不解析应用部位[Edit By WYZ]
	if (!parse_by_component)
	{
		data = tmp_data;
		return;
	}

	// 解析应用部位
	for (size_t i = 0; i < tmp_data.get_steel_count(); ++i)
	{
		single_steel_data& single_data = tmp_data.get_steel_data(i);

		vector<CString> all_pos;
		string_tools::split_string(single_data.m_component, _T(" ,;"));

		if (all_pos.empty())
		{
			data.add_single_steel(single_data);
			continue;
		}

		// 一个钢筋表中指定了多个应用部位
		for (size_t j = 0; j < all_pos.size(); ++j)
		{
			single_steel_data new_steel = single_data;
			new_steel.m_component = all_pos[j];
			data.add_single_steel(new_steel);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void set_single_steel_grid_data_caption(CTHDataBase& grid, const CString& strComponent)
{
	// 标题行的数据在单个钢筋表的第1行
	CTHCell *cell = grid.GetCell(2, 4);
	if (!cell)
	{
		_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
		return;
	}

	cell->SetText(strComponent);

}

// start_row : 数据行的起始行, end_row : 数据行的终止行, start_column : 表格的起始列
void set_single_steel_grid_data_design(CTHDataBase& db, const single_steel_data& p0, int start_row, int end_row, int start_column)			
{
	if (start_row <= 0 || end_row <= 0 || start_column <= 0) return; // 没有0行的数据行，0表示表格中没有数据行
	if (start_row > end_row) return;

	CTHCell* cell = nullptr;
	CString cell_tag;
	bool flag = false;
	steel_design_item design;
	for (int i = start_row; i <= end_row; ++i) // 包括起始行和终止行
	{
		cell_tag = db.GetRowTag(i);
		flag = p0.get_design_item(cell_tag, design);
		if (false == flag) continue; // 没有对应标识的数据

		cell = db.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}

		// design.m_value中存储的是下拉列表中的索引号，从0开始，填入界面时先要检查是否是
		// 下拉列表，如果是的话，就把下拉列表中对应索引号的文本填入单元格

		CString label_text;
		int cell_type = 0;
		CString cell_text = 0;

		// 获取数据
		cell_type = cell->GetCellType();
		label_text = cell->GetLabelText(); // 任何类型的控件都有LabelText，只不过只在有下拉列表的控件中才生效
		if (CT_CHECKBOX == cell_type)
		{
			cell_text = design.m_value;
		}
		else if ( (CT_DROPDOWN == cell_type || CT_DROPLIST == cell_type)
			&& false == is_environment_variable(label_text))
		{
			// 目前仅支持这两种下拉控件，如果以后概念筋中需要新增其它类型的下拉列表类控件，又再添加，不过修改的时候要连同获取数据的地方同步操作

			// 老的代码是依次判断strLabel是否为某个环境变量，当增加新的环境变量时，代码也要添加分支，不太好维护，
			// 现改为统一处理，可避免这个问题，如果某个环境变量需要特殊处理，可用原来的模式，再将原来的代码也注
			// 释在下面，以备需要进参考。(大样信息和设计思想采用同样的方式处理，注意上面的if中多加的IsEnvironmentVariable条件)
			//////////////////////////////////////////////////////////////////////////
			//	if (ENV_GANG_JIAO_XIAN == label_text)
			//	{
			//		cell_text = design.m_value;
			//	}	
			//	else if (ENV_HUN_NING_TU == label_text)
			//	{
			//		cell_text = design.m_value;
			//	}
			//	else if (ENV_SHENG_CE_GUAN == label_text)
			//	{
			//		cell_text = design.m_value;
			//	}
			////////////////////////////////////////////////////////////////////////// 

			// 应韩厚正2014年1月23日要求，下拉框形式进字符串类型的变量要原样传给钢筋，他说是为了
			// 好做序列化，考虑整个框架不会造成老代码受影响，具体参见相应的头文件
			cell_text = get_item_text(design.m_value, design.m_using_string_value, label_text);
		}
		else
		{
			cell_text = design.m_value;
		}

		cell->SetText(cell_text);
	}
}

// start_row : 数据行的起始行, end_row : 数据行的终止行, start_column : 表格的起始列
void set_single_steel_grid_data_basic(CTHDataBase& db, const single_steel_data& p0, int start_row, int end_row, int start_column)
{
	if (start_row <= 0 || end_row <= 0 || start_column <= 0) return; // 没有0行的数据行，0表示表格中没有数据行
	if (start_row > end_row) return;

	CTHCell *cell = nullptr;
	CString cell_tag;
	bool flag = false;
	steel_basic_item base;
	for (int i = start_row; i <= end_row; ++i) // 包括起始行和终止行
	{
		cell_tag = db.GetRowTag(i);
		flag = p0.get_basic_item(cell_tag, base);
		if (false == flag) continue; // 没有对应标识的数据

		cell = db.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(base.m_diameter);

		cell = db.GetCell(i, start_column + 1);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(base.m_steel_type);

		cell = db.GetCell(i, start_column + 2);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(base.m_hook_type);

		cell = db.GetCell(i, start_column + 3);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(base.m_distance);

		cell = db.GetCell(i, start_column + 4);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(base.m_force_type);

	}

}

// start_row : 数据行的起始行, end_row : 数据行的终止行, start_column : 表格的起始列
void set_single_steel_grid_data_layout(CTHDataBase& db, const single_steel_data& p0, int start_row, int end_row, int start_column)
{
	if (start_row <= 0 || end_row <= 0 || start_column <= 0) return; // 没有0行的数据行，0表示表格中没有数据行
	if (start_row > end_row) return;

	CTHCell *cell = nullptr;
	CString cell_tag;
	CString main_id;
	CString sub_id;
	bool flag = false;
	steel_layout_item arrange;
	for (int i = start_row; i <= end_row; ++i) // 包括起始行和终止行
	{
		cell_tag = db.GetRowTag(i);
		split_steel_geid_line_id(cell_tag, main_id, sub_id);
		flag = p0.get_layout_item(main_id, sub_id, arrange);
		if (false == flag) continue; // 没有对应标识的数据

		cell = db.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(arrange.m_space_distance);

		cell = db.GetCell(i, start_column + 1);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(arrange.m_space_count);

	}

}

// start_row : 数据行的起始行, end_row : 数据行的终止行, start_column : 表格的起始列
void set_single_steel_grid_data_edge(CTHDataBase& db, const single_steel_data& p0, int start_row, int end_row, int start_column)
{
	if (start_row <= 0 || end_row <= 0 || start_column <= 0) return; // 没有0行的数据行，0表示表格中没有数据行
	if (start_row > end_row) return;

	CTHCell *cell = nullptr;
	CString cell_tag;
	CString main_id;
	CString sub_id;
	bool flag = false;
	steel_edge_item edge;
	for (int i = start_row; i <= end_row; ++i) // 包括起始行和终止行
	{
		cell_tag = db.GetRowTag(i);
		split_steel_geid_line_id(cell_tag, main_id, sub_id);
		flag = p0.get_edge_item(main_id, sub_id, edge);
		if (false == flag) continue; // 没有对应标识的数据

		cell = db.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(edge.m_location);

		cell = db.GetCell(i, start_column + 1);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(edge.m_logic_type);

		cell = db.GetCell(i, start_column + 2);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(edge.m_edge_distance);

		cell = db.GetCell(i, start_column + 3);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(edge.m_round_level);

		cell = db.GetCell(i, start_column + 4);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(edge.m_round_type);

		cell = db.GetCell(i, start_column + 5);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = db.GetCell(...); 返回指针为空"), false));
			continue;
		}
		cell->SetText(edge.m_edge_expect);

	}

}

// start_row : 数据行的起始行, end_row : 数据行的终止行, start_column : 表格的起始列
void set_single_steel_grid_data_detail(CTHDataBase& db, const single_steel_data& p0, int start_row, int end_row, int start_column)
{
	if (start_row <= 0 || end_row <= 0 || start_column <= 0) return; // 没有0行的数据行，0表示表格中没有数据行
	if (start_row > end_row) return;

	CTHCell *cell = nullptr;
	CString cell_tag;
	CString main_id;
	CString sub_id;
	bool flag = false;
	steel_detail_item detail;
	for (int i = start_row; i <= end_row; ++i) // 包括起始行和终止行
	{
		cell_tag = db.GetRowTag(i);
		split_steel_geid_line_id(cell_tag, main_id, sub_id);
		flag = p0.get_detail_item(main_id, sub_id, detail);
		if (false == flag) continue; // 没有对应标识的数据

		cell = db.GetCell(i, start_column);
		if (!cell)
		{
			_ASSERTE((_T("CTHCell *cell = grid.GetCell(...); 返回指针为空"), false));
			continue;
		}

		// detail.m_value中存储的是下拉列表中的索引号，从0开始，填入界面时先要检查是否是
		// 下拉列表，如果是的话，就把下拉列表中对应索引号的文本填入单元格

		CString strLabel;
		int nCellType = 0;
		CString strText = 0;

		// 获取数据
		nCellType = cell->GetCellType();
		strLabel = cell->GetLabelText(); // 任何类型的控件都有LabelText，只不过只在有下拉列表的控件中才生效
		if (CT_CHECKBOX == nCellType)
		{
			strText = detail.m_value;
		}
		else if ( (CT_DROPDOWN == nCellType	|| CT_DROPLIST == nCellType)
			&& false == is_environment_variable(strLabel) ) // 目前仅支持这两种下拉控件，如果以后概念筋中需要新增其它类型的下拉列表类控件，又再添加，不过修改的时候要连同获取数据的地方同步操作
		{
			// 老的代码是依次判断strLabel是否为某个环境变量，当增加新的环境变量时，代码也要添加分支，不太好维护，
			// 现改为统一处理，可避免这个问题，如果某个环境变量需要特殊处理，可用原来的模式，再将原来的代码也注
			// 释在下面，以备需要进参考。(大样信息和设计思想采用同样的方式处理，注意上面的if中多加的IsEnvironmentVariable条件)
			//////////////////////////////////////////////////////////////////////////
			//	if (ENV_GANG_JIAO_XIAN == strLabel)
			//	{
			//		strText = detail.m_value;
			//	}	
			//	else if (ENV_HUN_NING_TU == strLabel)
			//	{
			//		strText = detail.m_value;
			//	}
			//	else if (ENV_SHENG_CE_GUAN == strLabel)
			//	{
			//		strText = detail.m_value;
			//	}
			////////////////////////////////////////////////////////////////////////// 

			strText = get_item_text(detail.m_value, detail.m_using_string_value, strLabel);
		}
		else
		{
			strText = detail.m_value;
		}

		cell->SetText(strText);

	}

}

// 获取一个钢筋表中设计思想，基本信息，布置信息，边距信息，大样信息表的起始终止行号
// 注意：返回的是行号，不是第几行，如第3行则为2，五个子表中没有数据行在第1行，所以
// 为0不是表示第一行，而是表示没有数据行。
// 返回的结果在rowInfo中，各元素代表的含义如下：
// [0]表示设计思想数据行的起始行，[1]表示设计思想数据行的终止行
// [2]表示基本信息数据行的起始行，[3]表示基本信息数据行的终止行
// [4]表示布置信息数据行的起始行，[5]表示布置信息数据行的终止行
// [6]表示边距信息数据行的起始行，[7]表示边距信息数据行的终止行
// [8]表示大样信息数据行的起始行，[9]表示大样信息数据行的终止行
// 函数会对结果进行合法性检查：
// 如果行数小于等于0，则对应的配对起始、终止行强制改为0，表示没有找到对应的行
// 如果终止行与起始行相同，表示只有一行数据
// 如果起始行大于终止行，则把对应的配对起始、终止行强制改为0，表示没有找到对应的行
void get_grid_row_info(CGridDataBase& grid, vector<int>& rowInfo)
{
	rowInfo.assign(10, 0); // 共10个元素，全部初始化为0

	int row_count = grid.GetRowCount();

	CString cell_tag = _T("");
	CString detail_end_tag = _T("");
	// 先找到设计思想这一个标题所在的行（只要表正常，行数不会出现不够的情况，因为假如
	// 没有设计思想表，其实是隐藏起来的，行高为0，但设计思想表仍存在于钢筋表中）
	for (int i = 0; i < row_count; ++i)
	{
		cell_tag = grid.GetRowTag(i);
		if (TAG_ROW_DESIGN == cell_tag)
		{
			rowInfo[0] = i + 1; // 数据行在标题后一行
		}
		else if (TAG_ROW_BASE == cell_tag)
		{
			rowInfo[2] = i + 2; // 数据行在标题后两行
		}
		else if (TAG_ROW_ARRANGE == cell_tag)
		{
			rowInfo[4] = i + 2; // 数据行在标题后两行
		}
		else if (TAG_ROW_EDGE == cell_tag)
		{
			rowInfo[6] = i + 2; // 数据行在标题后两行
		}
		else if (TAG_ROW_DETAIL == cell_tag)
		{
			rowInfo[8] = i + 2; // 数据行在标题后两行

			// 找到最后大样行标识，还要判断标识，否则当有多个钢筋面的时候，终止标识必然错误
			// 终止标识分两种情况：
			//   1.只有一个钢筋表，终止行为总行数减1
			//   2.有多个钢筋表，终止行为大样行标题之后的第一个设计思想行标题的前2行
			rowInfo[9] = row_count - 1; // 预先赋为最后一行的行号（最后一个表的终止行为总行数减1）
			for (int j = i + 1; j < row_count; ++j)
			{
				detail_end_tag = grid.GetRowTag(j);
				if (TAG_ROW_DESIGN == detail_end_tag && j > i + 2 )
				{
					rowInfo[9] = j - 2;
					break; // 找到就不能再找了，否则当有多个钢筋表就变成到最后一个钢筋表的设计思想行了。
				}
			}

			// 由于在大表中查找，会有多个钢筋表，所以第一个的最后一个标签找完后
			// 就不能再找了，否则找的就不是最前面一个钢筋表的行信息了。
			break;
		}
	}

	// 根据找到的起始行补充其余的元素
	rowInfo[1] = rowInfo[2] - 1; // 上一个表的数据终止行为下一个元素的起始行前一行
	rowInfo[3] = rowInfo[4] - 1; // 上一个表的数据终止行为下一个元素的起始行前一行
	rowInfo[5] = rowInfo[6] - 1; // 上一个表的数据终止行为下一个元素的起始行前一行
	rowInfo[7] = rowInfo[8] - 1; // 上一个表的数据终止行为下一个元素的起始行前一行
	// rowInfo[9] 最后一个表(大样表)的终止行已经在上面处理了

	// 对行数进行合法性检查，更改后的数据也要参加后面的逻辑判断，所以循环中
	// 使用多个if，不要使用else连接
	for (int i = 0; i < 10; i += 2) // 每次跳过两个元素
	{
		// 如果有一个为0，则起始、终止皆改为0
		if (0 == rowInfo[i] * rowInfo[i + 1])
		{
			rowInfo[i] = 0;
			rowInfo[i + 1] = 0;
		}

		// 检查有没有超出总行数，有则强制改为0
		if (rowInfo[i] >= row_count)
		{
			rowInfo[i] = 0;
			rowInfo[i + 1] = 0;
		}

		// 对修正后的数据再次检查有没有起始行大于终于行的情况
		if (rowInfo[i] > rowInfo[i + 1])
		{
			rowInfo[i] = 0;
			rowInfo[i + 1] = 0;
		}
	}
}

// 删除所有的钢筋表，只留一个标题行
void delete_all_steel_grid(CTHDataBase& db,BOOL bReservOneRow)
{
	// 注意：对db求子表数求的是大表的数目，永远为1个，要得到正确的钢筋子表数，先获取db中
	// 对应的大的GridDataBase表，再对其求子表数才正确。

	int nStartRow = 2;

	// 统计有几个钢筋子表
	CGridDataBase steelGrid;
	db.GetSubDataBase(0, steelGrid); // 只有一个钢筋大表
	steelGrid.EnableRowGroup(TRUE);
	int nGridCount = steelGrid.GetSubDataBaseCount();
	if(bReservOneRow)
	{
		nGridCount--;
		nStartRow++;
	}

	// 删除要对db进行，对子表操作不会改变大的db，因为子表是拷贝出来的。
	OpReturnValue flagOp;
	if (nGridCount >= 1)
	{
		flagOp = db.RemoveRowsAt(nStartRow, nGridCount, GP_STEEL); // 没有第2行则会崩溃，即表格至少有2行
		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.RemoveRowsAt(2, 1, ...)异常"), false));
			return;
		}
	}
}

void set_steel_data(CTHDataBase& db, const steel_data& data)
{
	OpReturnValue flagOp;
	int grid_count_need_add = int(data.get_steel_count()); // 要插入多少个数据表	

	// 获取子表前必须调用,否则内部缓存有问题,无法正确获取子表数,此问题在新构件启用自定时设计方法
	// 时发现,早期表格没有此问题,由于这个函数有点耗时,所以只要表格行组数没有变化时,只用调一次就
	// 可以,当表格结构发后变化时,最好再重新调用,为保证表填值正确,在最开始时调用一次
	// [Add By WYZ 2012年9月7日10:27:27]
	db.AutoFillRowNo();

	// 依次处理每一个钢筋表，一个表的处理分以下两个步骤：
	// 1.把表清空，全部删除
	// 2.获取表格中设计思想，基本信息，布置信息，边距信息，大样信息表的起始终止行号
	// 3.依次填充数据，如果表格的容纳范围小于数据量，则断言之，多余数据不再填充
	if(grid_count_need_add == 0)
		delete_all_steel_grid(db, TRUE);
	else
		delete_all_steel_grid(db, FALSE);

	vector<int> row_info; // 结构体中数据并未初始化，存储五个信息表的定位行信息
	for (int i = grid_count_need_add - 1; i >= 0; --i)// 注：千万不能把 i 定义为size_t无符号类型，会死循环
	{
		//////////////////////////////////////////////////////////////////////////
		// 由于对表赋值不能使用子表，子表是拷贝出来的，无法更新界面上的数据，所以只能
		// 对大的原始的DB进行操作，为了降低程序的复杂性，数据表倒序填写，先插入并填写
		// 最后一个子表，再填写倒写第二个，依次类推，直到第一个表，这样插入表之后对表
		// 的数据填充时始终都是对第一个表进行操作
		// 由于表被删除空了，只有一个标题行，第一个表，只能插到标题行后，不能插入到第
		// 二行前，当只有标题行时没有第二行，插入到第二行前会崩溃。
		//////////////////////////////////////////////////////////////////////////
		flagOp = db.InsertRowPost(1, 1, GP_STEEL);

		if (OP_ERROR == flagOp.count)
		{
			_ASSERTE((_T("flagOp = DB.InsertRowPost(...)异常"), false));
			return;
		}

		// 对插入的一个表进行数据填充
		const single_steel_data& single = data.get_steel_data(i);
		// [0]表示设计思想数据行的起始行，[1]表示设计思想数据行的终止行
		// [2]表示基本信息数据行的起始行，[3]表示基本信息数据行的终止行
		// [4]表示布置信息数据行的起始行，[5]表示布置信息数据行的终止行
		// [6]表示边距信息数据行的起始行，[7]表示边距信息数据行的终止行
		// [8]表示大样信息数据行的起始行，[9]表示大样信息数据行的终止行
		get_grid_row_info(db, row_info); // 返回结果详细说明请看此函数的注释文字
		if (10 != row_info.size()) continue; // 返回结果有问题，看下一个表

		set_single_steel_grid_data_caption(db, single.m_component ); // 设置应用部件
		set_single_steel_grid_data_design( db, single, row_info[0], row_info[1], 4);
		set_single_steel_grid_data_basic(  db, single, row_info[2], row_info[3], 4);
		set_single_steel_grid_data_layout( db, single, row_info[4], row_info[5], 4);
		set_single_steel_grid_data_edge(   db, single, row_info[6], row_info[7], 4);
		set_single_steel_grid_data_detail( db, single, row_info[8], row_info[9], 4);
	}
}

void steel_grid_tools::set_steel_data_to_grid(OUT CTHDataBase& db, IN const steel_data& data)
{
	set_steel_data(db, data);
}

// 解析布置信息表中的多值字符串（可能是多值，也可能是二级表结构）比如加密间距数，到目前为止，加密间距表达式出现过三种形式：
// （1） 1 2，3 4，5 6 表示三个支承线上左右两边的加密间距数（盖梁中用到），最后需要的结果数组为 1 2 3 4 5 6
// （2） 1，2 表示上部加密间距数和下部加密数（盖梁墩柱中用到），最后需要的结果数组为 1 2 
// （3） 5 表示只有一个间距数，为 5（大部分表格中都是这种格式）
// 为了让解析的方式一致，内部如果出现“,; ”（逗号、分号、空格）中的任何一个字符，都按分隔符处理
// 另外：
// （1）解析之前会忽略字符串首尾两端的空格，如果首尾两端有分隔符，请不要使用空格，仅在字符串
// 中间使用空格字符。
// （2）多个连续的空格只保留一个
// （3）任意数量的连续空格与逗号或者分号相邻时，将忽略这些空格
void set_multi_value_or_grid_string( IN const CString& text, OUT vector<CString>& all_num_string )
{
	all_num_string.clear();

	// 以下是测试时用的案例，结果均正确[2011-09-01 16:15:00]
	// 分隔符为" ,;"（空格、逗号、分号）
	// CString text = _T("  ,  1 2 , 3 46     89 ,,23    ,"); 结果：【0 1 2 3 46 89 0 23 0】
	// CString text = _T("                    "); 结果：【】
	// CString text = _T(","); 结果：【0 0】
	// CString text = _T(""); 结果：【】
	// CString text = _T("23"); 结果：【23】
	// CString text = _T("15,15"); 结果：【15 15】
	// CString text = _T("15 ,15"); 结果：【15 15】
	// CString text = _T("15, 15"); 结果：【15 15】
	// CString text = _T("15 , 15"); 结果：【15 15】
	// CString text = _T("100 100 100 100 100   100 100"); 结果：【100 100 100 100 100 100 100】

	CString tmp_text = text; // CStringToPairArr第一个参数不是const的，而且要对字符串进行一些处理，所以用这个临时变量

	tmp_text.Trim(); // 空格不允许出现在首尾两端
	// 如果只有一个数字则直接转换，不调用CStringToPairArr，否则转换错误没有数据返回
	if (_T("") == tmp_text) return;

	string_tools::rearrange_string_blank_chars(tmp_text, _T(" ,;")); // 先格式化后再解析

	// vecNumString是新加的字符串形式的布置间距数据成员，用于新的构件，所以要单独将单元格中的
	// 字符串按它的要求再解析一下放到里面，以便新构件使用。
	// 要求：
	//   字符串以分号相隔，每个解析为一个字符串，每个解析出来的字符串用逗号相隔，表示一个元素，
	//   相当于界面上的一个表，行之间用分号相隔，列之间用逗号相隔，但此处只解析分号相隔的行。
	// 注：如果以后出现字符的解析原则有新的需求，不应该改已有的解析原则，最好的方法是在数据成
	// 员中新增相应的数据格式，将添加新的解析方法为其赋值即可。
	// 现提出新要求如下：【Add By WYZ 2012年7月18日18:21:09】
	// 如果字符串用一个尖括号括起来的，表示这个字符串按二维表的规则解析，否则还是按原来的老方法解析，
	// 示例字符串如下：
	// <All, K1, 2 3*2 5 7, A D*3 C E;All, K2, 3 3 3, A D C E>
	// 解析的规则是：
	// 半角分号作为行的分隔符，半角逗号作为列的分隔符，半角空格作为单元格中元素的分隔
	// 基础单元格的数据支持乘号表达式
	// 解析为二维表时每行解析为一个字符串，不要行分隔符，并放到vector<CString>中，其中每行均要处理后
	// 再放入数组中，规则是列用一个半角逗号隔开，单元格中元素必须将乘号表达式解析开，再用一个半角空格分隔
	// 解析出来的数据只要保证vector<CString>数据正确即可，其它类型的数据可强制解析，也可不处理，此处
	// 尽量调用老的代码，保证所有类型的数据都被赋值

	if (tmp_text.GetLength() >= 2 && '<' == tmp_text[0] && '>' == tmp_text[tmp_text.GetLength() - 1])
	{
		// 去除首尾的<>
		tmp_text.Delete(0);
		tmp_text.Delete(tmp_text.GetLength() - 1);

		vector<CString> vecRows = string_tools::split_string(tmp_text, _T(";"));
		vector<CString> vecColumes;
		vector<CString> vecItems;

		for (size_t i = 0; i != vecRows.size(); ++i)
		{
			vecColumes.clear();
			vecColumes = string_tools::split_string(vecRows[i], _T(","));

			CString strRow;
			for (size_t j = 0; j != vecColumes.size(); ++j)
			{
				vecItems.clear();
				math_tools::parse_multiply_expression(vecColumes[j], vecItems);

				for (size_t k = 0; k != vecItems.size(); ++k)
				{
					// 单元格中最后一个元素后没有空格
					if (k < vecItems.size() - 1)
						strRow += vecItems[k] + _T(" ");
					else
						strRow += vecItems[k];
				}

				// 每一行最后一列没有逗号
				if (j < vecColumes.size() - 1)
					strRow += _T(",");
			}

			all_num_string.push_back(strRow); // 每一行中不含作为行分隔符的分号
		}
	}
	else
	{
		math_tools::parse_multiply_expression(tmp_text, all_num_string); //【注】表达式于2012-3-26 12:21:08改为个数在后，数据在前，为防止错误，原注释不改，仅在此作注释
	}
}

void steel_grid_tools::get_steel_cells( IN CTHDataBase& db, IN const CString& id,
							IN int col_index_of_value,
							OUT std::vector<CTHCell*>& all_cells )
{
	if (col_index_of_value < 0 || col_index_of_value >5) return;

	int nRowCount = db.GetRowCount();
	CString cell_tag;

	for (int nRow = 0; nRow < nRowCount; ++nRow)
	{
		cell_tag = db.GetRowTag(nRow);
		if (cell_tag == id)
		{
			all_cells.push_back(db.GetCell(nRow, col_index_of_value + 4));
		}
	}

	// 删除数据中获取失败的空指针
	for (vector<CTHCell*>::iterator it = all_cells.begin(); it != all_cells.end();/* Nothing */)
	{
		if (NULL == *it) it = all_cells.erase(it); else ++it;
	}
}

void steel_grid_tools::make_default_value_by_grid(const CString& grid_name, steel_data& data)
{
	data.clear_steel_data();

	unique_ptr<CTHDataBase> db(table_templates::create_data_base(grid_name));
	if (!db) return;

	get_steel_data_from_grid(*db, data);
}

//////////////////////////////////////////////////////////////////////////

// 判断指定行是否是DB中的子表标题行
bool is_sub_table_caption_line(CTHDataBase& db, int nRow)
{
	if (nRow < 0 || nRow >= db.GetRowCount()) return false; // 行数比行号大1

	CString strRowTag = db.GetRowTag(nRow);
	bool bIsCaption = false;
	if (TAG_ROW_DESIGN == strRowTag)
	{
		bIsCaption = true;
	} 
	else if (TAG_ROW_BASE == strRowTag)
	{
		bIsCaption = true;
	}
	else if (TAG_ROW_ARRANGE == strRowTag)
	{
		bIsCaption = true;
	}
	else if (TAG_ROW_EDGE == strRowTag)
	{
		bIsCaption = true;
	}
	else if (TAG_ROW_DETAIL == strRowTag)
	{
		bIsCaption = true;
	}
	else
	{
		bIsCaption = false;
	}

	return bIsCaption;

}

// 根据传入的子表标题字符串来返回子表的枚举类型
sub_steel_type get_steel_grid_sub_table_type(const CString& caption_line_row_tag)
{
	sub_steel_type tp = ST_NONE;
	if (TAG_ROW_DESIGN == caption_line_row_tag)
	{
		tp = ST_DESIGN;
	} 
	else if (TAG_ROW_BASE == caption_line_row_tag)
	{
		tp = ST_BASIC;
	}
	else if (TAG_ROW_ARRANGE == caption_line_row_tag)
	{
		tp = ST_LAYOUT;
	}
	else if (TAG_ROW_EDGE == caption_line_row_tag)
	{
		tp = ST_EDEG;
	}
	else if (TAG_ROW_DETAIL == caption_line_row_tag)
	{
		tp = ST_DETAIL;
	}
	else
	{
		tp = ST_NONE;
	}

	return tp;
}

// 查找ID在IDs中存不存在
bool is_steel_grid_id_exist(const CString& strTmp, const vector<CString>& strIDs)
{
	for (size_t i = 0; i != strIDs.size(); ++i)
		if (strIDs[i] == strTmp) return true;

	return false;
}

void show_or_hide_steel_grid_line(IN CTHDataBase& db, const vector<CString>& strIDs, bool bShow = false)
{
	int nRowCount = db.GetRowCount();
	if (nRowCount <= 0) return;

	for (int i = 0; i < nRowCount; ++i)
	{
		CString strRowTag = db.GetRowTag(i);

		if (is_steel_grid_id_exist(strRowTag, strIDs))
		{
			int nHeight = bShow ? gnRowHeight : 0; // 隐藏就是高度设置为0
			db.SetRowHeight(i, nHeight); // 所有行高都一样，以后如果不一样，则需要自己细化设置代码
		}
	}
}

void show_or_hide_steel_grid_sub_table(IN CTHDataBase& db, sub_steel_type eType, bool bShow = false)
{
	int nRowCount = db.GetRowCount();
	if (nRowCount <= 0) return;
	if (ST_NONE == eType) return;

	bool bContinue = false; // 是否继续隐藏
	CString strRowTag;

	for (int i = 0; i < nRowCount; ++i)
	{
		if (is_sub_table_caption_line(db, i))
		{
			strRowTag = db.GetRowTag(i);
			bContinue = (eType == get_steel_grid_sub_table_type(strRowTag)); // 必须保证它是行标题TAG再判断是否指定表的行标
		}

		if (bContinue)
		{
			int nHeight = bShow ? gnRowHeight : 0; // 隐藏就是高度设置为0
			db.SetRowHeight(i, nHeight); // 所有行高都一样，以后如果不一样，则需要自己细化设置代码
		}
	}
}

void steel_grid_tools::show_sub_grid( IN CTHDataBase& db, IN sub_steel_type type )
{
	show_or_hide_steel_grid_sub_table(db, type, true);
}

void steel_grid_tools::hide_sub_grid( IN CTHDataBase& db, IN sub_steel_type type )
{
	show_or_hide_steel_grid_sub_table(db, type, false);
}

void steel_grid_tools::get_steel_cell_info( IN CTHDataBase& db,
							   IN int row,
							   IN int col,
							   OUT CString& id,
							   OUT int col_index_of_value )
{

}

void steel_grid_tools::hide_row( IN CTHDataBase& db, IN const CString& id )
{
	vector<CString> vecStr(1, id);
	hide_rows(db, vecStr);
}

void steel_grid_tools::hide_rows( IN CTHDataBase& db, IN const std::vector<CString>& all_ids )
{
	show_or_hide_steel_grid_line(db, all_ids, false);
}

void steel_grid_tools::show_row( IN CTHDataBase& db, IN const CString& id )
{
	vector<CString> vecStr(1, id);
	show_rows(db, vecStr);
}

void steel_grid_tools::show_rows( IN CTHDataBase& db, IN const std::vector<CString>& all_ids )
{
	show_or_hide_steel_grid_line(db, all_ids, true);
}

void steel_grid_tools::set_steel_cells_text( IN CTHDataBase& db,
								IN const CString& id,
								IN int col_index_of_value,
								IN std::vector<CString>& all_values )
{
	if (all_values.empty()) return;

	vector<CTHCell*> vecCell;
	CString strText;
	get_steel_cells(db, id, col_index_of_value, vecCell);
	for (size_t i = 0; i != vecCell.size(); ++i)
	{
		if (NULL == vecCell[i]) continue;
		if (all_values.size() > i)
			strText = all_values[i]; // vecValues字符串个数不够时会保持最后一个元素的值

		vecCell[i]->SetText(strText);
	}
}

void steel_grid_tools::set_steel_cells_label( IN CTHDataBase& db,
								 IN const CString& id,
								 IN int col_index_of_value,
								 IN std::vector<CString>& all_values )
{
	if (all_values.empty()) return;

	vector<CTHCell*> vecCell;
	CString strText;
	get_steel_cells(db, id, col_index_of_value, vecCell);
	for (size_t i = 0; i != vecCell.size(); ++i)
	{
		if (NULL == vecCell[i]) continue;
		if (all_values.size() > i)
			strText = all_values[i]; // vecValues字符串个数不够时会保持最后一个元素的值

		vecCell[i]->SetLabelText(strText);
	}
}

// 处理一个单元格的操作动作
// is_ok为true表示此条件合法且需要执行，且条件满足
// is_ok为false表示此条件合法且需要执行，但条件不满足
void do_steel_grid_condition(IN OUT CTHDataBase& db, IN int nDBRow, IN CTHCell* cell, IN steel_grid_condition_action eToDo, IN bool is_ok)
{
	if (!cell) return;

	if (SGCA_HIDE == eToDo)
	{
		if (is_ok)
		{
			db.SetRowHeight(nDBRow, 0);
			app_print_info(MT_DEBUG, _T("    已操作：隐藏行[%d]"), nDBRow);
		}
		else
		{
			db.SetRowHeight(nDBRow, db.GetDefRowHeight());
			app_print_info(MT_DEBUG, _T("    已操作：显示行[%d]，行高为%d"), nDBRow, db.GetDefRowHeight());
		}
	}
	else if (SGCA_READONLY == eToDo)
	{
		if (is_ok)
		{
			cell->SetReadOnly(TRUE);
			app_print_info(MT_DEBUG, _T("    已操作：只读设置，所在行%d"), nDBRow);
		}
		else
		{
			cell->SetReadOnly(FALSE);
			app_print_info(MT_DEBUG, _T("    已操作：只读取消，所在行%d"), nDBRow);
		}
	}
	else
	{
		app_print_info(MT_DEBUG, _T("    操作行为异常，放弃设置：目前仅支持 SGCA_HIDE 和 SGCA_READONLY 操作"));
	}
}

// 扫描表格中所有条件中的单元格标识与行列号的对应信息
void scan_all_id_row_col_maping_info(CTHDataBase& db, const std::vector<steel_grid_conditions>& cond, vector<single_db_conditions>& info)
{
	info.clear();

	const int c_nRowCount = db.GetRowCount();
	const int c_nColStart = 4; // 有效数据的单元格所在的起始列数
	const int c_nColEnd = c_nColStart + 5; // 最后一列
	CString cell_tag = _T("");

	for (int i = 0; i < c_nRowCount; /* nothing */ )
	{
		cell_tag = db.GetRowTag(i);

		// 先找到设计思想这一个标题所在的行（只要表正常，行数不会出现不够的情况，因为假如
		// 没有设计思想表，其实是隐藏起来的，行高为0，但设计思想表仍存在于钢筋表中）
		if (TAG_ROW_DESIGN != cell_tag)
		{
			++i;
			continue;
		}

		// 为当前子表生成一个完整的条件映射信息
		single_db_conditions db_condition;
		db_condition.db_data_start_row = i - 2; // 新表的设计思想往上两行作为当前子表的终止行
		db_condition.db_data_end_row = -1;

		db_condition.conditions_in_db.clear();
		for (const steel_grid_conditions& gw_condition : cond)
		{
			condition_mapping one_condition;
			one_condition.src_pos_disable = gw_condition.is_force_condition();
			one_condition.is_ok_when_src_pos_disabled = gw_condition.is_condition_exist_and_true();
			one_condition.des_cell.row = -1;
			one_condition.des_cell.col = -1;
			one_condition.des_cell.id = gw_condition.get_des_pos().m_id;
			one_condition.des_cell.value_index = gw_condition.get_des_pos().m_value_index;
			one_condition.opt = gw_condition.get_condition_action();

			one_condition.condition_ref_cell.clear();
			for (int i = 0; i < gw_condition.get_condition_count(); ++i)
			{
				steel_grid_pos gw_item = gw_condition.get_condition_pos_at(i);
				condition_item_mapping item;
				item.condition_type = gw_condition.get_condition_type_at(i);
				item.src_cell_text = _T("");
				item.src_pos.row = -1;
				item.src_pos.col = -1;
				item.src_pos.id = gw_item.m_id;
				item.src_pos.value_index = gw_item.m_value_index;
				item.value = gw_condition.get_condition_value_at(i);
				one_condition.condition_ref_cell.push_back(item);
			}

			db_condition.conditions_in_db.push_back(one_condition);
		}

		// 当前行往下一行跳过设计思想标题行，进入数据区，开始处理当前子表
		++i;
		cell_tag = db.GetRowTag(i);

		while (TAG_ROW_DESIGN != cell_tag && i < c_nRowCount)
		{
			// 对条件中所需的标识行列号进行赋值
			for (condition_mapping& one_condition : db_condition.conditions_in_db)
			{
				if (0 == one_condition.des_cell.id.CompareNoCase(cell_tag))
				{
					one_condition.des_cell.row = i;
					one_condition.des_cell.col = c_nColStart + one_condition.des_cell.value_index;

					if (one_condition.des_cell.col < c_nColStart || one_condition.des_cell.col > c_nColEnd)
						one_condition.des_cell.col = -1;
				}

				for (condition_item_mapping& condition_item : one_condition.condition_ref_cell)
				{
					if (0 != condition_item.src_pos.id.CompareNoCase(cell_tag)) continue;

					condition_item.src_pos.row = i;
					condition_item.src_pos.col = c_nColStart + condition_item.src_pos.value_index;

					if (condition_item.src_pos.col < c_nColStart || condition_item.src_pos.col > c_nColEnd)
					{
						condition_item.src_pos.col = -1;
					}
					else
					{
						CTHCell* celll = db.GetCell(i, condition_item.src_pos.col);
						if (celll) condition_item.src_cell_text = celll->GetText();
					}
				}
			}

			++i;
			cell_tag = db.GetRowTag(i); // 行越出表格返回空字符串
		}

		if (TAG_ROW_DESIGN == cell_tag)
			db_condition.db_data_end_row = i - 2; // 新表的设计思想往上两行作为当前子表的终止行
		else
			db_condition.db_data_end_row = i - 1; // 表格结束了，则最后一行作为终止行

		info.push_back(db_condition);
	}
}

void steel_grid_tools::refresh_grid_status(IN CTHDataBase& db, IN int row, IN int col, const std::vector<steel_grid_conditions>& cond)
{
	if (!is_steel_grid(db)) return;

	DWORD st = GetTickCount();

	// 操作表格界面状态之前必须调这个成员,否则处于编辑状态的数据可能没有回写，会导致就地编辑控制贴错位置或数据显示不正常
	// 界面会把当前PU中的所有表格都送进来刷新（无论是不是钢筋表都是这样）如果当前表格不在pu的当前要显示的选项卡中，则界面
	// 表格是不会创建的，此时excel为空，直接返回即可
	CTHBaseExcel* excel = db.GetExcel();
	// if (excel) excel->Update(); // 表格有bug会导致单选按钮不能连续点击

	//////////////////////////////////////////////////////////////////////////
	// 本函数及其所调用到的函数一律禁止使用获取子表、及获取子表个数等与
	// 子表信息有关的函数，这样可以极大的提高算法的效率
	//////////////////////////////////////////////////////////////////////////

	// 当行列均为-1表示整表刷新，否则要求行列号都大于等于0
	if (!((-1 == row || -1 == col) || (row >= 0 && col >= 0))) return;

	// 扫描所有条件中的单元格标识与行列号的对应信息
	vector<single_db_conditions> id_mapping_info;
	scan_all_id_row_col_maping_info(db, cond, id_mapping_info);

	// 仅会处理指定行、列号所在的子表
	for (const single_db_conditions& db_condition : id_mapping_info)
	{
		// 如果当前子表是不要处理条件，则跳过
		bool will_do = (-1 == row || -1 == col) || (row >= db_condition.db_data_start_row && row <= db_condition.db_data_end_row);
		if (!will_do) continue;

		for (const condition_mapping& one_condition : db_condition.conditions_in_db)
		{
			// 如果当前条件的所有源单元格均不匹配触发单元格的行列号则不处理这个条件
			will_do = true;
			if (-1 != row && -1 != col)
			{
				will_do = false;
				for (const condition_item_mapping& src_ref_cells : one_condition.condition_ref_cell)
				{
					if (src_ref_cells.src_pos.row == row && src_ref_cells.src_pos.col == col)
					{
						will_do = true;
						break;
					}
				}
			}
			if (!will_do) continue;

			CTHCell* des_cell = db.GetCell(one_condition.des_cell.row, one_condition.des_cell.col);
			if (!des_cell) continue;

			if (one_condition.src_pos_disable)
			{
				do_steel_grid_condition(db, one_condition.des_cell.row, des_cell, one_condition.opt, one_condition.is_ok_when_src_pos_disabled);
			}
			else
			{
				// 检测是否所有条件均满足
				bool all_ok = true;
				for (const condition_item_mapping& condition_item : one_condition.condition_ref_cell)
				{
					if (_T("==") == condition_item.condition_type)
					{
						if (0 != condition_item.value.CompareNoCase(condition_item.src_cell_text))
						{
							all_ok = false; // 只要有一个条件不满足，就当作不匹配
							break;
						}
					}
					else if (_T("!=") == condition_item.condition_type)
					{
						if (0 == condition_item.value.CompareNoCase(condition_item.src_cell_text))
						{
							all_ok = false; // 只要有一个条件不满足，就当作不匹配
							break;
						}
					}
					else
					{
						all_ok = false; // 条件错误当作不满足处理
						break;
					}
				}

				do_steel_grid_condition(db, one_condition.des_cell.row, des_cell, one_condition.opt, all_ok);
			}
		}
	}

	// 全部处理后再刷新表格，否则不能在界面上看到修改后的状态
	db.ReFresh(); // 表格的刷新可以不调，但表格控件的刷新必须要调
	if (excel) excel->ReFresh(); // 重新刷新表格控制的界面显示，否则有隐藏的行可能导致界面显示异常或错位

	CString template_name;
	CTHCell* cell = db.GetCell(0, 0);
	if (cell) template_name = cell->GetLabelText();
	
	DWORD en = GetTickCount();
	CString output_info;
	output_info.Format(_T("表格状态刷新操作, 耗时:%d(ms)"), (int)(en - st));
	output_info += _T(", 表格:") + template_name;

	//////ReportMessage(4, output_info);
}

void steel_grid_tools::set_site_of_injection_label( IN CTHDataBase& db, const CString& text )
{
	CString strVarName;

	// 应用部件环境变量
	strVarName = ENV_YING_YONG_BU_WEI;
	strVarName.Delete(0, 1); // 仅删除第一个无关的@字符

	db.SetEnvVariable(strVarName, text);
}

void steel_grid_tools::set_site_of_injection_label( IN CTHDataBase& db, const vector<CString>& all_text )
{
	CString cond_value;
	CString strItem;
	for (size_t i = 0; i != all_text.size(); ++i)
	{
		strItem = all_text[i];
		strItem.Trim(); // 去除多余的换行符及空白字符

		cond_value += strItem + (_T("\n"));
	}

	set_site_of_injection_label(db, cond_value);
}

bool steel_grid_tools::is_steel_grid( IN CTHDataBase& db )
{
	if (db.GetRowCount() <= 10) return false; // 概念表不会小于10行
	if (TAG_ROW_DESIGN != db.GetRowTag(3)) return false; // 合法的概念筋表第3行一定是设计思想的标题行，且行标为TAG_ROW_DESIGN

	// 虽然判断的逻辑并不是很严密，但是只要是用本类创建的表一定会满足这个要求的，正常情况下不会有人传一个残缺的恶搞过的钢筋表进来导致误判的。
	return true;
}

void steel_grid_tools::set_site_of_injection_text( IN CTHDataBase& db, const CString& text, int index /*= -1*/ )
{
	int nRowCounter = db.GetRowCount();

	int nNum = 0;

	for (int i = 0; i < nRowCounter; ++i)
	{
		if (i >= 1 && TAG_ROW_DESIGN == db.GetRowTag(i))
		{
			CTHCell* cell = db.GetCell(i - 1, 4); // 应用部位在设计思想上一行，列号为4
			if (!cell)
			{
				_ASSERTE((_T("if (!cell) 为空"), false));
				continue;
			}

			if (-1 == index || nNum == index)
				cell->SetText(text);

			++nNum;
		}
	}
}

void steel_grid_tools::filte_steel_data_by_component( IN const steel_data& data_in, IN const CString component, OUT steel_data& steel_out )
{
	steel_out.clear_steel_data();

	for (size_t i = 0; i != data_in.get_steel_count(); ++i)
	{
		if (0 != component.CompareNoCase(data_in.get_steel_data(i).m_component))
			steel_out.add_single_steel(data_in.get_steel_data(i));
	}
}

void steel_grid_tools::extract_steel_data_by_component( IN const steel_data& data_in, IN const CString component, OUT steel_data& data_out )
{
	data_out.clear_steel_data();

	for (size_t i = 0; i < data_in.get_steel_count(); ++i)
	{
		const single_steel_data& steel = data_in.get_steel_data(i);
		if (steel.m_component.IsEmpty()
			|| 0 == steel.m_component.CompareNoCase(_T("All"))
			|| 0 == steel.m_component.CompareNoCase(_T("全部"))
			|| 0 == steel.m_component.CompareNoCase(_T("所有"))
			|| component.IsEmpty()
			|| 0 == component.CompareNoCase(_T("All"))
			|| 0 == component.CompareNoCase(_T("全部"))
			|| 0 == component.CompareNoCase(_T("所有")) )
		{
			data_out.add_single_steel(steel);
			continue;
		}

		// 解析表格中的应用部位，按半角空格拆分开
		vector<CString> all_loc = string_tools::split_string(steel.m_component, _T(","));

		for (const CString& loc : all_loc)
		{
			if (0 == component.CompareNoCase(steel.m_component))
			{
				data_out.add_single_steel(steel);
				break; // 不能重复放
			}
		}
	}

	if (data_out.has_steel_data() && !data_in.has_steel_data())
		data_out.add_single_steel(data_in.get_steel_data(0));
}

//////////////////////////////////////////////////////////////////////////

// 检测一个字符串是否表示“同……”的含义或为空(空在任何地方都合法)
bool is_string_means_same_as_or_null(const CString& strText)
{
	CString strTmp = strText;
	strTmp.Trim();
	if (strTmp.IsEmpty()) return true; // 为空也合法

	// 目前界面中出过以下形式：
	//   依赖值
	//   依赖……
	//   同……
	//   [同……]
	// 此处尽可能的全面罗列

	if ( _T("依赖") == strText.Left( int(_tcslen(_T("依赖"))) )
		|| _T("[依赖") == strText.Left( int(_tcslen(_T("[依赖"))) )
		|| _T("【依赖") == strText.Left( int(_tcslen(_T("【依赖"))) )
		|| _T("<依赖") == strText.Left( int(_tcslen(_T("<依赖"))) )
		|| _T("同") == strText.Left( int(_tcslen(_T("同"))) ) // 只要以“同”字开头的任意字符串都行
		|| _T("[同") == strText.Left( int(_tcslen(_T("[同"))) )
		|| _T("【同") == strText.Left( int(_tcslen(_T("【同"))) )
		|| _T("<同") == strText.Left( int(_tcslen(_T("<同"))) )  )
		return true;
	else
		return false;
}

// 表头数据检查
bool check_steel_grid_data_caption(const CString& header_text, single_steel_data& steel, bool need_report)
{
	UNUSED_ALWAYS(header_text);
	UNUSED_ALWAYS(steel);
	UNUSED_ALWAYS(need_report);

	return true; // 表头用户填入的数据不作格式限制
}

// 检查一个设计思想数据是否满足要求
bool check_steel_grid_ui_data_design_one_item(const CString& header_text, steel_design_item& steel, bool need_report)
{
	// 检验规则及细节参考界面钢筋P0转SteelP0设计思想的代码

	bool result  = true;

	// 原样传字符串时不用解析，由使用的人自己解析
	if (false == steel.m_using_string_value && false == is_string_means_same_as_or_null(steel.m_value))
	{
		// 解析表达式
		CString tmp_value_text = steel.m_value; // 非字符串形式按原来规则解析
		string_tools::rearrange_string_blank_chars(tmp_value_text, _T(" ,;")); // 首尾的空格会被删除

		// ADD By WYZ 2012年7月3日
		// 如果字符串用一个尖括号括起来的，表示这个字符串按二维表的规则解析，否则还是按原来的老方法解析，
		// 示例字符串如下：
		// <All, K1, 2 3*2 5 7, A D*3 C E;All, K2, 3 3 3, A D C E>
		// 解析的规则是：
		// 半角分号作为行的分隔符，半角逗号作为列的分隔符，半角空格作为单元格中元素的分隔
		// 基础单元格的数据支持乘号表达式
		// 解析为二维表时每行解析为一个字符串，不要行分隔符，并放到vector<CString>中，其中每行均要处理后
		// 再放入数组中，规则是列用一个半角逗号隔开，单元格中元素必须将乘号表达式解析开，再用一个半角空格分隔
		// 解析出来的数据只要保证vector<CString>数据正确即可，其它类型的数据可强制解析，也可不处理，此处
		// 尽量调用老的代码，保证所有类型的数据都被赋值
		if (tmp_value_text.GetLength() >= 2 && '<' == tmp_value_text[0] && '>' == tmp_value_text[tmp_value_text.GetLength() - 1])
		{
			// 去除首尾的<>
			tmp_value_text.Delete(0);
			tmp_value_text.Delete(tmp_value_text.GetLength() - 1);

			vector<CString> all_rows = string_tools::split_string(tmp_value_text, _T(";"));
			vector<CString> all_columns;
			vector<CString> all_items;

			for (size_t i = 0; i != all_rows.size(); ++i)
			{
				all_columns.clear();
				all_columns = string_tools::split_string(all_rows[i], _T(","));

				for (size_t j = 0; j != all_columns.size(); ++j)
				{
					all_items.clear();
					if (!math_tools::parse_multiply_expression(all_columns[j], all_items))
					{
						result = false;
						//////if (need_report)
							//////ReportMessage(0, header_text + _T("设计思想“")	+ steel.m_id + _T("”数据有误"));
					}

					if (all_items.size() > 100)
					{
						// 一般是乘号表达式写反了造成的
						//////if (need_report)
							//////ReportMessage(0, header_text + _T("设计思想“")	+ steel.m_id
								//////+ _T("”数据解析后，个数超过100，请检查"));
					}
				}
			}
		}
		else
		{
			vector<CString> string_items; // 处理后的字符串存入此数组
			if (!math_tools::parse_multiply_expression(tmp_value_text, string_items))
			{
				result = false;
				//////if (need_report)
					//////ReportMessage(0, header_text + _T("设计思想“")	+ steel.m_id + _T("”数据有误"));
			}

			if (string_items.size() > 100)
			{
				// 一般是乘号表达式写反了造成的
				//////if (need_report)
					//////ReportMessage(0, header_text + _T("设计思想“")	+ steel.m_id
						//////+ _T("”数据解析后，个数超过100，请检查"));
			}
		}
	}

	return result;
}

// 设计思想数据检查
bool check_steel_grid_data_design(const CString& header_text, single_steel_data& steel, bool need_report)
{
	bool result = true;
	for (size_t i = 0; i < steel.m_design_items.size(); ++i)
	{
		if (!check_steel_grid_ui_data_design_one_item(header_text, steel.m_design_items[i], need_report))
			need_report = false;
	}

	return result; // 设计思想没有特殊格式的数据要检查
}

// 检查一个基本信息数据是否满足要求
bool check_steel_grid_data_base_single_Item(const CString& header_text, steel_basic_item& steel, bool need_report)
{
	// 检验规则及细节参考界面钢筋P0转SteelP0基本信息的代码

	bool result = true;
	
	// 端距在不同的构件中可能有不同的值，无论有多少个值，都支持按空格，逗号分号，乘号表达式来解析。EDIT BY WYZ 2011年11月23日13:08:10
	CString strNonConst = steel.m_distance;
	string_tools::rearrange_string_blank_chars(strNonConst, _T(" ,;"));
	vector<CString> vecString;
	
	if (false == is_string_means_same_as_or_null(strNonConst))
	{
		if (!math_tools::parse_multiply_expression(strNonConst, vecString)) //【注】表达式于2012-3-26 12:21:08改为个数在后，数据在前，为防止错误，原注释不改，仅在此作注释
		{
			result = false;
			//////if (need_report)
				//////ReportMessage(0, header_text + _T("基本信息“")	+ steel.m_id + _T(" - 端距”数据有误"));
		}

		if (vecString.size() > 100)
		{
			// 一般是乘号表达式写反了造成的
			//////if (need_report)
				//////ReportMessage(0, header_text + _T("基本信息“")	+ steel.m_id
					//////+ _T("”数据解析后，个数超过100，请检查"));
		}
	}

	if (false == is_string_means_same_as_or_null(steel.m_force_type))
	{
		if (_T("CA") != steel.m_force_type
			&& _T("CB") != steel.m_force_type
			&& _T("CC") != steel.m_force_type
			&& _T("CD") != steel.m_force_type)
		{
			result = false;
			//////if (need_report)
				//////ReportMessage(0, header_text + _T("基本信息“")	+ steel.m_id + _T(" - 受力类型”数据有误"));
		}
	}

	return result;
}

// 基本信息数据检查
bool check_steel_grid_data_basic(const CString& header_text, single_steel_data& steel, bool need_report)
{
	bool result = true;
	for (size_t i = 0; i < steel.m_basic_items.size(); ++i)
	{
		if (!check_steel_grid_data_base_single_Item(header_text, steel.m_basic_items[i], need_report))
			need_report = false;
	}

	return result; // 基本信息没有特殊格式的数据要检查
}

// 检查一个布置信息数据是否满足要求
bool check_steel_grid_ui_data_layout_one_item(const CString& header_text, steel_layout_item& steel, bool need_report)
{
	// 检验规则及细节参考界面钢筋P0转SteelP0布置信息的代码

	bool result = true;

	if (false == is_string_means_same_as_or_null(steel.m_space_count))
	{
		// 以下是测试时用的案例，结果均正确[2011-09-01 16:15:00]
		// 分隔符为" ,;"（空格、逗号、分号）
		// CString text = _T("  ,  1 2 , 3 46     89 ,,23    ,"); 结果：【0 1 2 3 46 89 0 23 0】
		// CString text = _T("                    "); 结果：【】
		// CString text = _T(","); 结果：【0 0】
		// CString text = _T(""); 结果：【】
		// CString text = _T("23"); 结果：【23】
		// CString text = _T("15,15"); 结果：【15 15】
		// CString text = _T("15 ,15"); 结果：【15 15】
		// CString text = _T("15, 15"); 结果：【15 15】
		// CString text = _T("15 , 15"); 结果：【15 15】
		// CString text = _T("100 100 100 100 100   100 100"); 结果：【100 100 100 100 100 100 100】

		CString tmpStr = steel.m_space_distance; // CStringToPairArr第一个参数不是const的，而且要对字符串进行一些处理，所以用这个临时变量
		tmpStr.Trim(); // 空格不允许出现在首尾两端
		if (_T("") == tmpStr) return true; // 空也正确，表示0或没有填写。如果只有一个数字则直接转换，不调用CStringToPairArr，否则转换错误没有数据返回

		string_tools::rearrange_string_blank_chars(tmpStr, _T(" ,;")); // 先格式化后再解析

		// 现提出新需求如下：【Add By WYZ 2012年7月18日18:21:09】
		// 如果字符串用一个尖括号括起来的，表示这个字符串按二维表的规则解析，否则还是按原来的老方法解析，
		// 示例字符串如下：
		// <All, K1, 2 3*2 5 7, A D*3 C E;All, K2, 3 3 3, A D C E>
		// 解析的规则是：
		// 半角分号作为行的分隔符，半角逗号作为列的分隔符，半角空格作为单元格中元素的分隔
		// 基础单元格的数据支持乘号表达式
		// 解析为二维表时每行解析为一个字符串，不要行分隔符，并放到vector<CString>中，其中每行均要处理后
		// 再放入数组中，规则是列用一个半角逗号隔开，单元格中元素必须将乘号表达式解析开，再用一个半角空格分隔
		// 解析出来的数据只要保证vector<CString>数据正确即可，其它类型的数据可强制解析，也可不处理，此处
		// 尽量调用老的代码，保证所有类型的数据都被赋值
		if (tmpStr.GetLength() >= 2 && '<' == tmpStr[0] && '>' == tmpStr[tmpStr.GetLength() - 1])
		{
			// 去除首尾的<>
			tmpStr.Delete(0);
			tmpStr.Delete(tmpStr.GetLength() - 1);

			vector<CString> vecRows = string_tools::split_string(tmpStr, _T(";"));
			vector<CString> vecColumes;
			vector<CString> vecItems;

			for (size_t i = 0; i != vecRows.size(); ++i)
			{
				vecColumes.clear();
				vecColumes = string_tools::split_string(vecRows[i], _T(","));

				CString strRow;
				for (size_t j = 0; j != vecColumes.size(); ++j)
				{
					vecItems.clear();
					if (!math_tools::parse_multiply_expression(vecColumes[j], vecItems))
					{
						result = false;
						//////if (need_report)
							//////ReportMessage(0, header_text + _T("布置信息“")	+ steel.m_id
								//////+ _T(" - ") + steel.m_sub_id + _T(" - 期望距数或根数”数据有误"));
					}

					if (vecItems.size() > 100)
					{
						// 一般是乘号表达式写反了造成的
						//////if (need_report)
							//////ReportMessage(0, header_text + _T("布置信息“")	+ steel.m_id
								//////+ _T(" - 期望距数或根数”数据解析后，个数超过100，请检查"));
					}
				}
			}
		}
		else
		{
			// 为了与其它几个子表统一，此处修改后可支持乘号表达式 EDIT BY WYZ 2011年11月23日13:15:28
			//【注】表达式于2012-3-26 12:21:08改为个数在后，数据在前，为防止错误，原注释不改，仅在此作注释
			vector<CString> vecStr;
			if (!math_tools::parse_multiply_expression(tmpStr, vecStr))
			{
				result = false;
				//////if (need_report)
					//////ReportMessage(0, header_text + _T("布置信息“")	+ steel.m_id
						//////+ _T(" - ") + steel.m_sub_id + _T(" - 期望距数或根数”数据有误"));
			}

			if (vecStr.size() > 100)
			{
				// 一般是乘号表达式写反了造成的
				//////if (need_report)
					//////ReportMessage(0, header_text + _T("布置信息“")	+ steel.m_id +
						//////_T(" - 期望距数或根数”数据解析后，个数超过100，请检查"));
			}
		}
	}

	return result;
}

// 布置信息数据检查
bool check_steel_grid_data_layout(const CString& header_text, single_steel_data& steel, bool need_report)
{
	bool result = true;
	for (size_t i = 0; i < steel.m_layout_items.size(); ++i)
	{
		if (!check_steel_grid_ui_data_layout_one_item(header_text, steel.m_layout_items[i], need_report))
			need_report = false;
	}

	return result;
}

// 检查一个边距信息数据是否满足要求
bool check_steel_grid_ui_data_edge_one_item(const CString& header_text, steel_edge_item& steel, bool need_report)
{
	// 检验规则及细节参考界面钢筋P0转SteelP0边距信息的代码

	bool result = true;

	if (false == is_string_means_same_as_or_null(steel.m_location))
	{
		if (_T("中心") != steel.m_location
			&& _T("边缘") != steel.m_location)
		{
			result = false;
			//////if (need_report)
				//////ReportMessage(0, header_text + _T("边距信息“")	+ steel.m_id
					//////+ _T(" - ") + steel.m_sub_id + _T(" - 定位点”") + _T("数据有误，只能为“中心”或“边缘”"));
		}
	}

	if (false == is_string_means_same_as_or_null(steel.m_logic_type))
	{
		if (_T("不小于") != steel.m_logic_type
			&& _T("等于") != steel.m_logic_type
			&& _T("不小于取整") != steel.m_logic_type)
		{
			result = false;
			//////if (need_report)
				//////ReportMessage(0, header_text + _T("边距信息“")	+ steel.m_id
					//////+ _T(" - ") + steel.m_sub_id
					//////+ _T(" - 定位方式”数据有误，只能为“不小于”、“等于”或“不小于取整”"));
		}
	}

	return result;
}

// 边距信息数据检查
bool check_steel_grid_data_edge(const CString& header_text, single_steel_data& steel, bool need_report)
{
	bool result = true;
	for (size_t i = 0; i < steel.m_edge_items.size(); ++i)
	{
		if (!check_steel_grid_ui_data_edge_one_item(header_text, steel.m_edge_items[i], need_report))
			need_report = false;
	}

	return result;
}

// 检查一个大样信息数据是否满足要求
bool check_steel_grid_ui_data_detail_one_item(const CString& header_text, steel_detail_item& steel, bool need_report)
{
	// 检验规则及细节参考界面钢筋P0转SteelP0大样信息的代码

	bool result = true;

	// 原样传字符串时不用解析，由使用的人自己解析
	if (false == steel.m_using_string_value && false == is_string_means_same_as_or_null(steel.m_value))
	{
		// 解析表达式
		CString strNonConst = steel.m_value; // 非字符串形式按原来规则解析
		strNonConst.Trim(); // 空格不允许出现在首尾两端
		if (_T("") == strNonConst) return true; // 空也正确，表示0或没有填写。
		string_tools::rearrange_string_blank_chars(strNonConst, _T(" ,;")); // 首尾的空格会被删除

		// ADD By WYZ 2012年7月3日
		// 如果字符串用一个尖括号括起来的，表示这个字符串按二维表的规则解析，否则还是按原来的老方法解析，
		// 示例字符串如下：
		// <All, K1, 2 3*2 5 7, A D*3 C E;All, K2, 3 3 3, A D C E>
		// 解析的规则是：
		// 半角分号作为行的分隔符，半角逗号作为列的分隔符，半角空格作为单元格中元素的分隔
		// 基础单元格的数据支持乘号表达式
		// 解析为二维表时每行解析为一个字符串，不要行分隔符，并放到vector<CString>中，其中每行均要处理后
		// 再放入数组中，规则是列用一个半角逗号隔开，单元格中元素必须将乘号表达式解析开，再用一个半角空格分隔
		// 解析出来的数据只要保证vector<CString>数据正确即可，其它类型的数据可强制解析，也可不处理，此处
		// 尽量调用老的代码，保证所有类型的数据都被赋值
		if (strNonConst.GetLength() >= 2 && '<' == strNonConst[0] && '>' == strNonConst[strNonConst.GetLength() - 1])
		{
			// 去除首尾的<>
			strNonConst.Delete(0);
			strNonConst.Delete(strNonConst.GetLength() - 1);

			vector<CString> vecRows = string_tools::split_string(strNonConst, _T(";"));
			vector<CString> vecColumes;
			vector<CString> vecItems;

			for (size_t i = 0; i != vecRows.size(); ++i)
			{
				vecColumes.clear();
				vecColumes = string_tools::split_string(vecRows[i], _T(","));

				for (size_t j = 0; j != vecColumes.size(); ++j)
				{
					vecItems.clear();
					if (!math_tools::parse_multiply_expression(vecColumes[j], vecItems))
					{
						result = false;
						//////if (need_report)
							//////ReportMessage(0, header_text + _T("大样信息“")	+ steel.m_id
								//////+ _T(" - ") + steel.m_sub_id + _T("”数据有误"));
					}

					if (vecItems.size() > 100)
					{
						// 一般是乘号表达式写反了造成的
						//////if (need_report)
							//////ReportMessage(0, header_text + _T("大样信息“")	+ steel.m_id
								//////+ _T(" - ") + steel.m_sub_id + _T("”数据解析后，个数超过100，请检查"));
					}
				}
			}
		}
		else
		{
			vector<CString> vecString; // 处理后的字符串存入此数组

			if (!math_tools::parse_multiply_expression(strNonConst, vecString))
			{
				result = false;
				//////if (need_report)
					//////ReportMessage(0, header_text + _T("大样信息“")	+ steel.m_id
						//////+ _T(" - ") + steel.m_sub_id + _T("”数据有误"));
			}

			if (vecString.size() > 100)
			{
				// 一般是乘号表达式写反了造成的
				//////if (need_report)
					//////ReportMessage(0, header_text + _T("大样信息“")	+ steel.m_id
						//////+ _T(" - ") + steel.m_sub_id + _T("”数据解析后，个数超过100，请检查"));
			}
		}
	}

	return result;
}

// 大样信息数据检查
bool check_steel_grid_data_detail(const CString& header_text, single_steel_data& steel, bool need_report)
{
	bool result = true;
	for (size_t i = 0; i < steel.m_detail_items.size(); ++i)
	{
		if (!check_steel_grid_ui_data_detail_one_item(header_text, steel.m_detail_items[i], need_report))
			need_report = false;
	}

	return result;
}

bool steel_grid_tools::check_steel_grid_data(IN const CString& err_text_prefix, IN CTHDataBase& db, bool need_report /*= true*/)
{
	// 检查钢筋表格中用户填入的所有数据中格式不正确的地方
	// 仅用于分析界面表格中数据的格式错误，如乘号表达式要
	// 求个数在后，且解析之后元素个数不得多于100个等，本模
	// 块检查的数据与具体构件无关，仅限于格式方面

	if (false == is_steel_grid(db))
	{
		//////if (need_report) //////ReportMessage(0, _T("表格异常，无法识别并检查钢筋数据"));
		return false; // 不是钢筋表，不作检查
	}

	// 函数参数说明
	//     strSteel: 表示要检查的钢筋表名，最好指定桥幅或者布孔线名等可详细定位的信息，整个字符串不要使用"[]【】"符号，以便检错
	//     时作为前缀输出，格式为“桥幅名-构件名-钢筋表名”，例如“桥幅1-等宽等高箱梁33-悬臂钢筋表”
	CString tmp_header_text = err_text_prefix;
	tmp_header_text.Trim();
	if (tmp_header_text.IsEmpty())
	{
		// “应用部位”文字单元格在整表的绝对行为2，绝对列为2处，此单元格的下拉列表中放置的是表
		// 格创建时的模板名，方便调试，详情参考创建钢筋表的相关函数。
		// 报出这个信息是为了便于找到使用此函数但没有正确传参的人修正代码

		// 这个错误只有代码有误才会报，正确的程序用户使用是不可能报出此信息的
		CString strErr = _T("CheckSteelGridUIData函数调用时第一个参数不能为空，请指定“桥幅名-构件名-钢筋表名”作为信息前缀。表模板信息：");
		
		CTHCell* cell_loc = db.GetCell(1, 1);
		CString tmp_text;
		if (cell_loc) tmp_text = cell_loc->GetLabelText();
		
		tmp_text.Trim();
		if (tmp_text.IsEmpty())
			strErr += _T("无法识别");
		else
			strErr += _T("\n") + tmp_text; // 模板显示前加个换行，以便输出窗口在支持换行时显示到新的一行，不支持则不换行

		//////if (need_report) //////ReportMessage(0, strErr);

		// 将信息前缀强制改为未知
		tmp_header_text = _T("未知钢筋表");
	}

	// 获取界面原样数据
	steel_data data;
	get_steel_data_from_grid(db, data, false, true); // 参数bReplaceIDBySteelName必须传true，否则返回的结果无法将ID转为钢筋条目名

	// 对获取到的数据进行检查
	bool is_all_ok = true;

	// 为保证输出信息更易读，将前缀使用中括号括起来
	tmp_header_text = _T("“") + tmp_header_text + _T("”");

	for (size_t i = 0; i < data.get_steel_count(); ++i)
	{
		if (false == check_steel_grid_data_caption(tmp_header_text, data.get_steel_data(i), need_report)) is_all_ok = false;
		if (false == check_steel_grid_data_design(tmp_header_text, data.get_steel_data(i), need_report)) is_all_ok = false;
		if (false == check_steel_grid_data_basic(tmp_header_text, data.get_steel_data(i), need_report)) is_all_ok = false;
		if (false == check_steel_grid_data_layout(tmp_header_text, data.get_steel_data(i), need_report)) is_all_ok = false;
		if (false == check_steel_grid_data_edge(tmp_header_text, data.get_steel_data(i), need_report)) is_all_ok = false;
		if (false == check_steel_grid_data_detail(tmp_header_text, data.get_steel_data(i), need_report)) is_all_ok = false;
	}

	return is_all_ok;
}

