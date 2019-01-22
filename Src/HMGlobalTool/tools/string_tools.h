#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_STRING_TOOLS_H__	
#define __HMGLOBALTOOL_TOOLS_STRING_TOOLS_H__

#include <time.h>
#include <string>
#include <vector>
#include <tchar.h>

class HM_GlobalTool_EXT string_tools
{
private:
	string_tools() = delete;
	~string_tools() = delete;

public:
	// 格式化并生成一个CString对象，行为与CString的Format保持一致，格式符详细参阅printf或格式控制符(格式化结果不得超过1K byte)
	static CString format(const TCHAR* format, ...);

	// 格式化及转换时间对象，支持的格式符列举如下（注意不要跟字符串转换的格式符搞混了）：
	// %a  星期几的简写
	// %A  星期几的全称
	// %b  月分的简写
	// %B  月份的全称
	// %c  标准的日期的时间串
	// %C  年份的后两位数字
	// %d  十进制表示的每月的第几天
	// %D  月 / 天 / 年
	// %e  在两字符域中，十进制表示的每月的第几天
	// %F  年 - 月 - 日
	// %g  年份的后两位数字，使用基于周的年
	// %G  年分，使用基于周的年
	// %h  简写的月份名
	// %H  24小时制的小时
	// %I  12小时制的小时
	// %j  十进制表示的每年的第几天
	// %m  十进制表示的月份
	// %M  十时制表示的分钟数
	// %n  新行符
	// %p  本地的AM或PM的等价显示
	// %r  12小时的时间
	// %R  显示小时和分钟：hh : mm
	// %S  十进制的秒数
	// %t  水平制表符
	// %T  显示时分秒：hh : mm : ss
	// %u  每周的第几天，星期一为第一天（值从0到6，星期一为0）
	// %U  第年的第几周，把星期日做为第一天（值从0到53）
	// %V  每年的第几周，使用基于周的年
	// %w  十进制表示的星期几（值从0到6，星期天为0）
	// %W  每年的第几周，把星期一做为第一天（值从0到53）
	// %x  标准的日期串
	// %X  标准的时间串
	// %y  不带世纪的十进制年份（值从0到99）
	// %Y  带世纪部分的十制年份
	// %z  时区名称，如果不能得到时区名称则返回空字符。
	// %Z  同%z
	// %%  百分号
	// 注意：参数如果使用到年月日的，均从0开始！！！
	//      格式化结果不得超过1K byte
	//      格式串中如果出现其它非格式字符则原样输出，如"%Y年%m月%d日"
	static CString time_format(const TCHAR* format, int y, int m, int d, int hr = 0, int mi = 0, int se = 0); // 不实行夏令时
	static CString time_format(const TCHAR* format, const tm& t);
	static CString time_format(const TCHAR* format, time_t t); // 使用时间戳格式化
	static CString time_format(const TCHAR* format); // 使用当前系统时间的本地时间进行格式化（不是格林尼治标准时间）
	static time_t convert_time_stamp(const TCHAR* format, const TCHAR* t);
	static time_t convert_time_stamp(const tm& t);
	static time_t convert_time_stamp(int y, int m, int d, int hr = 0, int mi = 0, int se = 0); // 不实行夏令时
	static time_t convert_time_stamp(); // 使用当前系统时间进行转换（不是格林尼治标准时间）
	static tm convert_time(const TCHAR* format, const TCHAR* t);
	static tm convert_time(time_t t);
	static tm convert_time(int y, int m, int d, int hr = 0, int mi = 0, int se = 0); // 不实行夏令时
	static tm convert_time(); // 使用当前系统时间进行转换（不是格林尼治标准时间）

	// 字符串转换
	// 注意：
	//   所以的函数名中的 string均为multi-byte 字符串，utf8均为 utf-8 格式的string，二者数据类型一样，但内部编码不一样
	//   wstring 均为unicode字符串，编码为 utf-16 形式
	//   CString 与项目设置的字符编码相关，相当于 multi-byte 的 string 或 utf-16 的 wstring
	static std::string wstring_to_string(const std::wstring& str);
	static std::string CString_to_string(const CString& str);
	static std::string utf8_to_string(const std::string& str);
	static std::wstring string_to_wstring(const std::string& str);
	static std::wstring CString_to_wstring(const CString& str);
	static std::wstring utf8_to_wstring(const std::string& str);
	static CString wstring_to_CString(const std::wstring& str);
	static CString string_to_CString(const std::string& str);
	static CString utf8_to_CString(const std::string& str);
	static std::string string_to_utf8(const std::string& str);
	static std::string wstring_to_utf8(const std::wstring& str);
	static std::string CString_to_utf8(const CString& str);

	// 将数字转为字符串，如果是小数，会并把尾部多余的0去掉,保留6小数
	static CString num_to_string(short num);
	static CString num_to_string(int num);
	static CString num_to_string(long num);
	static CString num_to_string(long long num);
	static CString num_to_string(unsigned short num);
	static CString num_to_string(unsigned int num);
	static CString num_to_string(unsigned long num);
	static CString num_to_string(unsigned long long num);
	static CString num_to_string(float num, int nozero_count_after_point = 6); // 结果的小数部分末尾的0去除，nozero_count_after_point表示小数点后非零的数据最大长度（最多20位，四舍五入处理）
	static CString num_to_string(double num, int nozero_count_after_point = 6); // 结果的小数部分末尾的0去除，nozero_count_after_point表示小数点后非零的数据最大长度（最多20位，四舍五入处理）
	static CString num_to_string(long double num, int nozero_count_after_point = 6); // 结果的小数部分末尾的0去除，nozero_count_after_point表示小数点后非零的数据最大长度（最多20位，四舍五入处理）

	// 将字符串中的中文逗号、句号、分号、左右小括号、左右中括号、全角空格全部换成英文半角字符
	static void replace_chs_punctuation_marks_to_en(CString& str);

	// 格式化字符串，完成以下功能:
	// 1.把字符串中首尾两端的半角空白字符（空格、回车、制表符）全部删除
	// 2.字符串中间连续多个相邻的半角空格仅保留一个
	// 3.出现任意数量的连续空格字符与分隔字符相邻时删除这些空格
	// 4.分隔字符串split的任何字符都是一个有效的分隔符
	// 5.分隔符split中出现空格算法的逻辑也仍然是正确的
	static const CString rearrange_string_blank_chars(const CString& text, const CString& split);
	static void rearrange_string_blank_chars(CString& text, const CString& split);

	// 把字符串按指定的字符串中的任意字符来拆分并返回结果，默认分隔符为空格逗号分号，分隔符可能是临时变量，不使用引用传递
	// 注意：多个连续的空格如果作为分隔符则会只当作一个处理，且首尾的空格会忽略去除，其它分隔符则原样处理，不指定空格作
	// 为分隔符时不会去除任何空格字符
	static std::vector<CString> split_string(const CString& text, const CString& split = _T(" ,;"));
	static void split_string(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));
	static void split_matrix_string(std::vector<std::vector<CString>>& result, const CString& text,
		TCHAR row_split = _T('\n'), TCHAR column_split = _T('\t'));
	 
	// 合并字符串
	// all_strings 待合并的字符串，split 分隔符，result 合并好的字符串
	// end_with_split表示尾端自动加带上一个split分隔符，否则不要（合并为行列表时仅控制最后一行的最后一列末尾是否添加行分隔符）
	// 合并后内容为空不会在结尾添加分隔符
	static void merge_string(CString& result, const std::vector<CString>& all_strings, const CString& split = _T(","), bool end_with_split = false);
	static CString merge_string(const std::vector<CString>& all_strings, const CString& split = _T(","), bool end_with_split = false);
	static void merge_matrix_string(CString& result, const std::vector<std::vector<CString>>& all_strings,
		TCHAR row_split = _T('\n'), TCHAR column_split = _T('\t'), bool end_with_split = false);

	// 根据dc计算字符串的绘制宽度（以像素为单位）
	static int get_text_width_in_px(const CString& text, CDC* dc);

	// 生成guid字符串，need_split_char为true表示带分隔符（只能为“-”号），结果如下：
	// 带分隔符格式为：66C78769-C996-488A-AA8F-3CB806782FE6，长度为36
	// 无分隔符格式为：66C78769C996488AAA8F3CB806782FE6，长度为32
	static void generate_guid(CString& guid_text, bool need_split_char = true);
	static CString generate_guid(bool need_split_char = true);

	// 将一个字符串进行MD5加密，如果无法正常完成加密，则返回空字符串，count 为加密次数
	// 不返回原字符串是为防止不检测返回值导致明文误用造成泄漏
	static void make_md5_value(const CString& text, CString& result, int count = 1);
	static CString make_md5_value(const CString& text, int count = 1);

	// 比较两个去空白字符后的值是否相等，不区分大小写
	static bool are_trimed_cstring_equal_no_case(const CString& first, const CString& second);

	// 比较两个去空白字符后的值是否相等，区分大小写
	static bool are_trimed_cstring_equal(const CString& first, const CString& second);

	// 根据指定的已经存在的字符串列表，以指定的名称为前缀，在后面加上下划线和数字，生成一个相对有意义的且不重复的字符串返回
	static CString generate_unique_name(const std::vector<CString>& exist_names, const CString& prefix);
};

#endif //__HMGLOBALTOOL_TOOLS_STRING_TOOLS_H__
