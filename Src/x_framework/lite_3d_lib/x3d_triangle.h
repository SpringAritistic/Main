#pragma once
#include "x3d_data_base.h"


// 表示一条三维空间中的三角形
class X_FRAMEWORK_API x3d_triangle : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_triangle)

public:
	x3d_triangle();
	x3d_triangle(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3);
	virtual ~x3d_triangle();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;

	void set_data(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3);
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	x3d_point       m_a;
	x3d_point       m_b;
	x3d_point       m_c;
};

