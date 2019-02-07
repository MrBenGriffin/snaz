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


namespace mt {

	mstack	Driver::empty_stack {};
	int 	Driver::accept = 0;

	Driver::Driver(std::istream &stream,bool advanced) :
		iterated(false) {
		if (stream.good() && !stream.eof()) {
			if(advanced) {
				scanner = new Advanced(&stream);
			} else {
				scanner = new Classic(&stream);
			}
			parser = new Parser(scanner,(*this));
		}
	}

	mtext Driver::parse(bool strip) {
		scanner->stripped=strip;
		scanner->defining=false;
		if (parser->parse() != accept) {
			std::cout << "Parse failed!!" << std::endl;
		}
		return final;
	}

	parse_result Driver::define(bool strip) {
		scanner->stripped=strip;
		scanner->defining=true;
		if (parser->parse() != accept) {
			std::cout << "Define failed!!" << std::endl;
		}
		return {final,iterated};
	}


	/*
	 * Because we are parsing, we want to try and keep strings in a lump, rather than have lots of them
	 * adding up (which happens because of state changes).  So, we append a string when we find it.
	 * This could possibly be done a bit more tidily - and though it looks slow, it's far better to manage this
	 * in the parse than it is in the expansion.
	 * */
	void Driver::store(const std::string &str) {
		if(!str.empty()) {
			Text text(str);
			if ( macro_stack.empty()) {
				text.add(final);
			} else {
				text.add(parm);
			}
		}
	}

	void Driver::storeWss(const std::string &str) {
		if(!str.empty()) {
			Wss wss(str);
			if ( macro_stack.empty()) {
				wss.add(final);
			} else {
				wss.add(parm);
			}
		}
	}

	void Driver::inject(const std::string &word) {
		Injection i(word);
		iterated = iterated || i.iterator;
		if ( macro_stack.empty()) {
			final.emplace_back(std::move(i));
		} else {
			parm.emplace_back(std::move(i));
		}
	}

	void Driver::store_macro() {
		Macro mac = macro_stack.front();
		macro_stack.pop_front();
		if (macro_stack.empty()) {
			final.emplace_back(std::move(mac));
		} else {
			parm = macro_stack.front().parms.back();
			macro_stack.front().parms.pop_back();
			parm.emplace_back(std::move(mac));
		}
	}

	void Driver::new_macro(const std::string &word) {
		if (!macro_stack.empty()) {
			macro_stack.front().parms.emplace_back(parm);
			parm.clear();
		}
		macro_stack.emplace_front(std::move(Macro(word)));
	}

	void Driver::add_parm() {
		macro_stack.front().parms.emplace_back(parm);
		parm.clear();
	}

	void Driver::expand(std::ostream& o,Messages& e,const std::string &title) {
		mtext result;
		mstack context;
		Handler handler({title}); //Implicitly constructs a Content..
		context.push_back({&handler,{nullptr,{0,0}}});
		expand(final,e,result,context);
		for(auto& i : result) {
			if (std::holds_alternative<Text>(i)) { o << std::get<Text>(i).get(); } else {
				if (std::holds_alternative<Wss>(i)) { o << std::get<Wss>(i).get(); } else {
					std::visit([&o](auto&& arg){ arg.visit(o);},i);
				}
			}
		}
	}

	//parameter expansion..
	void Driver::expand(const mtext& object,Messages& e,std::ostream& o,mstack& c) {
		mtext result;
		expand(object,e,result,c);
		for(auto& i : result) {
			if (std::holds_alternative<Text>(i)) { o << std::get<Text>(i).get(); } else {
				if (std::holds_alternative<Wss>(i)) { o << std::get<Wss>(i).get(); } else {
					std::visit([&o](auto&& arg){ arg.visit(o);},i);
				}
			}
		}
	}

	void Driver::expand(const mtext& object,Messages& e,mtext& x,mstack& c) {
		for(auto& j : object) {
			std::visit([&e,&x,&c](auto&& arg){ arg.expand(e,x,c);},j);
		}
	}

	// Evaluate ONLY injections.
	void Driver::inject(const mtext& object,Messages& e,mtext& x,mstack& c) {
		for(auto& i : object) {
			if (std::holds_alternative<Injection>(i)) {
				std::visit([&e,&x,&c](auto&& arg){ arg.expand(e,x,c);},i);
			} else {
				if(std::holds_alternative<Macro>(i)) {
					std::get<Macro>(i).inject(e,x,c);
				} else {
					x.push_back(i);
				}
			}
		}
	}

	// Do substitutes..
	void Driver::subs(const mtext& prog,mtext& out,std::vector<std::string>& subs,const std::string& prefix) {
		for (auto &t : prog) {
			if (std::holds_alternative<Text>(t)) {
				std::get<Text>(t).subs(out, subs, prefix);
			} else {
				if(std::holds_alternative<Macro>(t)) {
					std::get<Macro>(t).subs(out,subs,prefix);
				} else {
					out.push_back(t);
				}
			}
		}
	}


	std::ostream& Driver::visit(const Token& j, std::ostream& o) {
		std::visit([&o](auto&& arg){ arg.visit(o);},j);
		return o;
	}

	std::ostream& Driver::visit(const mtext& object, std::ostream& o) {
		for (auto& j : object) {
			std::visit([&o](auto&& arg){ arg.visit(o);},j); o << std::flush;
		}
		return o;
	}

	Driver::~Driver() {
		delete (scanner); scanner = nullptr;
		delete (parser); parser = nullptr;
	}

}

//injection