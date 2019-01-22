#pragma once
#ifndef _HM_MATH_H_
#define _HM_MATH_H_
#include "math.h"

#define PI 3.141592653
_HM_MathTool_BEGIN
extern "C" HM_MathTool_EXT bool  isDoubleEqual(const double& X1, const double& X2 = 0, const double& tolerance = 1.0e-6);
bool HM_MathTool_EXT isDoubleGreat(const double& X1, const double& X2 = 0, const double& tolerance = 1.0e-6);
bool HM_MathTool_EXT isDoubleLess(const double& X1, const double& X2 = 0, const double& tolerance = 1.0e-6);
bool HM_MathTool_EXT isDoubleGreatEqual(const double& X1, const double& X2 = 0, const double& tolerance = 1.0e-6);
bool HM_MathTool_EXT isDoubleLessEqual(const double& X1, const double& X2 = 0, const double& tolerance = 1.0e-6);
#endif // !
_HM_MathTool_END