#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "test.h"
#include "Driver.h"
#include "Definition.h"
#include "Internals.h"

using namespace std;

namespace testing {

    class String {
    public:
        static size_t length(const string& s) {
            return mbstowcs(NULL,s.c_str(), s.size());
        }
        static void toupper(string& s) {
            for (char &i : s)
                i = std::toupper(i);
        }
        static size_t fandr(string& source, const string basis, const string replace) {
            size_t rep_count = 0;
            size_t bsize = basis.length();
            if ( ( source.length() >= bsize ) && ( bsize > 0 ) ) {
                size_t rsize = replace.length();
                size_t spos = source.find(basis);
                while (  spos != string::npos ) {
                    source.replace(spos,bsize,replace); rep_count++;
                    spos = source.find(basis,spos+rsize);
                }
            }
            return rep_count;
        }
        static size_t natural(const string& s) {
            string::const_iterator in = s.begin();
            string::const_iterator out = s.end();
            size_t val = 0;
            while(in != out && isdigit(*in)) {
                val = val * 10 + *in - '0';
                in++;
            }
            return val;
        }
    };

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
                String::toupper(title);
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
                //								cout << "• " << name << " √ " << endl;

            default: {
                title.insert(0,1,' ');
                title.push_back(' ');
            } break;
        }
        length = String::length(title);
        length = length > 70 ? 70 : length;
        for (size_t i=0 ; i < 70 - length ; i++) head.append(f);
        cout  << blue << m << f << f << f << f << f << n << " " << title << " " << n << head << m << norm << endl;
    }

    void group::wss(std::string& basis,bool toSym) {
        if (toSym) {
            String::fandr(basis, "\t"  , "␉");
            String::fandr(basis, "\n"  , "␤");
            String::fandr(basis, "\x0D", "␍");
            String::fandr(basis, "\x0A", "␊");
        } else {
            String::fandr(basis, "␉", "\t");
            String::fandr(basis, "␤", "\n");
            String::fandr(basis, "␍", "\x0D");
            String::fandr(basis, "␊", "\x0A");
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
                        mt::Driver driver;
                        pcode = code;
                        wss(pcode,false);
                        std::istringstream cStream(code);
                        result expansion(name);
                        if(define) {
                            mt::Definition macro(name,pcode,0,-1,false,false,false);
                            macro.visit(expansion.out);
                        } else {
                            mt::mtext structure = driver.parse(cStream,true,false); //bool advanced, bool strip
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
                        infile >> min >> max >> ws;
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
                                error_index = String::natural(digits);
                                expected.erase(0,num+1);
                            }
                        }

                        mt::Driver driver;
                        pprogram = program;
                        wss(pprogram,false);
                        std::istringstream code(pprogram);
                        mt::mtext structure = driver.parse(code,true,false); //bool advanced, bool strip
                        pexpected = expected;
                        wss(pexpected,false);

                        result expansion(name);
                        mt::Driver::expand(structure,expansion.out);

                        if(expansion.out.str() == pexpected /*&& !expansion.second.marked()*/) {
                            if (showGood) {
                                title(name,2);
                            }
                        } else {
                            if(error_test /* && expansion.second.marked() */) {
                               /*
                                string message = expansion.second.line(error_index,false);
                                if(message == expected) {
                                    if(showGood) {
                                        title(name,2);
                                    }
                                } else {
                                    title(name,3);
                                    cout << " E program:"  << program << endl;
                                    cout << " E returned:\"" << message <<  "\" on line:" << error_index  << endl;
                                    cout << " E expected:\"" << expected << "\"" << endl;
                                    if(expansion.second.marked()) {
                                        cout << "Errors: " << expansion.second.str() << endl << endl;
                                    }
                                }
                                */
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
