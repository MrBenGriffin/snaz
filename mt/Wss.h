//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_WSS_H
#define MACROTEXT_WSS_H

#include <stack>
#include <string>
#include "support/Message.h"
#include "Script.h"
#include "mt/using.h"

namespace mt {
	using namespace Support;

	class Wss : public Script {
	private:
		static std::stack<const MacroText*> newline;
		static MacroText empty;
	public:
		explicit Wss(std::string );
		explicit Wss(const Wss*);
		Wss(std::string,location&);
		~Wss() override = default;
		std::string name() const override { return "`wss`"; }

		std::ostream& visit(std::ostream&, int) const override;
		void expand(Messages&,MacroText&,mstack&) const override;
		void inject(Messages&,MacroText&,mstack&) const override;
		void subs(MacroText&,const std::deque<std::string>&,const std::string&) const override;
		void doFor(Messages&,MacroText&,const forStuff&,mstack&) const override;
		void clone(MacroText&) const override;

		static void push(const MacroText*);
		static void pop();

	};
}

#endif //MACROTEXT_WSS_H
