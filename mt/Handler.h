//
// Created by Ben on 2019-05-01.
//

#ifndef MACROTEXT_HANDLER_H
#define MACROTEXT_HANDLER_H

#include <string>
#include <variant>
#include <vector>

#include "support/Message.h"
#include "mt/declarations.h"

using namespace Support;

namespace mt {

	using mtext=std::deque<Token>;
	class forStuff;
	class Instance;
	class Handler;
	using Carriage= std::pair<const Handler*, Instance>;
	using mstack=std::deque< Carriage >;

	class Handler {
	public:
		Handler() = default;
		virtual ~Handler() = default;
		virtual bool inRange(size_t) const;
		virtual void expand(Messages&,mtext&,Instance&,mstack&) const;
		virtual std::ostream& visit(std::ostream&) const;
		virtual void inject(Messages&,mtext&,mstack&) const;
		virtual void doFor(mtext&,const forStuff&) const;
		virtual void subs(mtext&,std::vector<std::string>& ,const std::string&) const;
		virtual void add(mtext&);
		virtual std::string name() const;
	};

}

#endif //MACROTEXT_HANDLER_H
