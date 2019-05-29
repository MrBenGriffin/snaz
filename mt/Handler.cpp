//
// Created by Ben on 2019-05-01.
//

#include "mt/Handler.h"
#include "mt/Instance.h"

namespace mt {

//	void Handler::expand(Messages&,MacroText&,const Instance*,mstack&) const = 0;
	std::ostream& Handler::visit(std::ostream& o) const { return o;}
	void Handler::inject(Messages&,MacroText&,mstack&) const {}
	void Handler::doFor(MacroText&,const forStuff&) const {}
	void Handler::subs(MacroText&,std::vector<std::string>& ,const std::string&) const {}
	bool Handler::inRange(size_t) const { return false; };
	std::string Handler::name() const { return ""; }

}