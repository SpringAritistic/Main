#pragma once
#include "x3d_data_base.h"


// 表示三维空间中的圆圈 (圆在oxy平面上，坐标原点位于圆的圆心）
class X_FRAMEWORK_API x3d_circle : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_circle)

public:
	x3d_circle();
	x3d_circle(double r, x3d_plane_type tp = x3d_plane_type::oxy);
	x3d_circle(x3d_point& p, double r, x3d_plane_type tp = x3d_plane_type::oxy);
	virtual ~x3d_circle();


public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double r, x3d_plane_type tp = x3d_plane_type::oxy);
	void set_data(x3d_point& p, double r, x3d_plane_type tp = x3d_plane_type::oxy);

public:
	double m_radius;              // 坐标原点在圆心，z坐标为0
	x3d_plane_type   m_type;              // 圆是在哪个平面上：oxy平面，oxz平面，oyz平面
};

