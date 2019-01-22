#include "stdafx.h"
#include "compare_tools.h"
#include "string_tools.h"
#include "math_tools.h"
#include "muparser/include/muParser.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace mu;


double math_tools::linear_interpolation(double start_x, double start_y, double end_x, double end_y, double value_x)
{
	// (x0,y0)与(x1,y1),要得到[x0,x1]区间内某一位置x在直线上的y值
	// 斜率：α = (x - x0) / (x1 - x0)
	// 结果：y = y0 + α(y1 - y0)

	// 检查点是否在区间内，如果不在区间内，为保证尽可能的获得一个可用的数据，返回起点处的Y值
	double dMinX = start_x;
	double dMaxX = end_x;
	if (compare_tools::is_less(dMaxX, dMinX)) swap(dMaxX, dMinX);

	if (compare_tools::is_less(value_x, dMinX)) return start_y;
	if (compare_tools::is_great(value_x, dMaxX)) return end_y;

	if (compare_tools::is_equal(end_x, start_x) || compare_tools::is_equal(end_y, start_y)) return start_y; // 检测区间是否退化为点（防止分母为0）
	if (compare_tools::is_equal(start_x, value_x)) return start_y; // 处理端点处的值，以免计算带来误差
	if (compare_tools::is_equal(end_x, value_x)) return end_y; // 处理端点处的值，以免计算带来误差

	double dA = (value_x - start_x) / (end_x - start_x);
	double dValueY = start_y + dA * (end_y - start_y);

	return dValueY;
}

int math_tools::get_great_integer(double val)
{
	if (compare_tools::is_less_equal(val, 0.0)) return 0;

	double intVal = (double) ((int) val);

	int realVal = 0;
	if (compare_tools::is_less(intVal, val))
		realVal = (int) intVal + 1; // 有零头时再加一
	else
		realVal = (int) intVal;

	return realVal;
}

double math_tools::pian_da_qu_zheng(double val, double level)
{
	// 0不能做除数，且为0表示不取整，直接原样返回值
	if (compare_tools::is_less_equal(level, 0.0)) return val;

	// 不考虑特殊情况下计算溢出的问题，因此没必要专门写一个防止采用整数取整导致溢出的算法。
	double dDiv = ((long) (val / level)) * level;
	double dDivUp = ((long) (val / level) + 1) * level;

	return (compare_tools::is_less(dDiv, val) ? dDivUp : dDiv); // 如果整除则原样返回
}

double math_tools::pian_xiao_qu_zheng(double val, double level)
{
	// 0不能做除数，且为0表示不取整，直接原样返回值
	if (compare_tools::is_less_equal(level, 0.0)) return val;

	// 不考虑特殊情况下计算溢出的问题，因此没必要专门写一个防止采用整数取整导致溢出的算法。
	double dDiv = ((long) (val / level)) * level;
	double dDivUp = ((long) (val / level) - 1) * level;

	return (compare_tools::is_great(dDiv, val) ? dDivUp : dDiv); // 如果整除则原样返回
}

double math_tools::normalize_angle(double val)
{
	if (compare_tools::is_great_equal(val, -M_PI) && compare_tools::is_less_equal(val, M_PI)) return val;

	double abs_val = abs(val);
	int count = (int)(abs_val / (2.0 * M_PI));
	abs_val -= count * 2.0 * M_PI;

	if (compare_tools::is_less(val, 0.0)) abs_val = -abs_val;

	return val;
}

double math_tools::deg_to_rad(double val)
{
	return val / 180.0 * M_PI;
}

double math_tools::rad_to_deg(double val)
{
	return val / M_PI * 180.0;
}

int math_tools::get_integer_nearby_double(double val, double diff)
{
	// 强制取整的结果
	double val_nearby_zero = (double) ((int) val);

	// 计算远离0点处的取整结果
	double val_away_from_zero = val;
	if (compare_tools::is_less(val, 0.0))
		val_away_from_zero = val_nearby_zero - 1.0;
	else
		val_away_from_zero = val_nearby_zero + 1.0;

	// 计算远离0点的取整结果与原数据的差值
	double diff_real = abs(val_away_from_zero - val);

	if (compare_tools::is_less_equal(diff_real, abs(diff)))
		return (int) val_away_from_zero;
	else
		return (int)val_nearby_zero;
}

bool math_tools::parse_single_multiply_item(IN const CString& exp_text, OUT vector<CString>& exp_text_items)
{
	exp_text_items.clear();
	int star_index = exp_text.Find(_T('*'));

	if (-1 == star_index)
	{
		exp_text_items.push_back(exp_text);
		return true; // 没有书写成乘号表达式也是合法的
	}

	CString value = _T(""); // 存储乘号表达式中的数值部分
	int count = 0; // 存储乘号表达式中的个数N部分

	int exp_type = 1; // 乘号表达式 m*n 的处理方式，返回0表示在个数前，1表示个数在后
	if (1 == exp_type)
	{
		// 2.个数在后，数据在前，A*n
		count = _ttoi(exp_text.Mid(star_index + 1)); // 不能写成nIndexStar否则会多取一个*号
		if (count <= 0) return false;

		value = exp_text.Mid(0, star_index);
	}
	else
	{
		// 1.个数在前，数据在后，n*A
		count = _ttoi(exp_text.Mid(0, star_index + 1));
		if (count <= 0) return false;

		value = exp_text.Mid(star_index + 1);
	}

	value.Trim();
	exp_text_items.assign(count, value);

	return true;
}

bool math_tools::parse_multiply_expression(IN const CString& text, OUT vector<CString>& result)
{
	bool is_ok = true; // 默认为转换正确

	result.clear();

	// 为避免后面拆分字符串出错，先把与*号相邻的所有空格全部删除，因为有的人喜欢*前后有空格，而且这是一种比较常见的情况
	CString text_copied = text;
	text_copied.Trim();
	for (int i = 0; i < text_copied.GetLength(); /* Nothing */)
	{
		// 空格不可能出现在字符串首或尾，因为之前使用Trim()处理了。
		// 其它地方的空格要处理，否则可能导致*表达式被后面的拆分语句拆开。
		if (_T(' ') != text_copied[i]) { ++i; continue; }

		// 多个连续空格也删除，否则不能处理“2      *   300”这种情况，经过本循环处理以后，最
		// 终结果不可能出现两个及以上的空格相邻的情况。
		if (_T('*') == text_copied[i - 1] || _T('*') == text_copied[i + 1]
			|| _T(' ') == text_copied[i - 1] || _T(' ') == text_copied[i + 1])
		{
			// 当前空格字符的前后有空格或者*号则删除当前空格。删除语句不会导致循环失效(区别于vector)
			text_copied.Delete(i);
			continue;
			// 此处不能 ++i; 删除一个字符相当于下标往后移了一位
		}

		++i; // 不能少，否则可能死循环
	}

	vector<CString> vecExpress = string_tools::split_string(text_copied, _T(" ,;"));
	if (!vecExpress.empty())
	{
		for (size_t i = 0; i != vecExpress.size(); ++i)
		{
			vector<CString> vecOneExpress;
			// 桥梁设计师的库函数在测试时发现错误,解析圆弧筋控制点时原样返回字符串,且返回值为true
			// 这个函数可以支持3*10 2 5这样的表达式，但是如果表达式中出现半角逗号则无法解析，比如表
			// 达式为 100,2,300 就会原样返回这个字符串，所以在调用这个函数之前先要把利用自己的函数把
			// 字符串全部分隔开，再逐一的调用桥梁设计师的库函数来解析。
			// BWDSGNMOD::ParseBWExpression()仅支持数字使用乘号，不允许字符使用乘号（如12*A），否则会
			// 报错（使用MessageBox报，会干扰界面），基于这一点，在调用之前先检测表达式中是否仅含有
			// 数字、小数点、以及乘号，且由于此处已经把字符串分解了，仅为一个数乘以另一个数的形式，所
			// 以只需要检测是不是一个数字乘以另一个数字，但后来发现要实现此功能，已经与自己写代码解析
			// 没有多大区别了，所以废弃桥梁设计师的函数BWDSGNMOD::ParseBWExpression，自己写一个
			// bool flag = BWDSGNMOD::ParseBWExpression(vecExpress[i], vecOneExpress);
			//【注】表达式于2012-3-26 12:21:08改为个数在后，数据在前，为防止错误，原注释不改，仅在此作注释
			bool flag = parse_single_multiply_item(vecExpress[i], vecOneExpress);
			if (false == flag)
			{
				_ASSERTE((_T("bool flag = parse_single_multiply_item(vecExpress[i], vecOneExpress);错误"), false));

				// 数据由外部专门的检测模块检测并报错，以便实现详细错误定位，此处报错注释掉[WYZ 2014年3月13日20:13:36]
				// CString strErrorInfo;
				// strErrorInfo.Format(_T("骨架或钢筋表格中乘号表达式“%s”格式有误，请检查"), vecExpress[i]);
				// //////ReportMessage(2, strErrorInfo);
				is_ok = false; // 仅标记一下转换发生过错误，而不返回，这样可以尽最大努力多转换几个字符串
			}

			if (vecOneExpress.empty()) continue;

			// 把转换后的字符串全部添加到结果数组中
			result.insert(result.end(), vecOneExpress.begin(), vecOneExpress.end());
		}
	}

	if (result.size() > 100)
	{
		// 数据由外部专门的检测模块检测并报错，以便实现详细错误定位，此处报错注释掉[WYZ 2014年3月13日20:13:36]
		// //////ReportMessage(1, _T("钢筋表格中表达式解析后，元素个数超过100，请检查"));
		// result.resize(100);
		_ASSERTE((_T("钢筋表格中表达式解析后，元素个数超过100，请检查"), false));
	}

	return is_ok;
}

double math_tools::calculate_expression(const CString& expression, bool var_case /*= true*/,
	const std::map<CString, double>& var_list /*= std::map<CString, double>()*/, double default_result /*= 0.0*/)
{
	if (expression.IsEmpty()) return 0;

	// muParser库会根据是否定义_UNICODE宏来决定字符串类型是unicode否是multibyte，所以CString可以直接给它，不用转换
	mu::Parser parser;

	for (auto& x : var_list)
	{
		if (x.first.IsEmpty()) continue;

		if (false == var_case) // 不区分大小写
		{
			CString uppercase_var = x.first;
			uppercase_var.MakeUpper();
			
			CString lowercase_var = x.first;
			lowercase_var.MakeLower();

			if (uppercase_var == lowercase_var) // 变量不是英文字母（可能是某个数学符号）
			{
				parser.DefineConst(string_type(x.first), (value_type)(x.second));
			}
			else
			{
				parser.DefineConst(string_type(lowercase_var), (value_type) (x.second));
				parser.DefineConst(string_type(uppercase_var), (value_type) (x.second));
			}
		}
		else
		{
			parser.DefineConst(string_type(x.first), (value_type) (x.second));
		}
	}

	parser.SetExpr(string_type(expression));

	double result = 0.0;

	try
	{
		result = parser.Eval();
	}
	catch (...)
	{
		result = default_result;
	}
	
	return result;
}
