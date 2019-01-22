#pragma once
#include "x3d_data_base.h"


// 表示三维空间中的椭圆圈  (椭圆圈位于oxy平面上，坐标原点位于椭圆的圆心）
class HM_GlobalTool_EXT x3d_ellipse : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_ellipse)

public:
	x3d_ellipse();
	x3d_ellipse(double a, double b);
	x3d_ellipse(const x3d_point& p, double a, double b);
	virtual ~x3d_ellipse();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double a, double b);
	void set_data(const x3d_point& p, double a, double b);
	
public:
	double   m_a;                 //  椭圆长半轴长 （x轴方向）
	double   m_b;                 //  椭圆短半轴长 （y轴方向）
};

