//
// Created by Ben on 2019-03-28.
//

#ifndef MACROTEXT_EDITORIAL_H
#define MACROTEXT_EDITORIAL_H

#include <unordered_map>
#include <functional>
#include <utility>

#include "support/Definitions.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "node/Node.h"
#include "node/Suffix.h"
#include "node/Content.h"
#include "mt/mt.h"

#include "content/Template.h"
#include "content/Segment.h"

using namespace Support;
using namespace Db;
using namespace std;

namespace content {

	struct hash_pair final {
		template<class TFirst, class TSecond>
		size_t operator()(const std::pair<TFirst, TSecond>& p) const noexcept {
			return p.first ^ (p.second + 0x9e3779b97f4a7c17 + (p.first << 6) + (p.first >> 2));
//			uintmax_t hash = (std::hash<TFirst>{}(p.first) << 2) ^ std::hash<TSecond>{}(p.second) ;
//			return std::hash<uintmax_t>{}(hash);
		}
	};

	class Editorial {
		using key=pair<size_t, size_t>;

	private:
		//node/segment
		unordered_map< key, mt::mtext, hash_pair> contentStore;
		unordered_map< key, size_t, hash_pair> qIndexes;
		Query* query;
		size_t lang;
		buildKind kind;

		void storeBuilt(Messages&);
		bool sanity(Messages &,key&,const node::Content*, const Segment*);

		Editorial(); // Disallow instantiation outside of the class.


	public:
		static Editorial& e();
		void set(Messages&,Connection&,size_t,buildKind);
		const mt::mtext* get(Messages&,const node::Content*,const Segment*);
		bool has(Messages&,const node::Content*,const Segment*);
		void unload(Messages&,Connection&);
	};
}

#endif //MACROTEXT_EDITORIAL_H
