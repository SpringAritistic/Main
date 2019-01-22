#pragma once
#include "x3d_data_base.h"


class x3d_line;

// 表示三维空间中的旋转操作
class X_FRAMEWORK_API x3d_action_rotate : public x3d_action_base
{
	DECLARE_SERIAL(x3d_action_rotate)

public:
	x3d_action_rotate();
	x3d_action_rotate(x3d_action_section* sec, double ang = M_PI, double sp = M_PI_4 / 4);
	x3d_action_rotate(x3d_action_section* sec, x3d_line* ln, double ang = 2 * M_PI, int n = 16);
	virtual ~x3d_action_rotate();

public:
	virtual void do_action(x3d_graph& result);

public:
	void set_data(x3d_action_section* sec, double ang = 2 * M_PI, double sp = M_PI_4 / 4);
	void set_data(x3d_action_section* sec, x3d_line* ln, double ang = 2 * M_PI, int n = 16);

	void rotate_along_zaxis(x3d_graph& result);

public:
	x3d_action_section*        m_section;           // 传入的点集
	x3d_line*                  m_line;              // 绕轴旋转
	double                     m_angle;             // 旋转的角度（弧度）
	int                        m_count;             // 旋转的细分段数
	double                     m_step;              // 旋转角度进步（弧度)
};

