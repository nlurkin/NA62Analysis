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
/// \Brief
/// Feature switch
/// \EndBrief
///
/// \Detailed
/// This header switches features used in the code depending if C++11 is enabled or not.\n
/// Currently:\n
/// If C++11 enabled:
/// - Uses std::unordered_map: map providing O(1) random access to elements (implemented with hashtable).\n
///   Provides a ~40% speed improvement on maps (accessing histograms, counters, events, analyzers, ...)
///   compared to std::map.
/// .
/// If C++11 disabled:
/// - Uses std::map: Fallback solution
/// \EndDetailed

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

namespace NA62Analysis {
template <class K, class V>
	struct NA62Map{ typedef std::unordered_map<K,V> type; ///< C++ trick for template typedef.
	};	///< Defines the type of map used in the framework (C++11 enabled)
	template <class K, class V>
	struct NA62MultiMap{ typedef std::unordered_multimap<K,V> type; ///< C++ trick for template typedef.
	};	///< Defines the type of multimap used in the framework (C++11 enabled)
} /* namespace NA62Analysis */
#else


#include <map>
namespace NA62Analysis {
	template <class K, class V>
	struct NA62Map{ typedef std::map<K,V> type; ///< C++ trick for template typedef.
	}; ///< Defines the type of map used in the framework (C++11 disabled)
	template <class K, class V>
	struct NA62MultiMap{ typedef std::multimap<K,V> type; ///< C++ trick for template typedef.
	}; ///< Defines the type of multimap used in the framework (C++11 disabled)
} /* namespace NA62Analysis */

#endif

#endif
