#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>


#include "path_object.h"
#include "frame/x_framework/tools/string_tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;



path_object::path_object(const CString& name /*= _T("")*/, bool is_folder/* = true*/)
{
	m_name = name;
	m_is_folder = is_folder;
	m_child = nullptr;
	m_parent = nullptr;
}

path_object::path_object(const path_object& src)
{
	*this = src;
}

path_object::path_object(const CString& path_text, const CString& split)
{
	reset(path_text, split);
}

path_object::path_object(const std::vector<CString>& all_nodes_name, bool last_node_is_folder /*= true*/)
{
	reset(all_nodes_name, last_node_is_folder);
}

path_object::path_object(const std::vector<std::pair<CString, bool>>& all_nodes)
{
	reset(all_nodes);
}

path_object::~path_object()
{
	clear();
}

path_object& path_object::operator=(const path_object& src)
{
	if (this != &src)
	{
		vector<path_object*> all_children = src.get_all_children();
		m_parent = src.m_parent;
		m_name = src.m_name;
		path_object* tmp = this;
		if (!all_children.empty())
			for (auto it = all_children.begin(); it != all_children.end(); ++it)
			{
				tmp->make_child((*it)->m_name, (*it)->is_folder());
				tmp = tmp->m_child;
			}

	}
	return *this;
}

bool path_object::operator==(const path_object& src)
{
	CString self_path = to_path_string(false);
	CString src_path = src.to_path_string(false);
	return 0 == self_path.CompareNoCase(src_path);
}

bool path_object::is_folder() const
{
	return !is_file();
}

bool path_object::is_file() const
{
	return !m_is_folder && !m_child;
}

void path_object::set_as_folder()
{
	m_is_folder = true;
}

void path_object::set_as_file()
{
	if (m_child) clear();
	m_is_folder = false;
}

bool path_object::is_child_path(const path_object* src) const
{
	path_object* cur = const_cast<path_object*>(get_root());
	path_object* src_cur = const_cast<path_object*>(src->get_root());

	while (true)
	{
		if (!cur || !src_cur) return false;
		if (0 != cur->m_name.CompareNoCase(src_cur->m_name)) return false;
		if (cur == this)
		{
			if (src_cur->m_child)
				return true;
			else
				return false;
		}

		cur = cur->m_child;
		src_cur = src_cur->m_child;
	}

	return false;
}

bool path_object::is_self_path(const path_object* src) const
{
	path_object* cur = const_cast<path_object*>(get_root());
	path_object* src_cur = const_cast<path_object*>(src->get_root());

	while (true)
	{
		if (!cur || !src_cur) return false;
		if (0 != cur->m_name.CompareNoCase(src_cur->m_name)) return false;
		if (cur == this)
		{
			if (src_cur->m_child)
				return false;
			else
				return true;
		}

		cur = cur->m_child;
		src_cur = src_cur->m_child;
	}

	return false;
}

bool path_object::is_self_or_child_path(const path_object* src) const
{
	path_object* cur = const_cast<path_object*>(get_root());
	path_object* src_cur = const_cast<path_object*>(src->get_root());

	while (true)
	{
		if (!cur || !src_cur) return false;
		if (0 != cur->m_name.CompareNoCase(src_cur->m_name)) return false;
		if (cur == this) return true;

		cur = cur->m_child;
		src_cur = src_cur->m_child;
	}

	return false;
}

void path_object::set_node_data(const CString& name, bool is_folder)
{
	if (!name) return;
	m_name = name;
	if (is_folder)
	{
		m_is_folder = true;
		return;
	}
	else
	{
		if (m_child) clear();
		m_is_folder = false;
	}
}

void path_object::get_node_data(CString& name, bool& is_folder) const
{
	name = m_name;
	is_folder = m_is_folder;
}

CString path_object::get_node_name() const
{
	return m_name;
}

void path_object::set_node_name(const CString& name)
{
	if (!name) return;
	m_name = name;
}

CString path_object::to_path_string(bool end_with_split, const CString& split /*= _T("\\")*/) const
{
	CString res;
	to_path_string(res, end_with_split, split);
	return res;
}

void path_object::to_path_string(CString& path, bool end_with_split, const CString& split /*= _T("\\")*/) const
{
	vector<CString> all_nodes_name = get_all_node_name();
	string_tools::merge_string(path, all_nodes_name, split, end_with_split);
}

vector<CString> path_object::get_all_node_name() const
{
	vector<pair<CString, bool>> all_nodes = get_all_nodes();
	vector<CString> all_nodes_name;
	for (auto it = all_nodes.begin(); it != all_nodes.end(); ++it)
		all_nodes_name.push_back((*it).first);

	return all_nodes_name;
}

void path_object::get_all_node_name(vector<CString>& result) const
{
	result = get_all_node_name();
}

vector<pair<CString, bool>> path_object::get_all_nodes() const
{
	vector<pair<CString, bool>> all_nodes;
	const path_object* current_node = this;
	while (current_node)
	{
		all_nodes.push_back(make_pair(current_node->m_name, current_node->is_folder()));
		current_node = current_node->get_child();
	}

	return all_nodes;
}

void path_object::get_all_nodes(vector<pair<CString, bool>>& result) const
{
	result = get_all_nodes();
}

void path_object::reset(const CString& path_text, const CString& split /*= _T("\\")*/)
{
	if (path_text.IsEmpty()) return;
	vector<CString> all_nodes_name;
	CString text = path_text;
	if ((text.Right(1) == split))
		text.TrimRight(split);

	string_tools::split_string(all_nodes_name, text, split);
	reset(all_nodes_name, path_text.Right(1) == split);      //若路径字符串尾字符为分隔符，则尾节点是文件夹
}

void path_object::reset(const std::vector<CString>& all_nodes_name, bool last_node_is_folder /*= true*/)
{
	clear();

	if (all_nodes_name.empty()) return;
	m_name = all_nodes_name.front();
	m_is_folder = last_node_is_folder;
	path_object* current_node = this;
	for (size_t i = 1; i < all_nodes_name.size(); ++i)
	{
		if (i + 1 == all_nodes_name.size() && !last_node_is_folder)
			current_node = current_node->make_child(all_nodes_name[i], false);
		else
			current_node = current_node->make_child(all_nodes_name[i], true);
	}
}

void path_object::reset(const std::vector<std::pair<CString, bool>>& all_nodes)
{
	vector<CString> all_nodes_name;
	for (auto it = all_nodes.begin(); it != all_nodes.end(); ++it)
		all_nodes_name.push_back((*it).first);

	reset(all_nodes_name, (*all_nodes.rbegin()).second);
}


void path_object::clear()
{
	if (!m_child)
	{
		m_parent = nullptr;
		return;
	}
	vector<path_object*> all_children = get_all_children();
	for (auto it = all_children.rbegin(); it != all_children.rend(); ++it)
	{
		safe_delete((*it)->m_child);
		m_parent = nullptr;
	}
	m_child = nullptr;
	all_children.clear();
}

path_object* path_object::get_node(const CString& path_text, const CString& split /*= _T("\\")*/)
{
	vector<CString> src_all_node_names = string_tools::split_string(path_text, split);
	return get_node(src_all_node_names);
}

path_object* path_object::get_node(const vector<CString>& path_node)
{
	vector<path_object*> all_children_node = get_all_children();
	if (path_node.empty()) return nullptr;

	if (1 == path_node.size())
	{
		if (0 == m_name.CompareNoCase(path_node.front()))
			return this;
		else
			return nullptr;
	}

	path_object* last_node = nullptr;
	for (size_t i = 0; i < all_children_node.size(); ++i)
	{
		if (path_node.size() <= i) return nullptr;
		if (!all_children_node[i]) return nullptr;

		if (0 != all_children_node[i]->m_name.CompareNoCase(path_node[i])) return nullptr;
		last_node = all_children_node[i];
	}

	return last_node;
}

void path_object::remove_node(const CString& path_text, const CString& split /*= _T("\\")*/)
{
	path_object* tmp_node = get_node(path_text, split);
	remove_node(tmp_node);
}

void path_object::remove_node(const vector<CString>& path_node)
{
	path_object* tmp_node = get_node(path_node);
	remove_node(tmp_node);
}

void path_object::remove_node(path_object* node)
{	
	if (!node) return;
	path_object* parent = node->get_parent();
	if (parent) parent->m_child = node->m_child;
	node->m_child->m_parent = parent;
	node->m_child = nullptr;
	safe_delete(node);
}

bool path_object::is_in_path_node(const CString& path_text, const CString& split /*= _T("\\")*/) const
{
	CString this_path = to_path_string(false, split);
	if (this_path.Find(path_text) == -1)
		return false;
	else
		return true;
}

bool path_object::is_in_path_node(const vector<CString>& path_node_name, bool last_node_is_folder) const
{
	path_object* node=&path_object(path_node_name, last_node_is_folder);
	return is_in_path_node(node);
}

bool path_object::is_in_path_node(path_object* node) const
{
	CString this_path = to_path_string(false);
	CString node_path = node->to_path_string(false);
	if (-1 == this_path.Find(node_path))
		return false;
	else
		return true;
}

void path_object::insert_node(const CString& file_name, const CString& path_text, const CString& split,bool is_back)
{
	path_object* tmp_node = get_node(path_text, split);
	insert_node(file_name, tmp_node, is_back);
}

void path_object::insert_node(const CString& file_name, const vector<CString>& path_node, bool is_back)
{
	path_object* tmp_node = get_node(path_node);
	insert_node(file_name, tmp_node, is_back);
}

void path_object::insert_node(const CString& file_name, path_object* node, bool is_back)
{
	if (!node||!file_name) return;
	path_object* new_node = new path_object(file_name, true);
	if (is_back)
	{
		new_node->m_parent = node->m_parent;
		node->m_parent->m_child = new_node;
		new_node->m_child = node;
		node->m_parent = new_node;
	}
	else
	{
		new_node->m_parent = node;
		new_node->m_child = node->m_child;
		node->m_child->m_parent = new_node;
		node->m_child = new_node;
	}
}

path_object* path_object::push_back(const CString& node_text, bool is_folder)
{
	path_object* tail = get_tail();
	if (!tail) return nullptr;

	path_object* new_nd = new path_object(node_text, is_folder);
	return push_back(new_nd);
}

path_object* path_object::push_back(path_object* node)
{
	if (!node) return nullptr;

	path_object* tail = get_tail();
	if (!tail) return nullptr;

	node->m_child = nullptr;
	node->m_parent = tail;
	tail->m_child = node;
	return node;
}

path_object* path_object::push_front(const CString& node_text, bool is_folder)
{
	path_object* new_nd = new path_object(node_text, is_folder);
	return push_front(new_nd);
}

path_object* path_object::push_front(path_object* node)
{
	if (!node) return nullptr;

	path_object* root = get_root();
	if (!root) return nullptr;

	if (node == root) return root;

	// 先连到root后面
	node->m_child = root->m_child;
	node->m_parent = root;
	root->m_child = node;

	// 再把root和node颠倒内容
	swap(node->m_name, root->m_name);
	swap(node->m_is_folder, root->m_is_folder); // 保留原root的文件夹标志不变
	node->m_is_folder = true; // 加到前面只能为文件夹类型

	return node;
}

void path_object::conect_path_object(path_object* path_obj)
{
	if (!path_obj) return;

	path_object* tail = get_tail();
	if (!tail) return;

	tail->m_child = path_obj;
	path_obj->m_parent = tail;
}

path_object* path_object::get_root()
{
	const path_object* res = this;
	return const_cast<path_object*>(res->get_root());
}

const path_object* path_object::get_root() const
{
	const path_object* res = this;
	while (true)
	{
		if (!res->m_parent) return res;
		res = res->m_parent;
	}

	return nullptr;
}

path_object* path_object::get_tail()
{
	const path_object* res = this;
	return const_cast<path_object*>(res->get_tail());
}

const path_object* path_object::get_tail() const
{
	const path_object* res = this;
	while (true)
	{
		if (!res->m_child) return res;
		res = res->m_child;
	}

	return nullptr;
}

path_object* path_object::make_child(const CString& name, bool is_folder /*= false*/)
{
	if (m_child)
		clear();
	m_child = new path_object(name, is_folder);
	m_child->m_parent = this;
	return m_child;
}

const path_object* path_object::get_child() const
{
	return m_child;
}

path_object* path_object::get_child()
{
	return m_child;
}

const path_object* path_object::get_parent() const
{
	return m_parent;
}

path_object* path_object::get_parent()
{
	return m_parent;
}

vector<path_object*> path_object::get_all_children() const
{
	vector<path_object*> all_children;
	path_object* tmp_node = m_child;
	while (tmp_node)
	{
		all_children.push_back(tmp_node);
		tmp_node = tmp_node->m_child;
	}
	return all_children;
}

