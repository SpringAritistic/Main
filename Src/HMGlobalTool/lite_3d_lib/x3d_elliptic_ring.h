#pragma once
#include "../interface/serialize_interface.h"
#include "x3d_data_base.h"


// 表示三维空间中的椭圆环
class HM_GlobalTool_EXT x3d_elliptic_ring : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_elliptic_ring)

public:
	x3d_elliptic_ring();
	x3d_elliptic_ring(double a, double b, double radius);
	x3d_elliptic_ring(const x3d_point& p, double a, double b, double radius);

	virtual ~x3d_elliptic_ring();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double a, double b, double radius);
	void set_data(const x3d_point& p, double a, double b, double radius);

public:
	double m_semi_major_axis;
	double m_semi_minor_axis;
	double m_radius;
};

