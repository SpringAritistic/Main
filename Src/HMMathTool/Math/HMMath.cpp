#include "stdafx.h"
#include "HMMath.h"
_HM_MathTool_BEGIN
bool isDoubleEqual(const double& X1, const double& X2, const double& tolerance)
{
	return fabs(X1 - X2) <= tolerance;
}
bool isDoubleGreat(const double& X1, const double& X2, const double& tolerance)
{
	return (X1 - X2 > tolerance);
}
bool isDoubleLess(const double& X1, const double& X2, const double& tolerance)
{
	return (X1 - X2 < tolerance);
}
bool  isDoubleGreatEqual(const double& X1, const double& X2 , const double& tolerance )
{
	return isDoubleEqual(X1, X2, tolerance) || isDoubleGreat(X1, X2, tolerance);
}
bool  isDoubleLessEqual(const double& X1, const double& X2, const double& tolerance)
{
	return isDoubleEqual(X1, X2, tolerance) || isDoubleLess(X1, X2, tolerance);
}
_HM_MathTool_END
