//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_TEXT_H
#define MACROTEXT_TEXT_H

#include <string>
#include <vector>
#include "support/Message.h"
#include "Token.h"
#include "mt/using.h"

namespace mt {
	using namespace Support;

	class Text : public Token {
	friend class Internals;
	private:
		std::string text;

	public:
	Text() = default;
	~Text() override = default;
	explicit Text(std::string);

	std::string get() const override;             //return text.
	void final(std::ostream&) const override;     //return final text.

	std::ostream& visit(std::ostream&) const override;
	void expand(Messages&,mtext&,mstack&) const override;
	void add(mtext&) override;
	std::string name() const override { return "`text`"; }

	void doFor(mtext&,const forStuff&) const override;
	void subs(mtext&,const std::vector<std::string>&,const std::string&) const override;

	void append(std::string);
	bool empty() const override;


};

};

#endif //MACROTEXT_TEXT_H

