#pragma once

//以下角度方向均是从正北方向顺时针转向过该点的切线，范围为（0,2pi）
//    	*       *
//	   ***     *
//		*     *
//		*    *
//		*   *
//		*  *
//		* *
//		*
enum class line_type
{
	unknown = 0,
	straight_line = 1,
	arc = 2,
	transition_curve = 3,
};

class X_FRAMEWORK_API x3d_control_item
{

public:
	x3d_control_item();
	x3d_control_item(line_type type_value, double x_start_coordinate,double y_start_coordinate);
	x3d_control_item(line_type type_value, const std::pair<double,double>& point_value);

	~x3d_control_item();

public:
	void set_line_type(line_type type_value);
	line_type get_line_type() const;
	void set_start_point_value(const std::pair<double,double>& value);
	std::pair<double, double> get_start_point_value() const;

private:
	line_type m_type;
	std::pair<double,double> m_start_coordinate;
};


//straight_line
class X_FRAMEWORK_API x3d_field_item : public x3d_control_item
{

public:
	x3d_field_item();
	x3d_field_item(const std::pair<double, double>& value,double leght,double angle);
	~x3d_field_item();

public:
	std::pair<double, double> get_point_value(double distance) const; 

public:
	double m_lenght; // the total lenght of the line
	double m_angle;  //the line angle which between the line and the x axis
};


//arc_curve
class X_FRAMEWORK_API x3d_arc_item : public x3d_control_item
{

public:
	x3d_arc_item();
	x3d_arc_item(const std::pair<double, double>& value,double radius_value,
				double start_angle,double end_angle);
	~x3d_arc_item();

public:
	void get_point_value(double distance, std::pair<double, double>& point_value);

public:
	double m_start_angle;	//the tangent angle on the start point of the curve
	double m_end_angle;		//the tangent angle on the end point of the curve
	double m_radius;		//the radius of the arc curve 
};


//transition_curve
class X_FRAMEWORK_API x3d_transition_curve_item : public x3d_control_item
{

public:
	x3d_transition_curve_item();
	~x3d_transition_curve_item();

public:
	void get_point_value(double distance, std::pair<double, double>& point_value);

public:
	double m_start_angle;	//the tangent angle on the start point of the curve
	double m_end_angle;		//the tangent angle on the end point of the curve
	double m_start_radius;	//the radius of the arc curve on the start point of the curve
	double m_end_radius;	//the radius of the arc curve on the end point of the curve
	double m_parameter;		
	double m_lenght;
};


class X_FRAMEWORK_API horizontal_curve_item
{

public:
	horizontal_curve_item();
	~horizontal_curve_item();

public:
	void get_point_by_distance(double dist, std::pair<double,double> point_value);
	void convert_data(/******/);
public:
	std::vector<x3d_control_item*>  m_items;
};
