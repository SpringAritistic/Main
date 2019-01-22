#include "stdafx.h"

#include "x_framework/macro.h"
#include "x_framework/tools/file_tools.h"

#include "accelerator_map.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


accelerator_map::accelerator_map()
{
	m_accelerators_handle = nullptr;
}

accelerator_map::~accelerator_map()
{
	// 先处理本程序的工作再处理基类的事
	// 释放自定义的加速键资源
	DestroyAcceleratorTable(m_accelerators_handle);
	m_accelerators_handle = nullptr;
}

bool get_key_value_by_text(const CString& key_text, WORD& key_value)
{
	if (0 == key_text.CompareNoCase(_T("up")))
	{
		key_value = VK_UP;
	}
	else if (0 == key_text.CompareNoCase(_T("down")))
	{
		key_value = VK_DOWN;
	}
	else if (0 == key_text.CompareNoCase(_T("left")))
	{
		key_value = VK_LEFT;
	}
	else if (0 == key_text.CompareNoCase(_T("right")))
	{
		key_value = VK_RIGHT;
	}
	else if (0 == key_text.CompareNoCase(_T("f1")))
	{
		key_value = VK_F1;
	}
	else if (0 == key_text.CompareNoCase(_T("f2")))
	{
		key_value = VK_F2;
	}
	else if (0 == key_text.CompareNoCase(_T("f3")))
	{
		key_value = VK_F3;
	}
	else if (0 == key_text.CompareNoCase(_T("f4")))
	{
		key_value = VK_F4;
	}
	else if (0 == key_text.CompareNoCase(_T("f5")))
	{
		key_value = VK_F5;
	}
	else if (0 == key_text.CompareNoCase(_T("f6")))
	{
		key_value = VK_F6;
	}
	else if (0 == key_text.CompareNoCase(_T("f7")))
	{
		key_value = VK_F7;
	}
	else if (0 == key_text.CompareNoCase(_T("f8")))
	{
		key_value = VK_F8;
	}
	else if (0 == key_text.CompareNoCase(_T("f9")))
	{
		key_value = VK_F9;
	}
	else if (0 == key_text.CompareNoCase(_T("f10")))
	{
		key_value = VK_F10;
	}
	else if (0 == key_text.CompareNoCase(_T("f11")))
	{
		key_value = VK_F11;
	}
	else if (0 == key_text.CompareNoCase(_T("f12")))
	{
		key_value = VK_F12;
	}
	else if (0 == key_text.CompareNoCase(_T("f13")))
	{
		key_value = VK_F13;
	}
	else if (0 == key_text.CompareNoCase(_T("f14")))
	{
		key_value = VK_F14;
	}
	else if (0 == key_text.CompareNoCase(_T("f15")))
	{
		key_value = VK_F15;
	}
	else if (0 == key_text.CompareNoCase(_T("f16")))
	{
		key_value = VK_F16;
	}
	else if (0 == key_text.CompareNoCase(_T("f17")))
	{
		key_value = VK_F17;
	}
	else if (0 == key_text.CompareNoCase(_T("f18")))
	{
		key_value = VK_F18;
	}
	else if (0 == key_text.CompareNoCase(_T("f19")))
	{
		key_value = VK_F19;
	}
	else if (0 == key_text.CompareNoCase(_T("f20")))
	{
		key_value = VK_F20;
	}
	else if (0 == key_text.CompareNoCase(_T("f21")))
	{
		key_value = VK_F21;
	}
	else if (0 == key_text.CompareNoCase(_T("f22")))
	{
		key_value = VK_F22;
	}
	else if (0 == key_text.CompareNoCase(_T("f23")))
	{
		key_value = VK_F23;
	}
	else if (0 == key_text.CompareNoCase(_T("f24")))
	{
		key_value = VK_F24;
	}
	else
	{
		WORD key_char = key_text[0];
		if (key_char >= _T('0') && key_char <= _T('9')
			|| key_char >= _T('a') && key_char <= _T('z')
			|| key_char >= _T('A') && key_char <= _T('Z'))
			key_value = key_char;
		else
			return false;
	}

	return true;
}

void accelerator_map::create_accelerator_table_by_xml()
{
	m_db_accelerator_map.clear();
	DestroyAcceleratorTable(m_accelerators_handle);

	// 开始读取xml配置
	CString xml_file_full_path = file_tools::get_app_path_with_xg() + _T("gui_xml\\accelerator_map.xml");
	tinyxml2::XMLDocument doc;
	if (tinyxml2::XML_SUCCESS != doc.LoadFile(string_tools::CString_to_string(xml_file_full_path).c_str())) return;

	tinyxml2::XMLElement* node = doc.FirstChildElement("accelerator_map");
	if (!node || node->NoChildren()) return;

	node = node->FirstChildElement("accelerator");
	if (!node) return;
	
	static WORD id = ID_ACCELERATOR_CMD_MIN;

	vector<ACCEL> accelerators;
	do
	{
		CString cmd = get_xml_node_attribute_str(node, _T("cmd"));
		CString ctrl = get_xml_node_attribute_str(node, _T("ctrl"));
		CString alt = get_xml_node_attribute_str(node, _T("alt"));
		CString shift = get_xml_node_attribute_str(node, _T("shift"));
		CString key = get_xml_node_attribute_str(node, _T("key"));

		if (cmd.IsEmpty()) continue;
		if (key.IsEmpty()) continue;

		key.MakeUpper(); // 键值定义使用大写字母，否则API不识别

		ACCEL accel;
		accel.fVirt = FVIRTKEY;
		if (_T("true") == ctrl) accel.fVirt |= FCONTROL;
		if (_T("true") == alt) accel.fVirt |= FALT;
		if (_T("true") == shift) accel.fVirt |= FSHIFT;

		if (!get_key_value_by_text(key, accel.key)) continue;

		accel.cmd = id;

		accelerators.push_back(accel);
		m_db_accelerator_map.emplace(make_pair(id, cmd));
			
		++id;
			
		// id为整数，取值在ID_ACCELERATOR_CMD_MIN 到 ID_ACCELERATOR_CMD_MAX之间
		if (id > ID_ACCELERATOR_CMD_MAX) id = ID_ACCELERATOR_CMD_MIN;

		node = node->NextSiblingElement("accelerator");
	} while (node);

	if (!accelerators.empty())
		m_accelerators_handle = CreateAcceleratorTable(accelerators.data(), accelerators.size());
}

bool accelerator_map::is_accelerator_table_empty() const
{
	return m_db_accelerator_map.empty();
}

HACCEL accelerator_map::get_accelerator_handle() const
{
	return m_accelerators_handle;
}

CString accelerator_map::get_accelerator_command_by_id(DWORD id)
{
	auto it = m_db_accelerator_map.find(id);
	if (m_db_accelerator_map.end() == it)
		return _T("");
	else
		return it->second;
}
