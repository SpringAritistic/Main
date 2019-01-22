#pragma once


// 存储 “快捷键-命令” 的映射关系
class accelerator_map
{
public:
	accelerator_map();
	~accelerator_map();

public:
	void create_accelerator_table_by_xml();
	bool is_accelerator_table_empty() const;
	HACCEL get_accelerator_handle() const;
	CString get_accelerator_command_by_id(DWORD id);

private:
	HACCEL m_accelerators_handle; // 创建的加速键表句柄
	std::map<int, CString> m_db_accelerator_map; // 创建加速键表后的id与命令的映射表

private:
	accelerator_map(const accelerator_map& mp) = delete;
};

