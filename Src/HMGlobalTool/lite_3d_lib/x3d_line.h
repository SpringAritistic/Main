#pragma once
#include "x3d_data_base.h"


// 表示一条三维空间中的直线（直线段）
class HM_GlobalTool_EXT x3d_line : public x3d_geometry_base
{
	DECLARE_SERIAL(x3d_line)

	// 保证线段的起点、中点、终点不动
	enum keeppoint
	{
		start = 0,
		mid = 1,
		end = 2
	};

public:
	x3d_line();
	x3d_line(const x3d_point& p1, const x3d_point& p2); // 起点和终点构成的线段
	x3d_line(const x3d_point& p, const x3d_point& p1, const x3d_point& p2);
	x3d_line(double x1, double y1, double z1, double x2, double y2, double z2);
	x3d_line(const x3d_point& p, const x3d_vector& v, double len);
	virtual ~x3d_line();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false) override;
	virtual void regenerate_vertext(x3d_graph& graph) override;

public:
	// 指定起点、终点构造一条直线段
	void set_data(const x3d_point& p1, const x3d_point& p2);

	// 指定基点、起点、终点，以基点为原点，用起、终点坐标值构造一条直线段（即起、终点加上基点坐标进行偏移）
	void set_data(const x3d_point& b, const x3d_point& p1, const x3d_point& p2);

	// 指定起点、方向向量、长度构造一条直线段
	void set_data(const x3d_point& p, const x3d_vector& v, double len);

	// 获得线段的长度
	double get_length() const;
	double get_length_square()const;
	
	// 判断是否构成直线段（首尾点是否重合）
	bool is_zero_length();

	// 得到方向向量
	x3d_vector get_vector() const;

	// 得到直线的法向量(平行于指定平面,有两个法向量，取正的）,normalize为false表示不将结果转换为单位向量（效率更高）
	x3d_vector get_normal_vector(x3d_plane_type py = x3d_plane_type::oxy, bool normalize = false) const;
	 
	// 判断一个点是否在此直线上，need_extend为true表示在延长线上也算
	bool is_on_line(const x3d_point& p, bool need_extend = false) const;

	// 判断一个点与直线的关系
	// 不在直线上，返回none；点在线段上（包括起点或终点）返回self；
	// 点在起点延伸线上，返回start
	// 点在终点延长线上返回end
	x3d_intersection_type relation_with_point(const x3d_point& p) const;

	// 判断两条直线是否共面
	bool is_plane(const x3d_line& l)const;

	// 判断直线段(不延长)与另一直线段(可指定延长方式）是否相交,若相交则输出交点p及交点位置
	bool is_cross(const x3d_line& l, std::pair<x3d_point, x3d_intersection_type>& pt,
		x3d_extend_type et = x3d_extend_type::none, double precision = 1e-6) const;

	// 保持起点及方向不变，将直线段缩放n倍
	void self_scale(double n);
	x3d_point scale(double n) const;
	x3d_point mid_point() const;

	// 将线段变为指定的长度，type可以指定是保持起点，中点，终点不动
	x3d_line& self_assign_length(double len, keeppoint tp = keeppoint::start);
	x3d_line assign_length(double len, keeppoint tp = keeppoint::start) const;

	// 得到点在直线上的垂足（投影点）坐标
	x3d_point get_foot_point(const x3d_point& p) const;

	// 得到点关于此直线的对称点
	x3d_point get_symmetry_point(const x3d_point& p) const;

	// 得到与另一条直线最近点的坐标（点在本直线上）
	x3d_point get_closest_point(const x3d_line& l) const;

	// 与另外一条直线交点，没有交点返回false，有多个交点结果取任意一个并返回true
	// 空间两条线求交往往有大误差，max_dist表示两线求交的空间最大距离（小于此距离时认为有交点），这个
	// 值用于工程上有意义（内部mm单位，桥梁工程一般精度在1mm以内够了，考虑运算累计误差，默认为0.001)
	bool get_cross_point(const x3d_line& l, x3d_point& p, double max_dist = 0.001) const;

	// 得到一条直线关于此直线的对称直线
	x3d_line get_symmetry_line(const x3d_line& l) const;

	// 得到与另一条直线的夹角（弧度）
	double get_angle(const x3d_line& line) const;

	// 按向量平移直线段
	x3d_line move(const x3d_vector& vec) const;
	x3d_line& self_move(const x3d_vector& vec);

	// 得到偏移线段(偏移距离为正表示沿多段线方向向右偏移；偏移保证z坐标不变)
	// 可以指定在哪个平面上进行偏移（可选oxy，oxz，oyz，默认为oxy）
	x3d_line offset(double dist, x3d_plane_type py = x3d_plane_type::oxy) const;
	x3d_line& self_offset(double dist, x3d_plane_type py = x3d_plane_type::oxy);

public:
	x3d_point       m_s;             // 线段的起点的三维坐标
	x3d_point       m_e;             // 线段的终点的三维坐标
};

