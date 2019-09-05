//
// Created by Ben Griffin on 2019-02-07.
//

#include "mt.h"
#include "Instance.h"
#include "support/Convert.h"

namespace mt {

	Instance::Instance(const plist* p, node::Metrics* m, bool gen) :
		generated(gen),myFor(nullptr),metrics(m) {
		copy(p); tidy();
	}

	Instance::Instance(const plist* p, unique_ptr<forStuff>& stuff,node::Metrics* m) :
		generated(true),myFor(std::move(stuff)),metrics(m) {
		copy(p); tidy();
	}

	Instance::Instance(node::Metrics* m) :
		generated(false),myFor(nullptr),metrics(m) {
	}

	Instance::Instance(const Instance & o) : myFor(nullptr),generated(o.generated),it(o.it) {
		for(auto& parm : o.parms) {
			MacroText nParm;
			nParm.add(parm);
			parms.emplace_back(move(nParm));
		}

		metrics = o.metrics;
		if(o.myFor != nullptr) {
			myFor = make_unique<forStuff>(*o.myFor);
		}
	}

	size_t Instance::size() {
		return parms.size() == 1 ? (parms.front().empty() ? 0 : 1) : parms.size();
	}

	void Instance::tidy() {
		size_t pSize = size();
		it = {pSize == 0 ? 0 : 1, pSize };
	}

	void Instance::copy(const plist *parameterList) {
		// std::deque<MacroText>
		if(parameterList != nullptr) {
			for(auto& parameter : *parameterList) {
				MacroText nParm;
				nParm.add(parameter);
				parms.emplace_back(move(nParm));
			}
		}
	}

	Instance::~Instance() {
		metrics = nullptr;
		myFor = nullptr;
		parms.clear();
	}

	forStuff::forStuff(const std::string& vt,const std::string& ct) : nodeSym(vt),iterSym(ct),nodeInj("i"),iterInj("k") {
	}

	forStuff::forStuff(const forStuff& o) : nodeSym(o.nodeSym),iterSym(o.iterSym),nodeInj(o.nodeInj),iterInj(o.iterInj) {}

}
