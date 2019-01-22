#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_MATH_TOOLS_H__
#define __HMGLOBALTOOL_TOOLS_MATH_TOOLS_H__

#include <map>
#include <vector>


class X_FRAMEWORK_API math_tools
{
private:
	math_tools() = delete;
	~math_tools() = delete;

public:
	// 根据线性插值求结果
	// dStartX	起点坐标X
	// dStartY	起点坐标Y
	// dEndX	终点坐标X
	// dEndY	终点坐标Y
	// dValueX	要查询的点坐标X，当它比最小值还小时取小值端点处的值，比最大值还大时取大值端点处的值，落在端点时直接取端点处值
	static double linear_interpolation(double start_x, double start_y, double end_x, double end_y, double value_x);

	// 根据一个浮点数返回一个不小于这个浮点数的正整数
	// 如果浮点数小于等于0则返回0
	static int get_great_integer(double val);

	// 对一个浮点数按取整级别进行偏大取整,如果刚好整除就返回原值，如果取整级别是个小数则取整后结果也可能是小数
	// 取整级别为0表示不取整，且只能大于等于0
	// 无论正数还是负数均向无穷大正数方向取整，一般用于尺寸等取整
	static double pian_da_qu_zheng(double val, double level);

	// 对一个浮点数按取整级别进行偏小取整,如果刚好整除就返回原值，如果取整级别是个小数则取整后结果也可能是小数
	// 取整级别为0表示不取整，且只能大于等于0
	// 无论正数还是负数均向无穷小的负数方向取整，一般用于标高取整，即标高向低的方向取整
	static double pian_xiao_qu_zheng(double val, double level);

	// 对弧度进行规格化，将其转换到-Pi到Pi范围内
	static double normalize_angle(double val);

	// 角度转弧度
	static double deg_to_rad(double val);
	
	// 弧度转角度
	static double rad_to_deg(double val);

	// 对指定浮点数进行强制取整，如果浮点数取整前距离它近的整数差距小于等于diff，则进位后取整
	// 如指定diff为0.1，则2.1得到2，2.9得到3，2.8得到2，-2.2得到-2，-2.9得到-3
	// 本函数主要为了解决计算一些实际工程中精度误差造成的个数差1的行为
	static int get_integer_nearby_double(double val, double diff);

	// 解析乘号表达式，此函数仅能解析一个运算变量乘以另一个运算变量的情况，如：
	// 3*2 或 6.5*3 或 8.0*5.5 或 -3*8
	// 此函数会对乘号前面的内容进行数字转换，并强制要求只能转换成整数，乘号后面的运算变量
	// 格式任意，函数仅进行Trim并原样保留，对于有多个乘号的情
	// 况，转换的时候仅识别第一个乘号。当返回值为false的时候是尽最大可能转换，结果中的字
	// 符串可能并不一定符合要求，使用者就当注意。
	// 如果*前面的数字强制转换后为0，则结果vector中仅放入整个字符串作为内容
	// 如果*后面的内容Trim后为空字符串，则结果vector中存入指定数目的空字符串
	// 以下为正确示例及相应的运算结果：
	// 100.2*3		->	100.2    100.2    100.2
	// A*5			->	A    A    A    A    A
	//【注】表达式于2012-3-26 12:21:08改为个数在后，数据在前，为防止错误，原注释不改，仅在此作注释
	static bool parse_single_multiply_item(IN const CString& exp_text, OUT std::vector<CString>& exp_text_items);

	// 解析字符串表达式，它把字符串按照空格、逗号、分号来拆分开，再逐一解析表达式
	// 要解析的表达式中只能有数字字符、空格、逗号、分号、乘号、小数点，例外的是乘号表达式中
	// 表示内容的可以是字符串或字符，表示个数的则只能是整数，否则会被强制修正示例：
	// 100 0 结果：100 0
	// 100,0 结果：100 0
	// 100*3 结果：100 100 100
	// A*3	 结果：A A A
	// AB*3	 结果：AB AB AB
	// 10 2*10 5 结果：10 10 10 5
	// 多个空格当作一个使用，首尾两端的空格被忽略
	// 空格和分隔符相邻，则忽略空格
	// 空格和乘号相邻，则忽略空格
	// 一般要使用之前会调用GWFormatString格式化一下字符串，修正一些格式错误的字符串，以便结果更精确
	//【注】表达式于2012-3-26 12:21:08改为个数在后，数据在前，为防止错误，原注释不改，仅在此作注释
	// 个数只能为整数，数值则可以是小数，错误数据格式内部也会最大可能解析并确保程序不会异常，但不保证结果正确
	static bool parse_multiply_expression(IN const CString& text, OUT std::vector<CString>& result);

	// 计算数据表达式的值
	// 如果计算错误则返回default_result
	// 如果表达式中不含变量，则var_list可以不用传,如3+(5-6)*2
	// 如果变量不区分大小写，则var_case为false，区分则为true
	static double calculate_expression(	const CString& expression, bool var_case = true,
		const std::map<CString, double>& var_list = std::map<CString, double>(), double default_result = 0.0);
};

#endif //__HMGLOBALTOOL_TOOLS_MATH_TOOLS_H__
