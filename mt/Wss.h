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
	public:
		explicit Wss(std::string );
		explicit Wss(const Wss*);
		~Wss() override = default;
		std::string name() const override { return "`wss`"; }

		std::ostream& visit(std::ostream&) const override;
		void expand(Messages&,MacroText&,mstack&) const override;
		void inject(Messages&,MacroText&,mstack&) const override;
		void subs(MacroText&,const std::vector<std::string>&,const std::string&) const override;
		void doFor(MacroText&,const forStuff&) const override {}
		void clone(MacroText&) const override;

		static void push(const MacroText*);
		static void pop();

	};
}

#endif //MACROTEXT_WSS_H
