//
// Created by Ben Griffin on 2019-02-07.
//

#include "mt.h"
#include "Instance.h"
#include "support/Convert.h"

namespace mt {
	Instance::Instance(const plist *p, iteration i, bool gen) : parms(p), it(i), generated(gen),myFor(nullptr) {}

	Instance::Instance(const Instance &o) :
		parms(o.parms), it(o.it), generated(o.generated),myFor(o.myFor) {}

	Instance::Instance(const plist *p, forStuff& stuff) :
		parms(p),generated(true),myFor(&stuff) {
	}

	size_t Instance::size() {
		return parms ? (parms->size() == 1 ? (parms->front().empty() ? 0 : 1) : parms->size()) : 0;
	}

	forStuff::forStuff(const std::string& vt,const std::string& ct,size_t v,size_t c) {
		stuff = {
			{vt,std::move(Support::tostring(v))}
			,
			{ct,std::move(Support::tostring(c))}
		};
	}

	forStuff::forStuff(const std::string& vt,const std::string& ct) {
		stuff = {
				{vt,""},{ct,""}
		};
	}

	void forStuff::set(const std::string value,size_t count) {
		stuff[0].second = value;
		stuff[1].second = Support::tostring(count);
	}

}
