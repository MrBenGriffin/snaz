#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "test.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt/Driver.h"
#include "mt/Definition.h"
#include "mt/Internals.h"

using namespace std;

namespace testing {

	struct result {
	public:
		string name;
		ostringstream out;
		result(string s) : name(s) {}
	};

	group::group(string b) : base(b) {}

	void group::title(string title,int type) {
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
				//cout << "• " << name << " √ " << endl;
			default: {
				title.insert(0,1,' ');
				title.push_back(' ');
			} break;
		}
		Support::length(title,length);
		length = length > 70 ? 70 : length;
		for (size_t i=0 ; i < 70 - length ; i++) head.append(f);
		cout  << blue << m << f << f << f << f << f << n << " " << title << " " << n << head << m << norm << endl;
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

	void group::load(string filename="main", bool showGood, bool showDefines) {
		ifstream infile(base+filename);
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
						cout << "Template definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'S': {
						//Segment 	id  name	type*	break
						//S			1	text	2		@wBr()
						cout << "Segment definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'L': {
						//Layout  	id  name		template(s)	segment(s)
						//L			1	TestLayout	20,21,22	12,13,14
						cout << "Layout definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'N': {
						//Node		id	parent		linkref		layout	title
						//N			2	0			home		1		Home Page
						cout << "Node definition not yet implemented." << endl;
						getline(infile,name); break;
					}
					case 'C': {
						//Content	node	segment		code
						//C			2		1			@iTitle(I0)
						cout << "Content definition not yet implemented." << endl;
						getline(infile,name); break;
					}

					case 'X': {
						stopping = true; break;
					}

					case '!': { // insert testfile
						infile >> ws;
						getline(infile, name);
						name.erase(name.find_last_not_of(" \t'\"")+1);
						title(name,1);
						testing::group insert(base);
						insert.load(name,showGood);
					} break;

					case '#': { // comment
						string comment;
						getline(infile,comment);
						title(comment);
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
						infile >> ws >> define  >> ws;
						do getline(infile, code ,'\t'); while (!infile.eof() && code.empty());
						getline(infile, expected);
						expected.erase(0, expected.find_first_not_of('\t'));
						pcode = code;
						wss(pcode,false);
						bool advanced = mt::Definition::test_adv(code);
						std::istringstream cStream(code);
						mt::Driver driver(cStream,advanced);
						result expansion(name);
						if(define) {
							mt::Definition macro(name,pcode,0,-1,false,false,false);
							macro.visit(expansion.out);
						} else {
							mt::mtext structure = driver.parse(false); 	//bool advanced, bool strip
							mt::Driver::visit(structure,expansion.out);
						}
						string visited=expansion.out.str();
						wss(pcode,true);
						if(visited == expected) {
							if (showGood) {
								title(code,2);
							}
						} else {
							title(code,3);
							cout << lred << " -found:"  << blue << visited  << endl;
							cout << lred << " -not  :" << expected << endl << norm;
						}
					} break;

					case 'U': { // define macro
						//U	a	0	1	11	[%1]
						//U	b	0	1	11	⌽a(⍟1)
						//U	macroname	1	4	11 expansion (11 = strip,pstrip,preParse)
						string expansion,bools;
						signed long min,max;
						infile >> ws;
						getline(infile,name,'\t');
						infile >> ws >> min >> max >> ws;
						getline(infile,bools,'\t');
						if (bools.size() < 3) {
							bools.append(string(3-bools.size(),'0')); //default to false..
						}
						getline(infile,expansion);
						wss(expansion,false);
						mt::Definition macro(name,expansion,min,max,bools[0]=='1',bools[1]=='1',bools[2]=='1');
						mt::Definition::add(macro);
						if(showDefines) {
							cout << "Defined " << name << ":" ;
							std::ostringstream result;
							mt::Definition::vis(name,result);
							std::string definition = result.str();
							wss(definition,true);
							cout << definition << " " << min << "-" << max << " SPx:" << bools << std::endl;
						}
					} break;

					default: {
						bool error_test = false;
						size_t error_index = 0;
						getline(infile, name ,'\t');
						name = c + name;
						string program,expected,pprogram,pexpected;
						do getline(infile, program ,'\t'); while (!infile.eof() && program.empty());
						getline(infile, expected);
						expected.erase(0, expected.find_first_not_of('\t'));

						if(!expected.empty() && expected[0] == '!') {
							expected.erase(0,1);
							error_test = true;
							auto num = expected.find('!');
							if(num != string::npos) {
								string digits(expected.substr(0,num));
								error_index = Support::natural(digits);
								expected.erase(0,num+1);
							}
						}

						pprogram = program;
						wss(pprogram,false);
						std::istringstream code(pprogram);
						bool advanced = mt::Definition::test_adv(pprogram);
						mt::Driver driver(code,advanced);
						mt::mtext structure = driver.parse(false); //bool advanced, bool strip
						pexpected = expected;
						wss(pexpected,false);
						Support::Messages errs;
						result expansion(name);
						driver.expand(expansion.out,errs,name);
						if(expansion.out.str() == pexpected /*&& !expansion.second.marked()*/) {
							if (showGood) {
								title(name,2);
							}
						} else {
							if(error_test  && errs.marked() ) {
								 string message = errs.line(error_index);
								 if(message == expected) {
									 if(showGood) {
										 title(name,2);
									 }
								 } else {
									 title(name,3);
									 cout << " E program:"  << program << endl;
									 cout << " E returned:\"" << message <<  "\" on line:" << error_index  << endl;
									 cout << " E expected:\"" << expected << "\"" << endl;
									 if(errs.marked()) {
										 cout << "Errors: ";
										 errs.str(cout);
										 cout << endl;
									 }
								 }
							} else {
								ostringstream pstuff;
								string parsed,returned = expansion.out.str();
								mt::Driver::visit(structure,pstuff);
								parsed = pstuff.str();
								wss(returned,true); wss(parsed,true);
								title(name,3);
								cout << lred << " - program:" << blue << program << lred << endl;

								cout << " -  parsed:" << blue << parsed << lred << endl;
								cout << " - returned:" << returned << endl;
								cout << " - expected:" << expected << endl;
								cout << norm;

							}
						}
					}
				}
			}
			if(filename == "main") {
				cout << blue << "•═══════════════════════════════════════════════════════════════════════════════•" << endl << endl << endl;
			}
			infile.close();
		} else {
			title("filename "+filename+" not found",3);
		}
	}

}
