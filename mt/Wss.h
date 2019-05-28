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
		friend class Token;
		static std::stack<const mtext*> newline;
	public:
		explicit Wss(std::string );
		~Wss() override = default;
		std::string name() const override { return "`wss`"; }

		std::ostream& visit(std::ostream&) const override;
		void expand(Messages&,mtext&,mstack&) const override;
		void inject(Messages&,mtext&,mstack&) const override;
		void subs(mtext&,const std::vector<std::string>&,const std::string&) const override;

		static void push(const mtext*);
		static void pop();

	};
}

#endif //MACROTEXT_WSS_H