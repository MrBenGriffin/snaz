//
// Created by Ben on 2019-05-01.
//

#include "mt/declarations.h"
#include "mt/Token.h"
#include "mt/Macro.h"
#include "mt/Wss.h"
#include "mt/Text.h"
#include "mt/Injection.h"
#include "mt/MacroText.h"

namespace mt {

	void Token::expand(Messages&,MacroText&,mstack&) const {}
	void Token::doFor(MacroText&,const forStuff&) const {}
	bool Token::empty() const { return true; }

}