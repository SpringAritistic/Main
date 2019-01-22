#pragma once
#ifndef __HMGLOBALTOOL_INTERFACE_CMD_CONTAINER_INTERFACE_H__	
#define __HMGLOBALTOOL_INTERFACE_CMD_CONTAINER_INTERFACE_H__

#include <utility>
#include <map>
#include <set>

#include "app_ui_interface.h"


// 使用post方式异步执行命令调用对象
class X_FRAMEWORK_API command_post_doing_function : public functor_by_message
{
public:
	command_post_doing_function();
	virtual ~command_post_doing_function();

public:
	CString m_domain;
	CString m_command;
	CString m_parameter;

private:
	virtual void call();
};


// 命令对象，一个命令对象是能处理一系列命令的容器
// 一个命令对象要能被框架识别，必须向框架注册实例，对于用户会自己创建的实例会在构造时自动注册，析构时反注册，这
// 种场合最适合用于界面窗口类向框架自动注册，此时由于对象的实例是用户自己产生的，只要会产生实例就不需要手工注册。
// 另一种情况是适用于非界面类，这种类一般用于响应与具体窗口没有直接关联的消息（如项目打开消息），此时用户没有这
// 个必要去自己生成一个对象再注册，而且要自己管理这个对象，可利用框架的注册接口自动生成一个实例直接向框架注册并
// 由框架接管内存，示例如下：
// register_cmd_container_by_class_name(project_commands);
class X_FRAMEWORK_API cmd_container_interface
{
public:
	// 对象构造的时候会自动注册到命令管理类中（非自动回收方式）
	// 派生类务必注意正确给此参数传值，以免造成内存泄漏
	// 为大多数使用场景考虑，即作为界面类时不需要回收
	// 非界面类务必注意此参数是否需要传为true
	cmd_container_interface(bool auto_gc = false);

	// 无论注册的时候是否是自动类型，均会进行反注册
	virtual ~cmd_container_interface();

public:
	// 进行命令响应，当前命令执行后期望不要再往下路由则返回true,否则返回false，如果命令的enable为false则不响应直接返回false
	// 各派生类请注明param的明确含义（如表示文件路径名或其它含义）
	// drive_mode设置或返回此命令的驱动方式（0表示从命令行驱动，1表示由界面直接发起）一般用于界面驱动时不回显到输出窗口
	// 注：不允许传入不支持的命令
	virtual bool fire(const CString& domain, const CString& cmd, const CString& param = _T(""), int drive_mode = 0);

	// 返回命令状态，enable为true表示命令可用，checked为true表示处于选中状态（如果命令为按钮则无效），is_transparent为true为透明命令，不允许传入不支持的命令
	// 实际应用中，很多命令容器的派生类要判断命令是否可用，往往需要调用一些非const的方法才得到，虽然不合理，但是给予支持，因此定为非const的
	virtual void get_status(const CString& domain, const CString& cmd, bool& enable, bool& checked, bool& is_transparent);

	// 设置自身感兴趣的命令，如果过滤的命令为空，表示能接收到全部命令
	// 注意，这里说的全部命令并不是真正的全部，只有域为空或域为app的命令才能接收到，比如app.refresh，各子对象自己
	// 定义的局部命令（如section.new），以及系统内部命令（如sys.start_page）默认是接收不到的，除非在此接口中
	// 显式的指定才能收到。另外，对于同一个命令，多个对象响应时顺序由注册顺应决定，但一些特殊命令（如保存等）框架会
	// 适当干涉执行顺序以满足程序行为合理的考虑
	virtual void set_command_filter(std::vector<std::tuple<CString, CString, bool>>& cmds_can_do) const;
	
public:
	// 当前对象是否支持指定的命令
	bool is_command_supported(const CString& domain, const CString& cmd) const;

	// 判断一个命令是否是透明命令(即在常规命令执行过程中也可以立即触发的命令)，默认为false
	// 用到get_status，因此为非const的
	bool is_transparent_command(const CString& domain, const CString& command);

public:
	// 解析命令得到域和命令，如app.open得到域为app，命令为open，如果域为空，则返回空
	// 注意带透明标记的命令，0为非透明，1为透明，全写示例为：editor.display_dim.1表示此命令为透明命令
	static void parse_command(const CString& cmd_full_text, CString& domain, CString& cmd);
	static void parse_command(const CString& cmd_full_text, CString& domain, CString& cmd, bool& is_transparent);
	
	// 解析命令得到域和命令，如app.open得到域为app，命令为open，如果域为空，则返自动填充为"app"
	static void parse_and_fill_command(const CString& cmd_full_text, CString& domain, CString& cmd);

	// 解析命令得到域，如app.open得到域为app，如果域为空，则返回空
	static CString parse_command_domain(const CString& cmd_full_text);
	
	// 解析命令得到域，如app.open得到域为app，如果域为空，则返自动填充为"app"
	static CString parse_and_fill_command_domain(const CString& cmd_full_text);

	// 解析命令并返回去除域后的命令，如app.open得open
	static CString parse_command_cmd(const CString& cmd_full_text);

	// 解析命令并返回透明标记，true表示透明
	static bool parse_transparent_flag(const CString& cmd_full_text);

	// 给定命令和域合成一个完整的全命令,如果域为空仍按空域处理，此时不会加域和命令之间的点号，不带透明标志
	static CString merge_domain_and_cmd(const CString& domain, const CString& cmd);

	// 给定命令和域合成一个完整的全命令,如果域为空，则返自动填充为"app"，不带透明标志
	static CString merge_and_fill_domain(const CString& domain, const CString& cmd);

	// 给定命令和域合成一个完整的全命令,如果域为空，则返自动填充为"app"，如果透明标记为空则填充"0"
	static CString merge_full_command(const CString& domain, const CString& cmd, bool is_transparent);

	// 给定命令进行检查并填充缺失的域为app，结果返回参数中传入的字符串，字符串当作无透明标记，如果带则会解析错误
	static CString& fill_command_domain(CString& cmd);
};


//////////////////////////////////////////////////////////////////////////

// 命令管理类
class X_FRAMEWORK_API cmd_trigger_manager
{
public:
	// 直接执行指定的命令，如果不存在指定的命令或执行失败，则返回false，如果指定的命令没有域则默认的域为"app"
	// 如果命令响应对象为窗口，且有多个窗口响应相同的命令时（如相同类名的窗口加载不一样路径的数据），优先触发当前窗口
	static bool fire(const CString& domain, const CString& command, const CString& param = _T(""));
	
	// 投递到主程序的消息队列中异步执行（特殊用途，如起始页点击打开窗口，且本身会销毁起始页窗口就需要异步执行）
	static void post_fire(const CString& domain, const CString& command, const CString& param = _T(""));

	// 注册或命令或修改命令类型，成功返回true，否则返回false，如果已经注册则仅修改auto_gc属性并返回true
	// auto_gc为true表示传入的对象由命令管理类接手管理内存，反注册或模块卸载的时候删除
	// auto_gc为true此功能要求传入对象必须是堆上分配的，为false则不限定
	static bool register_command_object(cmd_container_interface* cmd_obj, bool auto_gc = false);

	// 取消指定命令的注册（如果注册时指定自动回收，则会在反注册时析构此对象）
	// 返回true表示被自动回收了，返回false表示未自动回收对象
	// 注意：被回收了不代表元素被delete，而没有被回收则也会从注册对象中移除
	// 注意：如果一个对象被注册为自动管理，则允许外部手工调用这个对象的反注册，但是[[[禁止]]]管理这个对象的
	//      内存，如delete等，否则反注册崩溃！！！
	static bool unregister_command_object(cmd_container_interface* cmd_obj);

	// 清空所有已经注册的命令，自动注册的对象会析构掉
	static void remove_all_command();

	// 返回所有能处理指定命令的对象
	// transparent_type为0表示返回全部能响应的命令处理容器
	// transparent_type为1表示返回全部非透明方式响应的命令处理容器
	// transparent_type为2表示返回全部透明方式响应的命令处理容器
	static std::vector<cmd_container_interface*> get_command_object(
		const CString& domain, const CString& command, int transparent_type = 0);

	// 当前命令是否有对象可以处理，如果没有任何对象能响应则返回false,否则返回true
	static bool has_any_object_can_do(const CString& domain, const CString& command);

	// 返回处理此命令的对象中的状态，返回时按如下优先顺序
	// 1.查找所有响应中有过滤条件且能处理此命令的命令处理对象集合A，所有无过滤条件的命令处理对象集合B
	// 2.在集合A中查找enable和checked均为true的对象，如果有则返回其状态，否则进行后继操作
	// 3.在集合B中查找enable和checked均为true的对象，如果有则返回其状态，否则进行后继操作
	// 4.在集合A中查找enable为true的对象，如果有则返回其状态，否则进行后继操作
	// 5.在集合B中查找enable为true的对象，如果有则返回其状态，否则进行后继操作
	// 6.在集合A中查找checked为true的对象，如果有则返回其状态，否则进行后继操作
	// 7.在集合B中查找checked为true的对象，如果有则返回其状态，否则进行后继操作
	// 8.直接置enable为false，checked为false并返回其状态
	// 为提高程序效率，内部不会遍历六次，而是一次就把6种状态的对象找到，从1往后按优先级逐一返回
	static void get_command_state(const CString& domain, const CString& command,
		bool& enable, bool& checked, bool& is_transparent);

	// 调用get_command_state判断命令是否可用
	static bool is_command_enable(const CString& domain, const CString& command);

	// 调用get_command_state判断命令是否可用
	static bool is_command_checked(const CString& domain, const CString& command);

	// 判断一个命令是否是透明命令(即在常规命令执行过程中也可以立即触发的命令)
	static bool is_transparent_command(const CString& domain, const CString& command);

private:
	static std::map<cmd_container_interface*, bool> m_command_objects;

private:
	cmd_trigger_manager() = delete;
	~cmd_trigger_manager() = delete;
	cmd_trigger_manager(const cmd_trigger_manager&) = delete;
	cmd_trigger_manager(cmd_trigger_manager&&) = delete;
	cmd_trigger_manager& operator=(const cmd_trigger_manager) = delete;
};

// 以自动回收方式注册到命令管理类中，注册成功会返回传入的对象，失败会自动析构传入的对象并返回nullptr，主要用于辅助下面的注册宏
X_FRAMEWORK_API cmd_container_interface* register_auto_gc_cmd_and_return_object(cmd_container_interface* obj);

// 用于根据类名注册一个自动回收的实例到命令管理类中并返回实例对象
// 命令处理对象的实例由用户生成的不能使用此接口注册，交由框架管理的响应对象会在构造时自动注册，析构时自动反注册
#ifndef register_cmd_container_by_class_name
#define register_cmd_container_by_class_name(class_name) \
	register_auto_gc_cmd_and_return_object(new class_name()) // 不要打分号，由外部使用的地方手工加更符合使用习惯
#endif


#endif //__HMGLOBALTOOL_INTERFACE_CMD_CONTAINER_INTERFACE_H__
