#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_XML_TOOLS_H__	
#define __HMGLOBALTOOL_TOOLS_XML_TOOLS_H__	

#include "tinyxml2.h"

// 获取指定的xml节点的属性，如果获取失败或不存在，则返回默认值
enum class xml_tree_encoding { unknown, utf_8, gb_2312};
HM_GlobalTool_EXT xml_tree_encoding get_xml_tree_encoding(const tinyxml2::XMLElement* node);

// 字符串读取时，如果编码方式指定为unknown，则内部会自动识别xml定义头判断编码，否则按指定编码直接读取（主要是为了已知编码时提高效率）
// xml的节点为空时会返回一个默认值，允许node传入nullptr
HM_GlobalTool_EXT CString get_xml_node_text(const tinyxml2::XMLElement* node, xml_tree_encoding ec = xml_tree_encoding::unknown);
HM_GlobalTool_EXT CString get_xml_node_attribute_str(const tinyxml2::XMLElement* node, const CString& attribute, xml_tree_encoding ec = xml_tree_encoding::unknown);
HM_GlobalTool_EXT bool get_xml_node_attribute_bool(const tinyxml2::XMLElement* node, const CString& attribute); // 不写此属性则返回false
HM_GlobalTool_EXT int get_xml_node_attribute_int(const tinyxml2::XMLElement* node, const CString& attribute);
HM_GlobalTool_EXT unsigned int get_xml_node_attribute_uint(const tinyxml2::XMLElement* node, const CString& attribute);
HM_GlobalTool_EXT float get_xml_node_attribute_float(const tinyxml2::XMLElement* node, const CString& attribute);
HM_GlobalTool_EXT double get_xml_node_attribute_double(const tinyxml2::XMLElement* node, const CString& attribute);

#endif //__HMGLOBALTOOL_TOOLS_XML_TOOLS_H__
