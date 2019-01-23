#include <utility>

#include <typeinfo>
#include <cctype>
#include <fstream>
#include <cassert>
#include "userMacro.h"

#include "Driver.h"
#include "Wss.h"
#include "Injection.h"
#include "Macro.h"

namespace mt {

	size_t Driver::wss_type = typeid(Wss).hash_code();
	size_t Driver::mac_type = typeid(Macro).hash_code();
	size_t Driver::inj_type = typeid(Injection).hash_code();
	size_t Driver::str_type = typeid(std::string).hash_code();
	mstack Driver::empty_stack {};

	vMap Driver::vis = {
		  {mac_type,[](std::any& j,std::ostream &o) {
			  o << "❰"; std::any_cast<Macro>(j).visit(o); o << "❱";
		  	}
		},{inj_type,[](std::any& j,std::ostream &o){
				o << "⎧"; std::any_cast<Injection>(j).visit(o); o << "⎫";
			}
		},{str_type,[](std::any& j,std::ostream &o){
				o << "‘" << std::any_cast<std::string>(j) << "’";
			}
		},{wss_type,[](std::any& j,std::ostream &o){
				o << "“" << std::any_cast<Wss>(j).text << "”";
			}
		}
	};

	eMap Driver::exp = {
	   {mac_type,[](std::any& j,std::ostream &o,mstack& c){ std::any_cast<Macro>(j).expand(o,c); }},
	   {inj_type,[](std::any& j,std::ostream &o,mstack& c){ std::any_cast<Injection>(j).expand(o,c); }},
	   {str_type,[](std::any& j,std::ostream &o,mstack& c){
			o << std::any_cast<std::string>(j);
	   	}},
	   {wss_type,[](std::any& j,std::ostream &o,mstack& c){
			  o << std::any_cast<Wss>(j).text;
	   }}
	};



	Driver::Driver() {
	}

	mtext Driver::parse(std::istream &stream, bool advanced, bool strip) {
		if (!stream.good() && stream.eof()) {
			return {{},false};
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
			if ( macro_stack.empty()) {
				if (!final.empty() && final.back().type().hash_code() == str_type) {
					std::string back = std::move(std::any_cast<std::string>(final.back()));
					final.pop_back();
					back.append(str);
					final.emplace_back(std::move(back));
				} else {
					final.emplace_back(std::move(str));
				}
			} else {
				if (!parm.empty() && parm.back().type().hash_code() == str_type) {
					std::string back = std::move(std::any_cast<std::string>(parm.back()));
					parm.pop_back();
					back.append(str);
					parm.emplace_back(std::move(back));
				} else {
					parm.emplace_back(std::move(str));
				}
			}
		}
	}

	void Driver::storeWss(const std::string &str) {
		if(!str.empty()) {
			if (macro_stack.empty()) {
				if (!final.empty() && final.back().type().hash_code() == wss_type) {
					Wss back = std::move(std::any_cast<Wss>(final.back()));
					final.pop_back();
					final.emplace_back(std::move(Wss(back,str)));
				} else {
					final.emplace_back(std::move(Wss(str)));
				}
			} else {
				if (!parm.empty() && parm.back().type().hash_code() == wss_type) {
					Wss back = std::move(std::any_cast<Wss>(parm.back()));
					parm.pop_back();
					parm.emplace_back(std::move(Wss(back,str)));
				} else {
					parm.emplace_back(std::move(Wss(str)));
				}
			}
		}
	}


	void Driver::inject(const std::string &word) {
		Injection i(word);
		iterated = iterated || i.iterator;
		if (macro_stack.empty()) {
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

	void Driver::expand(mtext& object,std::ostream& outStream,mstack& context) {
		for(auto& j : object) {
			if (j.has_value()) {
				size_t type = j.type().hash_code();
				Driver::exp[type](j,outStream,context);
			}
		}
	}

	std::ostream& Driver::visit(std::any& j, std::ostream &o) {
		if (j.has_value()) {
			size_t type = j.type().hash_code();
			Driver::vis[type](j,o);
		}
		return o;
	}

	std::ostream& Driver::visit(mtext& object, std::ostream &o) {
		for (auto &j : object) {
			visit(j,o);
		}
		return o;
	}

	Driver::~Driver() {
		delete (scanner); scanner = nullptr;
		delete (parser); parser = nullptr;
	}

}

//injection