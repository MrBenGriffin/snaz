//
// Created by Ben on 2019-05-29.
//

#ifndef MACROTEXT_MACROTEXT_H
#define MACROTEXT_MACROTEXT_H

#include <deque>
#include <memory>

#include "support/Message.h"

#include "mt/Wss.h"
#include "mt/Text.h"
#include "mt/Macro.h"
#include "mt/Injection.h"

// Tokens are one of (macro,script(text,wss),injection).

namespace mt {
	class Token;
	using namespace std;
	using namespace Support;

	class MacroText {
	private:
		//std::deque<Token> ?::: ---> Token is an abstract class, and so cannot be dealt with like this here.
		std::deque<unique_ptr<Token>> tokens;
	public:
		MacroText();
		MacroText(MacroText &&) noexcept; // = default; //ensure that we do not use a copy constructor on move..
		MacroText(const MacroText&) = delete;

		void adopt(MacroText &) noexcept; // = default; //ensure that we do not use a copy constructor on move..
		void reset();

		bool simple() const;
		bool empty() const;
		
		void trim();
		void emplace(unique_ptr<Wss>&);
		void emplace(unique_ptr<Text>&);
		void emplace(unique_ptr<Macro>&);
		void emplace(unique_ptr<Injection>&);

		void add(const std::string&);
		void add(const MacroText&);
		void add(const MacroText*);

		//Migrated from Driver.
		void str(Messages &,std::string&,mstack &) const;
		void expand(Messages&,MacroText&,mstack&) const;
		void expand(Messages&,std::ostream&,mstack&) const;
		std::ostream& visit(std::ostream&) const;
		std::ostream& final(std::ostream&) const;
		void doFor(MacroText&,const forStuff&) const;

		void inject(Messages&,MacroText&,mstack&) const;
		void subs(MacroText&,const std::vector<std::string>&,const std::string&) const;

		static std::string expand(Messages&,std::string&,mstack&,bool); //dirty


	};
}


#endif //MACROTEXT_MACROTEXT_H