/*
 * containers.hh
 *
 *  Created on: 2 May 2014
 *      Author: ncl
 */

#ifndef CONTAINERS_HH_
#define CONTAINERS_HH_
#include <iostream>

#ifdef NA62_C11
#include <unordered_map>

namespace std
{
template<> struct hash<TString>
{
	size_t operator()(const TString& v) const
	{
		return v.Hash();
	}
};
}

namespace AnalysisFW{
	template <class K, class V>
	struct NA62Map{ typedef std::unordered_map<K,V> type;};
}
#else


#include <map>
namespace AnalysisFW{
	template <class K, class V>
	struct NA62Map{ typedef std::map<K,V> type;};
}

#endif

#endif
