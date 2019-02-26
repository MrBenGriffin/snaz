#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"

namespace mt {
	using namespace Support;
	void iTax::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iForTax::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iForTaxNodes::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iExistSimilar::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iForSimilar::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

}