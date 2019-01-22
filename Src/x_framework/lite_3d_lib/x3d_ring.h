#pragma once
#include "x3d_data_base.h"


// 表示三维空间中的圆环（截面为正圆形的环，类似于手镯的造型）
class X_FRAMEWORK_API x3d_ring : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_ring)

public:
	x3d_ring();
	x3d_ring(double inner, double outer);
	x3d_ring(const x3d_point& p, double inner, double outer);
	virtual ~x3d_ring();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double inner, double outer);
	void set_data(const x3d_point& p, double inner, double outer);

private:
	double inner_radius; //内径
	double outer_radius; //外径
};

