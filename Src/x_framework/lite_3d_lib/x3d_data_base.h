#pragma once
#include <vector>
#include <array>
#include <map>
#include "../interface/data_center_interface.h"
#include "x3d_definition.h"


class x3d_node_base;
class x3d_node_group;
class x3d_graph;
class x3d_geometry_base;
class x3d_trans_matrix;
class x3d_vector;
class x3d_color;
class x3d_vertex;
class x3d_aabb_box;


//////////////////////////////////////////////////////////////////////////

// 三维顶点相当于一个三维点，不要再新定义一个点的数据结构
// 这里的点只用于三维空间的定位使用，不是表示一个绘制出来的点对象
typedef x3d_vector x3d_point;

//////////////////////////////////////////////////////////////////////////



// 表示三维空间中的变换矩阵（4*4）
// 此工具名称为变换矩阵，内部一切接口都是为了能便于模型空间变换使用，没有特殊情况，不会提供常规矩阵的操作接口，如
// 有需要请使用其它常规矩阵的类或开源库
class X_FRAMEWORK_API x3d_trans_matrix
{
public:
	explicit x3d_trans_matrix(bool is_identity = true); // is_identity为true表示初始化为单位矩阵，否则初始化全0的矩阵
	x3d_trans_matrix(const x3d_trans_matrix& src);
	~x3d_trans_matrix();

public:
	void normalize(); // 标准化行列式，即初始化为单位矩阵
	void initialize(bool is_identity = true); // is_identity为true表示初始化为单位矩阵，否则初始化全0的矩阵

	double get_value(int row_index, int column_index) const;
	void set_value(int row_index, int column_index, double val);
	int get_row_count() const;
	int get_column_count() const;

	void get_values(std::vector<std::vector<double>>& vals) const;
	void set_values(std::vector<std::vector<double>>& vals);
	void get_values(std::array<std::array<double, 4>, 4>& vals) const;
	void set_values(std::array<std::array<double, 4>, 4>& vals);

public:
	// 以下矩阵变换对多次变换具备迭加效果（所有的変换均使用右乘矩阵的方式调用乘法运算）
	void rotate(const x3d_vector& vec, double a);	                   // 在当前状态下绕指定向量方向逆时针旋转指定的度数（弧度）
	void rotate(const x3d_point& p1, const x3d_point& p2, double a);   // 在当前状态下绕指定直线（p1,p2）逆时针旋转指定的度数（弧度）
	void move(double dx, double dy, double dz);		                   // 在当前状态下沿x,y,z轴方向移动指定的距离
	void scale(double sx, double sy, double sz);	                   // 在当前状态下沿坐标轴方向各缩放指定的比例
	void scale(const x3d_point& p, double sx, double sy, double sz);   // 相对于参考点沿坐标轴方向各缩放指定的比例
	void scale(const x3d_vector& vec, double s);	                   // 在当前状态下沿指定向量方向均缩放指定的比例
	x3d_vector get_rotation() const;				                   // 包含其它变换造成的旋转量
	x3d_vector get_move() const;					                   // 包含其它变换造成的位移量
	x3d_vector get_scale() const;					                   // 包含其它变换造成的缩放量
	void no_move();									                   // 去除位移变换信息

	// 对一个顶点进行变换
	x3d_point& transform_vertex_self(x3d_point& pt) const; // 返回参数自身引用以简化使用者代码
	x3d_point transform_vertex(const x3d_point& pt) const;

	// 对当前的变换矩阵再右乘一个变换矩阵，相当于再迭加一种变换（注意此处是右乘，如果要左乘，则将相乘矩阵的顺序换一下）
	x3d_trans_matrix& operator*(const x3d_trans_matrix& right_mx);

public:
	double		m_00, m_01, m_02, m_03;
	double		m_10, m_11, m_12, m_13;
	double		m_20, m_21, m_22, m_23;
	double		m_30, m_31, m_32, m_33;
};



// 表示一个三维向量或三维点
class X_FRAMEWORK_API x3d_vector
{
public:
	x3d_vector();
	x3d_vector(const x3d_vector& src);
	x3d_vector(double x, double y, double z);
	x3d_vector(const x3d_point& s, const x3d_point& e);
	~x3d_vector();

public:
	x3d_vector& operator +=(const x3d_vector& vec);
	x3d_vector& operator -=(const x3d_vector& vec);
	x3d_vector& operator*(double scale);

	double operator*(const x3d_vector& vec) const; // 两个向量的点积，返回一个数值
	bool operator==(const x3d_vector& vec) const; // 用compare_tools的默认精度，如果要控制精度，请使用is_equal方法

	x3d_vector operator+(const x3d_vector& vec) const;
	x3d_vector operator-(const x3d_vector& vec) const;
	x3d_vector operator-() const;
	x3d_vector operator*(double scale) const;    // 不改变本身的向量数乘版本

public:
	// 在当前状态下绕指定向量方向逆时针旋转指定的度数（弧度）
	x3d_vector& rotate(const x3d_vector& vec, double a);

	// 将自身朝某向量移动一段距离
	x3d_vector& move(const x3d_vector& v, double l);

	// 给三维向量赋值
	x3d_vector& set_value(double x, double y, double z);

	// 根据两个点得到向量
	x3d_vector& set_value(const x3d_point& s, const x3d_point& e);

	// 按制定精度比较是否为重合点
	bool is_equal(const x3d_point& pt, double pre = 1.0e-8) const;

	// 取模
	double get_module() const;
	double get_module_square() const;

	// 得到单位向量
	x3d_vector get_unit_vector() const;

	// 将自身规格化为单位向量（如果模为0则规格为0向量）
	x3d_vector& normalize();

	// 求与另一个共线向量的比值（若不共线则返回零）（若有零向量则直接返回0）
	double get_ratio_to_another_vector(const x3d_vector& v) const;

	// 求与另一个三维点的距离（表示点时有意义）
	double get_distance(const x3d_vector& p) const;
	double get_distance_square(const x3d_vector& p) const;

	// 与另一个向量叉乘
	x3d_vector get_cross_vector(const x3d_vector& v) const;

	// 求朝某向量移动一段距离的点坐标
	x3d_vector get_point_by_vector(const x3d_vector& v, double l) const;

	// 得到与另一个向量共面的平面上的夹角(弧度)
	double get_angle(const x3d_vector& vec) const;

	// 判断与另一个向量是否共线
	bool is_collinear(const x3d_vector& vec) const;

	// 判断与另一个向量是否垂直
	bool is_perpendicular(const x3d_vector& vec) const;

	// 向量的三个分量是否全为0
	bool is_all_zero() const;

public:
	double	m_x;	// 表示点时为x坐标，表示向量时为x方向的分量
	double	m_y;	// 表示点时为y坐标，表示向量时为y方向的分量
	double	m_z;	// 表示点时为z坐标，表示向量时为z方向的分量
};



// 表示一个颜色值，整数的颜色分量为0-255,浮点数的颜色分量为0.0f~1.0f
// 原则上来说浮点数的表示方法可以表示无穷种颜色，而整数形式只能保存有限的颜色，但目前显示技术使用整型保
// 存的颜色数足够了，比使用浮点数省空间多了（整形共需4byte，浮点数共需16byte）
class X_FRAMEWORK_API x3d_color
{
public:
	x3d_color(); // 相当于是全0的黑色（alpha为1)
	x3d_color(const x3d_color& src);
	x3d_color(unsigned char r, unsigned char g, unsigned int b, unsigned char a = 255);
	x3d_color(int r, int g, int b, int a = 255);
	x3d_color(float r, float g, float b, float a = 1.0f);
	x3d_color(double r, double g, double b, double a = 1.0);
	x3d_color(COLORREF rgb, unsigned char a = 255);
	virtual ~x3d_color();

public:
	void set_color(unsigned char r, unsigned char g, unsigned int b, unsigned char a = 255);
	void set_color(int r, int g, int b, int a = 255);
	void set_color(float r, float g, float b, float a = 1.0f);
	void set_color(double r, double g, double b, double a = 1.0);
	void set_color(COLORREF rgb, unsigned char a = 255);
	COLORREF get_color() const;
	void get_color(float& r, float& g, float& b) const;
	void get_color(float& r, float& g, float& b, float& a) const;
	void get_color(double& r, double& g, double& b) const;
	void get_color(double& r, double& g, double& b, double& a) const;

	unsigned char ri() const;
	unsigned char gi() const;
	unsigned char bi() const;
	unsigned char ai() const;

	void ri(unsigned char r);
	void gi(unsigned char g);
	void bi(unsigned char b);
	void ai(unsigned char a);

	float rf() const;
	float gf() const;
	float bf() const;
	float af() const;

	void rf(float r);
	void gf(float g);
	void bf(float b);
	void af(float a);

	double rd() const;
	double gd() const;
	double bd() const;
	double ad() const;

	void rd(double r);
	void gd(double g);
	void bd(double b);
	void ad(double a);

public:
	unsigned char	m_r; // red
	unsigned char	m_g; // green
	unsigned char	m_b; // blue
	unsigned char	m_a; // alpha
};



// 表示一个三维顶点
// 目前BIM系统仅需要支持光照和颜色设置即可，纹理、雾化等暂不用支持，因此可以不用包含uv坐标等其它信息,暂时只包含法向量
class X_FRAMEWORK_API x3d_vertex
{
public:
	x3d_vertex();
	x3d_vertex(const x3d_point& pt, const x3d_vector& nm);
	x3d_vertex(const x3d_point& pt, const x3d_color& cl = x3d_color(), const x3d_vector& nm = x3d_vector(0, 0, -1));
	virtual ~x3d_vertex();

public:
	// 设置坐标，颜色，法向量
	void set(const x3d_point& pt, const x3d_vector& nm);
	void set(const x3d_point& pt, const x3d_color& cl = x3d_color(), const x3d_vector& nm = x3d_vector(0, 0, -1));

public:
	x3d_point		m_point;			// 点坐标
	x3d_color		m_color;			// 颜色
	x3d_vector		m_normal_vector;	// 法向量（必须是单位向量）
};



// 表示三维空间中的AABB包围盒，原则上此接口只用于判断模型是否在指定区域或初步进行碰撞检测，并不需要从图形基类
// 派生，但由于某些场景下需要显示也包围盒以便于调试，因此需要绘制成图形，所以决定从图形基类派生
class X_FRAMEWORK_API x3d_aabb_box
{
public:
	x3d_aabb_box();
	x3d_aabb_box(const x3d_point& min_pt, const x3d_point& max_pt);
	virtual ~x3d_aabb_box();

public:
	void set_min_pt(const x3d_point& pt);
	void set_max_pt(const x3d_point& pt);
	void set_data(const x3d_point& min_pt, const x3d_point& max_pt);
	x3d_point get_min_pt() const;
	x3d_point get_max_pt() const;

	// 计算转换为包围球时的半径
	double calc_radius() const;

	// 伸缩包围盒以便容纳或排除掉特定对象的范围
	void include_point(const x3d_point& pt);
	void include_box(const x3d_aabb_box& box);
	void include_graph(const x3d_graph& gp);

	// 一个点会把它划成两部分，从m_min_pt到pt以及从pt到m_max_pt，reserve_min_part为true表示取前一部分
	// 其它情况下一样，比如另一个包围盒或图形完全被包在本包围盒内时也会使用此参数
	void exclude_point(const x3d_point& pt, bool reserve_min_part = true);
	void exclude_box(const x3d_aabb_box& box, bool reserve_min_part = true);
	void exclude_graph(const x3d_graph& gp, bool reserve_min_part = true);

	// 判断一个点在不在包围盒内（在边线上也算在盒内）
	bool is_point_in_box(const x3d_point& pt) const;

private:
	// 校验并修正数据，确保一定满足aabb包围盒的坐标要求
	void check_data();

private:
	x3d_point m_min_pt;
	x3d_point m_max_pt;
};



// 表示一个图形的一个单元，由于图形x3d_graph类可表达一个完整的3D模型，复杂的模型必然包含不同的面，相互组合
// 而成，不同的面使用的点绘制方式不同（如圆柱的顶、底面使用扇形的方式绘制三角形，侧面使用三角带来绘制），这种情况
// 下如果将不同的面分为不同的图元子节点则不太合适（图元子节点适合按物体的部位来分更合理，如盖梁柱式墩的盖梁、墩柱
// 基础等均可作为子图元，一个部分的不同面在物理组成的逻辑上不合适分为子图无，因此这种情况下，适合将不同面的顶点分
// 为不同的组，每组称为一个单元，同一单元的点绘制方式必须一致，一个图元可以有多个单元，单元并不对应一个实体，因此
// 不提供矩阵变换功能（粒度太小，不合适，在图元上提供即可）
class X_FRAMEWORK_API x3d_graph_unit
{
public:
	x3d_graph_unit(x3d_drawing_type dt = x3d_drawing_type::none);
	~x3d_graph_unit();

public:
	x3d_drawing_type m_drawing_type;
	std::vector<x3d_vertex> m_vertexs;

public:
	// 使用指定颜色对单元内的所有顶点进行着色
	void set_color(x3d_color cl);
};



// 定义一个3d模型的通用数据基类
class X_FRAMEWORK_API x3d_node_base : public data_node_base
{
	DECLARE_SERIAL(x3d_node_base)

public:
	x3d_node_base();
	virtual ~x3d_node_base();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false);

public:
	void hide(bool is_hidden = false);
	bool is_hidden() const;
	bool is_graph_group() const;

private:
	// 节点是否为隐藏状态，隐藏状态时不会在图形窗口中渲染出来，默认为false
	// 一个节点是否显示除了本身为显示状态，还要所有的父节点都为显示状态才行
	bool m_is_hidden;

	// 是否为图元包类型
	// 使用此变量避开通过虚函数和RTTI机制实现图元包类型判断，以最大限度提升类型判断速度(本类相当底层，图元包的判断频率是相当高的)
	// 此变量仅允许图元包类改写，本类默认初始化为false
	bool m_is_graph_group;

private:
	friend class x3d_node_group;
};



// 定义一个3d模型的图元包类，主要用于图形数据的打包，便于批量的操作图形状态（如隐藏等），另一个功能是对大型场景
// 进行细分，以便优化场景树结构，提高渲染效率
class X_FRAMEWORK_API x3d_node_group : public x3d_node_base
{
	DECLARE_SERIAL(x3d_node_group)

public:
	x3d_node_group();
	virtual ~x3d_node_group();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false);

public:
	// 添加子图元（子图元的其它更多操作接口可使用data_node_base基类的接口）
	// 如果成功会返回转换后的图形对象便于外部进行后续操作，否则返回nullptr
	x3d_graph* add_child_graph(x3d_geometry_base& gp);
	x3d_graph* add_child_graph(x3d_graph* gp); // 指针由内部接管

	// 使用指定颜色对所有单元内的顶点进行着色
	void set_color(x3d_color cl);

	// 当前图元启用光照效果（以本色显示相当于关闭光照），当前图形及所有子图形均受影响
	void enable_light(bool enable = true);
	bool is_light_enabled() const;

	// 当前图元启用深度测试，当前图形及所有子图形均受影响
	void enable_depth_test(bool enable = true);
	bool is_depth_test_enabled() const;

	// 计算包围盒，only_self为true表示子图元不在计算范围内，仅计算自身的包围盒，无法计算返回false，否则返回true
	bool calc_bound_box(x3d_aabb_box& box, bool only_self = false) const;

public:
	bool m_enable_light = true; // 启用此图元的光照效果
	bool m_enable_depth_test = true; // 启用此图元的深度测试
};



// 表示一个由任意顶点数据构成的能被图形引擎接口识别的可显示的3d图形，所有的顶点可自由定位，而不像几
// 何图元一样位置受任何形状约束原则上，任何的几何图元都可转为本图元对象
// 注意区分几何图元，几何图元的顶点一般是不可修改的，如果要修改，则需要修改几何图元本身的几何参数，再生成顶
// 点，以确保顶点是符合几何图元的几何性质的，而自由图元则与几何无关，不关联任何的几何参数
// 注意：提供的接口最好能让不使用此3d轻量库的用户也能接入（如使用occ等）
class X_FRAMEWORK_API x3d_graph : public x3d_node_base
{
	DECLARE_SERIAL(x3d_graph)

public:
	x3d_graph();
	x3d_graph(x3d_graph& src, bool copy_tag = false);
	x3d_graph(x3d_geometry_base& src); // 支持从几何模型转为自由模型
	virtual ~x3d_graph();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false);

public:
	// 使用指定颜色对所有单元内的顶点进行着色，only_self为true表示子图元不改颜色，仅修改自身的图元颜色
	void set_color(x3d_color cl, bool only_self = false);

	// 当前图元启用光照效果（以本色显示相当于关闭光照），仅当前图形有效，不影响子图形节点
	void enable_light(bool enable = true);
	bool is_light_enabled() const;

	// 当前图元启用深度测试，仅当前图形有效，不影响子图形节点
	void enable_depth_test(bool enable = true);
	bool is_depth_test_enabled() const;

	// 计算包围盒，only_self为true表示子图元不在计算范围内，仅计算自身的包围盒，无法计算返回false，否则返回true
	bool calc_bound_box(x3d_aabb_box& box, bool only_self = false) const;

	// 从一个图元转换为自由图元
	// 一般用于根据任何图元生成的图元转为本类型，如果本身已经是自由图元，则不转换，如果有子对象，则子对象也会转
	// 换为自由图元，并保证树层次结构不变
	// 传入的对象本对象仅复制顶点数据，不负责管理内存
	void convert_from(x3d_geometry_base& gp);

	// 添加子图元（子图元的其它更多操作接口可使用data_node_base基类的接口）
	// 如果成功会返回转换后的图形对象便于外部进行后续操作，否则返回nullptr
	x3d_graph* add_child_graph(x3d_geometry_base& gp);
	x3d_graph* add_child_graph(x3d_graph* gp); // 指针由内部接管

	// 变换接口
	void apply_trans_matrix(const x3d_trans_matrix& mat);	// 应用外部矩阵
	void rotate(const x3d_vector& vec, double a);			// 绕指定向量方向逆时针旋转指定的度数（弧度）
	void rotate(const x3d_point& p1, const x3d_point& p2, double a); // 绕指定直线（p1,p2）逆时针旋转指定的度数（弧度）
	void move(double dx, double dy, double dz);				// 沿x,y,z轴方向移动指定的距离
	void scale(double sx, double sy, double sz);			// 沿坐标轴方向各缩放指定的比例
	void scale(const x3d_vector& vec, double s);			// 沿指定向量方向均缩放指定的比例

public:
	//////////////////////////////////////////////////////////////////////////
	// 关于对第三方模型格式的支持，本工具目前仅支持通用的两种格式：3ds及stl
	// 以后如果有扩展请遵循以下要求：
	// 1.如果是国际通用的格式，且对其格式的解析不需要引入第三方库（或有开源代码，量小且能内嵌到本模块），则可
	//   在本类接口中继续添加，否则请在其它模块中自己处理为离散的顶点后再生成为本类的对象，以便框架使用，典型
	//   的格式如osg的模型，此模型需要借助庞大的osg模块才能实现导入，不宜放在此模块，由外部使用osg的模块导入
	//   模型后提取顶点生成本类对象更合理，这在工作量上是很小的事情
	// 2.对于小众的3D模型格式，不建议在此类的添加，强烈推荐在其它模块处理后提取顶点并生成为本类的对象
	//////////////////////////////////////////////////////////////////////////

	// 导入、导出3ds模型
	void import_3ds(const CString& path_name);
	void export_3ds(const CString& path_name);

	// 导入、导出stl模型
	void import_stl(const CString& path_anme);
	void export_stl(const CString& path_anme);


public:
	// 存储顶点的单元集合，first为单元名，second为单元数据
	// 一个3d模型往往由多个部分或多个面组成，不同的部分绘制方式，细分的数量等各不相同，因此提出图形单元的数据结构，
	// 要求图形的顶点以单元进行小粒度的管理（图元包是大粒度的管理方式，子节点一般介于单元和图元包中间）
	// 图元包一般用于场景的大粒度管理，子节点一般用于单个模型的不同部位管理，单元一般于用模型某部位多个绘制单元的管理
	std::map<CString, x3d_graph_unit> m_all_units;

	bool m_enable_light = true; // 启用此图元的光照效果
	bool m_enable_depth_test = true; // 启用此图元的深度测试
};


// 表示一个几何图形类
// 几何图元的顶点一般是不可修改的，如果要要修改，则需要修改几何图元本身的几何参数，再生成顶
// 点，以确保顶点是符合几何图元的几何性质的
class X_FRAMEWORK_API x3d_geometry_base : public x3d_node_base
{
	DECLARE_SERIAL(x3d_geometry_base)

public:
	x3d_geometry_base();
	virtual ~x3d_geometry_base();

public:
	virtual void copy(const data_node_base& node, bool copy_tag = false);

	// 重新生成顶点数据以保证参数改变后取到的顶点数据是匹配的
	// 注意，此接口内部实现是不要考虑矩阵变换和基准点位置偏移，框架会在顶点求解后统一处理
	virtual void regenerate_vertext(x3d_graph& graph);

public:
	void set_span_step(double step);
	void set_span_count(double count);
	void set_base_point(const x3d_point& p);

	double get_span_step() const;
	double get_span_count() const;
	const x3d_point& get_base_point() const;

	// 根据总长计算合适的细分步进量或细分数目
	// 请注意结果小于等于0时表示无法计算，不要直接使用，以及步骤为0造成细分时死循环！！！
	double calc_span_step(double total_len) const;
	double calc_real_span_count(double total_len) const; // 理论计算出的细分数目，可以是小数
	int calc_span_count(double total_len) const; // 对计算出的细分数目进行偏大取整后返回


protected:
	// 几何模型细分顶点时的采样精细度设置参数，有些模型经常会出现参数变动后导致尺寸有巨大的差异性变化（如路线的线形
	// 受路线的参数多少和各线元参数影响，长度会经常变），为保证模型尺寸变大时在相同放大比例的精细要求下，细分的数目
	// 肯定要加大，这种情况下使用细分数目来进行顶点的采样就不太合适，而是适合用步进量更好，但大多数情况下，细分数目
	// 更通用，因此，此处提供两个参数，使用时按此规则：
	// 1.如果值为负或0则相应的值无效（改用另一个值），若两值均小于等于0,则采样精细程序由算法内部决定，适当取值即可
	// 2.两值均大于0时，按要细分的总长分别按细分数目和步进量计算得到两个采样的顶点数目，取数目多的一个作为原则
	// 3.若模型有多个单元组成，采样设置的参数是对全部单元生效还是只对敏感造型的单元生效，或者根据造型复杂程序对不同
	//   单元使用不同梯度的采样率（最精细的一个单元不大于设置参数的要求即可）均可，由具体模型择优决定
	// 4.对一些非弯曲的模型（如直线）则精细度参数没有意义
	double m_span_step; // 细分的长度步进量（相当于下一个点与当前点的长度间隔），默认0.0
	double m_span_count; // 细分数目,默认0.0(可以是小数，如果要整数，则自行使用框架中的数据工具取整)

	// 几何模型建模时使用的定位点坐标，定位点在几何模型的哪个部位，由派生类自由决定
	// 注意：几何模型的顶点坐标不相对于此定位点作为局部坐标系，而是与定位点使用同一坐标系
	x3d_point m_base_point;
};



// 表示三维空间中的建模操作的基类
class X_FRAMEWORK_API x3d_action_base : public CObject, public serialize_interface
{
	DECLARE_SERIAL(x3d_action_base)

public:
	x3d_action_base();
	x3d_action_base(const x3d_action_base& src);
	virtual ~x3d_action_base();

public:
	x3d_action_base& operator=(const x3d_action_base& data);

public:
	virtual void do_action(x3d_graph& result);
};



// 表示三维空间中的一个平面
class X_FRAMEWORK_API x3d_plane
{
public:
	x3d_plane();
	x3d_plane(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3);
	virtual ~x3d_plane();

public:
	// 给平面的三个点赋值
	void set_data(const x3d_point& p1, const x3d_point& p2, const x3d_point& p3);

	// 求平面的法向量，m1m2、m1m3与法向量满足右手定则，返回结果已经处理为单位向量
	// 若三个点不构成一个平面，则返回0
	x3d_vector get_normal_vector() const;

	// 得到点在直线上的垂足（投影点）坐标
	x3d_point get_footpoint_on_plane(const x3d_point& p) const;

	// 判断这三个点是否构成一个平面（即三点是否共线）
	bool is_plane();

	// 判断另外一个点是否在此平面上
	bool is_in_plane(const x3d_point& p)const;


public:
	x3d_point       m_1;
	x3d_point       m_2;
	x3d_point       m_3;
};


// 表示三维空间中用于拉伸/放样/旋转操作的截面的点集
// 注：点集有顺序要求:
// 1.操作将对每两个连续的点拉伸形成面；若要形成封闭图形，则最后一个点即为初始点
// 2.从起始面向终止面看，点的顺序满足顺时针方向
class X_FRAMEWORK_API x3d_action_section
{
public:
	struct point
	{
		point();
		point(x3d_point& pt, bool left = true, CString k = _T(""))
			:p(pt), is_left(left), key(k){};

		x3d_point	p;
		bool		is_left;
		CString		key;
	};

public:
	x3d_action_section();
	x3d_action_section(std::vector<x3d_point>& points);
	x3d_action_section(std::vector<x3d_action_section::point>& points);
	x3d_action_section(const x3d_action_section& src);
	~x3d_action_section();

public:
	void set_data(std::vector<x3d_point>& points);
	void set_data(std::vector<x3d_action_section::point>& points);
	void add_point(const x3d_point& point, bool is_left = true, const CString& key = _T(""));
	void clear_points();


public:
	// bool表示法线是否在左边（截面点按逆时针首尾相连顺序给出，沿上一点往下一点为前进方向分左右侧
	std::vector<x3d_action_section::point>	m_all_points;
	x3d_normal_type							m_nm_type;		// 顶点的法向量方式
	bool                                    m_has_key;      // 是否使用关键点（）
};
