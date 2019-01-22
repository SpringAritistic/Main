#include "stdafx.h"

#include "tinyxml2.h"
#include "xml_tools.h"
#include "file_tools.h"
#include "string_tools.h"
#include "common_tools.h"

#include "auto_load_module.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


std::vector<CString> auto_load_module::m_models;

auto_load_module::auto_load_module()
{
}

auto_load_module::~auto_load_module()
{
}


void auto_load_module::load_model_by_xml()
{
	m_models.clear();

	bool is_debug = app_is_debug_mode();

	// 开始读取xml配置
	CString xml_file_full_path = file_tools::get_app_path_with_xg() + _T("gui_xml\\app_auto_load_module.xml");
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(xml_file_full_path).c_str())) return;

	tinyxml2::XMLElement* node = doc.FirstChildElement("app_auto_load_module");
	if (!node || node->NoChildren()) return;

	if (is_debug)
		node = node->FirstChildElement("debug");
	else
		node = node->FirstChildElement("release");
	
	if (!node || node->NoChildren()) return;

	// 加载所有的模块名
	node = node->FirstChildElement("module");
	if (!node) return;

	do
	{
		CString model_path = get_xml_node_attribute_str(node, _T("path_name"));
		if (!model_path.IsEmpty()) m_models.push_back(model_path);

		node = node->NextSiblingElement("module");
	} while (node);

	for (CString& name : m_models)
	{
		if (_T("\\") == name.Left(1) || _T("/") == name.Left(1)) name.Delete(0);

		name.Trim();
		if (name.IsEmpty()) continue;
		if (_T("\\") == name.Left(1) || _T("/") == name.Left(1)
			|| _T("\\") == name.Right(1) || _T("/") == name.Right(1))
			continue;

		CString path_name;
		if (-1 == name.Find(_T(':'))) // 没有:号认为是相对路径
			path_name = file_tools::get_app_path_with_xg() + name;
		else
			path_name = name;

		UINT old_error_mode = GetErrorMode();
		SetErrorMode(0); // 所有错误都弹出来,便于定位到底是哪个关联加载起来的dll加载失败

		// For MFC applications that load extension DLLs, we recommend that you use AfxLoadLibrary
		// instead of LoadLibrary. AfxLoadLibrary handles thread synchronization before you call
		// LoadLibrary. The interface (function prototype) to AfxLoadLibrary is the same as LoadLibrary.
		if (NULL == AfxLoadLibrary(path_name))
			AfxMessageBox(_T("模块：") + path_name + _T("无法加载"), MB_OK | MB_ICONERROR);

		SetErrorMode(old_error_mode);
	}
}
