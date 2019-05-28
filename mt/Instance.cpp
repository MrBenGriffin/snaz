//
// Created by Ben Griffin on 2019-02-07.
//

#include "mt.h"
#include "Instance.h"
#include "support/Convert.h"

namespace mt {

	Instance::Instance(plist p, node::Metrics* m, bool gen) :
		parms(std::move(p)), generated(gen),myFor(nullptr),metrics(m) {
		tidy();
	}

//	Instance::Instance(plist p, iteration i, node::Metrics* m, bool gen) :
//		parms(std::move(p)), it(i), generated(gen),myFor(nullptr),metrics(m) {
//		tidy();
//	}

	Instance::Instance(plist p, unique_ptr<forStuff>& stuff,node::Metrics* m) :
		parms(std::move(p)),generated(true),myFor(std::move(stuff)),metrics(m) {
		tidy();
	}

	Instance::Instance(node::Metrics* m) :
		generated(false),myFor(nullptr),metrics(m) {
	}

	Instance::Instance(const Instance & o) : myFor(nullptr),parms(o.parms),generated(o.generated),it(o.it) {
		metrics = o.metrics;
		if(o.myFor != nullptr) {
			myFor = make_unique<forStuff>(o.myFor->stuff);
		}
	}

	size_t Instance::size() {
		return parms.size() == 1 ? (parms.front().empty() ? 0 : 1) : parms.size();
	}

	void Instance::tidy() {
		while (!parms.empty() && parms.back().empty()) {
			parms.pop_back();
		}
		size_t pSize = size();
		it = {pSize == 0 ? 0 : 1, pSize };
	}

	void Instance::copy(const plist *p) {
		if(p != nullptr) {
			for (const auto& i : *p) {
				parms.push_back(i);
			}
		}
	}

	forStuff::~forStuff() {
		stuff.clear();
	}

	Instance::~Instance() {
		metrics = nullptr;
		myFor = nullptr;
		parms.clear();
	}

	Instance::Instance(unique_ptr<forStuff>& myFor) : myFor(std::move(myFor)) {}


	forStuff::forStuff(const std::string& vt,const std::string& ct,size_t v,size_t c) {
		stuff = {
				{vt,Support::tostring(v)}
			,
				{ct,Support::tostring(c)}
		};
	}

	forStuff::forStuff(const std::vector<std::pair<std::string,std::string>>& other) {
		stuff = other;
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
