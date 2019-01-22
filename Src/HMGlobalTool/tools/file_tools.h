#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_FILE_TOOLS_H__
#define __HMGLOBALTOOL_TOOLS_FILE_TOOLS_H__	

class HM_GlobalTool_EXT file_tools
{
private:
	file_tools();
	~file_tools();

public:
	// 获取应用程序的当前所在路径,不带结尾的斜杠
	static CString get_app_path();

	// 获取应用程序的当前所在路径（带结尾的斜杠）
	static CString get_app_path_with_xg();

	// 获取x_framework框架dll所在路径（有时间主程序和框架并不在同一目录，比如单元测试或插件文件）,不带结尾的斜杠
	static CString get_x_framework_path();
	
	// 获取x_framework框架dll所在路径（有时间主程序和框架并不在同一目录，比如单元测试或插件文件）,带结尾的斜杠
	static CString get_x_framework_path_with_xg();

	// 提取一个文件路径中的信息，返回目录、文件名、后缀，接口支持相对路径，路径信息不全时，会尽量提取尽可能多的信息
	// "C:\\test\\1.txt"	返回 "C:\\test"  "1"  "txt"
	// "C:\\test\\.txt"		返回 "C:\\test"  ""  "txt"
	// "C:\\test\\1"		返回 "C:\\test"  "1"  ""
	// "C:\\"				返回 "C:"  "1"  "txt"
	// "C:"					返回 "C:"  "1"  "txt"
	// "C"					返回 ""  "C"  ""
	static void get_file_path_info(CString& path, CString& file_name, CString& suffix, const CString& full_path_name);
	static void get_file_path_info(CString& path, CString& file_name_with_suffix, const CString& full_path_name);
	
	// 从文件路径字符串中提取文件名（不含后缀），支持相对路径
	// "C:\\test\\1.txt"返回"1"
	static CString get_name_from_file_path_name(const CString& path_name);

	// 从文件路径字符串中提取文件名（包含文件名及后缀），支持相对路径
	// "C:\\test\\1.txt"返回"1.txt"
	static CString get_full_name_from_file_path_name(const CString& path_name);

	// 从文件路径字符串中提取后缀，不含点号，支持相对路径
	// "C:\\test\\1.txt"返回"txt"
	static CString get_ext_from_file_path_name(const CString& path_name);

	// 从文件路径字符串中提取路径，结尾不包含斜杠，支持相对路径
	// "C:\\test\\1.txt"返回"C:\\test"
	// "C:\\test\\www"返回"C:\\test"(www认为是文件名，且无后缀)
	// "C:\\test\\www\\"返回"C:\\test\\www"
	static CString get_path_from_file_path_name(const CString& path_name);

	// 从文件路径字符串中提取路径，结尾包含斜杠，支持相对路径
	// "C:\\test\\1.txt"返回"C:\\test\\"
	// "C:\\test\\www"返回"C:\\test\\"(www认为是文件名，且无后缀)
	// "C:\\test\\www\\"返回"C:\\test\\www\\"
	static CString get_path_from_file_path_name_with_xg(const CString& path_name);

	// 获取指定路径下的所有指定后缀的文件全路径（不含隐藏文件）
	// 注意：函数不会清空files中的内容
	// path可以是绝对路径，也可以是相对路径，结尾的斜杠可以不要也可以加上，路径只能是文件夹路径
	// suffix指定文件后缀，不含点号，如果为空表示返回没有后缀的文件名
	// include_sub_dir为true表示递归查找子文件夹，否则只找指定文件夹下的文件
	static void get_all_files_full_path_in_dir(std::vector<CString>& files, const CString& path, const CString& suffix, bool include_sub_dir = true);

	// 是否是相对路径
	static bool is_relative_path(const CString& path);

	// 指定路径是否存在且为文件(文件可以没有后缀)
	static bool is_file_path(const CString& path);

	// 指定路径是否存在且为文件夹
	static bool is_folder_path(const CString& path);

	// 指定的路径是否已经存在（可以是文件夹路径，也可以是文件路径）
	static bool is_path_or_file_exist(const CString& path);

	// 嵌套创建文件夹（指定任意路径，如果不存在会级联创建所有路径中的文件夹），成功执行返回true，否则返回false
	static bool create_directory_nested(const CString& dir_path);

	// 嵌套创建文件路径及文件（指定任意路径的文件，如果路径不存在会级联创建所有路径中的文件夹，最后再创建文件），成功执行返回true，否则返回false
	// override为true则覆盖同路径下的同名文件，否则不覆盖文件
	static bool create_file_nested(const CString& file_path, bool override = true);

	// 删除文件或文件夹，路径不存在也认为是删除成功
	// 整个操作全部执行成功返回true,否则返回false
	// 某一文件删除失败时会继续删除其它文件或文件夹
	// del_self为true表示如果路径为文件夹，则连同自身也删除，否则仅删除文件夹内部的文件及子文件夹
	static bool delete_file_or_directory(const CString& dir_path, bool del_self = false);

	// 删除指定的文件夹（文件夹内部的文件及子文件夹也删除）
	// del_self为true表示连同自身也删除，否则仅删除文件夹内部的文件及子文件夹
	static bool delete_directory(const CString& dir_path, bool del_self = false);

	// 删除指定的文件
	static bool delete_file(const CString& file_path);

public:
	/////////////////////////////////已废弃的接口/////////////////////////////////////////

	// 接口已经废弃，请改用delete_file_or_directory()
	// 清空整个文件夹的内容(包括子文件夹), 但保留该文件夹
	// 路径不存在也认为是删除成功,整个操作全部执行成功返回true,否则返回false
	static bool delete_files_and_sub_directory(const CString& dir_path);
};

#endif //__HMGLOBALTOOL_TOOLS_FILE_TOOLS_H__
