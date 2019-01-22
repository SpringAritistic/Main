#pragma once
#include "x3d_data_base.h"



// 表示三维空间中的拉伸操作
class HM_GlobalTool_EXT x3d_action_extrude : public x3d_action_base
{
	DECLARE_SERIAL(x3d_action_extrude)

public:
	x3d_action_extrude();
	virtual ~x3d_action_extrude();

public:
	virtual void do_action(x3d_graph& result);

	// 得到拉伸后的点集
	x3d_action_section* get_extrude_section();

public:	
	// 传入点集及矩阵，点集不能为空
	void set_data(x3d_action_section* sec, x3d_trans_matrix* mtx = nullptr);

	// 传入垂直等截面拉伸的长度, is_reverse为true表示点的顺序与拉伸方向满足右手系，为false则满足左手系
	void set_data(x3d_action_section* sec, double l, bool is_reverse = false);

	// 传入点集及各点转换的回调函数
	void set_data(x3d_action_section* sec, std::function<void(size_t, x3d_action_section::point&)> func);

	x3d_action_section* get_section() const;

	x3d_trans_matrix* get_matrix() const;

private:

	// 根据传入的点集通过回调函数生成拉伸后周围的面
	void get_surround_graph_by_fuc(x3d_graph& result);
	
private:
	x3d_action_section*    m_section;               // 传入的点集
	x3d_trans_matrix*      m_trans_matrix;	   	    // 生成终止图形的变换矩阵	
	double                 m_length;                // 垂直拉伸的距离
	bool                   m_reverse;               // 为true表示点的顺序与拉伸方向满足右手系，为false则满足左手系
	std::function<void(size_t, x3d_action_section::point&)> m_trans_fun_callback; // 顶点转换的回调函数
};

