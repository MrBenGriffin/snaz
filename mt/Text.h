//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_TEXT_H
#define MACROTEXT_TEXT_H

#include <string>
#include <vector>
#include "support/Message.h"
#include "Script.h"
#include "mt/using.h"
#include "mt/Injection.h"

namespace mt {
	using namespace Support;

	class Text : public Script {
		static Injection i,k;
	private:
		void interpolate(const string&,const string&,MacroText&, Injection&) const;
	public:
		Text() = default;
		~Text() override = default;
		explicit Text(std::string);
		explicit Text(const Text*);

		void clone(MacroText&) const override;

		std::ostream& visit(std::ostream&) const override;
		void expand(Messages&,MacroText&,mstack&) const override;
		std::string name() const override { return "`text`"; }

		void doFor(Messages&,MacroText&,const forStuff&,mstack&) const override;
		void subs(MacroText&,const std::deque<std::string>&,const std::string&) const override;
		void inject(Messages&,MacroText&,mstack&) const override;

	};

}

#endif //MACROTEXT_TEXT_H
