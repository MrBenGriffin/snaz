#include "Internal.h"
#include "InternalInstance.h"
#include "support/Message.h"

namespace mt {
	using namespace Support;
	void iTitle::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iTeam::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iSuffix::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iShortTitle::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iSegmentName::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iLayout::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iLayoutName::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iLink::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iLinkRef::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iID::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iBirth::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iContent::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iDeath::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iExistContent::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}
	void iTW::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

}