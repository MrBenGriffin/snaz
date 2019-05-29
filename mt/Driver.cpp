#include <utility>
#include <deque>
#include <typeinfo>
#include <cctype>
#include <fstream>
#include <cassert>

#include "Driver.h"
#include "Definition.h"
#include "Advanced.h"
#include "Classic.h"
#include "Internal.h"
#include "support/Fandr.h"

namespace mt {

	using namespace Support;
	int 	Driver::accept = 0;

	Driver::Driver(Messages& errs,std::istream &stream,bool advanced) :
			iterated(false) {
		if (stream.good() && !stream.eof()) {
			source = &stream; //only used for parse errors..
			if(advanced) {
				scanner = new Advanced(errs,&stream);
			} else {
				scanner = new Classic(errs,&stream);
			}
		}
	}

	void Driver::parseError(Support::Messages &errs) {
		vector<std::string> lines;
		auto* prog = dynamic_cast<const std::istringstream*>(source);
		string line = prog->str();
		Support::fandr(line, "\n"  , "␤");
		size_t c1 = position.first;
		size_t c2 = position.second;
		std::string marker(c1-2,'.');
		marker.push_back('^');
		if((c2-c1) > 2) {
			marker.append(std::string(c2-c1-1,'.'));
		}
		if((c2-c1) > 0) {
			marker.push_back('^');
		}
		errs << Message(syntax,line);
		errs << Message(syntax,marker);
	}

	mtext Driver::parse(Messages& errs,bool strip) {
		scanner->stripped=strip;
		scanner->defining=false;
		parser = new Parser(errs,scanner,(*this));
		if (parser->parse() != accept) {
			parseError(errs);
		}
		delete parser; parser= nullptr;
		return std::move(final);
	}

	mtext Driver::parse(Messages& errs,const std::string& code,bool strip) {
		bool advanced = Definition::test_adv(code);
		std::istringstream codeStream(code);
		Driver driver(errs,codeStream,advanced);
		return driver.parse(errs,strip);
	}

	parse_result Driver::define(Messages& errs,bool strip) {
		bool success = false;
		scanner->stripped=strip;
		scanner->defining=true;
		parser = new Parser(errs,scanner,(*this));
		try {
			success = parser->parse() == accept;
			if (!success) {
				parseError(errs);
			}
		} catch (...) {
//			scanner.
		}
		delete parser; parser= nullptr;
		return {success, {std::move(final),iterated}};
	}

	/*
	 * Because we are parsing, we want to try and keep strings in a lump, rather than have lots of them
	 * adding up (which happens because of state changes).  So, we append a string when we find it.
	 * This could possibly be done a bit more tidily - and though it looks slow, it's far better to manage this
	 * in the parse than it is in the expansion.
	 *
	 * These are the Parser Methods
	 * driver.store($1); }
	 * driver.storeWss($1); }
	 * driver.inject($1); }
	 * driver.store_macro(); }
	 * driver.new_macro($1); }
	 * driver.add_parm(); }
	 *
	 * */
	void Driver::store(const std::string &str) {
		if(!str.empty()) {
			if ( macro_stack.empty()) {
				Token::add(str,final);
			} else {
				Token::add(str,parm);
			}
		}
	}

	void Driver::storeWss(const std::string &str) {
		if(!str.empty()) {
			shared_ptr<Token> wss = make_shared<Wss>(str);
			if ( macro_stack.empty()) {
				Token::add(wss,final);
			} else {
				Token::add(wss,parm);
			}
		}
	}

	void Driver::inject(const std::string &word) {
		auto i = new Injection(word);
		iterated = iterated || i->iterator;
		if ( macro_stack.empty()) {
			final.emplace_back(i);
		} else {
			parm.emplace_back(i);
		}
	}

	void Driver::store_macro() {
		//std::forward_list< std::unique_ptr<Macro> >	macro_stack;
		auto mac = std::move(macro_stack.front());
		macro_stack.pop_front();
		if (macro_stack.empty()) {
			final.emplace_back(mac.release()); //This releases the macro to final.
		} else {
			parm = macro_stack.front()->parms.back();
			macro_stack.front()->parms.pop_back();
			parm.emplace_back(mac.release());
		}
	}

	void Driver::new_macro(const std::string &word) {
		if (!macro_stack.empty()) {
			macro_stack.front()->parms.emplace_back(parm);
			parm.clear();
		}
		macro_stack.emplace_front(make_unique<Macro>(word)); // new Macro(word));
	}

	void Driver::add_parm() {
		macro_stack.front()->parms.emplace_back(parm);
		parm.clear();
	}

	/*
	 * So that's it for the parser methods.
	 * */

	std::string Driver::expand(Messages& e,std::string& program,mstack& context) {
		bool advanced = Definition::test_adv(program);
		std::istringstream code(program);
		Driver driver(e,code,advanced);
		mtext structure = driver.parse(e,false); //no strip
		ostringstream result;
		driver.expand(e,structure,result,context);
		return result.str();
	}

//Template, Macro, and Parameters all come in here.
	void Driver::expand(Messages &e, const mtext &object, std::ostream &o, mstack &context) {
		try {
			mtext result;
			for(auto& j : object) {
				j->expand(e,result,context);
			}
			for (auto &i : result) {
				i->final(o);
			}
		} catch (exception ex) {
			e << Message(fatal, ex.what());
		}
	}

	void Driver::doFor(const mtext& prog,mtext& out,const forStuff& stuff) {
		for (auto &t : prog) {
			t->doFor(out, stuff);
		}
	}

// Evaluate ONLY injections.
	void Driver::inject(const mtext& object,Messages& e,mtext& x,mstack& c) {
		for(auto& i : object) {
			i->inject(e,x,c);
		}
	}

// Do substitutes (used by iRegex).
	void Driver::subs(const mtext& code,mtext& out,const std::vector<std::string>& subs,const std::string& prefix) {
		for (auto &i : code) {
			i->subs(out, subs, prefix);
		}
	}

	std::ostream& Driver::visit(const Token& i, std::ostream& o) {
		return i.visit(o);
	}

	std::ostream& Driver::visit(const mtext& object, std::ostream& o) {
		for (auto& i : object) {
			i->visit(o);
		}
		return o;
	}

	Driver::~Driver() {
		parm.clear();
		delete (scanner); scanner = nullptr;
		delete (parser); parser = nullptr;
	}

}

//injection