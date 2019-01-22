#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_AUTO_LOAD_MODULE_H__
#define __HMGLOBALTOOL_TOOLS_AUTO_LOAD_MODULE_H__

// 自动加载模块，程序必须在启动时加载的模块均被此工具加载，主要用于解决窗口类的分模块注册时模块未加载无法动态创建的问题
class X_FRAMEWORK_API auto_load_module
{
private:
	auto_load_module();
	~auto_load_module();

public:
	static void load_model_by_xml();

private:
	static std::vector<CString> m_models;
};

#endif //__HMGLOBALTOOL_TOOLS_AUTO_LOAD_MODULE_H__
