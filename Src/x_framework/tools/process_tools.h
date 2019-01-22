//////////////////////////////////////////////////////////////////////////
// 网络上充斥着大量进行调用的代码，基本上能看到源码的都不具备商业强度的要求（仅限于教程
// 类的网页，可能有些开源的库能很优秀的做到，不过限于时间和见识暂未了解到，加之这样的库
// 就算有，估计代码量也不会少），因此，自己实现一个轻量的进程调用的库，并能通过匿名管道
// 的方式将标准输出的信息导入到框架进程中，并转到框架的输出接口进行输出，与本框架整合度
// 高，易用性好。 by WYZ
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_PROCESS_TOOLS_H__
#define __HMGLOBALTOOL_TOOLS_PROCESS_TOOLS_H__

#include <functional>

class X_FRAMEWORK_API x_process_object
{
public:
	// 默认构造的是同步调用的进程（阻塞式），由于路径参数为空，因此不具备运行通用，仅出于使用便利性才给默认参数
	x_process_object(const CString& app_path_name = _T(""), const CString& param = _T(""));
	~x_process_object();

public:
	// 可执行文件的路径和名称（可以是exe,com,bat等可执行文件类型）
	CString		m_program_path_name;
	
	// 启动参数（注意:Windows的启动参数有上限，印象中是64K，请注意参数不要过长，尽量控制在300-500字节以内，通
	// 常100个字节内算比较正常，如果要传大量参数，则不适合使用此方式，可以考虑RPC或生成文件，并传文件路径等其它方式）
	CString		m_parameter;

	// 是否等待操作完成
	bool		m_wait_for_termination;

	// time_out仅当wait_for_termination为true时有效，为0表示不等待，为INFINITE表示一直等待直到进程结束
	std::size_t m_time_out;

	// 异步调用时有效，当异步任务结束时会调用此回调函数，同步调用时不用赋值
	std::function<void(const CString&, DWORD&)> m_end_call_back;
	CString*	m_param_1; // 外部指定，用于接收进程结束后的返回信息，m_param_1为任务名称，m_param_2为退出码
	DWORD*		m_param_2; // 外部指定，用于接收进程结束后的返回信息，m_param_1为任务名称，m_param_2为退出码

	// 执行过程中调用此函数检测是否外部要求此任务结束，默认为空对象
	std::function<bool()> m_will_be_stop;
};

// 向框架添加任务对象(不对外提供已添加任务对象的任何操作，只能由框架统一管理)
// 如果任务是要求等待完成才返回，则正常结束返回0，否则返回相关的非0的退出码
// 如果任务是不要求等待，异步完成的，则会在完成时调用相应的回调函数，此接口直接返回0
// 任务对象的参数不要使用引用传递，否则异步时会因为生命周期的原因强制要求外部传入的对象不能用局部对象，这一点体验不好
// 启动后的进程可以使用标准输出进行日志和信息的输出，本框架会以管道方式导入到框架所在的进程，且会对文本的开头字符进行
// 解析，如果以“错误”输出以自动将内容解析为错误输入到本进制的输出接口中，其它的类似，支持的字符串有四类，分别为：
// “错误”，“警告”，“严重警告”，“调试”
// 由于任务允许异步调用，因此异常捕获没有意义，内部不作异常处理
X_FRAMEWORK_API DWORD do_process_task(const CString& task_name, const x_process_object obj); // 在新进程中执行任务
X_FRAMEWORK_API bool has_process_task(const CString& task_name); // 当前是否有指定名称的任务
X_FRAMEWORK_API void force_stop_all_tasks(); // 强行结束所有任务
#endif //__HMGLOBALTOOL_TOOLS_PROCESS_TOOLS_H__
