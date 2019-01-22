#pragma once

// 线的延长方式
enum class x3d_extend_type{none, both, first, second};

// 线的交点方式
// 交点在直线段内为self，在起点延长线上为start，在终点延长线上为end,无交点为none
enum class x3d_intersection_type{none, self, start, end};

// 图元绘制方式
enum class x3d_drawing_type{none, dots, lines, line_strip, line_loop, triangles, triangles_trip, triangles_fan};

// 顶点的法向量计算方式
// plane:平面法向量方式，面之间区别明显
// vertex:顶点法向量方式，面之间区别不明显，适用光滑曲面
enum class x3d_normal_type{none, plane, vertex };

// 二维平面的方式，oxy平面，oxz平面，oyz平面
enum class x3d_plane_type{none, oxy, oxz, oyz};