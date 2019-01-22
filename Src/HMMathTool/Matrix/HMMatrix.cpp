#include "stdafx.h"
#include "iostream"
#include "HMMatrix.h"
using namespace std;
_HM_MathTool_BEGIN
Matrix& Matrix::ones(size_t i, size_t j)
{
	*this= Matrix(i, (j==0?i:j), 1);
	return *this;
}
Matrix&   Matrix::zero(size_t i, size_t j)
{
	*this = Matrix(i, (j == 0 ? i : j), 0);
	return *this;
}
Matrix&  Matrix::eye(size_t i, size_t j)
{
	this->zero(i, j);
	size_t count = min(this->Rows(),this->Cols());

	for (size_t k=0;k<count;k++)
		myArray[k][k] = 1;
	return *this;
}

Matrix&   Matrix::diag(const vector<double>&vec)
{
	size_t count = vec.size();
	this->zero(count);
	for (size_t i=0;i<count;i++)
	{
		myArray[i][i] = vec[i];
	}
	return *this;
}
_HM_MathTool_END
