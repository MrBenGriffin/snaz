//
// Created by Ben on 2019-05-15.
//

#ifndef MACROTEXT_SCRIPT_H
#define MACROTEXT_SCRIPT_H



#include <stack>
#include <string>
#include "support/Message.h"
#include "Token.h"
#include "mt/using.h"

namespace mt {
	using namespace Support;
	class Script : public Token {
	protected:
		std::string text;
	public:
		explicit Script(std::string);
		~Script() override = default;
		void append(std::string);
		void final(std::ostream&) const override;     //return final text.
		std::string get() const override;             //return text.
		bool empty() const override;

	};
}


#endif //MACROTEXT_SCRIPT_H
