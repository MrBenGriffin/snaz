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
		friend class Token;
	public:
		Text() = default;
		~Text() override = default;
		explicit Text(std::string);

		std::ostream& visit(std::ostream&) const override;
		void expand(Messages&,mtext&,mstack&) const override;
		std::string name() const override { return "`text`"; }

		void doFor(mtext&,const forStuff&) const override;
		void subs(mtext&,const std::vector<std::string>&,const std::string&) const override;
		void inject(Messages&,mtext&,mstack&) const override;


	};

}

#endif //MACROTEXT_TEXT_H
