//
// Created by Ben Griffin on 2019-02-07.
//

#include "Instance.h"
#include "support/Convert.h"
namespace mt {
	Instance::Instance(const plist *p, iteration i, bool gen) : parms(p), it(i), generated(gen) {}

	Instance::Instance(const Instance &o) :
		parms(o.parms), it(o.it), generated(o.generated), iValue(o.iValue),iCount(o.iCount) {}

	forStuff::forStuff(std::string& vt,std::string& ct,size_t v,size_t c) {
		stuff = {
				{std::move(vt),std::move(Support::tostring(v))}
				,
				{std::move(ct),std::move(Support::tostring(c))}
		};
	}
}
