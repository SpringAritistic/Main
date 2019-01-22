#pragma once
#include "x3d_data_base.h"


// 表示三维空间中的一个胶囊体
class X_FRAMEWORK_API x3d_capsule : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_capsule)

public:
	x3d_capsule();
	x3d_capsule(double r, double l);
	x3d_capsule(const x3d_point& p, double r, double l);
	virtual ~x3d_capsule();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	void set_data(double r, double l);
	void set_data(const x3d_point& p, double r, double l);


public:
	double   m_radius;            // 两头球半径 
	double   m_length;            // 中间段长度
};

