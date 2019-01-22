#pragma once
#ifndef _HMMATIX_
#define _HMMATIX_
//#include <initializer_list>
//#include "vector"
#include "Matrix.h"
//using std::vector;
_HM_MathTool_BEGIN
#define MYTYPE double
#define DOUBLEMATRIX _Matrix<MYTYPE>
class HM_MathTool_EXT Matrix :public DOUBLEMATRIX
{
public:
	Matrix() :DOUBLEMATRIX() { }
	Matrix(const DOUBLEMATRIX& A) { myArray = A.GetData(); }
	Matrix(const initializer_list<vector<MYTYPE>>& list) :DOUBLEMATRIX(list) {};//÷ÿ∂®“Â
	Matrix(const initializer_list<MYTYPE>& list) :DOUBLEMATRIX(list) {};
	Matrix(const Matrix&A) : DOUBLEMATRIX(A) { };
	Matrix(const size_t&m, const size_t&n, const MYTYPE& value = 0) :DOUBLEMATRIX(m, n, value) {};
	Matrix(const vector<vector<MYTYPE>>& _Array) :DOUBLEMATRIX(_Array) { };
	Matrix(const vector<MYTYPE>vec) : DOUBLEMATRIX(vec) { };
	template<int m, int n> Matrix(const MYTYPE(&A)[m][n]):DOUBLEMATRIX(A) { };

public:
	 Matrix&  eye(size_t i, size_t j=0);
	Matrix&  ones(size_t i, size_t j=0);
	Matrix&  zero(size_t i, size_t j=0);
	Matrix&  diag(const vector<MYTYPE>&vec);
public:
	Matrix operator +(const Matrix &M)
	{
		return Matrix(DOUBLEMATRIX(*this) + DOUBLEMATRIX(M));
	}
	Matrix operator -(const Matrix &M)
	{
		return Matrix(DOUBLEMATRIX(*this) - DOUBLEMATRIX(M));
	}
	Matrix operator *(const Matrix &M)
	{
		return Matrix(DOUBLEMATRIX(*this) * DOUBLEMATRIX(M));
	}
	Matrix operator /(const Matrix &M)
	{
		return Matrix(DOUBLEMATRIX(*this) / DOUBLEMATRIX(M));
	}
	Matrix &operator +=(const Matrix &M)
	{
		*this= Matrix(DOUBLEMATRIX(*this) += DOUBLEMATRIX(M));
		return *this;
	}
	Matrix &operator -=(const Matrix &M)
	{
		*this = Matrix(DOUBLEMATRIX(*this) -= DOUBLEMATRIX(M));
		return *this;
	}
	Matrix &operator *=(const Matrix &M)
	{
		*this = Matrix(DOUBLEMATRIX(*this) *= DOUBLEMATRIX(M));
		return *this;
	}
	Matrix operator /= (const Matrix &M)
	{
		*this = Matrix(DOUBLEMATRIX(*this) /= DOUBLEMATRIX(M));
		return *this;
	}
	template<class S=double> Matrix  operator *(const S &M)
	{
		return Matrix((DOUBLEMATRIX(*this) * MYTYPE(M)));
	}

	template<class S = double> Matrix  &operator *=(const S &M)
	{
		*this = Matrix((DOUBLEMATRIX(*this) *= MYTYPE(M)));
		return *this;
	}
	template<class S = double> Matrix  operator /(const S &M)
	{
		return Matrix((DOUBLEMATRIX(*this) / MYTYPE(M)));
	}

	template<class S = double> Matrix  &operator /=(const S &M)
	{
		*this = Matrix((DOUBLEMATRIX(*this) /= MYTYPE(M)));
		return *this;
	}
	template<class S = double>friend Matrix  operator *(const S &M,const Matrix&N)
	{
		return  Matrix((DOUBLEMATRIX(N) * MYTYPE(M)));
	}
};

#endif // 
_HM_MathTool_END
