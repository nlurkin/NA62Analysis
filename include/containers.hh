/*
 * containers.hh
 *
 *  Created on: 2 May 2014
 *      Author: ncl
 */

#ifndef CONTAINERS_HH_
#define CONTAINERS_HH_
#include <iostream>

/// \file containers.hh

#ifdef NA62_C11
#include <unordered_map>

namespace std
{
template<> struct hash<TString>
{
	size_t operator()(const TString& v) const
	{
		/// \MemberDescr
		/// \param v : TString to hash
		///
		/// Hash operation proxy for TString to use it in hashtable.
		/// \EndMemberDescr
		return v.Hash();
	}
};
}

namespace AnalysisFW{
	template <class K, class V>
	struct NA62Map{ typedef std::unordered_map<K,V> type; ///< C++ trick for template typedef.
	};	///< Defines the type of map used in the framework (C++11 enabled)
}
#else


#include <map>
namespace AnalysisFW{
	template <class K, class V>
	struct NA62Map{ typedef std::map<K,V> type; ///< C++ trick for template typedef.
	}; ///< Defines the type of map used in the framework (C++11 disabled)
}

#endif

#endif
