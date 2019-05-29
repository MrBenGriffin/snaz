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

namespace mt {
	using namespace Support;

	class Text : public Script {
	public:
		Text() = default;
		~Text() override = default;
		explicit Text(std::string);
		explicit Text(const Text*);
		unique_ptr<Token> clone() const override;

		std::ostream& visit(std::ostream&) const override;
		void expand(Messages&,MacroText&,mstack&) const override;
		std::string name() const override { return "`text`"; }


		void doFor(MacroText&,const forStuff&) const override;
		void subs(const std::vector<std::string>&,const std::string&) override;
		void inject(Messages&,MacroText&,mstack&) const override;

	};

}

#endif //MACROTEXT_TEXT_H
