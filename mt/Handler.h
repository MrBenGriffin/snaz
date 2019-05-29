//
// Created by Ben on 2019-05-01.
//

#ifndef MACROTEXT_HANDLER_H
#define MACROTEXT_HANDLER_H

#include <string>
#include <variant>
#include <vector>

#include "support/Message.h"
#include "mt/using.h"

using namespace Support;

namespace mt {

	class forStuff;

	class Handler {
	public:
		Handler() = default;
		virtual ~Handler() = default;
		virtual bool inRange(size_t) const;
		virtual void expand(Messages&,MacroText&,Instance&,mstack&) const = 0;
		virtual std::ostream& visit(std::ostream&) const;
		virtual void inject(Messages&,MacroText&,mstack&) const;
		virtual void doFor(MacroText&,const forStuff&) const;
		virtual void subs(MacroText&,std::vector<std::string>& ,const std::string&) const;
		virtual std::string name() const;
	};

}

#endif //MACROTEXT_HANDLER_H
