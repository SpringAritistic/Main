#pragma once
#include "x3d_data_base.h"


class x3d_polygon;

// 表示三维空间中的融合操作
class HM_GlobalTool_EXT x3d_action_fuse : public x3d_action_base
{
	DECLARE_SERIAL(x3d_action_fuse)

public:
	x3d_action_fuse();
	virtual ~x3d_action_fuse();

public:
	virtual void do_action(x3d_graph& result);

public:
	// 传入初始及终止面点集
	// 初始及终止面点集可以没有配对关键字，但需保证第一个及最后一个点相对应
	void set_data(x3d_action_section* beg, x3d_action_section* end);

	// 传入初始及终止面点集,以及关键点对应的拉伸轨迹
	void set_data(x3d_action_section* beg, x3d_action_section* end, size_t step, std::function<x3d_polygon*(size_t)> func);

private:
	x3d_action_section*        m_section_begin;               // 初始面的点集
	x3d_action_section*        m_section_end;                 // 终止面的点集
	size_t                     m_step;                        // 分成多少段，需与polygon相对应
	std::vector<x3d_polygon*>  m_all_track;                   // 各点对应的拉伸轨迹
};

