#pragma once
#include "x3d_data_base.h"


// 表示三维空间中的球体(坐标原点在球心)
class HM_GlobalTool_EXT x3d_ball : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_ball)

public:
	x3d_ball();
	x3d_ball(double r);
	x3d_ball(const x3d_point& p, double r);
	virtual ~x3d_ball();
	
public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double r);
	void set_data(const x3d_point& p, double r);

public:
	double m_radius;         // 球半径 
};

