#pragma  once
#ifndef _MATRIX_H_
#define _MATRIX_H_
#include "vector"
using std::initializer_list;
using std::vector;
template<class S >vector<S> linspace(S start, S end, size_t count = 0)
{
	vector<S>result;
	if (count <= 2)
		return{ start,end };

	double diff = double(end) - start;
	if (fabs(diff) > 1e-6)
	{
		double space = diff / (count - 1);
		for (size_t i = 0; i < count; i++)
			result.push_back(S(start + i*space));
	}

	return result;
}
template<class S >class _Matrix /*:public CObject*/
{
public:
	_Matrix() { clear(); };
	_Matrix(const initializer_list<vector<S>>& list) :myArray(list) {};//重定义
	_Matrix(const initializer_list<S>& list) :myArray({ list }) {};
	_Matrix(const _Matrix&A) { myArray = A.GetData(); };
	_Matrix(const size_t&m, const size_t&n, const S& value = 0)
	{
		clear();
		for (size_t i = 0; i < m; i++)
			myArray.push_back(vector<S>(n, value));
	}
	_Matrix(const vector<vector<S>>& _Array)
	{
		clear();
		myArray.insert(myArray.end(), _Array.begin(), _Array.end());
	}
	_Matrix(const vector<S>vec)
	{
		clear();
		myArray.push_back(vec);
	}
	//数组初始化
	template<int m, int n> _Matrix<S>(const S(&A)[m][n])
	{
		clear();
		for (int i = 0; i < m; i++)
		{
			vector<S>temp(A + i*n, A + (i + 1)*n);
			myArray.push_back(temp);
		}
	};
	template<int m> _Matrix<S>(const S(&A)[m])
	{
		clear();
		vector<S>temp(A, A + m);
		myArray.push_back(temp);
	}
protected:
	vector<vector<S>>myArray;
public:
	//转换为数组
	template<int m, int n> void _MatrixToArray(S(&A)[m][n])
	{
		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				A[i][j] = myArray[i][j];
	}
	void Add(const vector<S>vec) { myArray.push_back(vec); }
	void push_back(const vector<S>vec) { Add(vec); }
	void push_front(const vector<S>vec)
	{
		_Matrix<S> temp(*this);
		clear();
		Add(vec);
		myArray.insert(myArray.end(), temp.myArray.begin(), temp.myArray.end());
	}
	vector<S>getCols(size_t j = 0)
	{
		vector<S>result;
		for (size_t i = 0; i < Rows(); i++)
			result.push_back(myArray[i][j]);
		return result;
	}
	_Matrix getCols(vector<size_t> vec)
	{
		_Matrix<S> result;
		for (size_t i = 0; i < vec.size(); i++)
			result.Add(getCols(vec[i]));
		return result.Transpose();
	}
	vector<vector<S>> GetData()const { return myArray; }
	template<class T>void Add(const S&value, T row = Rows()) { myArray[size_t(row)].push_back(value); }
	
public:

	CString ToStr()
	{
		CString result("");
		for (size_t i = 0; i < Rows(); i++)
		{
			for (size_t j = 0; j < Cols(); j++)
			{
				CString temp;
				temp.Format(_T("%g"), myArray[i][j]);
				result += temp + '\t';
			}
			result += '\n';
		}
		result += '\n';
		return result;
	}
	void clear()
	{
		myArray = vector<vector<S>>();
	}
public:
	inline size_t Cols()const { return myArray[0].size(); }
	inline size_t Rows()const { return myArray.size(); }
private:
	//递归求行列式，仅限内部使用，若是外部使用，请用下面的Det();
	S Det(_Matrix<S> &A)
	{
		S sum(0);
		if (A.Rows() == A.Cols())
		{
			if (A.Rows() == 1)
				return A.myArray[0][0];
			_Matrix<S> B(A.Rows() - 1, A.Rows() - 1);
			int p = 0, q = 0;
			for (size_t i = 0; i < A.Rows(); i++)
			{

				for (size_t j = 0; j < A.Rows() - 1; j++)
				{
					p = j < i ? 0 : 1;
					for (size_t k = 0; k < A.Rows() - 1; k++)
					{
						B.myArray[j][k] = A.myArray[j + p][k + 1];

					}
				}
				q = i % 2 == 0 ? 1 : -1;
				sum += A.myArray[i][0] * q*Det(B);

			}
		}
		return sum;

	}
public:
	// 转置
	_Matrix<S> Transpose()
	{
		_Matrix<S> result;
		vector<S>VecTemp;
		for (size_t j = 0; j < Cols(); j++)
		{
			VecTemp.clear();
			for (size_t i = 0; i < Rows(); i++)
				VecTemp.push_back(myArray[i][j]);
			result.myArray.push_back(VecTemp);
		}
		return result;
	}
	//行列式
	S Det() { return Det(*this); }
	_Matrix<S> up_down()
	{
		_Matrix<S> result(*this);
		for (size_t i = 0; i < Rows() / 2; i++)
		{
			vector<S>temp = result[i];
			result[i] = result[Rows() - 1 - i];
			result[Rows() - 1 - i] = temp;
		}
		return	result;
	}
	_Matrix<S> left_right()
	{
		return getCols(linspace<size_t>(Cols() - 1, 0, Cols()));
	}

	//余子矩阵
	_Matrix<S> C(size_t m, size_t n)
	{
		if (Rows() < m || Cols() < n)
			return _Matrix<S>(this->myArray);
		_Matrix<S> B;
		vector<S>vecTemp;
		for (size_t i = 0; i < Rows(); i++)
		{
			if (i == m) continue;
			vecTemp.clear();
			for (size_t j = 0; j < Rows(); j++)
			{
				if (j == n) continue;
				vecTemp.push_back(myArray[i][j]);
			}
			B.myArray.push_back(vecTemp);
		}
		return B;
	}
	//余子式(不带符号)
	S M(size_t m, size_t n) { return Det(C(m, n)); }
	//代数余子式(带符号)
	S A(size_t m, size_t n) { return M(m, n)*pow(-1, m + n); }
	// 伴随矩阵
	_Matrix<S> A_()
	{
		_Matrix<S> B(this->myArray);
		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				B.myArray[i][j] = this->A(j, i);
		return B;
	}
	//逆
	_Matrix<S> Inv()
	{
		S D = Det();
		if (fabs(D) > 1e-6)
		{
			return A_() / D;
		}
		else
			return _Matrix<S>(Rows(), Cols());
	}

public:
	//自定义操作符
	virtual _Matrix<S> operator +(_Matrix<S> &M)
	{
		_Matrix<S> result(this->myArray);
		if (this->Cols() == M.Cols() && this->Rows() == M.Rows())
		{
			for (size_t i = 0; i < M.Rows(); i++)
				for (size_t j = 0; j < M.Cols(); j++)
					result.myArray[i][j] += M.myArray[i][j];
		}
		return result;
	}
	virtual _Matrix<S> operator -(_Matrix<S> &M)
	{
		_Matrix<S> result(this->myArray);
		if (this->Cols() == M.Cols() && this->Rows() == M.Rows())
		{
			for (size_t i = 0; i < M.Rows(); i++)
				for (size_t j = 0; j < M.Cols(); j++)
					result.myArray[i][j] -= M.myArray[i][j];
		}
		return result;
	}
	virtual _Matrix<S> operator *(_Matrix<S> &M)
	{
		_Matrix<S> result;
		vector<S> vecTemp;
		S temp;
		if (M.Rows() == Cols())
		{
			for (size_t i = 0; i < Rows(); i++)
			{
				vecTemp.clear();
				for (size_t j = 0; j < M.Cols(); j++)
				{
					temp = 0;
					for (size_t k = 0; k < Cols(); ++k)
					{
						temp += this->myArray[i][k] * M.myArray[k][j];
					}
					vecTemp.push_back(temp);
				}
				result.myArray.push_back(vecTemp);
			}
		}
		return result;
	}
	virtual _Matrix<S> operator /(_Matrix<S> &A) { return _Matrix<S>::operator * (A.Inv()); }
	virtual vector<S>& operator [](const size_t &i) { return myArray[i]; }
	//vec作为行号，返回对应行号的vec作为新的矩阵
	virtual _Matrix<S> operator [](const vector<size_t> &vec)
	{
		_Matrix<S> result;
		for (size_t i = 0; i < vec.size(); i++)
			result.Add(myArray[vec[i]]);
		return result;
	}

	virtual _Matrix<S>& operator +=(const _Matrix<S> &M)
	{
		if (this->Cols() == M.Cols() && this->Rows() == M.Rows())
		{
			for (size_t i = 0; i < M.Rows(); i++)
				for (size_t j = 0; j < M.Cols(); j++)
					myArray[i][j] += M.myArray[i][j];
		}
		return *this;
	}
	virtual _Matrix<S>& operator -=(const _Matrix<S> &M)
	{
		{
			if (this->Cols() == M.Cols() && this->Rows() == M.Rows())
			{
				for (size_t i = 0; i < M.Rows(); i++)
					for (size_t j = 0; j < M.Cols(); j++)
						myArray[i][j] -= M.myArray[i][j];
			}
			return *this;
		}
	}
	virtual _Matrix<S>& operator *=(_Matrix<S> &M)
	{
		_Matrix<S> result = _Matrix<S>::operator * (M);
		this->myArray = result.myArray;
		return *this;
	}
	virtual _Matrix<S>& operator /=(const _Matrix<S> &M)
	{
		_Matrix<S> N(M);
		return _Matrix<S>::operator *= (N.Inv());
	}
	virtual _Matrix<S>  operator +(const S &M)
	{
		_Matrix<S> result(this->myArray);
		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				result.myArray[i][j] += M;
		return result;
	}
	virtual _Matrix<S>  operator -(const S &M)
	{
		_Matrix<S> result(this->myArray);

		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				result.myArray[i][j] -= M;
		return result;
	}
	virtual _Matrix<S>&  operator -()
	{
		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				myArray[i][j] *= -1;
		return *this;
	}
	virtual _Matrix<S>  operator *(const S &M)
	{
		_Matrix<S> result(this->myArray);

		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				result.myArray[i][j] *= M;
		return result;
	}
	virtual _Matrix<S>  operator /(const S &M)
	{
		_Matrix<S> result(this->myArray);

		if (fabs(M) >= 1e-6)
		{
			for (size_t i = 0; i < Rows(); i++)
				for (size_t j = 0; j < Cols(); j++)
					result.myArray[i][j] /= double(M);
		}
		return result;
	}
	template<class T> _Matrix<S>  friend operator /(const T &M, _Matrix<S>&N)
	{
		_Matrix<S> result(N);

		for (size_t i = 0; i < N.Rows(); i++)
			for (size_t j = 0; j < N.Cols(); j++)
			{
				if (fabs(result[i][j]) >= 1e-6)
					result[i][j] = S(M) / result[i][j];
				else
					result = S(1e6);
			}
		return result;
	}
	virtual _Matrix<S>& operator +=(const S &M)
	{
		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				myArray[i][j] += M;
		return *this;
	}
	virtual _Matrix<S>& operator -=(const S &M)
	{
		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				myArray[i][j] -= M;
		return *this;
	}
	virtual _Matrix<S>& operator *=(const S &M)
	{
		for (size_t i = 0; i < Rows(); i++)
			for (size_t j = 0; j < Cols(); j++)
				myArray[i][j] *= M;
		return *this;
	}
	virtual _Matrix<S>& operator /=(const S &M)
	{
		if (fabs(M) >= 1e-6)
		{
			for (size_t i = 0; i < Rows(); i++)
				for (size_t j = 0; j < Cols(); j++)
					myArray[i][j] /= double(M);
		}
		return *this;
	}
	//获取第i个主对角线数组：
	vector<S> diag(int k = 0)
	{
		vector<S>result;
		for (size_t i = 0; i <int(Rows()); i++)
		{
			if (i + k >= 0 && i + k < Cols())
				result.push_back(myArray[i][i + k]);
		}
		return result;
	}


};

#endif