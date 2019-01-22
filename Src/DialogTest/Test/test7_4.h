#include "functional"
#include "algorithm"
#include "vector"
//#include "memory"
//#include "iterator"
using namespace std;


void test7_4();
void testTemplate();
void testEnverimentVar();

template <typename T> vector<T>GetVec(const T& value1)
{
	return vector<T>{ value1 };
};
template<class T, class U>void AddVec( vector<T>& value2, const vector<U>&value)
{
	for_each(value.begin(), value.end(), [&](const U&value1){value2.push_back(T(value1)); });
}

template <typename T, typename... U > vector<T>GetVec(const T& value1, const U&... args)
{
	vector<T>&vec = GetVec(value1);
	auto vec2 = GetVec(args...);
	AddVec(vec, vec2);
	return vec;
};
void testAlgorithm();

//template<>
//struct Count<> {
//	static const int value = 0;
//};
//
//template<typename T, typename... Args>
//struct Count<T, Args...> {
//	static const int value = 1 + Count<Args...>::value;
//};

