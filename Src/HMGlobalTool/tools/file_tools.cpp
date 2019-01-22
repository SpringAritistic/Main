#include "stdafx.h"
#include <string>
#include <vector>
#include <io.h>

#include "file_tools.h"

 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


file_tools::file_tools()
{
}


file_tools::~file_tools()
{
}

CString file_tools::get_app_path()
{
	TCHAR path_buffer[MAX_PATH];
	::GetModuleFileName(NULL, path_buffer, MAX_PATH);

	CString path, name;
	get_file_path_info(path, name, path_buffer);
	return path;
}

CString file_tools::get_app_path_with_xg()
{
	CString path = get_app_path();
	if (!path.IsEmpty()) path += _T('\\');
	return path;
}

CString file_tools::get_x_framework_path()
{
	extern HINSTANCE g_resource_handle;

	TCHAR path_buffer[MAX_PATH];
	::GetModuleFileName((HMODULE)g_resource_handle, path_buffer, MAX_PATH);

	CString path, name;
	get_file_path_info(path, name, path_buffer);
	return path;
}

CString file_tools::get_x_framework_path_with_xg()
{
	CString path = get_x_framework_path();
	if (!path.IsEmpty()) path += _T('\\');
	return path;
}

void file_tools::get_file_path_info(CString& path, CString& file_name, CString& suffix, const CString& full_path_name)
{
	path = _T("");
	file_name = _T("");
	suffix = _T("");

	// 先分解路径和文件名
	CString only_name_with_sufix;
	get_file_path_info(path, only_name_with_sufix, full_path_name);

	// 再分解文件名和后缀
	if (only_name_with_sufix.IsEmpty()) return;

	int pos = only_name_with_sufix.ReverseFind(_T('.'));
	if (pos >= 0)
	{
		file_name = only_name_with_sufix.Left(pos); // 不要'.'
		suffix = only_name_with_sufix.Mid(pos + 1);
	}
	else
	{
		file_name = only_name_with_sufix;
		suffix = _T("");
	}
}

void file_tools::get_file_path_info(CString& path, CString& file_name_with_suffix, const CString& full_path_name)
{
	path = _T("");
	file_name_with_suffix = _T("");

	int tmp_pos_01 = full_path_name.ReverseFind(_T('\\'));
	int tmp_pos_02 = full_path_name.ReverseFind(_T('/'));
	int pos = max(tmp_pos_01, tmp_pos_02);
	if (pos >= 0)
	{
		file_name_with_suffix = full_path_name.Mid(pos + 1);
		path = full_path_name.Left(pos); // 不要'\\'
		return;
	}

	pos = full_path_name.ReverseFind(_T(':'));
	if (pos >= 0)
	{
		file_name_with_suffix = full_path_name.Mid(pos + 1);
		path = full_path_name.Left(pos + 1); // 驱动器后面的冒号必须要才是合法的路径
	}
	else
	{
		file_name_with_suffix = full_path_name;
		path = _T("");
	}
}

CString file_tools::get_name_from_file_path_name(const CString& path_name)
{
	CString path, name, suffix;
	get_file_path_info(path, name, suffix, path_name);
	return name;
}

CString file_tools::get_full_name_from_file_path_name(const CString& path_name)
{
	CString path, name;
	get_file_path_info(path, name, path_name);
	return name;
}

CString file_tools::get_ext_from_file_path_name(const CString& path_name)
{
	CString path, name, suffix;
	get_file_path_info(path, name, suffix, path_name);
	return suffix;
}

CString file_tools::get_path_from_file_path_name(const CString& path_name)
{
	CString path, name;
	get_file_path_info(path, name, path_name);
	return path;
}

CString file_tools::get_path_from_file_path_name_with_xg(const CString& path_name)
{
	CString path = get_path_from_file_path_name(path_name);
	if (!path.IsEmpty()) path += _T("\\");
	return path;
}

void file_tools::get_all_files_full_path_in_dir(vector<CString>& files, const CString& path, const CString& suffix, bool include_sub_dir /*= true*/)
{
	CString full_path = path;
	if (full_path.Right(1) == _T("\\") || full_path.Right(1) == _T("/"))
		full_path.Delete(full_path.GetLength() - 1);

	if (full_path.IsEmpty()) full_path = get_app_path();

	// 使用C++库功能遍历文件，而不是系统API能增强代码可移植性

	struct _tfinddata_t fileFinder;
	intptr_t handle = _tfindfirst(path + _T("\\*.*"), &fileFinder); // 不能指定为*.txt这种带后缀的形式，否则无法遍历
	if (-1 == handle) return;

	// 第一个文件不用处理，因为第一个肯定是"."，即表示".\"当前目录
	// 第二也不用处理，肯定是".."，即"..\"上一级目录，只要往后处理即可
	do
	{
		CString curr_file_name(fileFinder.name);
		if (_T(".") == curr_file_name) continue;
		if (_T("..") == curr_file_name) continue;

		// 是目录则递归进入子目录处理，不能写成_A_SUBDIR == fileFinder.attrib
		if ((_A_SUBDIR & fileFinder.attrib) && include_sub_dir)
			get_all_files_full_path_in_dir(files, path + _T("\\") + curr_file_name, suffix, include_sub_dir);

		int dot_pos = curr_file_name.ReverseFind(_T('.'));
		if (-1 == dot_pos && !suffix.IsEmpty()) continue;

		if (suffix.IsEmpty())
		{
			files.push_back(path + _T("\\") + curr_file_name);
		}
		else
		{
			CString curr_file_suffix = curr_file_name.Mid(dot_pos + 1); // 不要.号
			if (0 == curr_file_suffix.CompareNoCase(suffix) && curr_file_name.GetLength() > suffix.GetLength()) // 防止文件名为空，只有后缀
				files.push_back(path + _T("\\") + curr_file_name);
		}

	} while (0 == _tfindnext(handle, &fileFinder));

	_findclose(handle);
}

bool file_tools::is_relative_path(const CString& path)
{
	return (-1 == path.Find(_T(':'))); // 没有:号认为是相对路径
}

bool file_tools::is_file_path(const CString& path)
{
	if (FALSE == PathFileExists(path)) return false;
	
	return (false == is_folder_path(path));
}

bool file_tools::is_folder_path(const CString& path)
{
	if (FALSE == PathFileExists(path)) return false;

	// 用API函数PathIsDirectory()也是一样的原理
	return (0 != (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(path)));
}

bool file_tools::is_path_or_file_exist(const CString& path)
{
	// 很多人不知道这个功能其实直接就有API接口可以调，有的人用文件打开是否成功来判断，有的用文件遍历的搜索的
	// 方式来判断，各种做法五花八门，根本不考虑效率和适用性，因此框架包装一下放在工具函数中
	return TRUE == ::PathFileExists(path);
}

// 只是创建完全路径的文件目录，具体的文件并没有创建
// 注意，传入的路径必须是文件夹路径，不能是文件路径，而且必须是绝对路径，否则可能错误
bool implement_create_directory_nested(const CString& full_path)
{
	// 先把路径统一为绝对路径且本地路径的形式且首尾两端没有\\字符和空白字符
	CString str = full_path;
	str.Trim();
	str.Replace(_T("//"), _T("\\")); // 把误写的连续//换为正确的单个'\'
	str.Replace(_T("/"), _T("\\")); // 把网络路径形式换为本地路径形式

	str.Trim(_T("\\ \t\r\n"));// 去除两端的'\\'及空白字符
	if (file_tools::is_relative_path(str)) str = file_tools::get_app_path_with_xg() + str;

	// 得到全部文件夹的完整路径，如C:\\AAA\\BBB\\CCC将得到以下四个字符串：
	// [0] C:
	// [1] C:\\AAA
	// [2] C:\\AAA\\BBB
	// [3] C:\\AAA\\BBB\\CCC
	// 注意：越长的路径越在后面
	vector<CString> all_folder_path;
	int cur_pos = str.Find(_T('\\'));
	do 
	{
		if (-1 == cur_pos)
		{
			all_folder_path.push_back(str); // 没找到原样放入
			break;
		}

		all_folder_path.push_back(str.Left(cur_pos));
		cur_pos = str.Find(_T('\\'), cur_pos + 1); // 前面已经确保首尾两端没有\\字符和空白字符
	} while (true);

	// 第0个元素删除为驱动盘符，要删除
	if (!all_folder_path.empty()) all_folder_path.erase(all_folder_path.begin());

	// 从短的路径开始创建
	for (auto& x : all_folder_path)
	{
		if (x.IsEmpty()) continue; // 多个连续的\\\\只当一个处理，产生的空名称忽略
		if (::PathFileExists(x)) continue;

		if (!::CreateDirectory(x, nullptr)) return false;
	}

	return true;
}

// 嵌套创建文件夹（指定任意路径，如果不存在会级联创建所有路径中的文件夹
bool file_tools::create_directory_nested(const CString& dir_path)
{
	// 先把路径统一为绝对路径且本地路径的形式且首尾两端没有\\字符和空白字符
	CString str = dir_path;
	str.Trim();
	str.Replace(_T("//"), _T("\\")); // 把误写的连续//换为正确的单个'\'
	str.Replace(_T("/"), _T("\\")); // 把网络路径形式换为本地路径形式

	str.Trim(_T("\\ \t\r\n"));// 去除两端的'\\'及空白字符
	if (file_tools::is_relative_path(str)) str = file_tools::get_app_path_with_xg() + str;

	if (::PathIsDirectory(str)) return true; // 路径已经存在
	if (!::PathIsDirectory(str)) implement_create_directory_nested(str); //如果目录不存在,则级联创建目录
	return (TRUE == ::PathIsDirectory(str)); // 完成后检测创建的路径是否存在
}

bool file_tools::create_file_nested(const CString& file_path, bool override/* = true*/)
{
	// 先把路径统一为绝对路径且本地路径的形式且首尾两端没有\\字符和空白字符
	CString str = file_path;
	str.Trim();
	str.Replace(_T("//"), _T("\\")); // 把误写的连续//换为正确的单个'\'
	str.Replace(_T("/"), _T("\\")); // 把网络路径形式换为本地路径形式

	str.Trim(_T("\\ \t\r\n"));// 去除两端的'\\'及空白字符
	if (file_tools::is_relative_path(str)) str = file_tools::get_app_path_with_xg() + str;

	if (::PathFileExists(str)) return true;

	if (str.GetLength() >= MAX_PATH) return false;
	TCHAR parent_dir[MAX_PATH] = _T("");
	_tcscpy_s(parent_dir, str);

	//获取上级目录
	if (!::PathRemoveFileSpec(parent_dir)) return false;

	//创建文件所过路径的全部目录
	if (!implement_create_directory_nested(parent_dir)) return false;

	DWORD create_flag = (override ? CREATE_ALWAYS : OPEN_ALWAYS);
	HANDLE file_handle = ::CreateFile(str, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr, create_flag, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

	if (file_handle == INVALID_HANDLE_VALUE) return false;
	::CloseHandle(file_handle);
	
	return true;
}

// 仅能删除存在的空文件夹
bool implement_delete_single_empty_folder(const CString& path)
{
	// 文件夹的属性设置和文件的操作一样
	if (FILE_ATTRIBUTE_NORMAL & GetFileAttributes(path))
		SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);

	return (TRUE == RemoveDirectory(path)); // 只能移除空文件夹
}

bool file_tools::delete_file_or_directory(const CString& dir_path, bool del_self /*= false*/)
{
	if (FALSE == ::PathFileExists(dir_path)) return true; // 路径不存在也认为是删除成功
	if (is_file_path(dir_path))
		return delete_file(dir_path);
	else
		return delete_directory(dir_path, del_self);
}

// 如果本身是文件夹则本身不删除
bool implement_delete_all_files_and_sub_directories_in_folder(const CString& cur_path)
{
	if (FALSE == PathFileExists(cur_path)) return true; // 路径不存在也认为是删除成功
	if (file_tools::is_file_path(cur_path)) return file_tools::delete_file(cur_path);

	// 处理子文件夹及内部所有文件
	bool all_ok = true;
	CFileFind file_find;
	BOOL has_found = file_find.FindFile(cur_path + _T("\\*.*"));
	while (has_found)
	{
		has_found = file_find.FindNextFile();
		CString cur_file_path = file_find.GetFilePath();

		if (file_find.IsDirectory())
		{
			if (file_find.IsDots()) continue;

			bool flag = implement_delete_all_files_and_sub_directories_in_folder(cur_file_path);
			if (false == flag) all_ok = false;

			if (false == implement_delete_single_empty_folder(cur_file_path)) all_ok = false;
		}
		else
		{
			if (false == file_tools::delete_file(cur_file_path)) all_ok = false;
		}
	}

	file_find.Close();

	return all_ok;
}

bool file_tools::delete_directory(const CString& dir_path, bool del_self /*= false*/)
{
	if (!is_folder_path(dir_path) || FALSE == ::PathFileExists(dir_path)) return false;

	bool res = implement_delete_all_files_and_sub_directories_in_folder(dir_path);
	
	if (del_self)
	{
		bool flag = implement_delete_single_empty_folder(dir_path);
		if (!flag) res = false;
		return res;
	}
	
	return res;
}

bool file_tools::delete_file(const CString& file_path)
{
	if (FALSE == ::PathFileExists(file_path)) return true; // 路径不存在也认为是删除成功
	if (!is_file_path(file_path)) return false;

	if (FILE_ATTRIBUTE_NORMAL & GetFileAttributes(file_path))
		SetFileAttributes(file_path, FILE_ATTRIBUTE_NORMAL);

	return (TRUE == DeleteFile(file_path));
}

bool file_tools::delete_files_and_sub_directory(const CString& dir_path)
{
	return delete_file_or_directory(dir_path, false);
}

