#include <ostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "test.h"
#include "support/Definitions.h"
#include "support/Regex.h"
#include "support/Convert.h"
#include "support/Encode.h"
#include "support/Fandr.h"
#include "support/Env.h"

#include "mt/Driver.h"
#include "mt/Definition.h"
#include "mt/Internal.h"
#include "mt/MacroText.h"

#include "node/Metrics.h"
#include "node/Content.h"

using namespace std;

namespace testing {

	group::group(Messages& msg,string b) : base(b) {
		msgs = &msg;
	}

	void group::title(ostream& o,string title,int type) {
		string head; size_t length;
		string f = "─";
		string m = "•";
		string n = "•";
		switch(type) {
			case 1: {
				Support::toupper(title);
				f = "═";
			} break;
			case 2: { // good test.
				m = green + "✔︎";
				n = " ";
				f = " ";
			} break;
			case 3: { // bad test.
				m = red + "✖";
				n = " ";
				f = " ";
			} break;
				//o << "• " << name << " √ " << endl;
			default: {
				title.insert(0,1,' ');
				title.push_back(' ');
			} break;
		}
		Support::length(title,length);
		length = length > 70 ? 70 : length;
		for (size_t i=0 ; i < 70 - length ; i++) head.append(f);
		o << blue << m << f << f << f << f << f << n << " " << title << " " << n << head << m << norm << endl;
	}

	void group::wss(std::string& basis,bool toSym) {
		if (toSym) {
			Support::fandr(basis, "\t"  , "␉");
			Support::fandr(basis, "\n"  , "␤");
			Support::fandr(basis, "\x0D", "␍");
			Support::fandr(basis, "\x0A", "␊");
		} else {
			Support::fandr(basis, "␉", "\t");
			Support::fandr(basis, "␤", "\n");
			Support::fandr(basis, "␍", "\x0D");
			Support::fandr(basis, "␊", "\x0A");
		}
	}

	void group::load(ostream& o,string filename="main", bool showGood, bool showDefines) {
		Support::Env& env = Support::Env::e();
		string newBase;
		env.basedir(newBase,Support::Tests,true,true);
		ifstream infile(newBase+filename);

		// We need a node to be 'current'.
		mt::mstack context;
		auto* currentNode = node::Content::root();
		node::Metrics metrics;
		metrics.nodeStack.push_back(currentNode);
		metrics.current = currentNode;
		metrics.page = 0;
		mt::Instance instance(&metrics);
		context.push_back({nullptr,&instance}); //This is our context.

		if (infile.is_open()) {
			char c;
			string name;
			bool stopping = false;
			while(!(infile >> c).eof() && !stopping) {
				switch (c) {

					case '\n':
					case '\r': break;

					case '/': getline(infile,name); break;

						// Not yet implemented...
					case 'T': {
						//Template 	id  code	suffix	break
						//T			1	@page()	html	@wBr()
						o << "Template definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'S': {
						//Segment 	id  name	type*	break
						//S			1	text	2		@wBr()
						o << "Segment definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'L': {
						//Layout  	id  name		template(s)	segment(s)
						//L			1	TestLayout	20,21,22	12,13,14
						o << "Layout definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'N': {
						//Node		id	parent		linkref		layout	title
						//N			2	0			home		1		Home Page
						o << "Node definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'C': {
						//Content	node	segment		code
						//C			2		1			@iTitle(I0)
						o << "Content definition not yet implemented." << endl;
						getline(infile,name); break;
					}

					case 'X': {
						stopping = true; break;
					}

					case '!': { // insert testfile
						infile >> ws;
						getline(infile, name);
						name.erase(name.find_last_not_of(" \t'\"")+1);
						title(o,name,1);
						testing::group insert(*msgs,base);
						insert.load(o,name,showGood);
					} break;

					case '#': { // comment
						string comment;
						getline(infile,comment);
						title(o,comment);
					} break;

					case 'F': { // flag
						string setting;
						signed long value;
						infile >> ws;
						getline(infile,setting,'\t');
						infile >> value >> ws;
						if (setting == "showPassed") {
							showGood = (value == 1);
						}
						if (setting == "showDefines") {
							showDefines = (value == 1);
						}
						if (setting == "pushNode") {
							auto* node = node::Content::root()->content(*msgs,value);
							metrics.nodeStack.push_back(node);
							metrics.current = metrics.nodeStack.back();
							metrics.page = 0;
						}
						if (setting == "popNode") {
							metrics.nodeStack.pop_back();
							metrics.current = metrics.nodeStack.back();
							metrics.page = 0;
						}
					} break;

					case '~': { // delete a user macro.
						getline(infile,name);
						name.erase(name.find_last_not_of(" \t")+1);
						mt::Definition::del(name);
					} break;

					case 'P': {
						//Parse		code    expected
						//P			2		1
						string code,pcode,expected;
						unsigned define;
						msgs->reset();
						infile >> ws >> define  >> ws;
						do getline(infile, code ,'\t'); while (!infile.eof() && code.empty());
						getline(infile, expected);
						expected.erase(0, expected.find_first_not_of('\t'));
						pcode = code;
						wss(pcode,false);
						bool advanced = mt::Definition::test_adv(code);
						std::istringstream cStream(code);
						mt::Driver driver(*msgs,cStream,advanced);
						ostringstream expansion;
						if(define) {
							mt::Definition macro(*msgs,name,pcode,0,-1,false,false,false);
							macro.visit(expansion);
						} else {
							mt::MacroText structure;
							driver.parse(*msgs,structure,false); 	//bool advanced, bool strip
							structure.visit(expansion);
						}
						if(msgs->marked()) {
							o << lred << "Parse Errors" << endl;
							msgs->str(o);
							o << norm << endl;
						}
						string visited=expansion.str();
						wss(pcode,true);
						if(visited == expected) {
							if (showGood) {
								title(o,code,2);
							}
						} else {
							title(o,code,3);
							o << lred << " -found:"  << blue << visited  << endl;
							o << lred << " -not  :" << expected << endl << norm;
						}
					} break;

					case 'U': { // define macro
						//U	a	0	1	11	[%1]
						//U	b	0	1	11	⌽a(⍟1)
						//U	macroname	1	4	11 expansion (11 = strip,pstrip,preParse)
						string expansion,bools;
						signed long min,max;
						msgs->reset();
						infile >> ws;
						getline(infile,name,'\t');
						infile >> ws >> min >> max >> ws;
						getline(infile,bools,'\t');
						if (bools.size() < 3) {
							bools.append(string(3-bools.size(),'0')); //default to false..
						}
						getline(infile,expansion);
						wss(expansion,false);
						mt::Definition* macro = new mt::Definition(*msgs,name,expansion,min,max,bools[0]=='1',bools[1]=='1',bools[2]=='1');
						if(!msgs->marked()) {
							mt::Definition::del(name);
							mt::Definition::library.emplace(name,macro);
						} else {
							o << lred << "Definition Parse Error while defining " << name << endl;
							msgs->str(o);
							o << norm << endl;
							delete macro;
						}
						if(showDefines) {
							o << "Defined " << name << ":" ;
							std::ostringstream result;
							mt::Definition::vis(name,result);
							std::string definition = result.str();
							wss(definition,true);
							o << definition << " " << min << "-" << max << " SPx:" << bools << std::endl;
						}
					} break;

					default: {
						bool error_test = false;
						bool regex_test = false;
						size_t error_index = 0;
						getline(infile, name ,'\t');
						name = c + name;
						string program,expected,pprogram,pexpected;
						do getline(infile, program ,'\t'); while (!infile.eof() && program.empty());
						getline(infile, expected);
						expected.erase(0, expected.find_first_not_of('\t'));

						if(!expected.empty()) {
							if(expected[0] == '!') {
								expected.erase(0, 1);
								error_test = true;
								auto num = expected.find_first_of("!?");
								if (num != string::npos) {
									string digits(expected.substr(0, num));
									error_index = Support::natural(digits);
									regex_test = expected[num] == '?';
									expected.erase(0, num + 1);
								}
							} else {
								if(expected[0] == '?') {
									expected.erase(0, 1);
									regex_test = true;
								}
							}
						}

						msgs->reset();
						pprogram = program;
						wss(pprogram,false);
						std::istringstream code(pprogram);
						bool advanced = mt::Definition::test_adv(pprogram);
						mt::Driver driver(*msgs,code,advanced);
						mt::MacroText structure;
						driver.parse(*msgs,structure,false); //bool advanced, bool strip
						pexpected = expected;
						wss(pexpected,false);
						ostringstream expansion; //need to set node!
						structure.expand(*msgs,expansion,context);
						if(!error_test) {
							bool testPassed = false;
							if(regex_test) {
								Support::Messages discard(nullptr);
								if (Support::Regex::available(discard)) {
									testPassed = Support::Regex::fullMatch(discard, pexpected,expansion.str()); //match entire string using pcre
									if (discard.marked()) {
										o << " E Error in Test Regex:" << expected << endl;
										o << lred << "Regex Errors: ";
										msgs->str(o);
										o << norm << endl;
									}
								} else {
									o << "E regex not available for test" << endl;
								}
							} else {
								testPassed = expansion.str() == pexpected;
							}
							if(testPassed && !msgs->marked() ) {
								if (showGood) {
									title(o,name,2);
								}
							} else {
								title(o,name,3);
								if(!testPassed) {
									ostringstream pstuff;
									string parsed,returned = expansion.str();
									structure.visit(pstuff);
									parsed = pstuff.str();
									wss(returned,true); wss(parsed,true);
									o << lred << " - program:" << blue << program << lred << endl;

									o << " -  parsed:" << blue << parsed << lred << endl;
									o << " - returned:" << returned << endl;
									o << " - expected:" << expected << norm << endl;
								}
								if(msgs->marked()) {
									o << lred << "Errors: ";
									msgs->str(o);
									o << norm << endl;
								}
							}
						} else {
							bool matched = false;
							string message = msgs->line(error_index);
							if(regex_test) {
								Support::Messages discard(nullptr);
								if(Support::Regex::available(discard)) {
									matched= Support::Regex::fullMatch(discard,pexpected,message); //match entire string using pcre
									if(discard.marked()) {
										o << " E Error in Test Regex:"  << expected << endl;
										o << lred << "Regex Errors: ";
										discard.str(o);
										o << norm << endl;
									}
								} else {
									o << "E regex not available for test" << endl;
								}
							} else {
								matched= (message == pexpected);
							}
							if(matched) {
								if(showGood) {
									title(o,name,2);
								}
							} else {
								title(o,name,3);
								o << " E program:"  << program << endl;
								o << " E error returned:\"" << message <<  "\" on line:" << error_index  << endl;
								o << " E value returned:\"" << expansion.str() << "\"" << endl;
								o << " E expected:\"" << expected << "\"" << endl;
								if(msgs->marked()) {
									o << lred << "Errors: ";
									msgs->str(o);
									o << norm << endl;
								}
							}
						}
//						cout << "- name -" << endl;
//						mt::Driver::visit(structure,cout);
						structure.reset();
					}
				}
			}
			if(filename == "main") {
				o << blue << "•═══════════════════════════════════════════════════════════════════════════════•" << endl << endl << endl;
			}
			infile.close();
		} else {
			title(o,"filename "+filename+" not found in "+ newBase,3);
		}
	}

}
