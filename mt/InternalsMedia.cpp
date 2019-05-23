#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"
#include "support/Media.h"
#include "Build.h"

namespace mt {
	using namespace Support;
	void iMedia::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		Media* media = Build::b().media();
		if(media != nullptr) {
			if (my.count == 1) {
				my.set(media->file(e,my.metrics,my.parm(1)));
			} else {
				my.set(media->attribute(e,my.parm(1),my.parm(2)));
			}
		} else {
			e << Message(error," media support was not found.");
		}
	}
	void iEmbed::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		Media* media = Build::b().media();
		if(media != nullptr) {
			if (my.count == 1) {
				my.set(media->embed(e,my.parm(1),false));
			} else {
				my.set(media->attribute(e,my.parm(1),my.parm(2)));
			}
		} else {
			e << Message(error," media support was not found.");
		}
	}
	void iExistMedia::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		const Media* media = Build::b().media();
		if(media != nullptr) {
			my.logic(media->exists(e,my.parm(1)),2);
		} else {
			e << Message(error," media support was not found.");
		}
	}

}