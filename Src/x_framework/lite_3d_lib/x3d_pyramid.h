#pragma once
#include "x3d_data_base.h"


// 表示三维空间中任意边数的正棱锥(圆心位于底面外接圆圆心)
class X_FRAMEWORK_API x3d_pyramid : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_pyramid)

public:
	x3d_pyramid();
    x3d_pyramid(int s, double r, double h);
    x3d_pyramid(const x3d_point& p, int s, double r, double h);
	virtual ~x3d_pyramid();

public:
    virtual void copy(const data_node_base& node, bool copy_tag = false) override;
    virtual void regenerate_vertext(x3d_graph& graph) override;

public:
    void set_data(int s, double r, double h);
    void set_data(const x3d_point& p,int s, double r, double h);
    void set_normal_type(x3d_normal_type type);

public:
    int      m_sides;             // 正多边形边数（坐标原点位于底部正多边形中心）
    double   m_radius;            // 正多边形外接圆半径  
    double   m_height;            // 高度
    x3d_normal_type m_normal_type;//法线方式

};

