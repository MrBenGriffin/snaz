#include <utility>

#include <typeinfo>
#include <cctype>
#include <fstream>
#include <cassert>
#include "userMacro.h"

#include "Driver.h"
namespace mt {

	size_t Driver::mac_type = typeid(macro).hash_code();
	size_t Driver::inj_type = typeid(Injection).hash_code();
	size_t Driver::str_type = typeid(std::string).hash_code();
	mstack Driver::empty_stack {};

	void Injection::parseStart() {
		switch(basis[0]) {
			case '^' : parseParent(); break;
			case '(' : parseBrackets(); break;
			case 'p' : parsePName(); break; //same as ^0
			case 'i' :
			case 'j' :
			case 'k' :
			case 'n' : parseIterated(); break;
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' : {
				value = stoul(basis);
			} break;
			default: {
				type=It::text;
			}
		}
	}
	void Injection::parseIterated() {
		switch(basis[0]) {
			case 'i': iterator = true; type = It::current; break;
			case 'j': iterator = true; type = It::current; offset = 1; break;
			case 'k': iterator = true; type = It::count; break;
			case 'n': type = It::size; break;
			default: break;
		}
		basis.erase(0,1);
		if(!basis.empty()) {
			if(basis[0] == '.') {
				modulus = true;
				basis[0] = '+';
			}
			offset = stol(basis);
		}
	}
	void Injection::parsePName() {
		basis.erase(0,1);
		if(basis[0] == 's') {
			stack=true;
			basis.erase(0,1);
		} else {
			sValue++;
		}
	}
	void Injection::parseBrackets() {
		basis.erase(0, 1);
		basis.pop_back();
		if (!basis.empty()) {
			if (basis.back() == '+') { // is it a list of parameters?
				basis.pop_back();      // remove the list marker
				list = true;           // mark as a list.
				value = stoul(basis);  // so get the offset.
			} else {
				parseStart();         // needs more work.
			}
		}
	}
	void Injection::parseParent() {
		while(basis[0] == '^') {       // go up the stack.
			sValue++;
			basis.erase(0,1);
		}
		parseStart();                 // now work out the rest.
	}
	Injection::Injection(const std::string src) :
			type(It::plain),value(0),sValue(0),offset(0),
			modulus(false),stack(false),list(false),iterator(false),basis(src) {
		// ⍟^*([ijk]|[0-9]+|\(^*([ijkn]([.+-][0-9]+)?|[0-9]++?|(p(s|[0-9]?)))\))
		parseStart();
	}

	void Injection::expand(std::ostream& result,mstack& context) {
		if (type == It::text) {
			result << basis;
		} else {
			auto& contextMacro = context[sValue].first;
			auto& contextParms = context[sValue].second;
			size_t parmCount = contextParms.size();
			if (stack) {
				result << "[TODO::STACK]";
			}
			if (list) {
				result << "[TODO::LIST " << value << "]";
			} else {
				switch (type) {
					case It::plain:
						if((contextMacro->minParms <= value <= contextMacro->maxParms) && (value <= parmCount)) {
							if(value > 0) {
								if(sValue == 0) {
									Driver::expand(contextParms[value - 1],result,context);
								} else {
									mstack subContext(context.begin() + sValue,context.end());
									Driver::expand(contextParms[value - 1],result,subContext);
								}
							} else {
								result << contextMacro->name;
							}
						} else {
							//TODO: Report value being asked for is out of the range of the definition.
						}
						break;
					case It::current:
						result << "[TODO::ITERATOR I]";
						if (offset != 0) {
							result << "[TODO::OFFSET " << offset << " from " << value << "]";
						}
						break;
					case It::count:
						result << "[TODO::ITERATOR K]";
						break;
					case It::size:
						result << parmCount;
						break;
					default:
						break;
				}
			}
		}
	}

	std::ostream& Injection::visit(std::ostream &result) {
		if(type == It::text) {
			result << basis;
		} else {
			result << "%";
			if (sValue != 0) {
				result << sValue;
			}
			if(stack) {
				result << "s";
			}
			result << "[";
			switch(type) {
				case It::plain:     result << value; break;
				case It::current:   result << "i"; break;
				case It::count:     result << "k"; break;
				case It::size:      result << "n"; break;
				default: break;
			}
			if(offset != 0) {
				if(modulus) {
					result << "." << offset;
				} else {
					result << std::showpos << offset;
				}
			}
			if(list) {
				result << "+";
			}
			result << "]";
		}
		return result;
	}

	macro::macro(std::string n) : name(std::move(n)) {};
	macro::macro(const macro &o) {
		name = o.name;
		parms = o.parms;
	}

	void macro::expand(std::ostream& o,mstack& context) {
		if(userMacro::has(name)) {
			userMacro::get(name).expand(o,parms,context);
		} else {
			visit(o);
		}
	}

	std::ostream& macro::visit(std::ostream &result) {
		result << "@" << name << "(";
		for (auto &p : parms) {
			if (!p.empty()) {
				result << "{";
				Driver::visit(p,result);
				result << "}";
			}
			if (&p != &parms.back()) {
				result << ",";
			}
		}
		result << ")";
		return result;
	}

	Driver::Driver() { }

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

	void Driver::store(const std::string &str) {
		if(!str.empty()) {
			if (macro_stack.empty()) {
				final.emplace_back(str);
			} else {
				parm.emplace_back(str);
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

//	using plist=std::vector<mtext>;
	void Driver::store_macro() {
		macro mac = macro_stack.front();
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
		macro_stack.emplace_front(std::move(macro(word)));
	}

	void Driver::add_parm(const std::string &word) {
		if(!word.empty()) {
			parm.emplace_back(word);
		}
		macro_stack.front().parms.emplace_back(parm);
		parm.clear();
	}

//		size_t b = 0;
//		size_t e = object.size();
//		if(trim) {
//			while(b++ < e) {
//				auto& i = object[b];
//				if(i.has_value() &&
//					!((i.type().hash_code() == str_type) && (std::any_cast<std::string>(i).find_first_not_of("\t\n\x0d\x0a") == std::string::npos))
//				) break;
//			}
//			while(--e > b) {
//				auto& i = object[e];
//				if(i.has_value() &&
//					!((i.type().hash_code() == str_type) && (std::any_cast<std::string>(i).find_first_not_of("\t\n\x0d\x0a") == std::string::npos))
//				) break;
//			}
//		}
//		for (; b < e; b++ ) {
//		auto& j = object[b];
//	}


	void Driver::expand(mtext& object,std::ostream& o,mstack& context) {
		for(auto& j : object) {
			if (j.has_value()) {
				size_t type = j.type().hash_code();
				if (type == mac_type) {
					std::any_cast<macro>(j).expand(o,context);
				} else {
					if (type == inj_type) {
						std::any_cast<Injection>(j).expand(o,context);
					} else {
						o << std::any_cast<std::string>(j);
					}
				}
			}
		}
	}

	std::ostream& Driver::visit(mtext& object, std::ostream &o) {
		for (auto &j : object) {
			if (j.has_value()) {
				size_t type = j.type().hash_code();
				if (type == mac_type) {
					std::any_cast<macro>(j).visit(o);
				} else {
					if (type == inj_type) {
						std::any_cast<Injection>(j).visit(o);
					} else {
						o << std::any_cast<std::string>(j);
					}
				}
			}
		}
		return o;
	}

	Driver::~Driver() {
		delete (scanner); scanner = nullptr;
		delete (parser); parser = nullptr;
	}

}

//injection