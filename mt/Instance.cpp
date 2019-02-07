//
// Created by Ben Griffin on 2019-02-07.
//

#include "Instance.h"
namespace mt {
	Instance::Instance(const plist *p, iteration i, bool gen) : parms(p), it(i), generated(gen) {}

	Instance::Instance(const Instance &o) :
		parms(o.parms), it(o.it), generated(o.generated), iValue(o.iValue),iCount(o.iCount) {}

}
