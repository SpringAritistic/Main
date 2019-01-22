#include "stdafx.h"
#include "tools/compare_tools.h"

#include "x3d_control_item.h"


x3d_control_item::x3d_control_item()
{
	m_type = line_type::unknown;
}


x3d_control_item::x3d_control_item(line_type type_value, double x_start_coordinate, double y_start_coordinate)
{
	m_type = type_value;
	m_start_coordinate.first = x_start_coordinate;
	m_start_coordinate.first = y_start_coordinate;
}

x3d_control_item::x3d_control_item(line_type type_value, const std::pair<double, double>& point_value)
{
	m_type = type_value;
	m_start_coordinate = point_value;
}

x3d_control_item::~x3d_control_item()
{
}


void x3d_control_item::set_line_type(line_type type_value)
{
	m_type = type_value;
}

line_type x3d_control_item::get_line_type() const
{
	return m_type;
}

void x3d_control_item::set_start_point_value(const std::pair<double, double>& value)
{
	m_start_coordinate = value;
}

std::pair<double, double> x3d_control_item::get_start_point_value() const
{
	return m_start_coordinate;
}


x3d_field_item::x3d_field_item() : x3d_control_item()
{
	set_line_type(line_type::straight_line);
	m_lenght = 0.0;
	m_angle = 0.0;
}

x3d_field_item::x3d_field_item(const std::pair<double, double>& value, double leght, double angle)
{	
	set_start_point_value(value);
	set_line_type(line_type::straight_line);
	m_lenght = leght;
	m_angle = angle;
}

x3d_field_item::~x3d_field_item()
{

}

std::pair<double, double> x3d_field_item::get_point_value(double distance) const
{
	std::pair<double, double> start_point_value = get_start_point_value();

	if (compare_tools::is_less_equal(distance, 0.0)) return start_point_value;

	double x_increment_value = distance * cos(m_angle);
	double y_increment_value = distance * sin(m_angle);

	return std::make_pair(start_point_value.first + x_increment_value
		                 , start_point_value.second + y_increment_value);
}

x3d_arc_item::x3d_arc_item()
{

}

x3d_arc_item::x3d_arc_item(const std::pair<double, double>& value, double radius_value
	, double start_angle, double end_angle)
{
	set_start_point_value(value);
	set_line_type(line_type::arc);
	m_radius = radius_value;
	m_start_angle = start_angle;
	m_end_angle = end_angle;
}

x3d_arc_item::~x3d_arc_item()
{

}

void x3d_arc_item::get_point_value(double distance,std::pair<double, double>& point_value) 
{
	if (compare_tools::is_less_equal(m_radius, 0.0)) return;
	std::pair<double, double> start_point_value = get_start_point_value();
	
	double angle = distance / m_radius;
	double osculation_angle = (M_PI - angle) * 0.5;
	double chord_length = m_radius * cos(osculation_angle) * 2;

	double cal_angle = m_start_angle + M_PI_2 - osculation_angle;
	double x_increment_value = chord_length * cos(cal_angle);
	double y_increment_value = chord_length * sin(cal_angle);

	point_value.first = start_point_value.first + x_increment_value;
	point_value.second = start_point_value.second + y_increment_value;
}

x3d_transition_curve_item::x3d_transition_curve_item()
{

}

x3d_transition_curve_item::~x3d_transition_curve_item()
{

}

void x3d_transition_curve_item::get_point_value(double distance, std::pair<double, double>& point_value)
{

}

void horizontal_curve_item::get_point_by_distance(double dist, std::pair<double, double> point_value)
{

}

void horizontal_curve_item::convert_data(/******/)
{

}
