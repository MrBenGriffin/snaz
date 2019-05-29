#include "Build.h"
#include "Internal.h"
#include "InternalInstance.h"
#include "support/Infix.h"
#include "support/Message.h"
#include "support/Convert.h"
#include "support/Timing.h"
#include "support/Env.h"
#include "node/Node.h"
#include "node/Content.h"

#include <iomanip>

namespace mt {
	using namespace Support;
	using namespace Support;

	void iEq::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::string left =  my.parm(1);
		std::string right = my.count > 1 ? my.parm(2): "";
		my.logic(left == right,3);
	}
	void iExpr::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		Infix::Evaluate expression;
		auto expr=my.parm(1);
		expression.set_expression(expr);
		for (size_t i=2; i <= my.count; i++) {
			char **end = nullptr;
			std::string parmValue = my.parm(i);
			std::ostringstream ost;
			ost << "p" << i-1; //parm by number.
			auto number = strtold(&parmValue[0], end);
			expression.add_parm(ost.str(),number);
		}
		long double value = expression.process(*my.errs);
		if(my.errs->marked()) {
			my.errs->prefix({usage,expr});
			my.errs->enscope("Within iExpr");
		}
		std::ostringstream ost; ost << value;
		my.set(ost.str());
	}
	void iForIndex::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		/**
		 * iForIndex(=,BIM=foo=bar,*,$,,[*:$]) (1:delimiter,2:list,3:valueToken,4:countToken,5:sort,6:code)
		 */
		InternalInstance my(this,e,o,instance,context);
		plist parms=toParms(my.parm(2),my.parm(1),my.parm(5)); //list,delimiter,sort
		my.generate(parms,my.praw(6),my.parm(3),my.parm(4));   //parms,code,vToken,cToken
	}
	void iIndex::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		enum optype {resize,ifempty,normalise,get,size,set,push,erase,find,back,append,drop,reverse,uappend,upush,pop,retrieve,remove,contains} op = normalise;
		string result,delimiter = my.parm(1);
		if (delimiter.size() == 1) {
			if (my.count > 1) { //now process the array
				string index_string = my.parm(2);
				deque<string> idx; istringstream iss (index_string);
				for( string line; getline(iss,line,delimiter[0]); ){
					idx.push_back(line);
				}
				if (my.count > 2) { //now process the operation
					string operation = my.parm(3);
					if (operation.size() > 1) {
						switch (operation[0]) {
							case 'r': { //resize, reverse, retrieve
								switch (operation[2]) {
									case 'm': op=remove; break;
									case 's': op=resize; break;
									case 't': op=retrieve; break;
									case 'v': op=reverse; break;
									default: op=normalise; break;
								}
							} break;
							case 'i': op=ifempty; break;
							case 'n': op=normalise; break;
							case 'g': op=get; break;
							case 's': op= operation[1] == 'e' ? set : size ; break;
							case 'p': op= operation[1] == 'o' ? pop : push ; break;
							case 'e': op=erase; break;
							case 'f': op=find; break;
							case 'c': op=contains; break;
							case 'b': op=back; break;
							case 'a': op=append; break;
							case 'd': op=drop; break;
							case 'u': op= operation[1] == 'a' ? uappend : upush ; break;
							default: {
								e << Message(error,"iIndex: unknown operation '"+ operation +"' found");
							} break;
						}
					}
				}
				string p1,p2;
				if (my.count > 3) { //now process p1
					p1 = my.parm(4);
				}
				if (my.count > 4 && (op != contains)) { //now process p2
					p2 = my.parm(5);
				}
				pair<int,bool> i(0,true); //used for those who have p1 as an index.
				bool finished = false;
				switch (op) {
					case resize: {
						i = znatural(p1);
						if (i.second) { idx.resize(i.first,p2); }
					} break;
					case ifempty: {
						i = znatural(p1);
						if (i.second) {
							if ( (unsigned)i.first < idx.size()) {
								if (idx[i.first].empty()) { idx[i.first] = p2;}
							} else {
								idx.resize(i.first);
								idx.push_back(p2);
							}
						}
					} break;
					case normalise: {
					} break;
					case get: {
						i = znatural(p1);
						if (i.second && (unsigned)i.first < idx.size()) { result = idx[i.first]; }
						finished = true;
					} break;
					case set: {
						i = znatural(p1);
						if (i.second) {
							if ( (unsigned)i.first < idx.size()) {
								idx[i.first] = p2;
							} else {
								idx.resize(i.first);
								idx.push_back(p2);
							}
						}
					} break;
					case size: {
						tostring(result,idx.size());
						finished = true;
					} break;
					case pop: {
						idx.pop_front();
					} break;
					case push: {
						idx.push_front(p1);
					} break;
					case upush: {
						signed x=0,y=(signed)idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						if (x >= (signed)idx.size()) {
							idx.push_front(p1);
						}
					} break;
					case erase: {
						i = znatural(p1);
						if (i.second && (unsigned)i.first < idx.size()) { idx.erase(idx.begin()+i.first); }
					} break;
					case remove: { //erase all indices that match p1.
						for (size_t x=0; x < idx.size(); x++ ) {
							if (p1 == idx[x]) {
								idx.erase(idx.begin()+ x--);
							}
						}
					} break;
					case contains: {
						size_t x=0,y= idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						my.logic(x < idx.size(),4); //@iIndex(;,A;B,contains,A,[T,F])=4
						return;
					} break;
					case find: {
						signed x=0,y=(signed)idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						if (x >= (signed)idx.size()) x = -1;
						tostring(result,x);
						finished = true;
					} break;
					case retrieve: { //return all indexes that start with p1.
						size_t plen=p1.size(); //
						for (size_t x=0;x < idx.size(); x++) {
							if (idx[x].compare(0,plen,p1) != 0) {
								idx.erase(idx.begin()+ x--);
							}
						}
					} break;
					case back: {
						if (! idx.empty()) {
							result = idx.back();
						}
						finished = true;
					} break;
					case append: {
						idx.push_back(p1);
					} break;
					case uappend: {
						signed x=0,y=(signed)idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						if (x >= (signed)idx.size()) {
							idx.push_back(p1);
						}
					} break;
					case drop: {
						if (! idx.empty()) {
							idx.pop_back();
						}
					} break;
					case reverse: {
						ostringstream rslt;
						for(auto j=idx.rbegin(); j!=idx.rend();++j) {
							rslt << *j;
							if (j+1 != idx.rend() || j->empty() ) {
								rslt.put(delimiter[0]);
							}
						}
						result = rslt.str();
						finished = true;
					} break;
				}
				if (!finished) {
					ostringstream rslt;
					for(auto j=idx.begin(); j!=idx.end();++j) {
						rslt << *j;
						if (j+1 != idx.end() || j->empty() ) {
							rslt.put(delimiter[0]);
						}
					}
					result = rslt.str();
				}
			}
		} else {
			e << Message(error,"The delimiter must be a single character from Unicode page 0. '" + delimiter + "' was found");
		}
		my.set(result);
	}
	void iConsole::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
//		enum channel { fatal, error, syntax, range, parms, warn, info, debug, usage, timing, trace, code };
		channel type = info;
		string message;
		if(my.count == 2) {
			std::string infoStr =  my.parm(1);
			message =  my.parm(2);
			if(!infoStr.empty()) {
				char hint = infoStr[0];
				hint = (char) tolower(hint);
				switch (hint) {
					case 'f': type=fatal; break;
					case 'x':
					case 'e': type=error; break;
					case 'w': type=warn; break;
					case 'g': type=syntax; break;
					case 'r': type=range; break;
					case 'd': type=debug; break;
					case 'u': type=usage; break;
					case 'p': type=code; break;
					case 't': doTrace(e,context); e.enscope(message); return;
					case 'b': Timing::t().get(e,'b'); e.enscope(message); return;
					case 'n': Timing::t().get(e,'n'); e.enscope(message); return;
					case 'c': Timing::t().get(e,'c',message); return;
					default: break;
				}
			}
		} else {
			message =  my.parm(1);
		}
		e << Message(type,message);
	}
	void iDate::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		ostringstream calculation;
		string stime = my.parm(1);
		string format;
		::time_t tt;
		if ( stime.empty() ) {
			tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
		} else {
			std::tm tm = {};
			std::stringstream ss(stime);
			if(stime.find('-') != string::npos) {
				ss >> std::get_time(&tm,"%Y-%m-%d %T");
			} else {
				ss >> std::get_time(&tm,"%Y %m %d %T");
			}
			tt = std::mktime(&tm);
		}
		if (my.count > 1) {
			format = my.parm(2);
		}
		if (format.empty()) {
			format = "%F %T";
		}
		calculation << put_time(gmtime(&tt),format.c_str());
		my.set(calculation.str());
	}
	void iEval::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		std::stringstream code;
		bool advanced = false;
		for(auto i=1; i <= my.count; i++) {
			string parm = my.parm(i);
			advanced = advanced || Definition::test_adv(parm);
			code << parm;
			if (i != my.count) {
				code << ",";
			}
		}
		ostringstream result;
		mt::MacroText structure;
		mt::Driver driver(e,code,advanced);
		driver.parse(e,structure,false); //no strip
		structure.expand(e,result,context);
		my.set(result.str());
	}
	void iFile::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		//second parameter is 'true' or 'false' - (default = false) and represents whether
		//or not to treat the file as macrotext.
		InternalInstance my(this,e,o,instance,context);
		Path base = Env::e().basedir(Built);
		std::string filename = my.parm(1);
		File file(base,filename);
		if(file.makeRelativeTo(base)) {
			file.makeAbsoluteFrom(base);
			if(file.exists()) {
				string contents = file.readFile();
				bool evaluate = my.boolParm(2);
				if(evaluate) {
					std::istringstream code(contents);
					mt::Driver driver(e,code,Definition::test_adv(contents));
					mt::MacroText structure;
					driver.parse(e,structure,false); //bool advanced, bool strip
					ostringstream result;
					structure.expand(e,result,context);
					my.set(result.str());
				} else {
					my.set(contents);
				}
			} else {
				e << Message(error,"File "+ file.output(true) +" was not found.");
			}
		} else {
			e << Message(error,"File "+ file.output(true) +" is not in a place to be found.");
		}
	}

	std::stack<const FieldContext*> iField::contextStack;
	void iField::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		if(contextStack.empty()) {
			e << Message(error,_name + " must be used within a context such as iForSubs or iForQuery.");
		} else {
			my.set(contextStack.top()->get(e,my.parm(1)));
		}
	}

	void iForSubs::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
//		ForSubs(table,Noderef,keyfield,orderfield,[context])
//		qstr << "select * from " << table << " where " <<  keyfield << "='" << noderef << "'";
//		if (orderfield.length() > 0) qstr << " order by " <<  orderfield;
		InternalInstance my(this,e,o,instance,context);
		if (sql != nullptr && sql->isopen()) {
			const node::Node* interest = my.node(2);
			if (interest != nullptr) {
				string table = my.parm(1); sql->escape(table);
				string nfield = my.parm(3); sql->escape(nfield);
				ostringstream querystring;
				querystring << "select * from " << my.parm(1) << " where " <<  my.parm(3) << "='" << interest->id() << "'";
				string order = my.parm(4);
				if(!order.empty()) {
					sql->escape(order);
					querystring << " order by " << order;
				}
				if (sql->query(e,query,querystring.str())) {
					if(query->execute(e)) {
						iField::contextStack.push(this);
						while(query->nextrow()) {
							my.expand(5);
						}
						iField::contextStack.pop();
					}
				}
				sql->dispose(query);
			}
		} else {
			e << Message(error,_name + " requires an open sql connection.");
		}
	}
	Support::Db::Query* iForSubs::query = nullptr;
	string iForSubs::get(Messages& errs,const string& name) const {
		string result;
		if(query != nullptr) {
			query->readfield(errs,name,result);
		}
		return result;
	}

	void iForQuery::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		if (sql != nullptr && sql->isopen()) {
			if (sql->select(e,query,my.parm(1))) {
				if(query->execute(e)) {
					iField::contextStack.push(this);
					while(query->nextrow()) {
						my.expand(2);
					}
					iField::contextStack.pop();
				}
			}
			sql->dispose(query);
		} else {
			e << Message(error,_name + " requires an open sql connection.");
		}
	}
	Support::Db::Query* iForQuery::query = nullptr;
	string iForQuery::get(Messages& errs,const string& name) const {
		string result;
		if(query != nullptr) {
			query->readfield(errs,name,result);
		}
		return result;
	}

	void iMath::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		ostringstream msg;
		long double nan = numeric_limits<long double>::signaling_NaN();
		long double x = 0.0;
		long double y = 0.0;
		string operation = my.parm(1);
		string operandx = my.parm(2);
		string operandy;
		Support::trim(operation); //strip spaces, tabs, newlines..
		if(operation.empty()) {
			msg << "Incorrect function parameter (" << operation << ")";
			msg << "Use one of +,-,{*,x},/,{M,m},{&lt;,f},{&gt;,c},=,|,&,U,N,R";
			msg << " optionally followed by formatting flag 'i','f','x'";
			e << Message(error,msg.str());
			return;
		}
		string::const_iterator a = operandx.begin();
		if ((*a == 'x') || (*a == 'X')) {
			auto cc = Support::hex(++a,x);
			if ( cc + 1 != operandx.size() ) x = nan;
		} else {
			x = Support::real(a);
		}
		if(my.count > 2) {
			operandy = my.parm(3);
			string::const_iterator yi = operandy.begin();
			if ((*yi == 'x') || (*yi == 'X')) {
				auto cc = Support::hex(++yi,y);
				if ( cc + 1 != operandy.size() ) y = nan;
			} else {
				y = Support::real(yi);
			}
		}
		long double answer = nan;
		if ( x != x || y != y ) { //standard nan test, apparently.
			answer = nan;
		} else {
			switch (operation[0]) {
				case '+':
					answer = x + y;
					break;
				case '-':
					answer = x - y;
					break;
				case '*':
				case 'x':
					answer = x * y;
					break;
				case '/':
					answer = x / y;
					break;
				case 'R':
					answer = drand48() * (y - x) + x;
					break;  //iMath(R,3,6) a value between 3 and 6)

				case 'M':            //Mod
				case 'm':
					answer = static_cast<int>(x) % static_cast<int>(y);
					break;
				case 'L':    //lessthan
				case '<':
					if (x < y) answer = 1;
					else answer = 0;
					break;
				case 'G':    //greaterthan
				case '>':
					if (x > y) answer = 1;
					else answer = 0;
					break;
				case '=':
					if (x == y) answer = 1;
					else answer = 0;
					break;
				case '|':
					if (std::fpclassify(x) == FP_NORMAL || std::fpclassify(y) == FP_NORMAL) answer = 1;
					else answer = 0;
					break;
				case 'A':
					if (std::fpclassify(x) == FP_NORMAL && std::fpclassify(y) == FP_NORMAL) answer = 1;
					else answer = 0;
					break;
				case 'U':
					answer = (static_cast<int>(x) | static_cast<int>(y));
					break; //answer = 1; else answer = 0; break;
				case 'N':
					answer = (static_cast<int>(x) & static_cast<int>(y));
					break; //answer = 1; else answer = 0; break;
				case 'f': { //Floor / minimum
					if (my.count < 3 || operandy.empty() || y == nan) {
						answer = floor(x);
					} else {
						if (x > y) answer = y; else answer = x;
					}
				} break;
				case 'c': { //Ceiling / maximum
					if (my.count < 3 || operandy.empty() || y == nan) {
						answer = ceil(x);
					} else {
						if (x < y) answer = y; else answer = x;
					}
				} break;
				case '&': {
					if ((operation.compare("&amp;") == 0)) {
						if (std::fpclassify(x) == FP_NORMAL && std::fpclassify(y) == FP_NORMAL) { answer = 1; } else { answer = 0; }
					}
					if ((operation.compare("&lt;") == 0)) {
						if (x < y) { answer = 1; } else { answer = 0; }
					}
					if ((operation.compare("&gt;") == 0)) {
						if (x > y) { answer = 1; } else { answer = 0; }
					}
				} break;
				default: {
					msg << "Incorrect function parameter (" << operation << "); ";
					msg << "Use one of +,-,{*,x},/,{M,m},{&lt;,f},{&gt;,c},=,|,&,U,N,R";
					msg << " optionally followed by formatting flag 'i','f','x'. ";
					msg << "f may be appended with the number of required decimal points.";
					e << Message(error,msg.str());
					return;
				} break;
			}
		}
		if(!operation.empty()) { //now is a formatter
			operation.erase(0,1);
		}
		if(operation.empty()) { //now is a formatter
			operation = "f0";
		}
		if (my.count > 3) {
			my.logic(answer,operation,4); //@iMath(*,2,5,[10,T,F]) (offset points at value before T)
		} else {
			my.set(Support::tostring(answer,operation)); //@iMath(*,2,5)
		}
	}

	void iNull::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		// Badly named, this evaluates contents but does not output text.
		InternalInstance my(this,e,o,instance,context);
		for (size_t i=1; i <= my.count; i++) {
			auto unused = my.parm(i);
		}
	}
	void iTiming::expand(Messages& e,MacroText& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		Timing& timer = Timing::t();
		std::string timerParm = my.parm(1);
		ostringstream result;
		size_t sz = timerParm.size();
		std::string p2 = my.count > 1 ? my.parm(2) : "";
		if(sz == 1) { //This isn't a get custom.
			char c = tolower(timerParm[0]);
			switch(c) {
				case 'l': {
					if (my.count == 3) {
						timer.get(result,c,p2,timer.unit(my.parm(3),none));
					} else {
						timer.get(result,c,"",timer.unit(p2,none));
					}
				} break;
				case 'n':
				case 'b': timer.get(result,c,"",timer.unit(p2,none)); break;
				case 'c': if(!p2.empty()) timer.set(c,p2); break;
				default : timer.get(result,'c',timerParm,timer.unit(p2,none));
			}
		} else {
			timer.get(result,'c',timerParm,timer.unit(p2,none));

		}
		my.set(result.str());
	}

}
