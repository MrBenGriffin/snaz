//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_NODEEXCEPTION_H
#define MACROTEXT_NODEEXCEPTION_H

#include <iostream>
#include <string>

#include "support/Message.h"

using namespace std;
using namespace Support;

struct BadNodeLocatorPath {
	BadNodeLocatorPath(Messages& errs,string::const_iterator start,string::const_iterator pos,string::const_iterator end) {
		if(!errs.suppressed()) {
			size_t pt = pos - start;
			ostringstream errStr;
			errStr << "NodeLocator Error at character position " << pt << " in path '" << string(start,end) << "'";
			errs << Message(error,errStr.str());
		}
	}
};

#endif //MACROTEXT_NODEEXCEPTION_H
