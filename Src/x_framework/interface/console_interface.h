#pragma once
#ifndef __HMGLOBALTOOL_INTERFACE_CONSOLE_INTERFACE_H__	
#define __HMGLOBALTOOL_INTERFACE_CONSOLE_INTERFACE_H__

#include <utility>

#include "../tools/mini_file_log.h"


enum  message_type
{
	MT_ERROR = 0,			// 错误
	MT_WARNING = 1,			// 警告
	MT_SERIOUS_WARNING = 2,	// 严重警告
	MT_INFO = 3,			// 一般信息
	MT_DEBUG = 4			// 调试信息
};

enum  output_object
{
	OO_NONE = 0,			// 不输出到任何媒介
	OO_UI_LOG = 1,			// 同时输出到界面窗口和默认日志文件
	OO_UI = 2,				// 仅输出到界面窗口
	OO_LOG = 3				// 仅输出到系统默认日志文件
};


// 命令行响应接口，任何对象，要能接收到命令行的交互输入信息，并能和命令行交互实时输出，必须从此接口派生
class X_FRAMEWORK_API console_session_interface
{
public:
	console_session_interface();
	virtual ~console_session_interface();

public:
	// 命令控制台告之本对象所持有会话被终止，如果本对象没有持有会话，则不会被通知到
	// 会话持有时输入会被重定向到输出窗口，当其它窗口申请会话后本对象将失去会话权
	// 此接口由框架调用，如果拥有会话的对象想要主动释放会话权可以调用end_console_session()完成
	virtual void session_be_finished() = 0;

	// 命令控制台向本对象发来操作动作，0表示按了ESC键，1表示按了回车键，2表示按了空格键
	// 命令行中的数据要本对象自己取，包括将命令行中的信息推到输出列表或清空也由本对象决定
	virtual void on_console_action(int action_type) = 0;

	// 是否处于与命令控制台的交互执行过程中，此接口很重要，请正确实现，否则会干扰命令控制台行为
	// 注意，此接口并不是返回是否有会话联系，而是表示在有会话的情况下可能会根据命令行动作执行相应的交互功能，而
	// 一个交互动作没有结束时返回true，否则返回false，比如现在正处于绘制一条线的过程中，用户只输入了起点，但还
	// 没有输入终点时，就算作是处于命令执行过程中
	virtual bool is_doing_command() = 0;

public:
	// 文字转发接口 ，将当前对象的字符输入转发给命令控制台，以实现在命令行同步输出的效果
	// 一般用于转发key_char事件，而不用于转发key_down事件，key_down事件由本对象派生类自行处理掉，不需要交互
	// 只有本对象持有会话权时才有效
	void notify_console_char_input(UINT char_value, UINT repeat_count, UINT flags);

	// 请求与命令行建立会话，只有请求建立会话之后命令控制台才会将实现交互的信息发到本对象
	// 请注意，在会话过程中，如果有其它对象也请求会话，则本对象会收到会话被终止的消息，且会话转到发起请求的对象上
	// 如果本对象已经有会话权，则再次请求将忽略请求，保持会话联系不变
	// 本对象析构时会自动结束会话
	void request_console_session();

	// 结束会话，只有本对象拥有会话权此接口才生效，否则无效，结束会话时不会调用本接口的session_be_finished方法
	void end_console_session();
};


// 命令行接口，同时支持也支持信息输出
class X_FRAMEWORK_API console_interface
{
public:
	console_interface();
	console_interface(const console_interface&) = delete;
	console_interface(console_interface&&) = delete;
	console_interface& operator=(const console_interface&) = delete;
	virtual ~console_interface();

public:
	// 获取当前命令行输入，不含前导符内容
	virtual CString get_current_input() const;

	// 清空当前命令行中输入的前导符后面的内容
	virtual void clear_current_input();

	// 清空输出内容，此接口用于派生类实现，一般用于清空界面显示，建议派生类不要在此接口中清空内部错误计数
	virtual void clear_all_output();

	// 添加命令行提示或清空上次提示及输入信息
	// new_line为true则先将编辑框中的有效文字上到列表，然后将传入内容写入编辑框，否则直接将传入内容追加到编辑框内容末尾
	// end_line为true表示内容添加后自动刷到列表，为false表示新加的内容留在编辑窗口,只要end_line不为true则中途输出的所有信息均不添加命令行前缀
	// 如果输入信息prompt为空，则表示将命令行编辑框中的前导符以及数据刷到列表中，且清空当前编辑框内容并仅显示系统默认前导符
	virtual void add_prompt(const CString& prompt, bool new_line = true, bool end_line = false);

	// 在编辑框的内容末尾插入新的输入参数，所添加的内容不会当作提示符来处理
	// 添加的内容不会导致编辑框中的内容刷到列表中
	virtual void add_input(const CString& str);

public:
	// 命令控制台触发以下动作时调用，0表示按了ESC键，1表示按了回车键，2表示按了空格键
	void notify_console_action(int action_type);

	// 设置要记住的最后一次用户在命令行中执行的命令
	void set_last_command(const CString& domain, const CString& command);
	void get_last_command(CString& domain, CString& command);
	void clear_last_command(); // 将域和命令均清为空字符串

	// 设置默认的命令前导提示符，当没有命令在执行时命令行会自动显示前导符
	void set_default_prompt(const CString& prompt);
	CString get_default_prompt() const;

	// 输出输出窗口的的警告、错误等数量统计信息（清空的不计在内）
	void print_message_count_report();

	// 统计从最后一次计数置为0以后的所有错误总数（后台的总错误数，不是输出窗口的错误数，输出窗口的清空操作对它没有影响）
	long get_total_error_count() const;
	
	// 置错误总数为0
	void reset_total_error_count();

	// 把文字输出到日志文件中
	void save_text_to_log_file(const CString& text);

	// 重新设置日志文件的输出位置，此操作会正常关闭日志文件后再打开新位置处的输出日志，请提供已存在的正确路径（文件可以不存在）
	void reset_log_file_path_name(const CString& path_name);
	
	// 将日志设置为框架默认的路径和名称（默认为bin\\main_application_log.txt）
	void reset_log_file_default_file_path_name();

	// 信息输出工具类，保持编辑框中的内容不变，直接将内容输入到列表末尾
	// 支持直接输出CString或字符数组，支持带格式符输出，使用示例：
	// db_output::write(_T("output text"));
	// db_output::write(_("output a integer : %d, a double %lf"), 123, 456.789);
	// CString info = _T("text in CString object"); db_output::write(info);
	// 内部均通过print_info_implement来实现，派生类实现print_info_implement即可
	void print_info(const TCHAR* format, ...); // 以 MT_INFO 类型输出到 OO_UI_LOG
	void print_info(message_type type, const TCHAR* format, ...); // 以指定类型输出 OO_UI_LOG
	void print_info(output_object out, message_type type, const TCHAR* format, ...); // 以指定类型输出到指定目标

protected:
	// 文字转发接口 ，将其他能与控制台交互的对象的key_char输入转发到本控制台,一般用于实现该对象的键盘输入自动同步到控制台
	// 只有持有会话权的对象才会自动将输入重定向到控制台，对象要有会话权需要从console_session_interface派生并调用申请接口
	virtual void on_char_input(UINT char_value, UINT repeat_count, UINT flags) = 0;
	
	// 此接口为输入信息的实现接口，派生类实现时请不要改为public访问权限，因为对外的输入接口是print_info
	// 派生类实现时，务必要调用基类的此函数，否则错误数字统计不正确。（基类实现仅统计错误、警告数）
	virtual void print_info_implement(output_object out, message_type type, const CString& text) = 0;

private:
	// 后台的总错误数，不是输出窗口的错误数，统计从最后一次计数置为0以后的所有错误总数（输出窗口的清空操作对它没有影响）
	long	m_total_error_count;

	// 统计最后一次清理之后的输出信息
	long	m_error_count;
	long	m_warning_count;
	long	m_serious_warning_count;

	CString m_default_prompt;
	std::pair<CString, CString> m_last_command;

	// 当前会话对象，此对象的内存由外部管理
	console_session_interface* m_current_session_obj;

	// 用于日志输出的文件
	mini_file_log m_log_file;

private:
	friend class console_session_interface;
};


//////////////////////////////////////////////////////////////////////////


// 获取或设置命令行实例(实例对象的内存由外部管理，接口内不干涉)
X_FRAMEWORK_API console_interface* get_console_interface();
X_FRAMEWORK_API void set_console_interface(console_interface* console_obj);

// 直接以 MT_INFO 类型输出到日志（没有界面也能正常工作）
X_FRAMEWORK_API void app_print_info(const TCHAR* format, ...);

// 以指定类型直接输出到日志（没有界面也能正常工作）
X_FRAMEWORK_API void app_print_info(message_type type, const TCHAR* format, ...);

// 以指定类型直接输出到日志（没有界面也能正常工作）
X_FRAMEWORK_API void app_print_info(output_object out, message_type type, const TCHAR* format, ...);
#endif //__HMGLOBALTOOL_INTERFACE_CONSOLE_INTERFACE_H__
