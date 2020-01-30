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
//#include "parser.tab.hpp"

namespace mt {

	using namespace Support;
	int    Driver::accept = 0;

	void Driver::yield(MacroText& result) { result.adopt(*_final); }

	Driver::Driver(MacroText& text) : _final(&text), iterated(false) {}

	Driver::Driver(Messages &errs, std::istream &stream, bool advanced) :
			_final(nullptr), iterated(false) {
		if (stream.good() && !stream.eof()) {
			source = &stream; //only used for parse errors..
			if (advanced) {
				scanner = new Advanced(errs, &stream);
			} else {
				scanner = new Classic(errs, &stream);
			}
		}
	}

	void Driver::parseError(Support::Messages &errs) {
		errs.push(Message(fatal, "Parse Failed."));
	}

	void Driver::parse(Messages &errs, MacroText &result, bool strip) {
		_final = &result;
		scanner->stripped = strip;
		scanner->defining = false;
		parser = new Parser(errs, scanner, (*this));
		try {
			parser->parse(); //if this is not accept, it failed. (but errs are done already)
		} catch (...) {
			parseError(errs);
		}
		delete parser;
		parser = nullptr;
		_final = nullptr;
	}

	void Driver::parse(Messages &errs, MacroText &result, const std::string &code, bool strip) {
		bool advanced = Definition::test_adv(code);
		std::istringstream codeStream(code);
		Driver driver(errs, codeStream, advanced);
		driver.parse(errs, result, strip);
	}

	void Driver::define(Messages &errs, parse_result &result, bool strip) {
		bool success = false;
		_final = &(result.second.first);
		scanner->stripped = strip;
		scanner->defining = true;
		parser = new Parser(errs, scanner, (*this));
		try {
			success = parser->parse() == accept;
			if (!success) {
				parseError(errs);
			}
		} catch (...) {
			parseError(errs);
		}
		delete parser;
		parser = nullptr;
		_final = nullptr;
		result.first = success;
		result.second.second = iterated;
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
	void Driver::store(const std::string &str, Parser::location_type&) {
		if (!str.empty()) {
			if (macro_stack.empty()) {
				_final->add(str);
			} else {
				parm.add(str);
			}
		}
	}

	void Driver::storeWss(const std::string &str, Parser::location_type& pos) {
		if (!str.empty()) {
			auto wss = make_unique<Wss>(str, pos);
			if (macro_stack.empty()) {
				_final->emplace(wss);
			} else {
				parm.emplace(wss);
			}
		}
	}

	void Driver::inject(const std::string &word, Parser::location_type& pos) {
		auto i = make_unique<Injection>(word, pos);
		iterated = iterated || i->iterator;
		if (macro_stack.empty()) {
			_final->emplace(i);
		} else {
			parm.emplace(i);
		}
	}

	void Driver::store_macro() {
		//std::forward_list< std::unique_ptr<Macro> >	macro_stack;
		auto mac = std::move(macro_stack.front()); //mac is std::unique_ptr<Macro>
		macro_stack.pop_front();
		if (macro_stack.empty()) {
			_final->emplace(mac); //This releases the macro to final.
		} else {
			//parm is a local MacroText.
			parm.adopt(macro_stack.front()->parms.back());
			macro_stack.front()->parms.pop_back();
			parm.emplace(mac);
		}
	}

	void Driver::new_macro(const std::string &word, Parser::location_type& pos) {
		if (!macro_stack.empty()) {
			macro_stack.front()->parms.emplace_back(std::move(parm));
			parm.reset();
		}
		macro_stack.emplace_front(make_unique<Macro>(word, pos)); // new Macro(word));
	}

	void Driver::add_parm() {
		macro_stack.front()->parms.emplace_back(std::move(parm));
		parm.reset();
	}

	/*
	 * The following are NOT used by the parser, but for manual composition.
	 * */

	void Driver::addParm(const std::string &p, Parser::location_type& pos) {
		store(p, pos); add_parm();
	}

	void Driver::adoptParm(MacroText& t) {
		macro_stack.front()->parms.emplace_back(std::move(t));
	}


	/*
	 * So that's it for the parser methods.
	 * */

	Driver::~Driver() {
		delete (scanner); scanner = nullptr;
		delete (parser); parser = nullptr;
	}

}

//injection
