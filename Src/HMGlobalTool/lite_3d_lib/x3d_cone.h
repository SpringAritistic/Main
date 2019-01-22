#pragma once
#include "x3d_data_base.h"


// 表示三维空间中的圆锥(坐标原点位于底面的圆心）
class HM_GlobalTool_EXT x3d_cone : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_cone)

public:
	x3d_cone();
	x3d_cone(double r, double h);
	x3d_cone(const x3d_point& p, double r, double h);
	virtual ~x3d_cone();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double r, double h);
	void set_data(const x3d_point& p, double r, double h);
	
public:
	double   m_radius;            // 半径 
	double   m_height;            // 高度
};

