//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_NODE_NODEEXCEPTION_H
#define MACROTEXT_NODE_NODEEXCEPTION_H

#include <iostream>
#include <string>

#include "support/Message.h"

using namespace std;
using namespace Support;
namespace node {
	struct BadLocatorPath {
		BadLocatorPath(Messages &errs, string::const_iterator start, string::const_iterator pos,
						   string::const_iterator end) {
			if (!errs.suppressed()) {
				size_t pt = pos - start;
				ostringstream errStr;
				errStr << "Locator Error at character position " << pt << " in path '" << string(start, end) << "'";
				errs << Message(error, errStr.str());
			}
		}
	};
}
#endif //MACROTEXT_NODEEXCEPTION_H
