//
// Created by Ben Griffin on 2019-02-07.
//

#include "mt.h"
#include "Instance.h"
#include "support/Convert.h"

namespace mt {
	Instance::Instance(const plist *p, iteration i, node::Metrics* m, bool gen) :
		parms(p), it(i), generated(gen),myFor(nullptr),metrics(m) {
	}

	Instance::Instance(const Instance &o) :
		parms(o.parms), it(o.it), generated(o.generated),myFor(o.myFor),metrics(o.metrics) {}

	Instance::Instance(const plist *p, forStuff& stuff,node::Metrics* m) :
		parms(p),generated(true),myFor(&stuff),metrics(m) {
	}

	Instance::Instance(node::Metrics* m) :
		parms(nullptr),generated(false),myFor(nullptr),metrics(m) {
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

	void forStuff::set(const std::string& value,size_t count) {
		stuff[0].second = value;
		stuff[1].second = Support::tostring(count);
	}

}
