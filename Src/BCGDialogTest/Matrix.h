#include "vector"
template<class T>class Matrix
{
public:
	~Matrix()
	{
		clear();
	}
public:
	Matrix()
	{
		init();
	}
	template<class M>Matrix(size_t rows, size_t columns, const M&value = M())
	{
		init();
		Set(rows, columns, value);
	};
	template<class M>Matrix(const Matrix<M>&A)
	{
		Set(A.m_rows, A.m_columns);
		 M * data = GetData(A);
		 if (data)
		 {
			 for (size_t i = 0; i < GetCount() - 1; i++)
			 {
				 m_data[i] = (T)data[i];
			 }
		 }
	};
public:

public: size_t GetCount(){ return m_rows*m_columns ; }
	template<class M=T> void Set(size_t rows, size_t columns, const M&value = M())
	{
		clear();
		m_rows = rows;
		m_columns = columns;
		size_t count = GetCount();
		m_data = new T[count];//特意加1，最后一个作为 .end() 使用
		m_pData = &m_data;

		for (size_t i = 1; i <= m_rows; i++)
			m_pData[i] = m_pData[i - 1] + m_columns;
		//为了模仿.end()，特意添加m_pData[m_rows]
		//赋初值；
		for (size_t i = 0; i < count; i++)
		{
			m_data[i] = (T)value;
		}
	}
public:
	/*template<class T>*/ T* operator [](size_t i) { return m_pData[i]; };
	//using namespace std;
	// vector<vector<M=T>> ToVec()
	//{
	//	vector< vector<T>>vec;
	//	
	//	for (size_t i = 0; i < m_rows;i++)
	//	{
	//		vec.push_back(vector<T>(m_pData[i], m_pData[i + 1]));
	//	}
	//	return vec;
	//};
	void clear()
	{
		if (m_rows > 0 || m_columns > 0)
		{
			delete[] m_data;	//数组形式
		}
		else
			delete m_data;		//单指针形式
		m_data = nullptr;
		//
		//m_pData = nullptr;
		//if (m_pData != nullptr)
		//{
		//	for (size_t i = 0; i <= m_rows; i++)
		//		m_pData[i] = nullptr;
		//}
		m_rows = 0;
		m_columns = 0;
	}
	T* begin(int i = -1)
	{
		if (GetCount()<1)
			return nullptr;
		if (i <= -1)
			return &m_data[0];
		if (i < m_rows)
			return m_pData[i];
		else
			return nullptr;
	}
	T* end(int i = -1)
	{
		if (GetCount() < 1)
			return nullptr;
		if (i<=-1)
			return m_pData[m_rows];
		if (i < m_rows)
			return m_pData[i+1];
		else
			return nullptr;
	}
	T& front(int i = -1)
	{
		ASSERT((GetCount() < 1) || i >= m_rows);
		if (i <= -1)
			return m_data[0];
		if (i < m_rows)
			return m_pData[i][0];
	}
	T& back(int i = -1)
	{
		ASSERT((GetCount() < 1) || i >= m_rows);
		if (i <= -1)
			return m_data[GetCount() - 1];
		if (i < m_rows)
			return m_pData[i][m_columns-1];
	}
private:
	void init()
	{
		m_rows = 0;
		m_columns = 0;
		delete m_data;		//单指针形式
		m_data = nullptr;
	}
public:
	/*template<class T>*/ friend T* GetData(const Matrix<T>& A){ return A.m_data; }
	///*template<class T>*/  T* GetData(){ return m_data; }
public:
	T** m_pData;//表面
	size_t m_rows;
	size_t m_columns;
private:
	T* m_data;//本质核心
};