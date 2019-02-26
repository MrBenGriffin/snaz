#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"

namespace mt {
	using namespace Support;

	void iBase64::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iDecode::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iEncode::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iHex::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iUnHex::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iUpper::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iLower::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iUrlEncode::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iDigest::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
}