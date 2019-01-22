#include "StdAfx.h"

#include "../tools/string_tools.h"

#include "xml_tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


inline string get_declaration_encoding(const string& dec_text)
{
	string text_lower(dec_text);
	for (char& ch : text_lower) ch = tolower(ch);

	string::size_type pos_s = dec_text.find("encoding"); // 不能用find_first_of否则只会匹配字符串中的任意字符即返回位置
	if (string::npos == pos_s) return "";

	pos_s = text_lower.find_first_of('\"', pos_s);
	if (string::npos == pos_s) return "";

	string encoding = "";
	string::size_type pos_e = text_lower.find_first_of('\"', pos_s + 1);
	if (string::npos == pos_e)
		encoding = text_lower.substr(pos_s + 1);
	else
		encoding = text_lower.substr(pos_s + 1, pos_e - pos_s - 1); // pos_e只可能大于pos_s，所以子串长度不会为0或负数

	return encoding;
}

xml_tree_encoding get_xml_tree_encoding(const tinyxml2::XMLElement* node)
{
	if (!node) return xml_tree_encoding::unknown;

	const tinyxml2::XMLDocument* doc = node->GetDocument();
	tinyxml2::XMLDeclaration* dec = doc ? const_cast<tinyxml2::XMLDocument*>(doc)->NewDeclaration() : nullptr;
	const char* dec_text = dec ? dec->Value() : nullptr;
	string dec_info(dec_text ? dec_text : "");
	safe_delete_array(dec_text); // 内存要自己释放

	string encoding = get_declaration_encoding(dec_info);
	for (char& ch : encoding) ch = tolower(ch);

	if ("utf-8" == encoding)
		return xml_tree_encoding::utf_8;
	else if ("gb2312" == encoding)
		return xml_tree_encoding::gb_2312; // 为MBCS编码
	else
		return xml_tree_encoding::unknown;
}

CString get_xml_node_attribute_str(const tinyxml2::XMLElement* node, const CString& attribute, xml_tree_encoding ec/* = xml_tree_encoding::unknown*/)
{
	CString result = _T("");
	if (!node) return result;

	// 属性名一般不用中文，一般情况下只用英文和数字还有下划线组合，且不以数字开头，因此不用对属性按xml编码进行转换
	const char* res = node->Attribute(string_tools::CString_to_string(attribute).c_str());
	if (res)
	{
		if (xml_tree_encoding::unknown == ec) ec = get_xml_tree_encoding(node);

		string attr(res);
		if (xml_tree_encoding::utf_8 == ec)
			result = string_tools::utf8_to_CString(attr); // utf8
		else if (xml_tree_encoding::utf_8 == ec)
			result = string_tools::string_to_CString(attr); // 为MBCS编码
		else
			result = string_tools::string_to_CString(attr); // 其它编码暂不支持，当作MBCS处理
	}

	// safe_delete_array(res); 内存由tinyxml自动处理

	return result;
}

bool get_xml_node_attribute_bool(const tinyxml2::XMLElement* node, const CString& attribute)
{
	if (!node) return false;

	// 属性名一般不用中文，一般情况下只用英文和数字还有下划线组合，且不以数字开头，因此不用对属性按xml编码进行转换
	return node->BoolAttribute(string_tools::CString_to_string(attribute).c_str());
}

int get_xml_node_attribute_int(const tinyxml2::XMLElement* node, const CString& attribute)
{
	if (!node) return 0;

	// 属性名一般不用中文，一般情况下只用英文和数字还有下划线组合，且不以数字开头，因此不用对属性按xml编码进行转换
	return node->IntAttribute(string_tools::CString_to_string(attribute).c_str());
}

unsigned int get_xml_node_attribute_uint(const tinyxml2::XMLElement* node, const CString& attribute)
{
	if (!node) return 0;

	// 属性名一般不用中文，一般情况下只用英文和数字还有下划线组合，且不以数字开头，因此不用对属性按xml编码进行转换
	return node->UnsignedAttribute(string_tools::CString_to_string(attribute).c_str());
}

float get_xml_node_attribute_float(const tinyxml2::XMLElement* node, const CString& attribute)
{
	if (!node) return 0.0f;

	// 属性名一般不用中文，一般情况下只用英文和数字还有下划线组合，且不以数字开头，因此不用对属性按xml编码进行转换
	return node->FloatAttribute(string_tools::CString_to_string(attribute).c_str());
}

double get_xml_node_attribute_double(const tinyxml2::XMLElement* node, const CString& attribute)
{
	if (!node) return 0.0;

	// 属性名一般不用中文，一般情况下只用英文和数字还有下划线组合，且不以数字开头，因此不用对属性按xml编码进行转换
	return node->DoubleAttribute(string_tools::CString_to_string(attribute).c_str());
}

CString get_xml_node_text(const tinyxml2::XMLElement* node, xml_tree_encoding ec /*= xml_tree_encoding::unknown*/)
{
	if (!node) return _T("");

	const char* text_buffer = node->GetText();
	string text = (text_buffer ? text_buffer : "");
	// safe_delete(text_buffer); // 不能删除，xml内部会处理

	if (xml_tree_encoding::unknown == ec) ec = get_xml_tree_encoding(node);

	CString t_text;
	if (xml_tree_encoding::utf_8 == ec)
		t_text = string_tools::utf8_to_CString(text);
	else if (xml_tree_encoding::gb_2312 == ec)
		t_text = string_tools::string_to_CString(text);
	else
		t_text = string_tools::string_to_CString(text);

	return t_text;
}
