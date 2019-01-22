#pragma once
#include "x3d_data_base.h"


// 表示平面上的一段多次抛物线（只有最高次项，形如Y=aX`n），坐标原点位于抛物线顶点
class HM_GlobalTool_EXT x3d_parabola : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_parabola)

	// 表示抛物线所在的平面及开口的朝向：xy表示在oxy平面，第一个字母表示开口朝向，带下划线的表示负方向
	enum direction
	{
		xy = 0,
		_xy = 1,
		yx = 2,
		_yx = 3,
		xz = 4,
		_xz = 5,
		zx = 6,
		_zx = 7,
		yz = 8,
		_yz = 9,
		zy = 10,
		_zy = 11
	};

public:
	x3d_parabola();
	x3d_parabola(double w, double h, double ll, double rl, direction dr = direction::yz, float n = 2);  // 传入抛物线半边的宽度和高度，以及朝开口方向看左边的宽和右边的宽
	x3d_parabola(const x3d_point& p, double w, double h, double ll, double rl,
		direction dr = direction::yz, float n = 2);
	virtual ~x3d_parabola();
	
public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double w, double h, double ll, double rl, direction dr = direction::yz, float n = 2);
	void set_data(const x3d_point& p, double w, double h, double ll, double rl,
		direction dr = direction::yz, float n = 2);
public:
	double           m_width;                   // 抛物线半边的宽度
	double           m_height;                  // 抛物线半边的高度
	double           m_left_length;             // 往抛物线开口方向看左半边的长度
	double           m_right_length;            // 往抛物线开口方向看右半边的长度
	direction        m_dir;                     // 抛物线所在平面及开口方向
	float            m_n;                       // 抛物线的幂次(默认为2)
};
