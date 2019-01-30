#include <utility>
#include <deque>
#include <typeinfo>
#include <cctype>
#include <fstream>
#include <cassert>

#include "Driver.h"
#include "Definition.h"

namespace mt {

	mstack Driver::empty_stack {};

	mtext Driver::parse(std::istream &stream, bool advanced, bool strip) {
		if (!stream.good() && stream.eof()) {
			return {};
		}
		parse_helper(stream,advanced,strip,false);
		return final;
	}

	parse_result Driver::define(std::istream &stream, bool advanced, bool strip) {
		if (!stream.good() && stream.eof()) {
			return {{},false};
		}
		parse_helper(stream,advanced,strip,true);
		return {final,iterated};
	}

	void Driver::parse_helper(std::istream &stream,bool advanced,bool stripped,bool defining) {
		iterated = false;
		delete (scanner);
		//TODO: This needs to allow for classic once we are happy with the advance scanners.
		//       if(advanced || !advanced) {
		try {
			scanner = new mt::Advanced(&stream,stripped,defining);
		}
		catch (std::bad_alloc &ba) {
			std::cerr << "Failed to allocate scanner: (" <<
					  ba.what() << "), exiting!!\n";
			exit(EXIT_FAILURE);
		}
		//       }
		delete (parser);
		try {
			parser = new mt::Parser((*scanner),(*this));
			// parser->set_debug_level(1);
		}
		catch (std::bad_alloc &ba) {
			std::cerr << "Failed to allocate parser: (" <<
					  ba.what() << "), exiting!!\n";
			exit(EXIT_FAILURE);
		}
		const int accept(0);
		if (parser->parse() != accept) {
			std::cerr << "Parse failed!!\n";
		}
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

	void Driver::expand(const mtext& object,std::ostream& o,const std::string &title) {
		mtext result;
		mstack  context;
		Content start(title);
		Instance zero(nullptr,{0,0});
		Handler handler(start);
		context.push_back({&handler,zero});
		expand(object,result,context);
		for(auto& i : result) {
			if (std::holds_alternative<Text>(i)) { o << std::get<Text>(i).get(); } else {
				if (std::holds_alternative<Wss>(i)) { o << std::get<Wss>(i).get(); } else {
					std::visit([&o](auto&& arg){ arg.visit(o);},i);
				}
			}
		}
	}

	//parameter expansion..
	void Driver::expand(const mtext& object,std::ostream& o,mstack& c) {
		mtext result;
		expand(object,result,c);
		for(auto& i : result) {
			if (std::holds_alternative<Text>(i)) { o << std::get<Text>(i).get(); } else {
				if (std::holds_alternative<Wss>(i)) { o << std::get<Wss>(i).get(); } else {
					std::visit([&o](auto&& arg){ arg.visit(o);},i);
				}
			}
		}
	}

	void Driver::expand(const mtext& object,mtext& x,mstack& c) {
		for(auto& j : object) {
			std::visit([&x,&c](auto&& arg){ arg.expand(x,c);},j);
		}
	}

	// Evaluate ONLY injections.
	void Driver::inject(const mtext& object,mtext& x,mstack& c) {
		for(auto& i : object) {
			if (std::holds_alternative<Injection>(i)) {
				std::visit([&x,&c](auto&& arg){ arg.expand(x,c);},i);
			} else {
				x.push_back(i);
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