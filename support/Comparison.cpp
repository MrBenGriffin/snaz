//
// Created by Ben Griffin on 2019-01-31.
//

#include <cmath>
#include "Comparison.h"
#include "Convert.h"
#include "node/Node.h"
#include "node/Content.h"
#include "content/Layout.h"

namespace Support {
	using namespace node;
	size_t Sort::offset = 0;
	string Sort::regex = "^.+$";

//	comparison function object (i.e. an object that satisfies the requirements of Compare)
//	which returns ​true if the first argument is less than (i.e. is ordered before) the second.

	bool asc_caseinsensitive(const string& s1, const string& s2) {
		string::const_iterator p1=s1.begin();
		string::const_iterator p2=s2.begin();
		if(Sort::offset > 0) {
			p1 += Sort::offset;
			p2 += Sort::offset;
		}
		while (p1!=s1.end() && p2 != s2.end()) {
			if ( std::toupper(*p1) != std::toupper(*p2) ) {
				return std::toupper(*p1) < std::toupper(*p2);
			}
			++p1; ++p2;
		}
		return s1.size() < s2.size();
	}

	bool asc_casesensitive(const string& s1, const string& s2) {
		return s1.compare(Sort::offset,string::npos,s2,Sort::offset,string::npos) < 0;
//		return s1.compare(s2) < 0;
	}

	bool asc_numeric(const string& s1, const string& s2) {
		long double a=NAN,b=NAN;
		try {
			a = stold(s1.substr(Sort::offset));
			b = stold(s2.substr(Sort::offset));
		}  catch (...) {
			return false;
		}
		return a < b;
	}

	//2013-11-06 09:30:44  --> 20131106093044

	bool asc_date(const string& s1, const string& s2) {
		unsigned long long v1=0, v2=0;
		for (char i1 : s1.substr(Sort::offset) ) {
			if(isdigit(i1)) {
				v1 = v1 * 10 + (i1 - '0');
			}
		}
		for (char i2 : s2.substr(Sort::offset)) {
			if(isdigit(i2)) {
				v2 = v2 * 10 + (i2 - '0');
			}
		}
		return v1 < v2;
	}

	bool desc_caseinsensitive(const string& s1, const string& s2) {
		return asc_caseinsensitive(s2,s1);
	}
	bool desc_casesensitive(const string& s1, const string& s2) {
		return asc_casesensitive(s2,s1);
	}
	bool desc_numeric(const string& s1, const string& s2) {
		return asc_numeric(s2,s1);
	}
	bool desc_date(const string& s1, const string& s2) {
		return asc_date(s2,s1);
	}

	//Now Nodes.

	bool sortscratch(const Node *n1,const Node *n2)   {
		Messages e;
		return (n1->comm().compare(n2->comm()) < 0);
	}
	bool sorttw(const Node *n1,const Node *n2)   {
		return n1->tw() < n2->tw();
	}
	bool sorttier(const Node *n1,const Node *n2)   {
		return n1->tier() < n2->tier();
	}
	bool sortfname(const Node *n1,const Node *n2)   {
		Messages e;
		return (n1->get(e,baseFilename).compare(n2->get(e,baseFilename)) < 0);
	}
	bool sortbirth(const Node *n1,const Node *n2)   {
		Messages e;
		return n1->get(e,birth) < n2->get(e,birth);
	}
	bool sortdeath(const Node *n1,const Node *n2)   {
		Messages e;
		return n1->get(e,death) < n2->get(e,death);
	}
	bool sorttitle(const Node *n1,const Node *n2)   {
		Messages e;
		return (n1->get(e,title).compare(n2->get(e,title)) < 0);
	}
	bool sorttitlei(const Node *n1,const Node *n2)   {
		Messages e;
		return asc_caseinsensitive(n1->get(e,title),n2->get(e,title));
	}
	bool sortlinkref(const Node *n1,const Node *n2)  {
		return (n1->ref().compare(n2->ref()) < 0);
	}
	bool sortlinkrefi(const Node *n1,const Node *n2)  {
		return asc_caseinsensitive(n1->ref(),n2->ref());
	}
	bool sortshorttitle(const Node *n1,const Node *n2)   {
		Messages e;
		return (n1->get(e,shortTitle).compare(n2->get(e,shortTitle)) < 0);
	}
	bool sortshorttitlei(const Node *n1,const Node *n2)   {
		Messages e;
		return asc_caseinsensitive(n1->get(e,shortTitle),n2->get(e,shortTitle));
	}

}
