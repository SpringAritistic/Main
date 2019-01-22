//////////////////////////////////////////////////////////////////////////
// 文件由王友周从开源项目eMule中移植而来，作轻微改动以便适用本程序，用于检测内存分配
// 失败及程序异常崩溃并生成调试用的转储文件
// 本模块功能比较小巧，并没有使用google breakpad及比较正式的商业软件中在独立进程中
// 监视的方式，因此一些比较特殊的崩溃和异常退出并不一定能检测到，目前发现的只有栈溢出
// 无法正常捕捉（栈溢出一般是由递归或死循环导致，这种问题在开发阶段或使用阶段很容易就
// 能发现，正式产品不太可能出现，不捕捉也没太大问题，关键是要捕捉到这种异常只能新起一个
// 异常捕获的进程才能实现，技术上要复杂一些，花大代价去实现不值得（有开源的可用，不过配
// 置略显麻烦，如google breakpad，所以留此注释））
// 项目网站 http://www.emule-project.net
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <cassert>
#include <ctime>

#include "windows.h"
#include "dbghelp.h"
#include "rpc.h"

#include "mini_dump.h"


// 变量的初始化放在最前面
bool mini_dump_helper::m_need_popup_report_window = false;

mini_dump_helper::mini_dump_helper()
{
}

mini_dump_helper::~mini_dump_helper()
{
}

int memory_malloc_callback_handler(size_t sz)
{
	static bool has_report = false;

	// 这个不属于结构化异常处理的功能，所以可以使用MFC的对象，对栈没有太大影响，如果内存完全耗光，
	// 会出现对话框函数都无法运行，栈上一点空间都分配不到了，这种情况遇到过多次，考虑到内存分配失败可以由程序员
	// 自己处理，有时候程序员是想要测试一块足够大的空间是否能正确分配，所以出现这种问题只要弹出消息框就可以，不
	// 要强制结束程序，后续代码正不正常依赖于程序员是否检查了内存分配的返回地址是否为null并作了相应处理
	if (!has_report && mini_dump_helper::m_need_popup_report_window)
	{
		// 当内存已经出错的时候，构造CString又会导致内存分配，造成本函数递归溢出，加此标记处理一下（必须放在Format之前）
		has_report = true;
		CString str;
		str.Format(_T("程序请求内存分配时失败(size: %u byte)"), sz);
		AfxMessageBox(str, MB_OK | MB_ICONWARNING | MB_APPLMODAL | MB_TOPMOST);
	}

	has_report = false;

	return 0;
}

// 禁止设置异常处理句柄
bool disable_set_unhandled_exception_filter()
{
	HMODULE hKernel32 = ::LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return false;

	void* org_entry = (void*) ::GetProcAddress(hKernel32, _T("SetUnhandledExceptionFilter"));
	if (!org_entry) return false;

#ifdef _M_IX86

	// [WYZ注] Code for x86:
	// 33 C0        xor   eax,eax
	// C2 04 00     ret   4
	unsigned char hook_api_code [] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };

#elif _M_X64

	// [WYZ注] Code for x64
	// 33 C0        xor   eax,eax
	// C3           ret
	unsigned char hook_api_code [] = { 0x33, 0xC0, 0xC3 };

#else

#	error "异常捕获的代码只能运行于 x86 和 x64 的CPU!"

#endif

	SIZE_T bytes_written = 0;
	BOOL is_ok = ::WriteProcessMemory(::GetCurrentProcess(), org_entry, hook_api_code, sizeof(hook_api_code), &bytes_written);
	return (TRUE == is_ok);
}

inline void get_app_path_with_xg(CString& path)
{
	path.Empty();

	TCHAR dump_file_path[MAX_PATH] = { _T('\0') };

	// 返回0为失败，正确时返回整个模块名，形如："D:\***\***\***.exe"
	::GetModuleFileName(NULL, dump_file_path, _countof(dump_file_path) - 1);
	TCHAR* pszFileName = _tcsrchr(dump_file_path, _T('\\'));
	if (pszFileName) pszFileName[1] = _T('\0'); // 在最后一个'\\'后截断(保留\\)

	path = dump_file_path;
}

inline void generate_guid(CString& guid_text)
{
	GUID guid; // 生成guid作为文件名
	if (S_OK == ::CoCreateGuid(&guid))
	{
		// GUID 格式如66C78769-C996-488a-AA8F-3CB806782FE6，长度为36
		TCHAR guid_char_array[40] = { _T('\0') }; // 程序已经异常，能少分配内存尽量少分配
		_sntprintf_s(guid_char_array, _countof(guid_char_array) - 1,
			_T("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"), // 0表示使用0填充
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

		guid_text = guid_char_array;
	}
}

void mini_dump_helper::enable_dump(bool popup_report_window/* = false*/)
{
	m_need_popup_report_window = popup_report_window;

	// 1.设置遇到未处理异常时（崩溃时）生成转储文件
	::SetUnhandledExceptionFilter(exception_top_level_filter);
	disable_set_unhandled_exception_filter();

	// 2.设置在内存分配失败时进行提示
	_set_new_mode(1);
	_set_new_handler(memory_malloc_callback_handler);
}

// 生成dump文件名
void generate_dump_file_name(CString& strDump)
{
	CString path;
	CString guid;
	get_app_path_with_xg(path);
	generate_guid(guid);
	strDump = path + guid + _T(".dmp");
}

LONG mini_dump_helper::exception_top_level_filter(EXCEPTION_POINTERS* ex_point)
{
	CString dump_file_and_error_id;
	dump_file_and_error_id.Format(_T("\"%u"), (unsigned int)::GetLastError());

	if (ex_point)
	{
		CString file_name;
		generate_dump_file_name(file_name);

		// 不用异常处理了，本身当前就在异常处理中，如果出异常，会再次被捕捉到
		// HANDLE file_handle = ::CreateFile(file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING | OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE file_handle = ::CreateFile(file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		// pExceptionInfo参数为空MiniDumpWriteDump也不会崩溃，但表示系统没有在崩溃时检测到可用的转储信息
		if (file_handle != INVALID_HANDLE_VALUE)
		{
			_MINIDUMP_EXCEPTION_INFORMATION ex_info = { 0 };
			ex_info.ThreadId = ::GetCurrentThreadId();
			ex_info.ExceptionPointers = ex_point;
			ex_info.ClientPointers = NULL;

			// dbghelp.dll V5.1及以上版本均有MiniDumpWriteDump函数，XP至少为6.1.xxx
			MINIDUMP_TYPE mdt = (MINIDUMP_TYPE) (MiniDumpWithPrivateReadWriteMemory
				| MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithFullMemoryInfo
				| MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules);
			BOOL dmp_result = ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), file_handle, mdt, &ex_info, nullptr, nullptr);
			if (!dmp_result)
			{
				file_name.Format(_T("dmp文件创建失败(error id [%u])"), (unsigned int)::GetLastError());
			}
		}
		else
		{
			file_name.Format(_T("dmp文件创建失败(error id [%u])"), (unsigned int)::GetLastError());
		}

		::CloseHandle(file_handle); // 不论文件是否成功创建均关闭

		dump_file_and_error_id += _T(" ") + file_name; // 用一个空格与错误代码分隔开,解析的地方按此规则解析
		dump_file_and_error_id.AppendChar(_T('\"'));
	}

	if (m_need_popup_report_window)
	{
		CString file_name;
		get_app_path_with_xg(file_name);
		file_name += _T("error_report.exe");
		::ShellExecute(nullptr, _T("open"), file_name, dump_file_and_error_id, nullptr, SW_SHOW);
	}

	// 操作系统有时候还是弹出崩溃的提示(win8/8.1系统内部机制改动造成)，此处强制结束确保界面不出现任何出错窗口，便于
	// 程序自动测试等特殊场合使用，退出代码为0表示程序正常退出，可以避免系统报错(win8/8.1之前的系统可以不用也不会报错)
	::TerminateProcess(::GetCurrentProcess(), 0);
	
	// 不要人为退出，可能会导致循环崩溃，而是返回正确的标志，由操作系统自己处理
	// 如果程序已经转储了文件，那么返回EXCEPTION_EXECUTE_HANDLER表示异常已处理，操作系统将不再干涉崩溃处理，但
	// 为方便程序调试，特返回EXCEPTION_CONTINUE_SEARCH表示此异常转由系统处理，借助操作系统的崩溃处理功能，自动
	// 显示调试器调试窗口，就能不新启动VS实例便可接管到这个崩溃问题，并启动调试器调试
	return EXCEPTION_EXECUTE_HANDLER;
}
