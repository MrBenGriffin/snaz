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
		size_t size;
		std::ostringstream text;
	public:
		explicit Script(const std::string&);
		explicit Script(const std::ostringstream&);
		~Script() override = default;
		void append(const std::string&);
		void append(const std::ostringstream&);
		void final(std::ostream&) const override;     //return final text.
		std::string get() const override;             //return text.
		bool empty() const override;
		void inject(Messages&,mtext&,mstack&) const override;
		const std::ostringstream& stream() const { return text;}             //return text.

	};
}


#endif //MACROTEXT_SCRIPT_H
