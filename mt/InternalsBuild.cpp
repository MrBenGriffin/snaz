#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"
#include "Build.h"
#include "BuildUser.h"

namespace mt {
	using namespace Support;
	void iTech::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		my.logic(Build::b().tech(),1);
	}
	void iPreview::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		my.logic(Build::b().current() == draft,1);
	}
	void iLang::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		auto lang = Build::b().language();
		my.logic(lang.ref,1);
	}
	void iLangID::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		my.logic(Build::b().lang(),1);
	}
	void iFullBuild::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		my.logic(Build::b().fullBuild(),1);
	}

}