#pragma once
#include "x3d_data_base.h"


class x3d_polygon;

// 表示三维空间中的放样操作
class X_FRAMEWORK_API x3d_action_setting_out : public x3d_action_base
{
	DECLARE_SERIAL(x3d_action_setting_out)

public:
	x3d_action_setting_out();
	virtual ~x3d_action_setting_out();

public:	
	void do_action(x3d_graph& result);

public:
	// 
	std::vector<std::pair<size_t, x3d_action_section*>>  m_path_section;  // vector<pair<路径上点的索引， 此点上对应的截面>>
	std::vector<x3d_point> m_path; // 放样的路径
};

