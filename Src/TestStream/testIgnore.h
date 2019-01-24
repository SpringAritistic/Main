#include "istream"
#include "limits"
#include "string"
class IgnoreLines
{
private: 
	int num;
public:
	explicit IgnoreLines(int n = 1) :num(n){}
	template <class charT, class traits> 
	friend std::basic_istream<charT, traits>& operator >> (std::basic_istream<charT, traits>& strm, const IgnoreLines& ign)
	{
		using namespace std;
		//string str("跳过以下");
		//str += std::to_string(ign.num) + "行";
		//for (auto & item : str)
		//	strm.putback(item);
		//strm.putback('\n');
		
		//strm.putback("跳过以下N行");		
		 for (int i = 0; i < ign.num; i++)
			strm.ignore(std::numeric_limits<std::streamsize>::max(), strm.widen('\n'));

		return strm;
	}
};