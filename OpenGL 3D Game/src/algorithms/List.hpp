#ifndef LIST_HPP
#define LIST_HPP

using namespace std;

namespace List
{
	template<typename T>
	int getIndexOf(vector<T> v, T x)
	{
		return find(v.begin(), v.end(), x) - v.begin();
	}

	template<typename T>
	bool contains(vector<T> v, T x)
	{
		return find(v.begin(), v.end(), x) != v.end();
	}
}

#endif