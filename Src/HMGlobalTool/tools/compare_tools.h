#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_COMPARE_TOOLS_H__
#define __HMGLOBALTOOL_TOOLS_COMPARE_TOOLS_H__

class HM_GlobalTool_EXT compare_tools
{
private:
	compare_tools();
	~compare_tools();

private:
	static long double m_precision; // 默认为1.0e-8

public:
	// 参数使用最大范围的浮点数可以利用隐式转换增强适用范围
	static bool is_equal(long double first, long double second, long double precision = m_precision);
	static bool is_not_equal(long double first, long double second, long double precision = m_precision);
	static bool is_great(long double first, long double second, long double precision = m_precision);
	static bool is_great_equal(long double first, long double second, long double precision = m_precision);
	static bool is_less(long double first, long double second, long double precision = m_precision);
	static bool is_less_equal(long double first, long double second, long double precision = m_precision);
	
	// 以下接口对精度的修改是全局性的
	static long double get_precision();
	static void set_precision(long double precision);
	static void restore_default_precision();
};

#endif //__HMGLOBALTOOL_TOOLS_COMPARE_TOOLS_H__
