//
// Created by Ben on 2019-02-06.
//

#ifndef MACROTEXT_DYNAMIC_H
#define MACROTEXT_DYNAMIC_H

#include "Message.h"

//set up dynamic library filenaming.
#ifdef __MACH__
#define SL(p,x) (p+#x + std::string(".dylib"))
#define SO(x) (#x + std::string(".dylib"))
#else
#define SL(p,x) p+#x + std::string(".so")
#define SO(x) #x + std::string(".so")
#endif

namespace Support {
	static bool dlerr(Messages &e) {
		const char *err = dlerror();
		if (err != nullptr) {
			string msg = err;
			ostringstream errs;
			errs << "Dynamic library load report: '" << err << "'.";
			e << Message(fatal, errs.str());
			return true;
		}
		return false;
	}
}


#endif //MACROTEXT_DYNAMIC_H
