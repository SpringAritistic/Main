#pragma once
#ifndef __HMGLOBALTOOL_TOOLS_PATH_OBJECT_H__
#define __HMGLOBALTOOL_TOOLS_PATH_OBJECT_H__

#include <utility>
#include <vector>


// 表示路径对象中的一个路径节点
class X_FRAMEWORK_API path_object
{
public:
	path_object(const CString& name = _T(""), bool is_folder = true);
	path_object(const CString& path_text, const CString& split);
	path_object(const std::vector<CString>& all_nodes_name, bool last_node_is_folder = true); // 参考原型相同的reset成员
	path_object(const std::vector<std::pair<CString, bool>>& all_nodes); // 参考原型相同的reset成员
	path_object(const path_object& src);
	virtual ~path_object();

public:
	path_object& operator=(const path_object& src);
	bool operator==(const path_object& src);

public:
	//是否是文件夹
	bool is_folder() const;

	//是否是文件
	bool is_file() const;

	//得到子节点或父节点的指针
	path_object* get_child();
	const path_object* get_child() const;
	path_object* get_parent();
	const path_object* get_parent() const;

	// 若设置为文件，则需判断是否有子节点，若有则须先清空子节点
	void set_as_folder();
	void set_as_file();

	// 判断传入的路径对象代表的路径是不是当前节点的子路径，当前节点会获取相对于根的全路径后再与传入路径相比
	bool is_child_path(const path_object* src) const;
	
	// 判断传入的路径对象代表的路径是不是与当前节点的路径相同，当前节点会获取相对于根的全路径后再与传入路径相比
	bool is_self_path(const path_object* src) const;

	// 判断传入的路径对象代表的路径是不是与当前节点的路径相同或者是子路径，当前节点会获取相对于根的全路径后再与传入路径相比
	bool is_self_or_child_path(const path_object* src) const;

	// 若设置为文件节点，则需判断是否有子节点，若有则须先清空子节点
	void set_node_data(const CString& name, bool is_folder);
	void get_node_data(CString& name, bool& is_folder) const;

	// 得到或设置节点文件名
	CString get_node_name() const;
	void set_node_name(const CString& name);

	// 提取存储的路径树生成路径字符串，默认以\分割，也可按指定分隔符分隔
	// end_with_split表示尾端自动加带上一个split分隔符，否则不要
	CString to_path_string(bool end_with_split, const CString& split = _T("\\")) const;
	void to_path_string(CString& path, bool end_with_split, const CString& split = _T("\\")) const;

	// 得到各级节点的名称，当前节点作为根节点放0号元素，依次往后
	std::vector<CString> get_all_node_name() const;
	void get_all_node_name(std::vector<CString>& result) const;

	// 得到各级节点的名称，当前节点作为根节点放0号元素，依次往后，first表示节点名，second表示是否为文件夹节点
	std::vector<std::pair<CString, bool>> get_all_nodes() const;
	void get_all_nodes(std::vector<std::pair<CString, bool>>& result) const;

	//根据提供的路径字符串生成路径树,默认以\分割，若不是请显示提供第二个参数
	void reset(const CString& path_text, const CString& split = _T("\\"));

	// 根据指定的各级节点构造路径对象，0号元素为根，依次往后
	// last_node_is_folder为true表示最后一个节点当作文件夹处理，否则当作文件处理
	void reset(const std::vector<CString>& all_items_name, bool last_node_is_folder = true);

	// 根据指定的各级节点构造路径对象，0号元素为根，依次往后，first表示节点名，second表示是否为文件夹节点
	void reset(const std::vector<std::pair<CString, bool>>& all_items);

	// 根据路径得到指定的节点
	path_object* get_node(const CString& path_text, const CString& split = _T("\\"));

	// 根据路径得到指定的节点，0号元素表示根节点名称
	path_object* get_node(const std::vector<CString>& path_node);

	// 移除指定的节点
	void remove_node(const CString& path_text, const CString& split = _T("\\"));

	// 移除指定的节点，0号元素为根，依次往后
	void remove_node(const std::vector<CString>& path_node);

	// 移除指定的节点，如果指定节点不是树的节点则不处理
	void remove_node(path_object* node);

	// 判断指定的节点是否是树上的节点
	bool is_in_path_node(const CString& path_text, const CString& split = _T("\\")) const;

	// 判断指定的节点是否是树上的节点，0号元素为根，依次往后
	bool is_in_path_node(const std::vector<CString>& path_node_name, bool last_node_is_folder = true) const;

	// 判断指定的节点是否是树上的节点，如果指定节点不是树的节点则不处理
	bool is_in_path_node(path_object* node) const;

	// 在指定节点处插入新节点,is_back为true表示在指定的节点前插入，否则在指定的节点后面插入
	// 如果指定的节点不存在，则不处理
	void insert_node(const CString& file_name, const CString& path_text, const CString& split = _T("\\"), bool is_back = true);

	// 在指定节点处插入新节点,is_back为true表示在指定的节点前插入，否则在指定的节点后面插入
	// 如果指定的节点不存在，则不处理
	void insert_node(const CString& file_name, const std::vector<CString>& path_node, bool is_back = true);

	// 在指定节点处插入新节点,is_back为true表示在指定的节点前插入，否则在指定的节点后面插入
	// 如果指定的节点不存在，则不处理
	// 注：插入的节点只能是堆上的对象，且不能为多个节点
	void insert_node(const CString& file_name, path_object* node, bool is_back = true);

	// 在最后面新增一个节点，返回新的尾节点（操作失败返回nullptr)
	path_object* push_back(const CString& node_text, bool is_folder);
	path_object* push_back(path_object* node); // 只能为一个节点，对象必须在堆上分配且交由本类管理

	// 在最前面新增一个节点并返回头节点（操作失败返回nullptr)
	// 接口内会确保头节点地址保持不变，插入前的头节点结构含义不变，但内容会变为新插入的节点内容
	path_object* push_front(const CString& node_text, bool is_folder);
	path_object* push_front(path_object* node); // 只能为一个节点，对象必须在堆上分配且交由本类管理

	// 将另一个路径串(可以是一条多节点的路径串)连接到当前串的后面，对象必须在堆上分配且交由本类管理
	void conect_path_object(path_object* path_obj);

	// 得到根节点
	path_object* get_root();
	const path_object* get_root() const;

	// 得到末尾节点
	path_object* get_tail();
	const path_object* get_tail() const;

	// 在堆上构造子节点对象
	path_object* make_child(const CString& name, bool is_folder = true);
	
	// 清空子节点
	void clear();                  

protected:
	//内部使用，得到所有子节点
	std::vector<path_object*> get_all_children() const;

protected:
	CString m_name = _T("");
	path_object* m_child = nullptr;
	path_object* m_parent = nullptr;
	bool m_is_folder = true;
};

#endif //__HMGLOBALTOOL_TOOLS_PATH_OBJECT_H__
