#pragma once
#include "x3d_data_base.h"


// 表示三维空间中的椭球体(坐标原点位于椭球心)
class HM_GlobalTool_EXT x3d_ellipsoid : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_ellipsoid)

public:
	x3d_ellipsoid();
	x3d_ellipsoid(double m_x, double m_y, double m_z);
	x3d_ellipsoid(const x3d_point& p ,double m_x,double m_y,double m_z);
	virtual ~x3d_ellipsoid();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double m_x, double m_y, double m_z);
	void set_data(const x3d_point& p, double m_x, double m_y, double m_z);

public:
	double m_x_axis_radius;	   //x轴方向的半长轴
	double m_y_axis_radius;	   //y轴方向的半长轴
	double m_z_axis_radius;	   //z轴方向的半长轴
};

