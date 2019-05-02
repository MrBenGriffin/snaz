//
// Created by Ben on 2019-05-01.
//

#include "mt/declarations.h"
#include "mt/Token.h"

namespace mt {

	void Token::expand(Messages&,mtext&,mstack&) const {}
	void Token::doFor(mtext&,const forStuff&) const {}
	bool Token::empty() const { return true; }
	void Token::subs(mtext& out,const std::vector<std::string>&,const std::string&) const {
		out.emplace_back(const_cast<Token*>(this));
	}
	void Token::inject(Messages&,mtext& out,mstack&) const {
		out.emplace_back(const_cast<Token*>(this));
	}
}
