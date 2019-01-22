#pragma once
#include "x3d_data_base.h"


// 表示OXY平面上的一段圆弧，这个圆弧上的点必定在同一平面内(坐标原点位于圆弧的圆心）
class HM_GlobalTool_EXT x3d_arc : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_arc)

public:
	x3d_arc();
	x3d_arc(double r, double a, double s, x3d_plane_type tp = x3d_plane_type::oxy);  // 半径.圆弧度数(0-2PI).圆弧起点与OX/OY正半轴夹角(0-2PI)
	x3d_arc(const x3d_point& p, double r, double a, double s, x3d_plane_type tp = x3d_plane_type::oxy);
	virtual ~x3d_arc();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double r, double a, double s, x3d_plane_type tp = x3d_plane_type::oxy);
	void set_data(const x3d_point& p, double r, double a, double s, x3d_plane_type tp = x3d_plane_type::oxy);

public:
	double           m_radius;            // 半径 
	double           m_angle;             // 圆弧的度数（0-2PI）
	double           m_start_angle;       // 圆弧起点与ox/oy正半轴的角度（0-2PI）
	x3d_plane_type   m_type;              // 圆弧是在哪个平面上：oxy平面，oxz平面，oyz平面
};

// 用三点表示一段圆弧
class HM_GlobalTool_EXT x3d_arc2
{
public:
	x3d_arc2();
	x3d_arc2(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3);
	~x3d_arc2();

public:
	// 求圆心
	x3d_point get_center() const;

	void set_data(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3);

public:
	x3d_point m_p1;
	x3d_point m_p2;
	x3d_point m_p3;

};