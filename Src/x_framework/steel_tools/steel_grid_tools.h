#pragma once
#include "steel_data.h"

class steel_grid_conditions;
class CTHDataBase;


class X_FRAMEWORK_API steel_grid_tools
{
private:
	steel_grid_tools(void);
	~steel_grid_tools(void);

public:
	// 创建表，需要指定五个子表的模板名和默认的应用部位
	static CTHDataBase* create_steel_grid(IN const CString& caption,
										  IN const CString& design_grid,
										  IN const CString& basic_grid,
										  IN const CString& layout_grid,
										  IN const CString& edge_grid,
										  IN const CString& detail_grid,
										  IN bool is_fixed = true, // 钢筋表是固定的，还是可由调用者增加、删除钢筋表
										  IN bool enable_component = false); // 应用部位是否启用
	
	// 根据对应的钢筋表生成一套默认值
	static void make_default_value_by_grid(const CString& grid_name, steel_data& data);

	// 为了让表格的状态正常，至少要在单元格内容被改变以及表格所在的窗口显示的时候调用（否则加载起来刚开始状态是不正确的）
	// 刷新表格界面状态，nDBRow,nDBCol为内容发生变化的单元格行列号(行列号的定位相对于整个Database，如果其中一个为-1则
	// 表示刷新整个表，否则只刷新行列号所在的子表（一个子表为包含了设计思想、基本信息等5部分的完整钢筋表），强烈建议没有特殊理由不要传-1)
	// 此函数内部要进行表格的搜索工作，稍微耗时，最好当表格所在的窗口处于显示状态且有单元格内容被修改时才
	// 调用，后台时不用调用，因为界面的显示与后台数据获取正确与否没有关系。特别是联动设计时后台大量表格会
	// 有数据被修改，应该避免后台表格也调用本函数。本函数的执行性能没有作过测试。
	// 设置一个表有两个条件，示例代码：
	// 	// 1.要求当SDZJ_A内容第0列值为20 且 SXGJW内容第0列不是选中状态时，设置SDZJ_B内容第0列为只读，否则为可读写
	// 	steel_grid_conditions cond_01( steel_grid_pos(_T("SDZJ_A"), 0), _T("=="), _T("20"), steel_grid_pos(_T("SDZJ_B"), 0), SGCA_READONLY );
	// 	cond_01.And(steel_grid_pos(_T("SXGJW"), 0), _T("!="), _T("0"));
	// 
	// 	// 2.要求当SXZJJQ为选中状态时，设置SSGJ_A内容第1列为只读，否则为可读写
	// 	steel_grid_conditions cond_02( steel_grid_pos(_T("SXZJJQ"), 0), _T("=="), _T("1"), steel_grid_pos(_T("SSGJ_A"), 1), SGCA_READONLY );
	// 
	// 	vector<steel_grid_conditions> conds;
	// 	conds.push_back(cond_01);
	// 	conds.push_back(cond_02);
	// 	steel_grid_tools::refresh_grid_status(*m_pDB, 10, 5, conds);
	// 	
	// 	//另一种方法（推荐）：
	// 	steel_grid_conditions cond;
	// 	vector<steel_grid_conditions> conds;
	// 	// 只要SXST8、SXST9、SXST10任何一个不为0则可读写，否则只读
	// 	cond.Set( _T("SXST8"), 0, _T("=="), _T("0"), _T("SJSTJL"), 0, SGCA_READONLY );
	// 	cond.And( _T("SXST9"), 0, _T("=="), _T("0") );
	// 	cond.And( _T("SXST10"), 0, _T("=="), _T("0") );
	// 	conds.push_back(cond);
	// 	如果整个CTHDataBase都要刷新，则行、列均传-1（比指明具体的子表所在的单元格行列号要花费更多的时间）
	// 	当有多个条件时，条件之间的关系为“与”，如果有条件之间需要“或”关系，请转化为“与”再设置
	static void refresh_grid_status(IN CTHDataBase& db, IN int row, IN int col, const std::vector<steel_grid_conditions>& cond);
	
	// 设置应用部位的下拉标签文本文本（整个DataBase中保持一致单元格为只读也可以设置）
	static void set_site_of_injection_label(IN CTHDataBase& db, const CString& text); // 标签选项使用\n隔开
	static void set_site_of_injection_label(IN CTHDataBase& db, const std::vector<CString>& all_text); // 每个元素为一个选项
	
	// 设置应用部位的单元格文本文本（整个DataBase中保持一致单元格为只读也可以设置）
	// index为-1表示全部应用部位都修改，否则仅修改指定的第index（从0开始）个应用部件，如果不存在则放弃修改
	static void set_site_of_injection_text(IN CTHDataBase& db, const CString& text, int index = -1); // 标签选项使用\n隔开

	// 判断传入的表是否是概念筋钢筋表(仅判断是否是，并不验证这个表的格式与布局是否是正确)
	static bool is_steel_grid(IN CTHDataBase& db);

	// 检查钢筋表中用户填入的表格界面数据，如果有错误就报错，钢筋的具体数据处理时不再报错
	// strSteel: 表示要检查的钢筋表名，最好指定桥幅或者布孔线名等可详细定位的信息，整个字符串不要使用"[]【】"符号，以便检错
	//           时作为前缀输出，格式为“桥幅名-构件名-钢筋表名”，例如“桥幅1-等宽等高箱梁33-悬臂钢筋表”
	// bReport: 为true表示向界面报告出错信息，否则仅作检查但不报告
	// 数据检查后没有错误返回true，否则返回false，如果钢筋表中为空表（即0个子表，只有标题）也认为是不合法
	// 本函数不会对传入的界面表格数据作任何的更改和修正，仅进行检测
	static bool check_steel_grid_data(IN const CString& err_text_prefix, IN CTHDataBase& db, bool need_report = true);

	// 获取指定的单元格
	// Parameter: IN CTHDataBase& db
	//   DataBase
	// Parameter: IN const CString & strID
	//   数据行标识（表格模板设计器中指定的行标签）
	// Parameter: int nValueColIndex
	//   在内容中的列号，0 为内容中的第一列，内容列指除钢筋名、区域名等之外数值参数部
	//   分的列，如基本信息中的直径、边距信息中的定位点等均为第0列。
	// Parameter: OUT std::vector<CTHCell*> vecCell
	//   存储返回的单元格
	// Return Value:
	//   如果指定的列有误，则断言之，且该行返回空的单元格指针。
	//   如果参数正确，则返回DB中每一个钢筋子表中对应的单元格
	// 特殊说明：
	//   返回来的单元格请不要进行以下操作，否则会破坏表格结构，导致数据读取和写入错误。
	//   1. 更改合并单元格区域
	//   2. 更改单元格的TAG
	//   3. 更改单元格所在的行TAG
	//   3. 更改单元格所在的列TAG
	//   4. 设置环境变量（只有在表格设计器中设置的环境变量才生效）
	static void get_steel_cells(IN CTHDataBase& db, IN const CString& id, IN int col_index_of_value, OUT std::vector<CTHCell*>& all_cells);

	// 设置所有指定标识的单元格标签，参数含义同GetTHCells
	// 要设置的值在vecValues中，如果符合要求的单元格数量多于字符串数量，则其余单元格
	// vecValues中每个元素对应一个单元格中的标签，如果标签有多项，请使用\n分隔
	// 全部设置成最后一个字符串值，如果少于，则不管。
	// 如果需要更新内容的单元格较多，建议使用SetP0Data效率更高
	static void set_steel_cells_label(IN CTHDataBase& db, IN const CString& id, IN int col_index_of_value, IN std::vector<CString>& all_values);

	// 设置所有指定标识的单元格文本值，参数含义同GetTHCells
	// 要设置的值在vecValues中，如果符合要求的单元格数量多于字符串数量，则其余单元格
	// 全部设置成最后一个字符串值，如果少于，则不管。
	// 如果需要更新内容的单元格较多，建议使用SetP0Data效率更高
	static void set_steel_cells_text(IN CTHDataBase& db, IN const CString& id, IN int col_index_of_value, IN std::vector<CString>& all_values);

	// 返回指定的单元格在概念筋表中的条目信息
	// Parameter: IN CTHDataBase & db
	//   DataBase
	// Parameter: IN int nRow
	//   在DB中的行号
	// Parameter: IN int nCol
	//   在DB中的列号
	// Parameter: OUT CString & strID
	//   数据行标识（表格模板设计器中指定的行标签）
	// Parameter: OUT int nValueColInex
	//   在内容中的列号，0为内容中的第一列，内容列指除钢筋名、区域名等之外数值参数部
	//   分的列，如基本信息中的直径、边距信息中的定位点等均为第0列。
	static void get_steel_cell_info(IN CTHDataBase& db, IN int row, IN int col, OUT CString& id, OUT int col_index_of_value);

	// 隐藏或显示指定行
	static void hide_row(IN CTHDataBase& db, IN const CString& id);
	static void hide_rows(IN CTHDataBase& db, IN const std::vector<CString>& all_ids); // 隐藏多行尽量用这个函数，避免在DB中多次反复定位行降低效率
	static void show_row(IN CTHDataBase& db, IN const CString& id);
	static void show_rows(IN CTHDataBase& db, IN const std::vector<CString>& all_ids); // 显示多行尽量用这个函数，避免在DB中多次反复定位行降低效率

	// 隐藏或显示子表
	static void hide_sub_grid(IN CTHDataBase& db, IN sub_steel_type type);
	static void show_sub_grid(IN CTHDataBase& db, IN sub_steel_type type);
	
	// 获取表格中数据并全部填入到P0数据结构中,bAnalyticAppLoc表示表格数据是否按应用部位进行解析，具体参考通概念筋说明文档.docx
	// 本函数不会对用户界面填写的数据进行检查，无论对错，均原样读取，数据是否正确由使用P0的人处理
	// bReplaceIDBySteelName为true表示获取的钢筋数据中ID和子ID均改为钢筋条目名称填充，否则使用ID填充
	static void get_steel_data_from_grid(IN CTHDataBase& db, OUT steel_data& data, IN bool parse_by_component = false, IN bool replace_steel_item_id_to_name = false);

	// 把用户传入的P0数据全部填入到表格中
	// 钢筋数据中ID和子ID均为钢筋ID，不得使用条目名称填充
	static void set_steel_data_to_grid(OUT CTHDataBase& db, IN const steel_data& data);

	// 过滤steelP0In数据，将应用部位为strBW的钢筋数据删除（应用部分必须严格与表格中的一样才有效），其余的保留并输出到steelP0Out中，应用部位strBW不区分大小写【Add By WYZ 2012-7-11 13:19:15】
	static void filte_steel_data_by_component(IN const steel_data& data_in, IN const CString component, OUT steel_data& steel_out);
	
	// 提取steelP0In数据，将应用部位为strBW的钢筋数据保留并输出到steelP0Out中，应用部位strBW不区分大小写【Add By WYZ 2012-7-11 13:19:15】
	// 钢筋表中的应用部位会按半角解析，只要某一项包含了strBW指定的名称则命中，另外strBW为空或为"All"或“全部”或“所有”时，所有的钢筋均符合要求
	// strBW中不允许包含多个类型
	// 如果找不到则返回第0个钢筋数据
	static void extract_steel_data_by_component(IN const steel_data& data_in, IN const CString component, OUT steel_data& data_out);
};
