#pragma once
#include "x3d_data_base.h"


// 表示一条三维空间中的一个矩形框(矩形框处在oxy平面上，坐标原点位于矩形框中心)
class HM_GlobalTool_EXT x3d_rectangle : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_rectangle)

public:
	x3d_rectangle();
    x3d_rectangle(double l, double w);
    x3d_rectangle(const x3d_point& p,double l, double w);
	virtual ~x3d_rectangle();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
    virtual void regenerate_vertext(x3d_graph& graph) override;

public:
    void set_data(double l, double w);
    void set_data(const x3d_point& p, double l, double w);
	

public:
	double	m_l;		// 长
	double	m_w;		// 宽
};

